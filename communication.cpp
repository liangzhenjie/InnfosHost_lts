#include "communication.h"
#include <QtSerialPort/QSerialPort>
#include <QIODevice>
#include <QDebug>
#include <QApplication>
#include <QTime>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QFile>
#include <QTextStream>
#include <chrono>
#include <thread>

Communication * Communication::m_pCommucation = nullptr;
Communication::Communication(QObject *parent):
    QThread(parent),
    m_bQuit(false),
    m_nBaudRate(0),
    m_bPause(false),
    m_bRecieve(true)
{

}

Communication *Communication::getInstance()
{
    if (!m_pCommucation)
    {
        m_pCommucation = new Communication();
    }
    return m_pCommucation;
}

void Communication::autoDestroy()
{
    if(m_pCommucation)
    {
#ifdef RELEASE_LOG
    m_pCommucation->recordRemainCmds();
#endif
        while (true)
        {
            if(!m_pCommucation->hasDataWaitToSend())
            {
                delete m_pCommucation;
                break;
            }
            msleep(10);
        }

    }

    m_pCommucation = nullptr;
}

Communication::~Communication()
{
    m_qmMutex.lock();
    m_bQuit = true;
    m_qmMutex.unlock();
    wait();
}

void Communication::run()
{
    bool bNameChange = false;
    bool bRateChange = false;
    bool bPause = false;
    int nFailCnt = 0;

    m_qmMutex.lock();
    QString curName;
    quint32 curRate = 0;
    QByteArray sendData;

    int nSingalCnt = 0;
    int nWaitTime = 1;

    if(curName != m_qsName)
    {
        curName = m_qsName;
        bNameChange = true;
    }
    if(curRate != m_nBaudRate)
    {
        curRate = m_nBaudRate;
        bNameChange = true;
    }
    sendData.clear();
    if(m_waitSendDatas.size() > 0)
    {
        sendData = m_waitSendDatas.front();
        m_waitSendDatas.pop_front();
    }
    bPause = m_bPause;
    m_qmMutex.unlock();

    QSerialPort serial;
    QElapsedTimer timer;
    timer.start();
    while (!m_bQuit) {
        if(!bPause)
        {
            if(bNameChange)
            {
                serial.close();
                serial.setPortName(curName);
                bool bSuccess = serial.setBaudRate(curRate);
                if(!serial.open(QIODevice::ReadWrite)){
                    emit OpenPort(false);
                    qDebug()<<"open failed";
                    m_waitSendDatas.clear();
                    return;
                }
                else
                {
                    emit OpenPort(true);
                }
            }

            if(bRateChange)
            {
                serial.setBaudRate(curRate);
            }
            //timer.restart();
            if(m_bRecieve && serial.waitForReadyRead(nWaitTime))
            {
                QByteArray readData = serial.readAll();
                if(serial.error() == QSerialPort::ReadError)
                {
                    emit timeout(tr("error read %1  errorCode %2").arg(readData.toHex().data()).arg(serial.error()));
                    serial.clearError();
                }

                qDebug()<<"readProxy"<<readData.toHex()<<QTime::currentTime().toString("hh:mm:ss.zzz");
                emit response(readData);
            }
            //timer.restart();
            if(sendData.size() > 0 && serial.isWritable())
            {
                int nLen = serial.write(sendData);
                if(serial.waitForBytesWritten(30))
                {
                    emit request(sendData);
                    qDebug()<<"SendProxy"<<nLen<<sendData.toHex()<<QTime::currentTime().toString("hh:mm:ss.zzz") << m_waitSendDatas.size();
                    sendData.clear();
                }
                else {

                    if(++nFailCnt > 2)
                    {
                        serial.clearError();
                        sendData.clear();
                        nFailCnt = 0;
                    }
                }
            }

        }
        //timer.restart();
        m_qmMutex.lock();
        bNameChange = false;
        bRateChange = false;
        if(curName != m_qsName)
        {
            curName = m_qsName;
            bNameChange = true;
        }
        if(curRate != m_nBaudRate)
        {
            curRate = m_nBaudRate;
            bRateChange = true;
        }

        if (bNameChange || bRateChange)//if port or port rate was changed,all data that wait to send need clear
        {
            sendData.clear();
        }
        //sendData.clear();
        if(m_waitSendDatas.size() > 0 && sendData.size()==0)
        {
            int nCnt = m_waitSendDatas.size() > 3?3:m_waitSendDatas.size();
            for(int i=0;i<1;++i)
            {
                sendData.append(m_waitSendDatas.front());
                m_waitSendDatas.pop_front();
            }

        }
        bPause = m_bPause;
        m_qmMutex.unlock();
        std::this_thread::sleep_for(std::chrono:: nanoseconds (100));
        //qDebug() << "lock:" <<timer.elapsed();
        //msleep(1);
        //qDebug() << "loop:" <<timer.elapsed();
    }
}

void Communication::startCommunication(const QString &portName, const quint32 nBaudRate)
{
    QMutexLocker locker(&m_qmMutex);
    m_bQuit = false;
    m_qsName = portName;
    m_nBaudRate = nBaudRate;
    m_waitSendDatas.clear();

    qDebug() <<"try"<<portName;
    if(!isRunning())
    {
        qDebug() <<"start"<<portName;
        start();
    }
}

void Communication::sendData(const QByteArray &data)
{
    QMutexLocker locker(&m_qmMutex);
    if(!m_bPause)
    {
        m_waitSendDatas.push_back(data);
    }

}

bool Communication::hasDataWaitToSend()
{
    //log end

    return m_waitSendDatas.size() > 0;
}

void Communication::stop()
{
    m_qmMutex.lock();
    m_bQuit = true;
    m_qmMutex.unlock();
    wait();
}

void Communication::reStart()
{
    QMutexLocker locker(&m_qmMutex);
    m_bPause = false;
}

void Communication::recordRemainCmds()
{
    if(m_waitSendDatas.size() > 0)
    {
        QFile * pFile = new QFile("waitSend.txt",this);
        if(pFile->open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
        {
            QTextStream stream2(pFile);
            stream2 << m_waitSendDatas.size();
            endl(stream2);
            for(int i=0;i<m_waitSendDatas.size(); ++i)
            {
                QTextStream stream(pFile);
                stream << m_waitSendDatas.at(i).toHex();
                endl(stream);
            }
        }
        pFile->close();
    }

}
