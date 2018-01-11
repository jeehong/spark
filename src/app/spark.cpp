//#include <QtWidgets>
#include <QMainWindow>
#include <QScrollBar>
#include <QTableWidget>
#include <QTableView>
#include <QHeaderView>
#include <QDebug>

#include "spark.h"
#include "ui_spark.h"
#include "src/mid/mid_can.h"
#include "src/mid/mid_bits.h"
#include "src/mid/mid_list.h"
#include "src/mid/mid_data.h"
#include "src/mid/mid_pool.h"
#include "src/mid/comm_typedef.h"

Spark::Spark(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::Spark)
{
    ui->setupUi(this);
    ui->pushButton_2->setEnabled(FALSE);
    ui->lcdNumber->setDigitCount(13);
    ui->lcdNumber_2->setDigitCount(13);
    ui->lcdNumber_3->setDigitCount(13);
    // rx_window
    int rx_window_column_width[RX_WINDOW_ITEMS] =
    {
        90/*Rx*/,30/*Chn*/,70/*Identifer*/,30/*Flag*/,30/*DLC*/,
        30/*D0*/,30/*D1*/,30/*D2*/,30/*D3*/,30/*D4*/,30/*D5*/,
        30/*D6*/,30/*D7*/,80/*Time-Stamp*/,80/*Delta-Stamp*/
    };
    init_dock_window(&rx.window,
                    "Rx Window",
                    700,
                    200,
                    "Rx Chn Identifer Flag DLC D0 D1 D2 D3 D4 D5 D6 D7 Time-Stamp Delta-Stamp",
                    RX_WINDOW_ITEMS,
                    rx_window_column_width);

    // rx_parse_window
    int rx_parse_window_column_width[RX_WINDOW_ITEMS] =
    {
        80/*Name*/,60/*Identifer*/,60/*Start-Bit*/,65/*Bit-Length*/,
        40/*Factor*/,40/*Offset*/,30/*Byte Order*/,30/*Bits Order*/,
        40/*Row*/,80/*Phy*/,80/*Time-Stamp*/,80/*Delta-Stamp*/
    };
    init_dock_window(&rx_parse_window,
                    "Rx Parse Window",
                    700,
                    200,
                    "Name Identifer Start-Bit Bit-Length Factor Offset Mot MSB Row Phy Time-Stamp Delta-Stamp",
                    RX_PARSE_ITEMS,
                    rx_parse_window_column_width);

    rx.refresh = FALSE;
    // rx_parse init
    rx_parse = init_data_parse();

    memset(&tx.msg_config, 0, sizeof(tx.msg_config));
    tx.msg_config.config_delta_time_stamp = 100;
    tx.msg_config.dlc = MAX_DLC;
    // tx_window
    int tx_window_column_width[RX_WINDOW_ITEMS] =
    {
        90/*Tx*/,30/*Chn*/,70/*Identifer*/,30/*Flag*/,30/*DLC*/,
        30/*D0 */,30/*D1*/,30/*D2*/,30/*D3*/,30/*D4*/,30/*D5*/,
        30/*D6*/,30/*D7*/,80/*Time-Stamp*/,80/*Delta-Stamp*/
    };
    init_dock_window(&tx.window,
                    "Tx Window",
                    700,
                    200,
                    "Tx Chn Identifer Flag DLC D0 D1 D2 D3 D4 D5 D6 D7 Time-Stamp Delta-Stamp",
                    TX_WINDOW_ITEMS,
                    tx_window_column_width);
    tx.msgs = 0;
    // tx_parse init
    tx.parse = init_data_parse();

    rx.fixed_posions = FALSE;
    rx.accept_id = 0;
    rx.reject_id = 0;
    rx.dec_display = FALSE;
    for(int index = 0; index < RX_LISTS_MAX; index ++)
    {
        rx.locate[index].line = 0;
        rx.locate[index].mutex = U32_INVALID_VALUE;
    }
    mid_can_init(CAN_DEVICE_KVASER);
    load_pool = mid_pool_register(10);
    tx.list = mid_can_tx_list();
    uiTimer.setInterval(100);
    connect(&uiTimer, SIGNAL(timeout()), this, SLOT(main_window_update()));

    data_thread = new Spark_Thread();
    connect(data_thread, SIGNAL(threadSignal()), this, SLOT(data_process_slot()));
    data_thread->start(QThread::NormalPriority);
}

Spark::~Spark()
{
    delete ui;
}

Spark_Thread::Spark_Thread(QObject *parent)
    : QThread(parent)
{
}

void Spark_Thread::run()
{
    while(1)
    {
        mid_can_process();
        //qDebug("thread one----------");
        //emit threadSignal();
        usleep(1000);
    }
    exec();
}

void Spark::data_process_slot()
{
	
}

struct data_parse_t * Spark::init_data_parse()
{
    struct data_parse_t *obj;
	
    obj = new struct data_parse_t;
    obj->list = list_init(sizeof(struct mid_data_config_t));
	obj->msg_lines = 0;
	obj->object = 0;
	obj->setting.id = 0;
	obj->setting.start_bit = 0;
	obj->setting.bits_length = 0;
    obj->setting.factor = 1;
    obj->setting.offset = 0;
    obj->setting.bytes_order = CAN_FORMAT_INTEL;	/* Intel */
    obj->setting.bits_order = CAN_ORDER_LSB; 		/* lsb */

    return obj;
}

void Spark::update_rx_parse_line(const struct can_bus_frame_t *frame)
{
    QString string;
    struct list_item_t *temp_item;
    struct mid_data_config_t *temp_element;

    if(rx_parse == NULL)
        return;
    for(temp_item = rx_parse->list->ended.next; temp_item != &rx_parse->list->ended; temp_item = temp_item->next)
    {
        temp_element = (struct mid_data_config_t *)temp_item->data;
        if(temp_element->id == frame->id)
        {
            string = QString("%1").arg(temp_element->flag, 10, 10, QLatin1Char('0'));
            rx_parse_window.model->item(temp_element->flag, 0)->setText(string);

            string = QString("%1").arg(frame->id, 8, 16, QLatin1Char('0')).toUpper();
            rx_parse_window.model->item(temp_element->flag, 1)->setText(string);

            string = QString("%1").arg(temp_element->start_bit, 2, 10, QLatin1Char('0')).toUpper();
            rx_parse_window.model->item(temp_element->flag, 2)->setText(string);

            string = QString("%1").arg(temp_element->bits_length, 2, 10, QLatin1Char('0')).toUpper();
            rx_parse_window.model->item(temp_element->flag, 3)->setText(string);

            rx_parse_window.model->item(temp_element->flag, 4)->setText(string.number(temp_element->factor, 'g', 6));

            rx_parse_window.model->item(temp_element->flag, 5)->setText(string.number(temp_element->offset, 'g', 6));

            string = QString("%1").arg(temp_element->bytes_order, 1, 10, QLatin1Char('0')).toUpper();
            rx_parse_window.model->item(temp_element->flag, 6)->setText(string);

            string = QString("%1").arg(temp_element->bits_order, 1, 10, QLatin1Char('0')).toUpper();
            rx_parse_window.model->item(temp_element->flag, 7)->setText(string);

            mid_data_can_calc(temp_element, (U8*)&frame->buf[0], temp_element->bytes_order, temp_element->bits_order, frame->dlc);

            rx_parse_window.model->item(temp_element->flag, 8)->setText(string.number(temp_element->row, 'g', 6));

            rx_parse_window.model->item(temp_element->flag, 9)->setText(string.number(temp_element->phy, 'g', 6));

            rx_parse_window.model->item(temp_element->flag, 10)->setText(string.number(frame->time_stamp / 1000.0, 'g', 10));

            rx_parse_window.model->item(temp_element->flag, 11)->setText(string.number(frame->delta_time_stamp / 1000.0, 'g', 10));

            rx_parse_window.table->setRowHeight(temp_element->flag, 20);
        }
        else
        {}
    }
}

void Spark::update_receive_message_window()
{
    U32 index = 0;
    const struct can_bus_frame_t *can_frame;
    struct can_bus_frame_t *temp_element;
    QString string;
    U32 format = rx.dec_display == TRUE ? 10 : 16;
    U32 half_bytes = rx.dec_display == TRUE ? 0 : 2;

    if(rx.fixed_posions == TRUE)
    {}
    for(can_frame = mid_can_new_frame();
        rx.refresh == TRUE && can_frame != NULL;
        can_frame = mid_can_new_frame())
    {
        if(rx.accept_id != 0
                && can_frame->id != rx.accept_id)
        {
            continue;
        }
        if(rx.reject_id != 0
                && can_frame->id == rx.reject_id)
        {
            continue;
        }
        for(index = 0; index < RX_LISTS_MAX; index ++)
        {
            if(rx.locate[index].mutex == can_frame->id)
            {
                break;
            }
            else if(rx.locate[index].mutex == (long)U32_INVALID_VALUE)
            {
                rx.locate[index].mutex = can_frame->id;
                rx.locate[index].line = index;
                operate_table_line(&rx.window, 1, index);

                break;
            }
        }

        string = QString("%1").arg(rx.locate[index].line, 10, 10, QLatin1Char('0'));
        rx.window.model->item(rx.locate[index].line, 0)->setText(string);

        rx.window.model->item(rx.locate[index].line, 1)->setText(string.number(can_frame->chn, 10));

        string = QString("%1").arg(can_frame->id, 8, 16, QLatin1Char('0')).toUpper();
        rx.window.model->item(rx.locate[index].line, 2)->setText(string);

        rx.window.model->item(rx.locate[index].line, 3)->setText(string.number(can_frame->flag, 10));

        rx.window.model->item(rx.locate[index].line, 4)->setText(string.number(can_frame->dlc, 10));

        string = QString("%1").arg(can_frame->buf[0], half_bytes, format, QLatin1Char('0')).toUpper();
        rx.window.model->item(rx.locate[index].line, 5)->setText(string);

        string = QString("%1").arg(can_frame->buf[1], half_bytes, format, QLatin1Char('0')).toUpper();
        rx.window.model->item(rx.locate[index].line, 6)->setText(string);

        string = QString("%1").arg(can_frame->buf[2], half_bytes, format, QLatin1Char('0')).toUpper();
        rx.window.model->item(rx.locate[index].line, 7)->setText(string);

        string = QString("%1").arg(can_frame->buf[3], half_bytes, format, QLatin1Char('0')).toUpper();
        rx.window.model->item(rx.locate[index].line, 8)->setText(string);

        string = QString("%1").arg(can_frame->buf[4], half_bytes, format, QLatin1Char('0')).toUpper();
        rx.window.model->item(rx.locate[index].line, 9)->setText(string);

        string = QString("%1").arg(can_frame->buf[5], half_bytes, format, QLatin1Char('0')).toUpper();
        rx.window.model->item(rx.locate[index].line, 10)->setText(string);

        string = QString("%1").arg(can_frame->buf[6], half_bytes, format, QLatin1Char('0')).toUpper();
        rx.window.model->item(rx.locate[index].line, 11)->setText(string);

        string = QString("%1").arg(can_frame->buf[7], half_bytes, format, QLatin1Char('0')).toUpper();
        rx.window.model->item(rx.locate[index].line, 12)->setText(string);

        rx.window.model->item(rx.locate[index].line, 13)->setText(string.number(can_frame->time_stamp / 1000.0, 'g', 10));

        rx.window.model->item(rx.locate[index].line, 14)->setText(string.number(can_frame->delta_time_stamp / 1000.0, 'g', 10));
        rx.window.table->setRowHeight(rx.locate[index].line, 20);
        rx.window.table->currentIndex();
        update_rx_parse_line(can_frame);
    }
    index = 0;
    for(struct list_item_t *list = tx.list->ended.next; list != &tx.list->ended; list = list->next)
    {
        temp_element = (struct can_bus_frame_t *)list->data;

        {
            string = QString("%1").arg(index, 10, 10, QLatin1Char('0'));
            tx.window.model->item(index, 0)->setText(string);

            tx.window.model->item(index, 1)->setText(string.number(temp_element->chn, 10));

            string = QString("%1").arg(temp_element->id, 8, 16, QLatin1Char('0')).toUpper();
            tx.window.model->item(index, 2)->setText(string);

            tx.window.model->item(index, 3)->setText(string.number(temp_element->flag, 10));

            tx.window.model->item(index, 4)->setText(string.number(temp_element->dlc, 10));

            string = QString("%1").arg(temp_element->buf[0], 2, format, QLatin1Char('0')).toUpper();
            tx.window.model->item(index, 5)->setText(string);

            string = QString("%1").arg(temp_element->buf[1], 2, format, QLatin1Char('0')).toUpper();
            tx.window.model->item(index, 6)->setText(string);

            string = QString("%1").arg(temp_element->buf[2], 2, format, QLatin1Char('0')).toUpper();
            tx.window.model->item(index, 7)->setText(string);

            string = QString("%1").arg(temp_element->buf[3], 2, format, QLatin1Char('0')).toUpper();
            tx.window.model->item(index, 8)->setText(string);

            string = QString("%1").arg(temp_element->buf[4], 2, format, QLatin1Char('0')).toUpper();
            tx.window.model->item(index, 9)->setText(string);

            string = QString("%1").arg(temp_element->buf[5], 2, format, QLatin1Char('0')).toUpper();
            tx.window.model->item(index, 10)->setText(string);

            string = QString("%1").arg(temp_element->buf[6], 2, format, QLatin1Char('0')).toUpper();
            tx.window.model->item(index, 11)->setText(string);

            string = QString("%1").arg(temp_element->buf[7], 2, format, QLatin1Char('0')).toUpper();
            tx.window.model->item(index, 12)->setText(string);

            tx.window.model->item(index, 13)->setText(string.number(temp_element->time_stamp / 1000.0, 'g', 10));

            tx.window.model->item(index, 14)->setText(string.number(temp_element->delta_time_stamp / 1000.0, 'g', 10));
            tx.window.table->setRowHeight(index, 20);
            tx.window.table->currentIndex();
            index ++;
        }
    }
}

void Spark::main_window_update()
{
    const canBusStatistics *status = mid_can_bus_statistic();
    unsigned long c = status->stdData + status->stdRemote + status->extData + status->extRemote;

    ui->lcdNumber->display(QString().setNum(c));
    ui->lcdNumber_2->display(QString().setNum(0));
    ui->lcdNumber_3->display(QString().setNum(status->errFrame));
	mid_pool_push_var(load_pool, status->busLoad, 1);
    ui->progressBar->setValue(mid_pool_get_avg(load_pool));

    update_receive_message_window();
}

void Spark::init_dock_window(struct tabel_dock_t *config,
                            QString window_name,
                            int window_width,
                            int window_height,
                            QString table_head,
                            int table_columns,
                            int *column_width)
{
    config->table = NULL;
    config->model = NULL;
    config->window_name = window_name;
    config->window_width = window_width;
    config->window_height = window_height;
    config->table_head = table_head;
    config->table_columns = table_columns;
    memcpy(config->column_width,
            column_width,
            sizeof(config->column_width[0]) * config->table_columns);
}

void Spark::init_dock_table(tabel_dock_t *config)
{
    QStringList list = config->table_head.simplified().split(" ");

    config->table = new QTableView(this);
    config->table->verticalHeader()->hide();
    config->model = new QStandardItemModel();
    config->model->setHorizontalHeaderLabels(list);
    config->table->setModel(config->model);
    config->table->setRowHeight(0, 20);
    for(int index = 0; index < config->table_columns; index ++)
    {
        config->table->setColumnWidth(index, config->column_width[index]);
    }
    config->table->show();
}

void Spark::creat_dock_window(struct tabel_dock_t *config)
{
    QDockWidget *dock = new QDockWidget(config->window_name, this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setMinimumWidth(config->window_width);
    dock->setMinimumHeight(config->window_height);
    dock->setFloating(TRUE);
    init_dock_table(config);
    dock->setWidget(config->table);

    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void Spark::operate_table_line(struct tabel_dock_t *window, int operate, U32 lines)
{
    QStandardItem *item;

    if(operate == 1)
    {
        for(int index = 0; index < window->table_columns; index ++)
        {
            item = new QStandardItem;
            item->setTextAlignment(Qt::AlignCenter);
            window->model->setItem(lines, index, item);
        }
    }
    else if(operate == -1)
    {
        window->model->removeRow(lines);
    }
}

void Spark::on_comboBox_currentIndexChanged(int index)
{
    mid_can_set_channel(index);
}

void Spark::on_pushButton_8_clicked()
{
	unsigned int index;
    unsigned int channels;
    QStringList string;

    mid_can_refresh_device();
    channels = mid_can_get_channels();
    for(index = 0; index < channels; index ++)
    {
        string << mid_can_get_channel_info(index);
    }
	ui->comboBox->clear();
    ui->comboBox->insertItems(0, string);
	ui->comboBox->setCurrentIndex(0);

    ui->comboBox_2->clear();
    ui->comboBox_2->insertItems(0, QStringList()
     << QApplication::translate("Mainww", "1M", 0)
     << QApplication::translate("MainWindow", "500K", 0)
     << QApplication::translate("MainWindow", "250K", 0)
     << QApplication::translate("MainWindow", "125K", 0)
     << QApplication::translate("MainWindow", "100K", 0)
     << QApplication::translate("MainWindow", "83K", 0)
     << QApplication::translate("MainWindow", "62K", 0)
     << QApplication::translate("MainWindow", "50K", 0)
     << QApplication::translate("MainWindow", "10K", 0)
    );
    ui->comboBox_2->setCurrentIndex(1);
}

void Spark::on_comboBox_2_activated(int index)
{
    switch(index)
    {
        case 0: mid_can_set_baudrate(canBITRATE_1M); break;
        case 1: mid_can_set_baudrate(canBITRATE_500K); break;
        case 2: mid_can_set_baudrate(canBITRATE_250K); break;
        case 3: mid_can_set_baudrate(canBITRATE_125K); break;
        case 4: mid_can_set_baudrate(canBITRATE_100K); break;
        case 5: mid_can_set_baudrate(canBITRATE_83K); break;
        case 6: mid_can_set_baudrate(canBITRATE_62K); break;
        case 7: mid_can_set_baudrate(canBITRATE_50K); break;
        case 8: mid_can_set_baudrate(canBITRATE_10K); break;
        default: mid_can_set_baudrate(canBITRATE_500K); break;
    }
}

void Spark::on_comboBox_2_currentIndexChanged(int index)
{
    index = index;
}

void Spark::on_pushButton_2_clicked()
{
    if(mid_can_on_off() == TRUE)
    {
        ui->pushButton_9->setEnabled(FALSE);
        ui->pushButton_2->setText("Bus Off");
        uiTimer.start();
    }
    else
    {
        ui->pushButton_9->setEnabled(TRUE);
        ui->pushButton_2->setText("Bus On");
        uiTimer.stop();
    }
}

// setting apply
void Spark::on_pushButton_9_clicked()
{
    if(mid_can_apply_cfg() == TRUE)
    {
        ui->comboBox->setEnabled(FALSE);
        ui->comboBox_2->setEnabled(FALSE);
        ui->pushButton_8->setEnabled(FALSE);
        ui->pushButton_2->setEnabled(TRUE);
        ui->pushButton_9->setText("Disconnect");
    }
    else
    {
        ui->comboBox->setEnabled(TRUE);
        ui->comboBox_2->setEnabled(TRUE);
        ui->pushButton_8->setEnabled(TRUE);
        ui->pushButton_2->setEnabled(FALSE);
        ui->pushButton_9->setText("Connect");
    }
}

void Spark::on_pushButton_4_clicked()
{
    if(rx.window.table == NULL)
    {
        creat_dock_window(&rx.window);
    }

    if(rx.refresh == TRUE)
    {
        mid_can_bus_output(OFF);
        rx.refresh = FALSE;
        ui->pushButton_4->setText("Start");
    }
    else
    {
        mid_can_bus_output(ON);
        rx.refresh = TRUE;
        ui->pushButton_4->setText("Stop");
    }
}

void Spark::on_pushButton_clicked()
{
    mid_can_clear_stastic_info();
}

//rx parse ID
void Spark::on_lineEdit_22_textEdited(const QString &arg1)
{
    bool ok;

    rx_parse->setting.id = arg1.toInt(&ok, 16);
}

//rx parse Start Bit
void Spark::on_lineEdit_23_textEdited(const QString &arg1)
{
    bool ok;
    rx_parse->setting.start_bit = arg1.toInt(&ok, 10);
}

//rx parse Bits Length
void Spark::on_lineEdit_20_textEdited(const QString &arg1)
{
    bool ok;
    rx_parse->setting.bits_length = arg1.toInt(&ok, 10);
}

//rx parse factor
void Spark::on_lineEdit_19_textEdited(const QString &arg1)
{
    rx_parse->setting.factor = arg1.toFloat();
}

//rx parse offset
void Spark::on_lineEdit_21_textEdited(const QString &arg1)
{
    rx_parse->setting.offset = arg1.toFloat();
}

// rx parse moto order
void Spark::on_checkBox_8_clicked(bool checked)
{
    rx_parse->setting.bytes_order = (BYTES_ORDER_e)checked;
}

// rx parse msb order
void Spark::on_checkBox_7_clicked(bool checked)
{
    rx_parse->setting.bits_order = (enum BITS_ORDER_e)checked;
}

// rx parse start
void Spark::on_pushButton_7_clicked()
{
    struct mid_data_config_t *element;
    QStandardItem *newItem;

    if(rx_parse->setting.id == 0)
    {
        return;
    }
    if(rx_parse_window.table == NULL)
    {
        creat_dock_window(&rx_parse_window);
    }
    for(int index = 0; index < RX_PARSE_ITEMS; index ++)
    {
        newItem = new QStandardItem;
        newItem->setTextAlignment(Qt::AlignCenter);
        rx_parse_window.model->setItem(rx_parse->object, index, newItem);
    }
    list_insert(rx_parse->list, rx_parse->object);
    element = (struct mid_data_config_t *)list_find_entity(rx_parse->list, rx_parse->object);
    memcpy(element, &rx_parse->setting, sizeof(*element));
	element->flag = rx_parse->object;
    rx_parse->object ++;
}

// accept id
void Spark::on_lineEdit_12_textEdited(const QString &arg1)
{
    bool ok;

    rx.accept_id = arg1.toInt(&ok, 16);
}

// reject id
void Spark::on_lineEdit_13_textEdited(const QString &arg1)
{
    bool ok;

    rx.reject_id = arg1.toInt(&ok, 16);
}

void Spark::on_checkBox_clicked(bool checked)
{
    rx.fixed_posions = checked;
}

// rx dec display
void Spark::on_checkBox_2_clicked(bool checked)
{
    rx.dec_display = checked;
}

// Clear rx Window
void Spark::on_pushButton_10_clicked()
{
    if(rx.window.model == NULL)
    {
        return;
    }
    for(U32 index = 0; index < RX_LISTS_MAX; index ++)
    {
        if(rx.locate[index].mutex != (long)U32_INVALID_VALUE)
        {
            operate_table_line(&rx.window, -1, 0);
            rx.locate[index].line = 0;
            rx.locate[index].mutex = U32_INVALID_VALUE;
        }
        else
        {
            break;
        }
    }

}

// tx id
void Spark::on_lineEdit_textEdited(const QString &arg1)
{
    bool ok;

    tx.msg_config.id = arg1.toInt(&ok, 16);
}

// tx dlc
void Spark::on_lineEdit_2_textEdited(const QString &arg1)
{
    bool ok;

    tx.msg_config.dlc = arg1.toInt(&ok, 10);
}

// tx interval
void Spark::on_lineEdit_3_textEdited(const QString &arg1)
{
    bool ok;

    tx.msg_config.config_delta_time_stamp = arg1.toInt(&ok, 10);
}

// tx Byte0
void Spark::on_lineEdit_4_textEdited(const QString &arg1)
{
    bool ok;

    tx.msg_config.buf[0] = arg1.toInt(&ok, 16);
}

// tx Byte1
void Spark::on_lineEdit_5_textEdited(const QString &arg1)
{
    bool ok;

    tx.msg_config.buf[1] = arg1.toInt(&ok, 16);
}

// tx Byte2
void Spark::on_lineEdit_7_textEdited(const QString &arg1)
{
    bool ok;

    tx.msg_config.buf[2] = arg1.toInt(&ok, 16);
}

// tx Byte3
void Spark::on_lineEdit_6_textEdited(const QString &arg1)
{
    bool ok;

    tx.msg_config.buf[3] = arg1.toInt(&ok, 16);
}

// tx Byte4
void Spark::on_lineEdit_10_textEdited(const QString &arg1)
{
    bool ok;

    tx.msg_config.buf[4] = arg1.toInt(&ok, 16);
}

// tx Byte5
void Spark::on_lineEdit_8_textEdited(const QString &arg1)
{
    bool ok;

    tx.msg_config.buf[5] = arg1.toInt(&ok, 16);
}

// tx Byte6
void Spark::on_lineEdit_11_textEdited(const QString &arg1)
{
    bool ok;

    tx.msg_config.buf[6] = arg1.toInt(&ok, 16);
}

// tx Byte7
void Spark::on_lineEdit_9_textEdited(const QString &arg1)
{
    bool ok;

    tx.msg_config.buf[7] = arg1.toInt(&ok, 16);
}

// tx window clear
void Spark::on_pushButton_11_clicked()
{
    if(tx.msgs == 0)
    {
        return;
    }
    tx.list->destory_all_items(tx.list);
    do
    {
        tx.msgs --;
        operate_table_line(&tx.window, -1, tx.msgs);
    } while(tx.msgs != 0);
}

// tx delete id
void Spark::on_pushButton_12_clicked()
{
    struct list_item_t *rm_item = (struct list_item_t *)list_find_entity(tx.list, tx.msg_config.id);
    if(rm_item != NULL)
    {
        tx.list->destory_item(tx.list, tx.msg_config.id);
        tx.msgs --;
        operate_table_line(&tx.window, -1, tx.msgs);
    }
}

// tx start window
void Spark::on_pushButton_3_clicked()
{
    struct can_bus_frame_t *temp_element;
    QStandardItem *newItem;

    if(tx.msg_config.config_delta_time_stamp == 0 || tx.msg_config.id == 0)
    {
        return;
    }
    if(tx.list == 0)
    {
        return;
    }
    if(tx.window.table == NULL)
    {
        creat_dock_window(&tx.window);
    }
    if(list_find_entity(tx.list, tx.msg_config.id) != NULL)
    {
        return;
    }
    list_insert(tx.list, tx.msg_config.id);
    temp_element = (struct can_bus_frame_t *)list_find_entity(tx.list, tx.msg_config.id);

    if(temp_element == NULL)
    {
        return;
    }
    if(tx.msg_config.id > 0x7FF)
    {
        tx.msg_config.flag = canMSG_EXT;
    }
    else
    {
        tx.msg_config.flag = canMSG_STD;
    }
    tx.msg_config.new_data = 0;

    tx.msg_config.time_stamp = 0;
    tx.msg_config.squ = 0;
    memcpy(temp_element, &tx.msg_config, sizeof(*temp_element));

    for(int index = 0; index < TX_WINDOW_ITEMS; index ++)
    {
        newItem = new QStandardItem;
        newItem->setTextAlignment(Qt::AlignCenter);
        tx.window.model->setItem(tx.msgs, index, newItem);
    }
    tx.msgs ++;
}

// tx parse id
void Spark::on_lineEdit_14_textEdited(const QString &arg1)
{
    bool ok;

    tx.parse->setting.id = arg1.toInt(&ok, 16);
}

// tx parse start-bit
void Spark::on_lineEdit_15_textEdited(const QString &arg1)
{
    bool ok;

    tx.parse->setting.start_bit = arg1.toInt(&ok, 10);
}

// tx parse bits-length
void Spark::on_lineEdit_16_textEdited(const QString &arg1)
{
    bool ok;

    tx.parse->setting.bits_length = arg1.toInt(&ok, 10);
}

// tx parse factor
void Spark::on_lineEdit_17_textEdited(const QString &arg1)
{
    tx.parse->setting.factor = arg1.toFloat();
}

// tx parse offset
void Spark::on_lineEdit_18_textEdited(const QString &arg1)
{
    tx.parse->setting.offset = arg1.toFloat();
}

// tx parse value
void Spark::on_lineEdit_24_textEdited(const QString &arg1)
{
    tx.parse->setting.phy = arg1.toFloat();
}

// tx parse byte order 1:motorolar 0:intel
void Spark::on_checkBox_5_clicked(bool checked)
{
    tx.parse->setting.bytes_order = (BYTES_ORDER_e)checked;
}

// tx parse bit order 1:msb 0:lsb
void Spark::on_checkBox_6_clicked(bool checked)
{
    tx.parse->setting.bits_order = (enum BITS_ORDER_e)checked;
}

// tx parse start
void Spark::on_pushButton_6_clicked()
{
    struct can_bus_frame_t *temp_element;
    float temp_float;

    temp_element = (struct can_bus_frame_t *)list_find_entity(tx.list, tx.parse->setting.id);
    if(temp_element == NULL)
    {
        return;
    }
    temp_float = (tx.parse->setting.phy - tx.parse->setting.offset) / tx.parse->setting.factor;
    data_pack(temp_element->buf,
                temp_float + 0.5,
                tx.parse->setting.bytes_order,
                tx.parse->setting.bits_order,
                tx.parse->setting.start_bit,
                tx.parse->setting.bits_length);
}
