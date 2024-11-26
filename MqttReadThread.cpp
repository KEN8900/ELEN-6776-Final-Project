//
// Created by ken on 8/2/24.
//

#include <QTimer>

#include "MqttReadThread.h"

MqttReadThread::MqttReadThread(QObject *parent,
                               const QString &brokerIp, int brokerPort) : MqttThreadConnector(parent, brokerIp, brokerPort) {

}

MqttReadThread::~MqttReadThread() = default;

void MqttReadThread::onMessageReceived(const QByteArray &message, const QMqttTopicName &url) {
    //TODO: write the message into DB.
    qDebug() << "=================> MqttReadThread(" << getThreadId() << ")::messageReceived====> msg = " << message
             << ", topic = " << url
             << "\n";
}

void MqttReadThread::getSubscribeInfo(QString& url, quint8& qos) {
    //TODO: get the subscribe info from DB
    url = "test/topic";
    qos = 0;
}
