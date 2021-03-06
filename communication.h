﻿#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QVector>


class Communication : public QThread
{
    Q_OBJECT
public:
    static Communication * getInstance();
    static void autoDestroy();
    ~ Communication();
    void run()Q_DECL_OVERRIDE;
    void startCommunication(const QString &portName,const quint32 nBaudRate);
    void sendData(const QByteArray & data);
    bool hasDataWaitToSend();
    void stop();
    void reStart();
    void recordRemainCmds();
protected:
    explicit Communication(QObject * parent = nullptr);
signals:
    void error(const QString &error);
    void response(const QByteArray &response);
    void request(const QByteArray &request);
    void timeout(const QString &timeout);
    void OpenPort(bool bSuccess);//
private:
    QString m_qsName;
    quint32 m_nBaudRate;
    QMutex m_qmMutex;
    bool m_bQuit;
    QVector <QByteArray> m_waitSendDatas;
    //QMap<quint8,QVector <QByteArray>> m_dataMap;
    static Communication * m_pCommucation;
    bool m_bPause;
public:
    bool m_bRecieve;
};

#endif // COMMUNICATION_H
