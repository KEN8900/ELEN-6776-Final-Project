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
    [[nodiscard]] bool isYou(const QString& strThreadId) const;

    void reqStart(int _workTimeMS=500);  // mainThread requires the subthread to run
    void reqStop(int stopTimeMS=5000);   // mainThread requires the subthread to stop

    bool isWorking(int maxBusyTimeMS=10000); // a function to let the outside check the working status

    [[nodiscard]] QString toString() const; // To print out some useful info

    enum connectionState{
        closed=0,
        connecting,
        firstConnect,
        threadExit
    };
    Q_ENUM(connectionState)

protected:
    //subclass must override this virtual method.
    virtual void onMessageReceived(const QByteArray &message, const QMqttTopicName &url); //Reader

    virtual void getSubscribeInfo(QString& url, quint8& qos);
    virtual void getPublishInfo(QString& url, quint8& qos, QByteArray& msg);

    virtual void onDataSendStart(qint32 id, const QString& url, const QByteArray& msg);
    virtual void onDataSentFinish(qint32 id);

    // Reader/Writer: client.subscribe(url, qos); client.publish(url, message, qos, false);
    // Writer: quint32 id = client.publish(url, message, qos, false);

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

    void setLivingTime();                //Critical data
    QDateTime getLivingTime();           //critical data
    bool threadNeedExit();
    void setThreadExit();

    QString brokerIp;
    int brokerPort;

//    Qt::HANDLE threadId;
    QString threadId;

    QDateTime livingTime;               //Critical data
    QMutex mutex;
    QMqttClient* threadClient= nullptr;          //ref the client created in stack of sub-thread by the run method
    QTimer* threadTimer= nullptr;
    int workTimeMS = 500;                       //default 500ms
};
