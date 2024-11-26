#include <QtCore>
//#include <iostream>
//#include <unistd.h>

#include <iostream>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/mib_api.h>

#include "MqttReadThread.h"
#include "MqttWriteThread.h"
#include "MqttThreadConnector.h"
#include "MqttThreadPool.h"
#include "SnmpGet.h"
//#include "SnmpReadThread.h"
//#include "SnmpWriteThread.h"
#include "SnmpThreadConnector.h"
//#include "RestRequest.h"

using namespace std;

//bool needToRun() {
//check DB                                      | --- cron create a new process per 5 min
//  taskName           | processId | livingTime | lastVisitTime |  visitorId | visitCount | isUserStop
//  ibmscppbackend        1234       14:01:14      14:02:15            1235       5             F   * no need to Run
//  ibmscppbackend        1234       14:01:14      14:07:15            1236       6             F
//  ibmscppbackend        1234       14:07:15      14:07:15            1236       6   *         F   * need to Run
//  ibmscppbackend        1236       14:12:15      14:12:15            1237       7   *         F
//  ibmscppbackend        1236       14:12:15      14:17:15            1238       8             T

// https://stackoverflow.com/questions/19409940/how-to-get-output-system-command-in-qt
// QProcess process;
//process.start(/* command line stuff */);
//process.waitForFinished(-1); // will wait forever until finished
//
//QString stdout = process.readAllStandardOutput();
//QString stderr = process.readAllStandardError();

// https://doc.qt.io/qt-5/qprocess.html#details

// ken@Ken-ROG-Flow-X13-GV302XV-GV302XV:~$ kill -9 11829
//bash: kill: (11829) - No such process
//ken@Ken-ROG-Flow-X13-GV302XV-GV302XV:~$ kill -9 11828
//ken@Ken-ROG-Flow-X13-GV302XV-GV302XV:~$ ps -ef | grep 11828
//ken        23520   21659  0 18:09 pts/1    00:00:00 grep --color=auto 11828

// ps -ef | grep ibmscppbackend.exe
// kill -9
//}

// int main(int argc, char *argv[]) {
//
//     QCoreApplication app(argc, argv);
//
//     MqttThreadPool mtp(2, 2, "127.0.0.1", 1883);
//     mtp.simpleTest();
//
//     return QCoreApplication::exec();
// }

//int main() {
//    SnmpGet snmp("leo", "172.31.226.160");
//    snmp.run();
//    return 0;
//}
