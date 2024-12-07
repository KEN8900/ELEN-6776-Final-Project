//
// Created by ken on 8/14/24.
//

#pragma once
#include <QThread>
#include <QMutex>
#include <QtMqtt/QMqttClient>
#include <QTimer>

class MqttThreadConnector : public QThread {
    Q_OBJECT
public:
    explicit MqttThreadConnector(QObject* parent,
                                 const QString &brokerIp,
                                 int brokerPort);
    ~MqttThreadConnector() override;
    [[nodiscard]] QString getThreadId() const;

    // a function that lets the outside know whether this is the specific thread they want to look for
    [[nodiscard]] bool specificThread(const QString& strThreadId) const;

    // mainThread makes the sub-thread to run. Runtime can modify here. Unit: millisecond.
    void reqStart(int _workTimeMS=500);

    // mainThread makes the sub-thread to stop. Stop time can modify here. Unit: millisecond.
    void reqStop(int stopTimeMS=5000);

    // a function that lets the outside check the working status
    bool isWorking(int maxBusyTimeMS=10000);

    // To print out some useful information
    [[nodiscard]] QString getInfo() const;

    enum connectionState{
        closed=0,
        connecting,
        firstConnect,
        threadExit
    };
    Q_ENUM(connectionState)

protected:
    //subclass must override this virtual method.
    virtual void onMessageReceived(const QByteArray& message, const QMqttTopicName& url); //Read

    virtual void getSubscribeInfo(QString& url, quint8& qos);
    virtual void getPublishInfo(QString& url, quint8& qos, QByteArray& msg);

    virtual void onDataSendStart(qint32 id, const QString& url, const QByteArray& msg);
    virtual void onDataSentFinish(qint32 id);

    // Read/Write: client.subscribe(url, qos); client.publish(url, message, qos, false);
    // Write: quint32 id = client.publish(url, message, qos, false);

private:
    connectionState state;

    void run() override; // sub-thread entry code, called by Operating System
    void installEventHandle(QMqttClient &client);
    void install_errorChanged(QMqttClient& client);
    void install_stateChanged(QMqttClient& client);
    void install_messageReceived(QMqttClient& client);
    void install_messageStatusChanged(QMqttClient& client);

    void routineWork();     //called by timer
    void doSubscribe();
    void doPublish();

    void setLivingTime();                //Critical data; need mutex
    QDateTime getLivingTime();           //critical data; need mutex
    bool threadNeedExit();
    void setThreadExit();

    QString brokerIp;
    int brokerPort;

//    Qt::HANDLE threadId;
    QString threadId;

    QDateTime livingTime;               //Critical data; need mutex
    QMutex mutex;
    QMqttClient* threadClient= nullptr;          //ref the client created in stack of sub-thread by the run method
    QTimer* threadTimer= nullptr;
    int workTimeMS = 500;                       //default 500 milliseconds; can be overwritten by _workTimeMS above
};
