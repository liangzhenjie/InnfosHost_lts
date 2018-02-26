#include "autorecoginze.h"
#include <QMessageBox>
#include "communication.h"
#include "innfosproxy.h"
#include <QTimer>
#include "mediator.h"
#include "motorform.h"
#include <QApplication>
#include "msgbox.h"

AutoRecoginze * AutoRecoginze::m_pAutoRecognize = nullptr;
AutoRecoginze::AutoRecoginze(QObject *parent) :
    QObject(parent),
    m_bFindAvaliable(false),
    m_bTryNext(true),
    m_bHasSameId(false),
    m_nConnectStatus(NO_CONNECT)
{
    findAvailablePorts();
    m_motorsInfo.clear();
}

void AutoRecoginze::findAvailablePorts()
{
    m_portList.clear();
    QList<QSerialPortInfo> allportList = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo info, allportList) {
        QString manufacturer = info.manufacturer();
        if(manufacturer == "FTDI")//filter some serialport that is not FTDI
            m_portList.append(info);
    }
}

AutoRecoginze *AutoRecoginze::getInstance()
{
    if (!m_pAutoRecognize)
    {
        m_pAutoRecognize = new AutoRecoginze;
        connect(Communication::getInstance(),&Communication::OpenPort,m_pAutoRecognize,&AutoRecoginze::onPortOpen);
    }

    return m_pAutoRecognize;
}

void AutoRecoginze::autoDestroy()
{
    if(m_pAutoRecognize)
       delete m_pAutoRecognize;
    m_pAutoRecognize = nullptr;
}

bool AutoRecoginze::startRecognize(bool bRetry)
{
    if(bRetry)
    {
        Communication::getInstance()->stop();
        findAvailablePorts();
        m_motorsInfo.clear();
    }
    m_bFindAvaliable = false;
    m_bTryNext = true;
    if(m_portList.size() == 0)
    {
        waitTimeout();
        return m_bFindAvaliable;
    }
    if (!m_bFindAvaliable && m_portList.size() > 0)
    {
        if(m_bTryNext)
        {
            QSerialPortInfo info = m_portList.at(0);
            Communication::getInstance()->startCommunication(info.portName(),1500000);

            m_portList.removeAt(0);
            m_bTryNext = false;
        }
    }

    return m_bFindAvaliable;
}

void AutoRecoginze::setAvailable()
{
    m_bFindAvaliable = true;
    Mediator::getInstance()->findAvailablePort(m_nConnectStatus);
}

void AutoRecoginze::addMototInfo(quint8 nDeviceId, quint32 nDeviceMac)
{
    if(m_motorsInfo.contains(nDeviceId))
    {
        m_bHasSameId = true;
        QString content = tr("There are at least two devices with the same id %1,\nplease confirm that id is unique!").arg(nDeviceId);
        int nRet = MsgBox::Warning(nullptr,tr("Warning"),content);
        if(nRet == 0)
        {
            QApplication::quit();
            return;
        }
    }
    m_motorsInfo.insert(nDeviceId,nDeviceMac);
    setConnectStatus(MOTOR_CONNECTED);
    setConnectStatus(USB_CONNECTED);
}

void AutoRecoginze::setConnectStatus(int nStatus)
{
    m_nConnectStatus |= nStatus;
}

void AutoRecoginze::openFailed()
{
    m_bTryNext = true;
}

void AutoRecoginze::waitTimeout()
{
    if(m_bHasSameId)
        return;
    if (m_motorsInfo.size() > 0)
    {
        setAvailable();
        MotorMgr::initAllMotors(m_motorsInfo);
        autoDestroy();
        return;
    }
    if (m_portList.size() > 0)
    {
        m_bTryNext = true;
        startRecognize();
    }
    else
    {
        Mediator::getInstance()->findAvailablePort(m_nConnectStatus);
    }
}

void AutoRecoginze::onPortOpen(bool bSuccess)
{
    if(bSuccess)
    {
        InnfosProxy::SendProxy(0,D_CAN_CONNECT);
        InnfosProxy::SendProxy(0,D_READ_ADDRESS);
        QTimer::singleShot(800,this,SLOT(waitTimeout()));
    }
    else
    {
        waitTimeout();
    }
}
