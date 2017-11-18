//#include <QtWidgets>
#include <QMainWindow>
#include <QScrollBar>
#include <QTableWidget>
#include <QTableView>
#include <QHeaderView>

#include "spark.h"
#include "ui_spark.h"
#include "src/mid/mid_can.h"
#include "src/mid/mid_bits.h"
#include "src/mid/mid_list.h"
#include "src/mid/mid_data.h"
#include "src/mid/comm_typedef.h"

Spark::Spark(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::Spark)
{
    ui->setupUi(this);
    rx_window_table = NULL;
    rx_window_model = NULL;
    rx_parse_table = NULL;
    rx_window_refresh = FALSE;

    // rx_parse init
    rx_parse = init_data_parse();
    // tx_parse init
    tx_parse = init_data_parse();
    memset(&tx_msg_config, 0, sizeof(tx_msg_config));
    tx_msg_config.config_delta_time_stamp = 100;
    tx_msg_config.dlc = MAX_DLC;
    tx_window_table = NULL;
    tx_window_model = NULL;
    tx_msgs_lines = 0;

    fixed_posions = FALSE;
    rx_accept_id = 0;
    rx_reject_id = 0;
    on_pushButton_10_clicked();
    rx_dec_display = FALSE;
    mid_can_init(CAN_DEVICE_KVASER);
    can_tx_list = mid_can_tx_list();
    uiTimer.setInterval(5);
    connect(&uiTimer, SIGNAL(timeout()), this, SLOT(main_window_update()));
}

Spark::~Spark()
{
    delete ui;
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
	obj->setting.bytes_order = 0;		/* Intel */
	obj->setting.bits_order = 0; 		/* lsb */

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
            rx_parse_model->item(temp_element->flag, 0)->setText(string);

            string = QString("%1").arg(frame->id, 8, 16, QLatin1Char('0')).toUpper();
            rx_parse_model->item(temp_element->flag, 1)->setText(string);

            string = QString("%1").arg(temp_element->start_bit, 2, 10, QLatin1Char('0')).toUpper();
            rx_parse_model->item(temp_element->flag, 2)->setText(string);

            string = QString("%1").arg(temp_element->bits_length, 2, 10, QLatin1Char('0')).toUpper();
            rx_parse_model->item(temp_element->flag, 3)->setText(string);

            rx_parse_model->item(temp_element->flag, 4)->setText(string.number(temp_element->factor, 'g', 6));

            rx_parse_model->item(temp_element->flag, 5)->setText(string.number(temp_element->offset, 'g', 6));

            string = QString("%1").arg(temp_element->bytes_order, 1, 10, QLatin1Char('0')).toUpper();
            rx_parse_model->item(temp_element->flag, 6)->setText(string);

            string = QString("%1").arg(temp_element->bits_order, 1, 10, QLatin1Char('0')).toUpper();
            rx_parse_model->item(temp_element->flag, 7)->setText(string);

            mid_data_can_calc(temp_element, (U8*)&frame->buf[0], temp_element->bytes_order, temp_element->bits_order, frame->dlc);

            rx_parse_model->item(temp_element->flag, 8)->setText(string.number(temp_element->row, 'g', 6));

            rx_parse_model->item(temp_element->flag, 9)->setText(string.number(temp_element->phy, 'g', 6));

            rx_parse_model->item(temp_element->flag, 10)->setText(string.number(frame->time_stamp / 1000.0, 'g', 10));

            rx_parse_model->item(temp_element->flag, 11)->setText(string.number(frame->delta_time_stamp / 1000.0, 'g', 10));

            rx_parse_table->setRowHeight(temp_element->flag, 20);
        }
        else
        {}
    }
}

void Spark::update_receive_message_window()
{
    U16 msgs_index = 0;
    QStandardItem *newItem;
    const struct can_bus_frame_t *can_frame;
    struct can_bus_frame_t *temp_element;
    QString string;
    U32 format = rx_dec_display == TRUE ? 10 : 16;

    if(fixed_posions == TRUE)
    {
        //rx_window_model->removeRow();
    }
    for(can_frame = mid_can_new_frame();
        rx_window_refresh == TRUE && can_frame != NULL;
        can_frame = mid_can_new_frame())
    {
        if(rx_accept_id != 0
                && can_frame->id != rx_accept_id)
        {
            continue;
        }
        if(rx_reject_id != 0
                && can_frame->id == rx_reject_id)
        {
            continue;
        }
        for(msgs_index = 0; msgs_index < RX_LISTS_MAX; msgs_index ++)
        {
            if(msgs[msgs_index].mutex_val == can_frame->id)
            {
                break;
            }
            if(msgs[msgs_index].mutex_val == U32_INVALID_VALUE)
            {
                msgs[msgs_index].mutex_val = can_frame->id;
                msgs[msgs_index].line_num = msgs_index;
                for(int index = 0; index < RX_WINDOW_ITEMS; index ++)
                {
                    newItem = new QStandardItem;
                    newItem->setTextAlignment(Qt::AlignCenter);
                    rx_window_model->setItem(msgs_index, index, newItem);
                }
                break;
            }
        }

        string = QString("%1").arg(msgs[msgs_index].line_num, 10, 10, QLatin1Char('0'));
        rx_window_model->item(msgs[msgs_index].line_num, 0)->setText(string);

        rx_window_model->item(msgs[msgs_index].line_num, 1)->setText(string.number(can_frame->chn, 10));

        string = QString("%1").arg(can_frame->id, 8, 16, QLatin1Char('0')).toUpper();
        rx_window_model->item(msgs[msgs_index].line_num, 2)->setText(string);

        rx_window_model->item(msgs[msgs_index].line_num, 3)->setText(string.number(can_frame->flag, 10));

        rx_window_model->item(msgs[msgs_index].line_num, 4)->setText(string.number(can_frame->dlc, 10));

        string = QString("%1").arg(can_frame->buf[0], 2, format, QLatin1Char('0')).toUpper();
        rx_window_model->item(msgs[msgs_index].line_num, 5)->setText(string);

        string = QString("%1").arg(can_frame->buf[1], 2, format, QLatin1Char('0')).toUpper();
        rx_window_model->item(msgs[msgs_index].line_num, 6)->setText(string);

        string = QString("%1").arg(can_frame->buf[2], 2, format, QLatin1Char('0')).toUpper();
        rx_window_model->item(msgs[msgs_index].line_num, 7)->setText(string);

        string = QString("%1").arg(can_frame->buf[3], 2, format, QLatin1Char('0')).toUpper();
        rx_window_model->item(msgs[msgs_index].line_num, 8)->setText(string);

        string = QString("%1").arg(can_frame->buf[4], 2, format, QLatin1Char('0')).toUpper();
        rx_window_model->item(msgs[msgs_index].line_num, 9)->setText(string);

        string = QString("%1").arg(can_frame->buf[5], 2, format, QLatin1Char('0')).toUpper();
        rx_window_model->item(msgs[msgs_index].line_num, 10)->setText(string);

        string = QString("%1").arg(can_frame->buf[6], 2, format, QLatin1Char('0')).toUpper();
        rx_window_model->item(msgs[msgs_index].line_num, 11)->setText(string);

        string = QString("%1").arg(can_frame->buf[7], 2, format, QLatin1Char('0')).toUpper();
        rx_window_model->item(msgs[msgs_index].line_num, 12)->setText(string);

        rx_window_model->item(msgs[msgs_index].line_num, 13)->setText(string.number(can_frame->time_stamp / 1000.0, 'g', 10));

        rx_window_model->item(msgs[msgs_index].line_num, 14)->setText(string.number(can_frame->delta_time_stamp / 1000.0, 'g', 10));
        rx_window_table->setRowHeight(msgs[msgs_index].line_num, 20);
        rx_window_table->currentIndex();
        update_rx_parse_line(can_frame);
    }
    msgs_index = 0;
    for(struct list_item_t *list = can_tx_list->ended.next; list != &can_tx_list->ended; list = list->next)
    {
        temp_element = (struct can_bus_frame_t *)list->data;

        {
            string = QString("%1").arg(msgs_index, 10, 10, QLatin1Char('0'));
            tx_window_model->item(msgs_index, 0)->setText(string);

            tx_window_model->item(msgs_index, 1)->setText(string.number(temp_element->chn, 10));

            string = QString("%1").arg(temp_element->id, 8, 16, QLatin1Char('0')).toUpper();
            tx_window_model->item(msgs_index, 2)->setText(string);

            tx_window_model->item(msgs_index, 3)->setText(string.number(temp_element->flag, 10));

            tx_window_model->item(msgs_index, 4)->setText(string.number(temp_element->dlc, 10));

            string = QString("%1").arg(temp_element->buf[0], 2, format, QLatin1Char('0')).toUpper();
            tx_window_model->item(msgs_index, 5)->setText(string);

            string = QString("%1").arg(temp_element->buf[1], 2, format, QLatin1Char('0')).toUpper();
            tx_window_model->item(msgs_index, 6)->setText(string);

            string = QString("%1").arg(temp_element->buf[2], 2, format, QLatin1Char('0')).toUpper();
            tx_window_model->item(msgs_index, 7)->setText(string);

            string = QString("%1").arg(temp_element->buf[3], 2, format, QLatin1Char('0')).toUpper();
            tx_window_model->item(msgs_index, 8)->setText(string);

            string = QString("%1").arg(temp_element->buf[4], 2, format, QLatin1Char('0')).toUpper();
            tx_window_model->item(msgs_index, 9)->setText(string);

            string = QString("%1").arg(temp_element->buf[5], 2, format, QLatin1Char('0')).toUpper();
            tx_window_model->item(msgs_index, 10)->setText(string);

            string = QString("%1").arg(temp_element->buf[6], 2, format, QLatin1Char('0')).toUpper();
            tx_window_model->item(msgs_index, 11)->setText(string);

            string = QString("%1").arg(temp_element->buf[7], 2, format, QLatin1Char('0')).toUpper();
            tx_window_model->item(msgs_index, 12)->setText(string);

            tx_window_model->item(msgs_index, 13)->setText(string.number(temp_element->time_stamp / 1000.0, 'g', 10));

            tx_window_model->item(msgs_index, 14)->setText(string.number(temp_element->delta_time_stamp / 1000.0, 'g', 10));
            tx_window_table->setRowHeight(msgs_index, 20);
            tx_window_table->currentIndex();
            msgs_index ++;
        }
    }
}

void Spark::main_window_update()
{
    const canBusStatistics *status = mid_can_process();
    unsigned long c = status->stdData + status->stdRemote + status->extData + status->extRemote;

    ui->lcdNumber->display(QString().setNum(c));
    ui->lcdNumber_2->display(QString().setNum(0));
    ui->lcdNumber_3->display(QString().setNum(status->errFrame));
    ui->progressBar->setValue(status->busLoad);
    update_receive_message_window();
}

void Spark::init_rx_window_table()
{
    QString ended_name = "Rx Chn Identifer Flag DLC D0 D1 D2 D3 D4 D5 D6 D7 Time-Stamp Delta-Stamp";
    QStringList list = ended_name.simplified().split(" ");

    rx_window_table = new QTableView(this);
    rx_window_table->verticalHeader()->hide();
    rx_window_model = new QStandardItemModel();
    rx_window_model->setHorizontalHeaderLabels(list);
    rx_window_table->setModel(rx_window_model);
    rx_window_table->setRowHeight(0, 20);
    rx_window_table->setColumnWidth(0, 90);    //Rx
    rx_window_table->setColumnWidth(1, 30);    //Chn
    rx_window_table->setColumnWidth(2, 70);    //Identifer
    rx_window_table->setColumnWidth(3, 30);    //Flag
    rx_window_table->setColumnWidth(4, 30);    //DLC
    rx_window_table->setColumnWidth(5, 30);    //D0
    rx_window_table->setColumnWidth(6, 30);    //D1
    rx_window_table->setColumnWidth(7, 30);    //D2
    rx_window_table->setColumnWidth(8, 30);    //D3
    rx_window_table->setColumnWidth(9, 30);    //D4
    rx_window_table->setColumnWidth(10, 30);   //D5
    rx_window_table->setColumnWidth(11, 30);   //D6
    rx_window_table->setColumnWidth(12, 30);   //D7
    rx_window_table->setColumnWidth(13, 80);	//Time-Stamp
    rx_window_table->setColumnWidth(14, 80);	//Delta-Stamp
    rx_window_table->show();
}

void Spark::creat_rx_dock_window()
{

    QDockWidget *dock = new QDockWidget(tr("Receive Messages"), this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setMinimumWidth(700);
    dock->setMinimumHeight(200);
    dock->setFloating(TRUE);
    init_rx_window_table();
    dock->setWidget(rx_window_table);

    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void Spark::init_rx_parse_table()
{
    QString ended_name = "Name Identifer Start-Bit Bit-Length Factor Offset Mot MSB Row Phy Time-Stamp Delta-Stamp";
    QStringList list = ended_name.simplified().split(" ");

    rx_parse_table = new QTableView(this);
    rx_parse_table->verticalHeader()->hide();
    rx_parse_model = new QStandardItemModel();
    rx_parse_model->setHorizontalHeaderLabels(list);
    rx_parse_table->setModel(rx_parse_model);
    rx_parse_table->setRowHeight(0, 20);
    rx_parse_table->setColumnWidth(0, 80);    //Name
    rx_parse_table->setColumnWidth(1, 60);    //Identifer
    rx_parse_table->setColumnWidth(2, 60);    //Start-Bit
    rx_parse_table->setColumnWidth(3, 65);    //Bit-Length
    rx_parse_table->setColumnWidth(4, 40);    //Factor
    rx_parse_table->setColumnWidth(5, 40);    //Offset
    rx_parse_table->setColumnWidth(6, 30);    //Byte Order
    rx_parse_table->setColumnWidth(7, 30);    //Bits Order
    rx_parse_table->setColumnWidth(8, 40);    //Row
    rx_parse_table->setColumnWidth(9, 80);    //Phy
    rx_parse_table->setColumnWidth(10, 80);    //Time-Stamp;
    rx_parse_table->setColumnWidth(11, 80);    //Delta-Stamp;
    rx_parse_table->show();
}

void Spark::creat_rx_parse_window()
{
    QDockWidget *dock = new QDockWidget(tr("Rx Parse Window"), this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setMinimumWidth(700);
    dock->setMinimumHeight(200);
    dock->setFloating(TRUE);
    init_rx_parse_table();
    dock->setWidget(rx_parse_table);

    addDockWidget(Qt::RightDockWidgetArea, dock);
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
        case 0: mid_can_set_baudrate(1000000); break;
        case 1: mid_can_set_baudrate(500000); break;
        case 2: mid_can_set_baudrate(250000); break;
        case 3: mid_can_set_baudrate(125000); break;
        case 4: mid_can_set_baudrate(100000); break;
        case 5: mid_can_set_baudrate(83000); break;
        case 6: mid_can_set_baudrate(62000); break;
        case 7: mid_can_set_baudrate(50000); break;
        case 8: mid_can_set_baudrate(10000); break;
        default: mid_can_set_baudrate(CAN_BAUDRATE_DEFAULT); break;
    }
}

void Spark::on_comboBox_2_currentIndexChanged(int index)
{

}

void Spark::on_pushButton_2_clicked()
{
    if(mid_can_on_off() == TRUE)
    {
        ui->comboBox->setEnabled(FALSE);
        ui->comboBox_2->setEnabled(FALSE);
        ui->pushButton_8->setEnabled(FALSE);
        ui->pushButton_9->setEnabled(FALSE);
        ui->pushButton_2->setText("Bus Off");
        uiTimer.start();
    }
    else
    {
        ui->comboBox->setEnabled(TRUE);
        ui->comboBox_2->setEnabled(TRUE);
        ui->pushButton_8->setEnabled(TRUE);
        ui->pushButton_9->setEnabled(TRUE);
        ui->pushButton_2->setText("Bus On");
        uiTimer.stop();
    }
}

// setting apply
void Spark::on_pushButton_9_clicked()
{
    mid_can_apply_cfg();
}

void Spark::on_pushButton_4_clicked()
{
    if(rx_window_table == NULL)
    {
        creat_rx_dock_window();
    }

    if(rx_window_refresh == TRUE)
    {
        rx_window_refresh = FALSE;
        ui->pushButton_4->setText("Start");
    }
    else
    {
        rx_window_refresh = TRUE;
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
    rx_parse->setting.bytes_order = checked;
}

// rx parse msb order
void Spark::on_checkBox_7_clicked(bool checked)
{
    rx_parse->setting.bits_order = checked;
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
    if(rx_parse_table == NULL)
    {
        creat_rx_parse_window();
    }
    for(int index = 0; index < RX_PARSE_ITEMS; index ++)
    {
        newItem = new QStandardItem;
        newItem->setTextAlignment(Qt::AlignCenter);
        rx_parse_model->setItem(rx_parse->object, index, newItem);
    }
    list_insert(rx_parse->list, rx_parse->object);
    element = (struct mid_data_config_t *)list_find_data(rx_parse->list, rx_parse->object);
    memcpy(element, &rx_parse->setting, sizeof(*element));
	element->flag = rx_parse->object;
    rx_parse->object ++;
}

// accept id
void Spark::on_lineEdit_12_textEdited(const QString &arg1)
{
    bool ok;

    rx_accept_id = arg1.toInt(&ok, 16);
}

// reject id
void Spark::on_lineEdit_13_textEdited(const QString &arg1)
{
    bool ok;

    rx_reject_id = arg1.toInt(&ok, 16);
}

void Spark::on_checkBox_clicked(bool checked)
{
    fixed_posions = checked;
}

// rx dec display
void Spark::on_checkBox_2_clicked(bool checked)
{
    rx_dec_display = checked;
}

// Clear rx Window
void Spark::on_pushButton_10_clicked()
{
    U32 msgs_index;

    if(rx_window_model != NULL)
    {
        for(msgs_index = 0; msgs_index < RX_LISTS_MAX; msgs_index ++)
        {
            if(msgs[msgs_index].mutex_val != U32_INVALID_VALUE)
            {
                rx_window_model->removeRow(msgs[msgs_index].line_num);
                msgs[msgs_index].mutex_val = U32_INVALID_VALUE;
            }
            else
            {
                break;
            }
        }
    }

    memset(msgs, U32_INVALID_VALUE, sizeof(msgs[0]) * RX_LISTS_MAX);
}

// tx id
void Spark::on_lineEdit_textEdited(const QString &arg1)
{
    bool ok;

    tx_msg_config.id = arg1.toInt(&ok, 16);
}

// tx dlc
void Spark::on_lineEdit_2_textEdited(const QString &arg1)
{
    bool ok;

    tx_msg_config.dlc = arg1.toInt(&ok, 10);
}

// tx interval
void Spark::on_lineEdit_3_textEdited(const QString &arg1)
{
    bool ok;

    tx_msg_config.config_delta_time_stamp = arg1.toInt(&ok, 10);
}

// tx Byte0
void Spark::on_lineEdit_4_textEdited(const QString &arg1)
{
    bool ok;

    tx_msg_config.buf[0] = arg1.toInt(&ok, 16);
}

// tx Byte1
void Spark::on_lineEdit_5_textEdited(const QString &arg1)
{
    bool ok;

    tx_msg_config.buf[1] = arg1.toInt(&ok, 16);
}

// tx Byte2
void Spark::on_lineEdit_7_textEdited(const QString &arg1)
{
    bool ok;

    tx_msg_config.buf[2] = arg1.toInt(&ok, 16);
}

// tx Byte3
void Spark::on_lineEdit_6_textEdited(const QString &arg1)
{
    bool ok;

    tx_msg_config.buf[3] = arg1.toInt(&ok, 16);
}

// tx Byte4
void Spark::on_lineEdit_10_textEdited(const QString &arg1)
{
    bool ok;

    tx_msg_config.buf[4] = arg1.toInt(&ok, 16);
}

// tx Byte5
void Spark::on_lineEdit_8_textEdited(const QString &arg1)
{
    bool ok;

    tx_msg_config.buf[5] = arg1.toInt(&ok, 16);
}

// tx Byte6
void Spark::on_lineEdit_11_textEdited(const QString &arg1)
{
    bool ok;

    tx_msg_config.buf[6] = arg1.toInt(&ok, 16);
}

// tx Byte7
void Spark::on_lineEdit_9_textEdited(const QString &arg1)
{
    bool ok;

    tx_msg_config.buf[7] = arg1.toInt(&ok, 16);
}

void Spark::init_tx_table()
{
    QString ended_name = "Tx Chn Identifer Flag DLC D0 D1 D2 D3 D4 D5 D6 D7 Time-Stamp Delta-Stamp";
    QStringList list = ended_name.simplified().split(" ");

    tx_window_table = new QTableView(this);
    tx_window_table->verticalHeader()->hide();
    tx_window_model = new QStandardItemModel();
    tx_window_model->setHorizontalHeaderLabels(list);
    tx_window_table->setModel(tx_window_model);
    tx_window_table->setRowHeight(0, 20);
    tx_window_table->setColumnWidth(0, 90);    //Tx
    tx_window_table->setColumnWidth(1, 30);    //Chn
    tx_window_table->setColumnWidth(2, 70);    //Identifer
    tx_window_table->setColumnWidth(3, 30);    //Flag
    tx_window_table->setColumnWidth(4, 30);    //DLC
    tx_window_table->setColumnWidth(5, 30);    //D0
    tx_window_table->setColumnWidth(6, 30);    //D1
    tx_window_table->setColumnWidth(7, 30);    //D2
    tx_window_table->setColumnWidth(8, 30);    //D3
    tx_window_table->setColumnWidth(9, 30);    //D4
    tx_window_table->setColumnWidth(10, 30);   //D5
    tx_window_table->setColumnWidth(11, 30);   //D6
    tx_window_table->setColumnWidth(12, 30);   //D7
    tx_window_table->setColumnWidth(13, 80);	//Time-Stamp
    tx_window_table->setColumnWidth(14, 80);	//Delta-Stamp
    tx_window_table->show();
}

void Spark::creat_tx_window()
{
    QDockWidget *dock = new QDockWidget(tr("Tx Window"), this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setMinimumWidth(700);
    dock->setMinimumHeight(200);
    dock->setFloating(TRUE);
    init_tx_table();
    dock->setWidget(tx_window_table);

    addDockWidget(Qt::RightDockWidgetArea, dock);
}

// tx window clear
void Spark::on_pushButton_11_clicked()
{
    QStandardItem *newItem;

    can_tx_list->destory_all_items(can_tx_list);

    for(; tx_msgs_lines != 0; tx_msgs_lines --)
    {
        tx_window_model->removeRow(tx_msgs_lines - 1);
    }
}

// tx delete id
void Spark::on_pushButton_12_clicked()
{
    struct list_item_t *rm_item = (struct list_item_t *)list_find_data(can_tx_list, tx_msg_config.id);
    if(rm_item != NULL)
    {
        can_tx_list->destory_item(can_tx_list, tx_msg_config.id);
        tx_msgs_lines --;
        tx_window_model->removeRow(tx_msgs_lines);
    }
}

// tx start window
void Spark::on_pushButton_3_clicked()
{
    struct can_bus_frame_t *temp_element;
    QStandardItem *newItem;

    if(tx_msg_config.config_delta_time_stamp == 0 || tx_msg_config.id == 0)
    {
        return;
    }
    if(can_tx_list == 0)
    {
        return;
    }
    if(tx_window_table == NULL)
    {
        creat_tx_window();
    }
    if(list_find_data(can_tx_list, tx_msg_config.id) != NULL)
    {
        return;
    }
    list_insert(can_tx_list, tx_msg_config.id);
    temp_element = (struct can_bus_frame_t *)list_find_data(can_tx_list, tx_msg_config.id);

    if(temp_element == NULL)
    {
        return;
    }
    if(tx_msg_config.id > 0x7FF)
    {
        tx_msg_config.flag = canMSG_EXT;
    }
    else
    {
        tx_msg_config.flag = canMSG_STD;
    }
    tx_msg_config.new_data = 0;

    tx_msg_config.time_stamp = 0;
    tx_msg_config.squ = 0;
    memcpy(temp_element, &tx_msg_config, sizeof(*temp_element));

    for(int index = 0; index < TX_WINDOW_ITEMS; index ++)
    {
        newItem = new QStandardItem;
        newItem->setTextAlignment(Qt::AlignCenter);
        tx_window_model->setItem(tx_msgs_lines, index, newItem);
    }
    tx_msgs_lines ++;
}

// tx parse id
void Spark::on_lineEdit_14_textEdited(const QString &arg1)
{
    bool ok;

    tx_parse->setting.id = arg1.toInt(&ok, 16);
}

// tx parse start-bit
void Spark::on_lineEdit_15_textEdited(const QString &arg1)
{
    bool ok;

    tx_parse->setting.start_bit = arg1.toInt(&ok, 10);
}

// tx parse bits-length
void Spark::on_lineEdit_16_textEdited(const QString &arg1)
{
    bool ok;

    tx_parse->setting.bits_length = arg1.toInt(&ok, 10);
}

// tx parse factor
void Spark::on_lineEdit_17_textEdited(const QString &arg1)
{
    tx_parse->setting.factor = arg1.toFloat();
}

// tx parse offset
void Spark::on_lineEdit_18_textEdited(const QString &arg1)
{
    tx_parse->setting.offset = arg1.toFloat();
}

// tx parse value
void Spark::on_lineEdit_24_textEdited(const QString &arg1)
{
    tx_parse->setting.phy = arg1.toFloat();
}

// tx parse byte order 1:motorolar 0:intel
void Spark::on_checkBox_5_clicked(bool checked)
{
    tx_parse->setting.bytes_order = checked;
}

// tx parse bit order 1:msb 0:lsb
void Spark::on_checkBox_6_clicked(bool checked)
{
    tx_parse->setting.bits_order = checked;
}

// tx parse start
void Spark::on_pushButton_6_clicked()
{
    struct can_bus_frame_t *temp_element;
    float temp_float;

    temp_element = (struct can_bus_frame_t *)list_find_data(can_tx_list, tx_parse->setting.id);
    if(temp_element == NULL)
    {
        return;
    }
    temp_float = (tx_parse->setting.phy - tx_parse->setting.offset) / tx_parse->setting.factor;
    data_pack(temp_element->buf,
                temp_float + 0.5,
				tx_parse->setting.bytes_order,
				tx_parse->setting.bits_order,
				tx_parse->setting.start_bit,
				tx_parse->setting.bits_length);
}
