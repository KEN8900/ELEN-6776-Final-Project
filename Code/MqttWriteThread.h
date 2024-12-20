#pragma once
#include "MqttThreadConnector.h"

class MqttWriteThread : public MqttThreadConnector {
    Q_OBJECT
public:
    explicit MqttWriteThread(QObject* parent,
                            const QString &brokerIp,
                            int brokerPort);
    ~MqttWriteThread() override;

protected:
    void getPublishInfo(QString& url, quint8& qos, QByteArray& msg) override;

    void onDataSendStart(qint32 id, const QString& url, const QByteArray& msg) override;
    void onDataSentFinish(qint32 id) override;

};

