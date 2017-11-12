#include "spark.h"
#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Spark w;
    w.show();

    return a.exec();
}

