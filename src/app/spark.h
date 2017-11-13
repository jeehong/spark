#ifndef SPARK_H
#define SPARK_H

#include <QtWidgets>
#include <QWidget>
#include <QTimer>
#include <QTableView>
#include <QStandardItemModel>

#include "src/mid/comm_typedef.h"
#include "src/mid/mid_can.h"
#include "src/mid/mid_list.h"

#define RX_LISTS_MAX        (500)
#define RX_WINDOW_ITEMS     (15)
#define TX_WINDOW_ITEMS     (15)
#define RX_PARSE_ITEMS      (10)

namespace Ui {
class Spark;
}

struct data_parse_t
{
    U32 msg_lines;
    U8 object;
    U32 id;
    U8 start_bit;
    U8 bits_length;
    float factor;
    float offset;
    float value;
    struct list_item_t *list;
};

class Spark : public QMainWindow
{
    Q_OBJECT

public:
    explicit Spark(QMainWindow *parent = 0);
    ~Spark();
    void button_refresh_press();
    void creat_rx_dock_window();
    void creat_rx_parse_window();
    void init_rx_window_table();
    void init_rx_parse_table();
private slots:
    void main_window_update();
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_8_clicked();
    void on_comboBox_2_activated(int index);
    void on_comboBox_2_currentIndexChanged(int index);
    void on_pushButton_2_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_clicked();
	void on_lineEdit_19_textEdited(const QString &arg1);
    void on_lineEdit_20_textEdited(const QString &arg1);
    void on_lineEdit_21_textEdited(const QString &arg1);
    void on_lineEdit_22_textEdited(const QString &arg1);
    void on_lineEdit_23_textEdited(const QString &arg1);
    void on_pushButton_7_clicked();

    void on_lineEdit_12_textEdited(const QString &arg1);

    void on_lineEdit_13_textEdited(const QString &arg1);

    void on_checkBox_clicked(bool checked);

    void on_checkBox_2_clicked(bool checked);

    void on_pushButton_10_clicked();

    void on_lineEdit_textEdited(const QString &arg1);

    void on_lineEdit_2_textEdited(const QString &arg1);

    void on_lineEdit_3_textEdited(const QString &arg1);

    void on_lineEdit_4_textEdited(const QString &arg1);

    void on_lineEdit_5_textEdited(const QString &arg1);

    void on_lineEdit_7_textEdited(const QString &arg1);

    void on_lineEdit_6_textEdited(const QString &arg1);

    void on_lineEdit_10_textEdited(const QString &arg1);

    void on_lineEdit_8_textEdited(const QString &arg1);

    void on_lineEdit_11_textEdited(const QString &arg1);

    void on_lineEdit_9_textEdited(const QString &arg1);

    void on_pushButton_3_clicked();

    void on_lineEdit_14_textEdited(const QString &arg1);

    void on_lineEdit_15_textEdited(const QString &arg1);

    void on_lineEdit_16_textEdited(const QString &arg1);

    void on_lineEdit_17_textEdited(const QString &arg1);

    void on_lineEdit_18_textEdited(const QString &arg1);

    void on_pushButton_6_clicked();

    void on_lineEdit_24_textEdited(const QString &arg1);

private:
    void creat_tx_window();
    void init_tx_table();
    void update_receive_message_window();
    void update_rx_parse_line(const struct can_bus_frame_t *frame);
    struct data_parse_t *init_data_parse();
    // rx parse
    QTableView *rx_parse_table;
    QStandardItemModel *rx_parse_model;
    struct data_parse_t *rx_parse;

    // rx control
    QTableView *rx_window_table;
    QStandardItemModel *rx_window_model;
    bool rx_window_refresh;
    int rx_accept_id;
    int rx_reject_id;
    bool rx_dec_display;
    bool fixed_posions;
    struct {
        U32 mutex_val;
        U32 line_num;
    } msgs[RX_LISTS_MAX];

    // tx control
    struct data_parse_t *tx_parse;
    struct can_bus_frame_t tx_msg_config;
    QTableView *tx_window_table;
    QStandardItemModel *tx_window_model;
    U32 tx_msgs_lines;
    struct list_item_t *can_tx_list;

    Ui::Spark *ui;
    QTimer uiTimer;
};

#endif // SPARK_H
