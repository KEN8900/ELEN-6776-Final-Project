#include <QtCore>
//#include <iostream>
//#include <unistd.h>

#include "MqttReadThread.h"
#include "MqttWriteThread.h"
#include "MqttThreadConnector.h"
#include "MqttThreadPool.h"

using namespace std;

int main(int argc, char *argv[]) {

     QCoreApplication app(argc, argv);

     MqttThreadPool mtp(2, 2, "127.0.0.1", 1883);
     mtp.simpleTest();

     return QCoreApplication::exec();
}
