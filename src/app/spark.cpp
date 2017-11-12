//#include <QtWidgets>
#include <QMainWindow>
#include <QScrollBar>
#include <QTableWidget>
#include <QTableView>
#include <QHeaderView>

#include "spark.h"
#include "ui_spark.h"
#include "src/mid/mid_can.h"
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
    rx_parse = new struct data_parse_t;
    rx_parse->list = list_init(sizeof(struct mid_data_config_t));
    rx_parse->msg_lines = 0;
    rx_parse->id = 0;
    rx_parse->object = 0;
    rx_parse->start_bit = 0;
    rx_parse->bits_length = 0;
    rx_parse->factor = 0;
    rx_parse->offset = 0;

    fixed_posions = FALSE;
    rx_accept_id = 0;
    rx_reject_id = 0;
    on_pushButton_10_clicked();
    rx_dec_display = FALSE;
    mid_can_init(CAN_DEVICE_KVASER);
    uiTimer.setInterval(5);
    connect(&uiTimer, SIGNAL(timeout()), this, SLOT(main_window_update()));
}

Spark::~Spark()
{
    delete ui;
}

void Spark::update_rx_parse_line(const struct can_bus_frame_t *frame)
{
    QString string;
    struct list_element_t *list;
    struct mid_data_config_t *temp_element;

    if(rx_parse == NULL)
        return;
    for(list = rx_parse->list->head; list != NULL; list = list->next)
    {
        temp_element = (struct mid_data_config_t *)list->data;
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

            mid_data_can_calc(temp_element, (U8*)&frame->buf[0], frame->dlc);

            rx_parse_model->item(temp_element->flag, 6)->setText(string.number(temp_element->row, 'g', 6));

            rx_parse_model->item(temp_element->flag, 7)->setText(string.number(temp_element->phy, 'g', 6));

            rx_parse_model->item(temp_element->flag, 8)->setText(string.number(frame->time_stamp / 1000.0, 'g', 10));

            rx_parse_model->item(temp_element->flag, 9)->setText(string.number(frame->delta_time_stamp / 1000.0, 'g', 10));

            rx_parse_table->setRowHeight(temp_element->flag, 20);
        }
        else
        {

        }
    }
}

void Spark::update_receive_message_window()
{
    U16 msgs_index = 0;
    QStandardItem *newItem;
    const struct can_bus_frame_t *can_frame;
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
    QString head_name = "Rx Chn Identifer Flag DLC D0 D1 D2 D3 D4 D5 D6 D7 Time-Stamp Delta-Stamp";
    QStringList list = head_name.simplified().split(" ");

    rx_window_table = new QTableView(this);
    rx_window_table->verticalHeader()->hide();
    rx_window_model = new QStandardItemModel();
    rx_window_model->setHorizontalHeaderLabels(list);
    rx_window_table->setModel(rx_window_model);
    rx_window_table->setRowHeight(0, 20);
    rx_window_table->setColumnWidth(0, 70);    //Rx
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
    dock->setMinimumWidth(650);
    dock->setMinimumHeight(200);
    dock->setFloating(TRUE);
    init_rx_window_table();
    dock->setWidget(rx_window_table);

    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void Spark::init_rx_parse_table()
{
    QString head_name = "Name Identifer Start-Bit Bit-Length Factor Offset Row Phy Time-Stamp Delta-Stamp";
    QStringList list = head_name.simplified().split(" ");

    rx_parse_table = new QTableView(this);
    rx_parse_table->verticalHeader()->hide();
    rx_parse_model = new QStandardItemModel();
    rx_parse_model->setHorizontalHeaderLabels(list);
    rx_parse_table->setModel(rx_parse_model);
    rx_parse_table->setRowHeight(0, 20);
    rx_parse_table->setColumnWidth(0, 70);    //Name
    rx_parse_table->setColumnWidth(1, 80);    //Identifer
    rx_parse_table->setColumnWidth(2, 60);    //Start-Bit
    rx_parse_table->setColumnWidth(3, 70);    //Bit-Length
    rx_parse_table->setColumnWidth(4, 70);    //Factor
    rx_parse_table->setColumnWidth(5, 60);    //Offset
    rx_parse_table->setColumnWidth(6, 40);    //Row
    rx_parse_table->setColumnWidth(7, 80);    //Phy
    rx_parse_table->setColumnWidth(8, 80);    //Time-Stamp;
    rx_parse_table->setColumnWidth(9, 80);    //Delta-Stamp;
    rx_parse_table->show();
}

void Spark::creat_rx_parse_window()
{
    QDockWidget *dock = new QDockWidget(tr("Rx Parse Window"), this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setMinimumWidth(750);
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

//rx ID
void Spark::on_lineEdit_22_textEdited(const QString &arg1)
{
    bool ok;

    rx_parse->id = arg1.toInt(&ok, 16);
}

//rx Start Bit
void Spark::on_lineEdit_23_textEdited(const QString &arg1)
{
    bool ok;
    rx_parse->start_bit = arg1.toInt(&ok, 10);
}

//rx Bits Length
void Spark::on_lineEdit_20_textEdited(const QString &arg1)
{
    bool ok;
    rx_parse->bits_length = arg1.toInt(&ok, 10);
}

//rx factor
void Spark::on_lineEdit_19_textEdited(const QString &arg1)
{
    rx_parse->factor = arg1.toFloat();
}

//rx offset
void Spark::on_lineEdit_21_textEdited(const QString &arg1)
{
    rx_parse->offset = arg1.toFloat();
}

// rx parse start
void Spark::on_pushButton_7_clicked()
{
    struct mid_data_config_t *element;
    QStandardItem *newItem;

    if(rx_parse->id == 0)
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
    element->flag = rx_parse->object;
    element->id = rx_parse->id;
    element->start_bit = rx_parse->start_bit;
    element->bits_length = rx_parse->bits_length;
    element->factor = rx_parse->factor;
    element->offset = rx_parse->offset;
    element->row = 0;
    element->phy = 0;
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
