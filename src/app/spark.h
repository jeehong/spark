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
#include "src/mid/mid_data.h"
#include "src/app/spark_thread.h"

#define RX_LISTS_MAX        (500)
#define RX_WINDOW_ITEMS     (15)
#define TX_WINDOW_ITEMS     (15)
#define RX_PARSE_ITEMS      (12)

namespace Ui {
class Spark;
}

struct data_parse_t
{
    U32 msg_lines;
    U8 object;
	struct mid_data_config_t setting;
    struct list_t *list;
};

struct tabel_dock_t
{
    QTableView *table;
    QStandardItemModel *model;
    QString window_name;
    int window_width;
    int window_height;
    QString table_head;
    int table_columns;
    int column_width[30];
};

struct tx_setting_t
{
    struct tabel_dock_t window;
    struct data_parse_t *parse;
    struct can_bus_frame_t msg_config;
    U32 msgs;
    struct list_t *list;
};

struct rx_setting_t
{
    struct tabel_dock_t window;
    bool refresh;
    int accept_id;
    int reject_id;
    bool dec_display;
    bool fixed_posions;
    struct {
        long mutex;
        U32 line;
    } locate[RX_LISTS_MAX];
};

class Spark_Thread : public QThread
{
  Q_OBJECT

public:
    Spark_Thread(QObject *parent = 0);
    void run();

signals:
    void threadSignal();
};


class Spark : public QMainWindow
{
    Q_OBJECT

public:
    explicit Spark(QMainWindow *parent = 0);

    ~Spark();
private slots:
    void data_process_slot();
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
    void on_checkBox_5_clicked(bool checked);
    void on_checkBox_6_clicked(bool checked);
    void on_checkBox_8_clicked(bool checked);
    void on_checkBox_7_clicked(bool checked);
    void on_pushButton_11_clicked(); 
    void on_pushButton_12_clicked();

private:
    void init_dock_window(struct tabel_dock_t *config,
                                QString window_name,
                                int window_width,
                                int window_height,
                                QString table_head,
                                int table_columns,
                                int *column_width);
    void creat_dock_window(struct tabel_dock_t *config);
    void init_dock_table(struct tabel_dock_t *config);
    void operate_table_line(struct tabel_dock_t *window, int operate, U32 lines);
    void update_receive_message_window();
    void update_rx_parse_line(const struct can_bus_frame_t *frame);

    struct data_parse_t *init_data_parse();
    // rx parse
	struct tabel_dock_t rx_parse_window;
    struct data_parse_t *rx_parse;
	U8 load_pool;

    // rx control
    struct rx_setting_t rx;

    // tx control
    struct tx_setting_t tx;


    Spark_Thread *data_thread;
    Ui::Spark *ui;
    QTimer uiTimer;
};

#endif // SPARK_H
