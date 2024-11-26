//
// Created by ken on 8/2/24.
//

#include <QTimer>
#include <sstream>

#include "MqttWriteThread.h"

MqttWriteThread::MqttWriteThread(QObject *parent,
                               const QString &brokerIp,
                               int brokerPort) : MqttThreadConnector(parent, brokerIp, brokerPort) {

}

MqttWriteThread::~MqttWriteThread() = default;

void MqttWriteThread::getPublishInfo(QString& url, quint8& qos, QByteArray& msg) {
    //TODO: use REST API to access database: tDevSendData table
    //to get records with state=SENDING to publish

    //HashTable: [url, message] : id (-1)

    url = "test/topic";
    qos = 0;
    msg = QByteArray("Hello World!");
}

void MqttWriteThread::onDataSendStart(qint32 id, const QString& url, const QByteArray& msg)  {

//    add HashTable: [url, message] : id (-1)-> id(12) , then update tDevSendData table

}
void MqttWriteThread::onDataSentFinish(qint32 id) {
    //    add HashTable: [url, message] : id (-1)-> 12 , then update tDevSendData record to state="ARRIVED"

}


