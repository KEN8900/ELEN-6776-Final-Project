//
// Created by ken on 8/14/24.
//
#include <QDateTime>
#include <sstream>
#include "MqttThreadConnector.h"

MqttThreadConnector::MqttThreadConnector(QObject *parent,
                                         const QString &brokerIp, int brokerPort) : QThread(parent) {
    state = MqttThreadConnector::connectionState::closed;
    // mainThread call here
    this->brokerIp = brokerIp;
    this->brokerPort = brokerPort;

    // set living time for the subThread (to be created by calling reqStart)
    setLivingTime();
}

MqttThreadConnector::~MqttThreadConnector() = default;


void MqttThreadConnector::reqStart(int _workTimeMS/*=500*/) {
// mainThread requests the sub-thread to run
    this->workTimeMS = _workTimeMS;
    auto th = QThread::currentThreadId();
    qDebug() << "MainThread(" << th << ") creates a sub-thread...";
    QThread::start();
}


void MqttThreadConnector::reqStop(int stopTimeMS/*=5000*/) {
    //mainThread wait/require the sub-thread to exit

    //Tells the thread's event loop to exit with a return code.
    //After calling this function, the sub-thread ---leaves the event loop, at (1) --- and
    // returns from the call to QEventLoop::exec().
    // The QEventLoop::exec() function returns returnCode.
    this->exit(); //(2.1)

    auto th = QThread::currentThreadId();
    // mainThread waits for the sub-thread to stop
    // Maximum 5 seconds.
    // If not success, force to terminate the thread
    // (2.2)
    // Unit: ms
    if (wait(stopTimeMS)) {
        qDebug() << "\t\t <<<<<<<<<< MainThread(" << th << ") successfully waited subThread(" << threadId
                 << ") to exit within " << stopTimeMS << "milliSeconds.";
        return;
    }

    qDebug() << "\t\t xxxxxxxxxxxxx MainThread(" << th << ") waits subThread(" << threadId
             << ") to stop exceeding 5 seconds, so force terminating subThread(" << threadId << ").";
    this->terminate();  //Thread did not exit in time, probably deadlocked, terminate it
    this->wait();       //Have to wait for fully terminated
    qDebug() << "\t\t xxxxxxxxxxxxx MainThread(" << th << ") has terminated the subThread(" << threadId
             << ") successfully.";

}
////////////////////////////////////////////////////////////////////////////
//                          business methods
void MqttThreadConnector::routineWork() {
    setLivingTime();
    doSubscribe();
    doPublish();
}

void MqttThreadConnector::doSubscribe() {
    QString url = "";
    quint8 qos;

    //template method: call subclass
    this->getSubscribeInfo(url, qos);
    if (url.isEmpty())
        return;

    threadClient->subscribe(url, qos);
}

void MqttThreadConnector::doPublish() {
    //Do we need to write something?
    QString url = "";
    quint8 qos;
    QByteArray msg;

    //template method: call subclass
    this->getPublishInfo(url, qos, msg);
    if (url.isEmpty())
        return;

    auto id = threadClient->publish(url, msg, qos, false);
    //template method: notify subclass
    this->onDataSendStart(id, url, msg);
}
///////////////////////////////////////////////////////////////////////////////
//                          thread controls
void MqttThreadConnector::run() {
    //sub-thread entry code, called by Operating System
    QThread::setTerminationEnabled(true);
    auto tId = QThread::currentThreadId();       //record the id of the sub-thread.
    threadId = QString::number(reinterpret_cast<quintptr>(tId), 16).toUpper();

    //CREATE QMqttClient in the stack of the sub-thread, so that
    //the sub-thread can receive the mqtt events in its event loop.
    QMqttClient client;
    this->threadClient = &client;               // yjf.add.
    client.setHostname(brokerIp);
    client.setPort(brokerPort);
    installEventHandle(client);

    //Timer to check the living for a certain period of time
    QTimer timer;
    this->threadTimer = &timer;
    // This method is called every 500 ms
    QObject::connect(&timer, &QTimer::timeout,
                     [&, this]() {
                            this->routineWork();
                     });
    timer.start(workTimeMS); // Unit: ms

    //let the sub-thread do the ---LOOP---:
    // 1) get event from the message queue
    // 2) dispatch the event to the event handle, such as this->subscribe
    this->exec();

    //subThread exits from the above LOOP:
    // (2.1), (2.2) make the subThread stop here.
    qDebug() << "<<<<<<<<<<<< SubThread(" << threadId << ") stopped successfully.";
}

void MqttThreadConnector::setLivingTime() {
    //subThread call this function
    mutex.lock();
    livingTime = QDateTime::currentDateTime();
    mutex.unlock();
}

QDateTime MqttThreadConnector::getLivingTime() {
    //MainThread call this function
    QDateTime lt;
    mutex.lock();
    lt = livingTime;
    mutex.unlock();
    return lt;
}

bool MqttThreadConnector::threadNeedExit() {
    //MainThread call this function
    bool needExit = false;

    mutex.lock();
    if (state == connectionState::threadExit)
        needExit = true;
    mutex.unlock();

    return needExit;
}

void MqttThreadConnector::setThreadExit() {
    // stop event senders
    this->threadTimer->stop();
//    this->threadClient->disconnectFromHost();     //WARNING: call it may cause process segment error.

    //subThread call this func
    mutex.lock();
    state = connectionState::threadExit;
    mutex.unlock();
}

bool MqttThreadConnector::isWorking(int maxBusyTimeMS/*==10*1000*/) {
    //MainThread call this function

    //1) Does this subThread encounter an ERROR that must stop running?
    auto needExit = threadNeedExit();                              //critical data
    if (needExit) {
        return false;
    }

    //2) Does this subThread have a heavy (dead) work that cannot send the heart pulse?
    const QDateTime current = QDateTime::currentDateTime();
    auto living = getLivingTime();                      //critical data
    const qint64 milliSecondsDiff = living.msecsTo(current);

    if (milliSecondsDiff <= maxBusyTimeMS) {
        return true;
    }
    qDebug() << "WARNING: xxxxxxxxxxx MqttThreadConnector("<< threadId << ")::isWorking = FALSE; milliSecondsDiff = " << milliSecondsDiff <<
             ", livingTime=" << living << ", current=" << current;

    return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                 MQTT communication logic

void MqttThreadConnector::installEventHandle(QMqttClient &client) {

    install_errorChanged(client);  //must put first, to check the states of the broker.

    install_stateChanged(client);
    install_messageReceived(client);
    install_messageStatusChanged(client);

    qDebug() << threadId << ": try to connectToHost for the first time ......";
    client.connectToHost();
}

void MqttThreadConnector::install_errorChanged(QMqttClient &client) {
    QObject::connect(&client,
                     &QMqttClient::errorChanged,
                     [&, this](QMqttClient::ClientError error) {
                         if (state == connectionState::threadExit or
                             error == QMqttClient::NoError) {
                             return;
                         }

                         this->setLivingTime();    //refresh the living time (critical data)
                         if (state == connectionState::firstConnect) {
                             qDebug() << threadId << ": xxxxxxxxxxxxxxxxx state = firstConnect...find a SERIOUS: "
                                      << error << ", thread exit.";
                             this->setThreadExit();
                         }
                     });
}

void MqttThreadConnector::install_stateChanged(QMqttClient& client) {
    QObject::connect(&client,
                     &QMqttClient::stateChanged,
                     [&, this](QMqttClient::ClientState clientState) {
                         if (state == connectionState::threadExit) {
                             return;
                         }

                         this->setLivingTime();    //refresh the living time (critical data)
                         switch (clientState) {
                             default:
                                 qDebug() << threadId << ": received unknown clientState: " << clientState
                                          << " in my state: " << state;
                             case QMqttClient::Disconnected:
                                 state = connectionState::closed;
                                 client.connectToHost();            //Start to connect to the broker
                                 return;
                             case QMqttClient::Connecting:
                                 state = connectionState::connecting;
                                 return;
                             case QMqttClient::Connected:
                                 if (state == connectionState::firstConnect) {
                                     client.setAutoKeepAlive(true);
                                     return;
                                 }
                                 //we establish the first connection with the broker
                                 client.setAutoKeepAlive(true);
                                 state = connectionState::firstConnect;
                                 routineWork();                 //CALL BUSINESS METHODS

                         }//switch (clientState)
                     }); //QObject::connect
}

void MqttThreadConnector::install_messageReceived(QMqttClient& client) {
    QObject::connect(&client,
                     &QMqttClient::messageReceived,
                     [&, this](const QByteArray &message, const QMqttTopicName &topic) {
                         this->setLivingTime();    //refresh the living time (critical data)

                         //call subclasses of MqttThreadConnector
                         this->onMessageReceived(message, topic);
                     });
}

void MqttThreadConnector::install_messageStatusChanged(QMqttClient& client) {

    QObject::connect(&client,
                     &QMqttClient::messageStatusChanged,
                     [&, this](qint32 id, QMqtt::MessageStatus msgStatus) {
                         if (state == connectionState::threadExit) {
                             return;
                         }

                        this->setLivingTime();    //refresh the living time (critical data)
                        switch(msgStatus) {
                            default:
                                qDebug() << "message status is unknown";
                                return;
                            case QMqtt::MessageStatus::Acknowledged:
                            case QMqtt::MessageStatus::Completed:
//                              //BUSINESS METHODS
                                this->onDataSentFinish(id);
                                return;

                            case QMqtt::MessageStatus::Unknown:
                                qDebug() << "The message status is unknown.";
                                return;
                            case QMqtt::MessageStatus::Published:
                                qDebug() << "The client received a message for one of its subscriptions. Qos 1 & 2.";
                                return;
                            case QMqtt::MessageStatus::Received:
                                qDebug() << "A message has been received. QoS 2: ";
                                return;
                            case QMqtt::MessageStatus::Released:
                                qDebug() << "A message has been released. QoS 2.";
                                return;

                        }//switch(msgStatus)
                     });//QObject::connect
}


QString MqttThreadConnector::getThreadId() const {
    return threadId;
}

bool MqttThreadConnector::isYou(const QString& strThreadId) const {
    return this->threadId == strThreadId;
}


QString MqttThreadConnector::toString() const {
    QString tmp = threadId;
    tmp += ", brokerIp= ";
    tmp += brokerIp.toStdString();
    tmp += ", brokerPort= ";
    tmp += QString::number(brokerPort);

    return tmp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  subclass override the following methods:
void MqttThreadConnector::onMessageReceived(const QByteArray &message, const QMqttTopicName &url) {
    qDebug() << "=================> MqttThreadConnector(" << threadId << ")::messageReceived====> msg = " << message
             << ", url = " << url
             << "\n";
}

void MqttThreadConnector::getSubscribeInfo(QString &url, quint8 &qos) {
    url = "";   //default implementation
    qos = 0;
}

void MqttThreadConnector::getPublishInfo(QString &url, quint8 &qos, QByteArray &msg) {
    url = "";
    qos = 0;
}

void MqttThreadConnector::onDataSendStart(qint32 id, const QString& url, const QByteArray& msg) {

}

void MqttThreadConnector::onDataSentFinish(qint32 id) {

}