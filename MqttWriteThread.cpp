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
    url = "test/topic";
    qos = 0;
    msg = QByteArray("Hello World!");
}

void MqttWriteThread::onDataSendStart(qint32 id, const QString& url, const QByteArray& msg)  {

}

void MqttWriteThread::onDataSentFinish(qint32 id) {

}


