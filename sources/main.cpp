#include "worker_object.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<QMap <int, float> >("QMap <int, float>");
    QCoreApplication a(argc, argv);
    WorkerObject workerObject;
    return a.exec();
}
