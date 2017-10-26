#ifndef SPARK_H
#define SPARK_H

#include <QWidget>

namespace Ui {
class Spark;
}

class Spark : public QWidget
{
    Q_OBJECT

public:
    explicit Spark(QWidget *parent = 0);
    ~Spark();

private:
    Ui::Spark *ui;
};

#endif // SPARK_H
