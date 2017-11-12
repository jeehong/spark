#-------------------------------------------------
#
# Project created by QtCreator 2017-10-26T21:43:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Spark
TEMPLATE = app


SOURCES += src/app/main.cpp\
            src/app/spark.cpp \
            src/mid/mid_can.cpp \
            src/mid/mid_bits.cpp \
            src/mid/mid_list.cpp \
            src/mid/mid_data.cpp

HEADERS  += src/app/spark.h \
            src/driver/canlib.h \
            src/driver/canevt.h \
            src/driver/canstat.h \
            src/driver/obsolete.h \
            src/driver/predef.h \
            src/mid/mid_can.h \
            src/mid/comm_typedef.h \
            src/mid/mid_bits.h \
            src/mid/mid_list.h \
            src/mid/mid_data.h

LIBS += canlib32.dll

message("Please copy 'canlib32.dll' from source to build directory")

DISTFILES += src/driver/canlib32.dll

RC_FILE = src/ui/ico.rc

FORMS    += src/ui/spark.ui
