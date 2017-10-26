#include "spark.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Spark w;
    w.show();

    return a.exec();
}
