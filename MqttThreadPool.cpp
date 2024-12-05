//
// Created by ken on 8/16/24.
//
#include <QString>

#include "MqttWriteThread.h"
#include "MqttReadThread.h"
#include "MqttThreadPool.h"

MqttThreadPool::MqttThreadPool(int numReadThread,
                               int numWriteThread,
                               const QString &brokerIp,
                               int brokerPort){
    this->numReadThread = numReadThread;
    this->numWriteThread = numWriteThread;
    this->brokerIp = brokerIp;
    this->brokerPort = brokerPort;

    mainThreadId = QThread::currentThreadId();
}

MqttThreadPool::~MqttThreadPool() = default;

void MqttThreadPool::createThread() {
    int createReadThread = numReadThread - static_cast<int>(readThreadList.size());
    int createWriteThread = numWriteThread - static_cast<int>(writeThreadList.size());

    if (createReadThread <= 0 and createWriteThread <= 0) {
        return;
    }

    //Create Read Thread
    for (int i=0; i < createReadThread; ++i) {
        qDebug() << "\t\t" << mainThreadId << " ThreadPool ... create read thread: " << i;

        auto t = new MqttReadThread(nullptr, brokerIp, brokerPort);

        readThreadList.append(t);
        
        if (i == 0) {
            t->reqStart(2000);
        }
        else
            t->reqStart(500);
    }

    //Create Write Thread
    for (int i=0; i < createWriteThread; ++i) {
        qDebug() << "\t\t" << mainThreadId << " ThreadPool ... create write thread: " << i;

        auto t = new MqttWriteThread(nullptr, brokerIp, brokerPort);

        writeThreadList.append(t);

        if (i == 0) {
            t->reqStart(2000);
        }
        else
            t->reqStart(500);
    }
}

void MqttThreadPool::killDeadThread() {
    // record which sub-thread is dead.

    QList<int> killIndex;
    QList<MqttReadThread*> toKillRead;
    QList<MqttWriteThread*> toKillWrite;

    for (auto rThread: readThreadList) {
        if (rThread->isWorking(600)) {
            //qDebug() << "***** threadId= " << pThread->getThreadId();
            continue;
        }
        qDebug() << "\t\t" << mainThreadId << " (Read Thread Pool) is ready to kill: " << rThread->getInfo();
        toKillRead.append(rThread); // readThreadList [0x15, 0x17, 0x18, 0x23]
    }

    for (auto wThread: writeThreadList) {
        if (wThread->isWorking(600)) {
            //qDebug() << "***** threadId= " << pThread->getThreadId();
            continue;
        }
        qDebug() << "\t\t" << mainThreadId << " (Write Thread Pool) is ready to kill: " << wThread->getInfo();
        toKillWrite.append(wThread); // readThreadList [0x15, 0x17, 0x18, 0x23]
    }

    //remove toKillRead threads from readThreadList
    for(auto killRead : toKillRead) {
        readThreadList.removeOne(killRead);
    }

    //remove toKillWrite threads from writeThreadList
    for(auto killWrite : toKillWrite) {
        writeThreadList.removeOne(killWrite);
    }

    // stop threads in the toKillRead list
    for (auto rTh: toKillRead) {  //iterate over the list
        rTh->reqStop();
        delete rTh;             //delete the memory pointed by the variable pThread
    }
    toKillRead.clear();

    // stop threads in the toKillWrite list
    for (auto wTh: toKillWrite) {  //iterate over the list
        wTh->reqStop();
        delete wTh;             //delete the memory pointed by the variable pThread
    }
    toKillWrite.clear();

//    qDebug() << "toKillRead=" << toKillRead;
//    qDebug() << "readThreadList=" << readThreadList;
}

void MqttThreadPool::killAllThread(){
    for (auto readThread : readThreadList){
        readThreadList.remove(0);
        readThread->reqStop();
        delete readThread;
    }

    for (auto writeThread : writeThreadList){
        writeThreadList.remove(0);
        writeThread->reqStop();
        delete writeThread;
    }
}

void MqttThreadPool::simpleTest() {

    for (int i = 0; i < 30; ++i) {
        //main thread can not exit
        QThread::sleep(1); //main thread sleep periodically. Unit: s

        createThread();
        killDeadThread();
    }

    qDebug() << "------------------ KILL ALL -------------------------------------";
    killAllThread();
    qDebug() << "\t\t" << "mainThread id:" << mainThreadId << "... Thread Pool stop all threads..." << "\n";
}

