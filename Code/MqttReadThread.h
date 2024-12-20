#pragma once
#include "MqttThreadConnector.h"

class MqttReadThread : public MqttThreadConnector {
    Q_OBJECT
public:
    explicit MqttReadThread(QObject* parent,
                            const QString &brokerIp,
                            int brokerPort);
    ~MqttReadThread() override;

protected:
    //subclass must override this virtual method.
    void onMessageReceived(const QByteArray &message, const QMqttTopicName &url) override;

    void getSubscribeInfo(QString& url, quint8& qos) override;

};

