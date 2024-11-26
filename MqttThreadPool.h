//
// Created by ken on 8/16/24.
//

#pragma once

class MqttThreadPool {

public:
    explicit MqttThreadPool(int numReadThread,
                            int numWriteThread,
                            const QString &brokerIp,
                            int brokerPort);

    ~MqttThreadPool();

    void simpleTest();

private:
    QList<MqttReadThread *> readThreadList;
    QList<MqttWriteThread *> writeThreadList;

    int numReadThread;
    int numWriteThread;
    QString brokerIp;
    int brokerPort;

    Qt::HANDLE mainThreadId;

    void createThread();

    void killDeadThread();
    void killAllThread();
};
