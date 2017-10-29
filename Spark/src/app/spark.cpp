//#include <QtWidgets>
#include <QMainWindow>
#include "spark.h"
#include "ui_spark.h"
#include "src/mid/mid_can.h"
#include "src/mid/comm_typedef.h"

Spark::Spark(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::Spark)
{
    ui->setupUi(this);
    rx_message = NULL;
    refresh_rx_window = FALSE;
    mid_can_init(CAN_DEVICE_KVASER);
    uiTimer.setInterval(5);
    connect(&uiTimer, SIGNAL(timeout()), this, SLOT(main_window_update()));
}

Spark::~Spark()
{
    delete ui;
}

void Spark::main_window_update()
{
    const struct can_bus_frame_t *can_frame;
    QString can_frame_string;
    const canBusStatistics *status = mid_can_process();
    unsigned long c = status->stdData + status->stdRemote + status->extData + status->extRemote;

    ui->lcdNumber->display(QString().setNum(c));
    ui->lcdNumber_2->display(QString().setNum(0));
    ui->lcdNumber_3->display(QString().setNum(status->errFrame));
    ui->progressBar->setValue(status->busLoad);


    for(can_frame = mid_can_new_frame(); refresh_rx_window == TRUE && rx_message != NULL && can_frame != NULL; can_frame = mid_can_new_frame())
    {
        can_frame_string.append(QString().sprintf("    %02d", can_frame->chn));
        can_frame_string.append(QString().sprintf("    %08X", can_frame->id));
        can_frame_string.append(QString().sprintf("    %02X", can_frame->flag));
        can_frame_string.append(QString().sprintf("    %02X", can_frame->dlc));
        if(can_frame->dlc > 0) can_frame_string.append(QString().sprintf("  %02X", can_frame->buf[0]));
        if(can_frame->dlc > 1) can_frame_string.append(QString().sprintf("  %02X", can_frame->buf[1]));
        if(can_frame->dlc > 2) can_frame_string.append(QString().sprintf("  %02X", can_frame->buf[2]));
        if(can_frame->dlc > 3) can_frame_string.append(QString().sprintf("  %02X", can_frame->buf[3]));
        if(can_frame->dlc > 4) can_frame_string.append(QString().sprintf("  %02X", can_frame->buf[4]));
        if(can_frame->dlc > 5) can_frame_string.append(QString().sprintf("  %02X", can_frame->buf[5]));
        if(can_frame->dlc > 6) can_frame_string.append(QString().sprintf("  %02X", can_frame->buf[6]));
        if(can_frame->dlc > 7) can_frame_string.append(QString().sprintf("  %02X", can_frame->buf[7]));
        can_frame_string.append(QString().sprintf("   %0.3f\r", (float)can_frame->time_stamp / 1000.0));
        rx_message->append(can_frame_string);
    }
}

void Spark::creat_rx_dock_window()
{
    QDockWidget *dock = new QDockWidget(tr("Rx  Chn       Identifer     Flag      DLC  D0   D1  D2   D3  D4   D5  D6   D7      Time"), this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setMinimumWidth(500);
    dock->setMinimumHeight(600);
    dock->setFloating(TRUE);

    rx_message = new QTextEdit(dock);

    dock->setWidget(rx_message);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    //connect(rx_message, SIGNAL(currentTextChanged(QString)),
      //      this, SLOT(insertCustomer(QString)));
}

void Spark::insertCustomer(const QString &customer)
{

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
     << QApplication::translate("MainWindow", "1M", 0)
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
    if(rx_message == NULL)
    {
        creat_rx_dock_window();
    }

    if(refresh_rx_window == TRUE)
    {
        refresh_rx_window = FALSE;
        ui->pushButton_4->setText("Start");
    }
    else
    {
        refresh_rx_window = TRUE;
        ui->pushButton_4->setText("Stop");
    }
}

void Spark::on_pushButton_clicked()
{
    mid_can_clear_stastic_info();
}
