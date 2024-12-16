#include <QTimer>

#include "MqttReadThread.h"

MqttReadThread::MqttReadThread(QObject *parent,
                               const QString &brokerIp, int brokerPort) : MqttThreadConnector(parent, brokerIp, brokerPort) {

}

MqttReadThread::~MqttReadThread() = default;

void MqttReadThread::onMessageReceived(const QByteArray &message, const QMqttTopicName &url) {
    qDebug() << "==========> MqttReadThread(" << getThreadId() << ")::messageReceived ==========> msg = " << message
             << ", topic = " << url
             << "\n";
}

void MqttReadThread::getSubscribeInfo(QString& url, quint8& qos) {
    url = "test/topic";
    qos = 0;
}
