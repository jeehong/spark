#ifndef SPARK_H
#define SPARK_H

#include <QtWidgets>
#include <QWidget>
#include <QTimer>

namespace Ui {
class Spark;
}

class Spark : public QMainWindow
{
    Q_OBJECT

public:
    explicit Spark(QMainWindow *parent = 0);
    ~Spark();
    void button_refresh_press();
    void creat_rx_dock_window();
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
    void insertCustomer(const QString &customer);

private:
    QTextEdit *rx_message;
    bool refresh_rx_window;
    Ui::Spark *ui;
    QTimer uiTimer;
};

#endif // SPARK_H
