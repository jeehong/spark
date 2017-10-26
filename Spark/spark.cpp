#include "spark.h"
#include "ui_spark.h"

Spark::Spark(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Spark)
{
    ui->setupUi(this);
}

Spark::~Spark()
{
    delete ui;
}
