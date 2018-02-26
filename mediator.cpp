﻿#include "mediator.h"
#include "communication.h"
#include "autorecoginze.h"
#include "proxyparser.h"
#include "motorform.h"
#include "dataUtil.h"
#include <QApplication>
#include "innfosproxy.h"
#include <QMessageBox>
#include <QDebug>
#include "innfoschartwidget.h"
#include <QTimer>

Mediator * Mediator::m_pInstance = nullptr;
Mediator *Mediator::getInstance()
{
    if(!m_pInstance)
    {
        m_pInstance = new Mediator;
    }

    return m_pInstance;
}

void Mediator::destroyAllStaticObjects()
{
    MotorMgr::autoDestroy();
    AutoRecoginze::autoDestroy();
    Communication::autoDestroy();
    DataUtil::autoDestroy();
    ProxyParser::autoDestroy();

    if(m_pInstance)
        delete m_pInstance;
    m_pInstance = nullptr;

}

bool Mediator::autoRecognize(bool bRetry)
{
#ifdef TEST_DEBUG
    connect(Communication::getInstance(),&Communication::request,ProxyWatcher::getInstance(),&ProxyWatcher::addSendItem);
#endif
    return AutoRecoginze::getInstance()->startRecognize(bRetry);
}

void Mediator::RefreshPosMode(int param[])
{

}

void Mediator::RefreshMode(int param[], const quint32 nMode)
{

}

void Mediator::SendParams(const QByteArray &buf)
{

}

void Mediator::SendRequest(const QByteArray &buf)
{
    Communication::getInstance()->sendData(buf);
}

void Mediator::SetPosModeEnable(bool bEnable)
{

}

void Mediator::SetModeEnable(bool bEnable, const quint32 nMode)
{

}

void Mediator::Handshake(quint32 nDeviceId, bool bSuccess)
{
    MotorForm * pMotor = MotorMgr::getInstance()->getMotorByDeviceId(nDeviceId);
    if(pMotor)
    {
        pMotor->responseHeart(bSuccess);
    }
}

void Mediator::SetCurParam(const int nDeviceID,const qreal value, const int nProxyId)
{
    MotorForm::Motor_Data_Id id = DataUtil::convertToMotorDataId((Directives)nProxyId);
    MotorForm * pMotor = MotorMgr::getInstance()->getMotorByDeviceId(nDeviceID);
    if(id != MotorForm::DATA_INVALID && pMotor)
        pMotor->setValueByProxy(nProxyId,value);

}

void Mediator::SetSucceed(const quint8 nDeviceId, const int nParam)
{
    MotorForm * pMotor = MotorMgr::getInstance()->getMotorByDeviceId(nDeviceId);
    switch (nParam) {
    case D_SET_DEVICE_ID:
        if(pMotor)
        {
            MotorMgr::getInstance()->RmMotorFromMap(pMotor->getOldDeviceId());//remove old id in map
            MotorMgr::getInstance()->AddMotorToMap(pMotor);//add new id in map
            pMotor->setDeviceIdSuccessfully();//motor's old id reset
        }
        break;
    case D_SET_SWITCH_MOTORS:
        if(pMotor && pMotor->getValue(MotorForm::MOTOR_SWITCH)==1)
        {
            QTimer::singleShot(3000,[=]{
                pMotor->requestAllValue();
                InnfosProxy::SendProxy(nDeviceId,D_READ_LAST_STATE);
            });
        }
        break;
    default:
        break;
    }
}

void Mediator::SetFailed(const int nParam)
{

}

void Mediator::activeModeSuccessfully(const int nDeviceID)
{
    MotorForm * pMotor = MotorMgr::getInstance()->getMotorByDeviceId(nDeviceID);
    if(pMotor)
        pMotor->activeModeSuccessfully();
}

void Mediator::revertSwitch(const quint8 nDeviceId)
{
    MotorForm * pMotor = MotorMgr::getInstance()->getMotorByDeviceId(nDeviceId);
    if(pMotor)
    {
        quint8 nSwitch = pMotor->getValue(MotorForm::MOTOR_SWITCH);
        nSwitch = nSwitch > 0 ? 0 : 1;
        pMotor->setValueByProxy(D_READ_MOTORS_SWITCH,nSwitch);
    }
}


void Mediator::reciveMotorInfo(const quint32 nDeviceMac, const quint8 nDeviceId)
{
    AutoRecoginze::getInstance()->addMototInfo(nDeviceId,nDeviceMac);
    //MotorMgr::getInstance()->AddMotor(nDeviceMac,nDeviceId);
    //MotorMgr::getInstance()->show();
   // MotorMgr::getInstance()->AdjustLayout();//
    //AutoRecoginze::getInstance()->setAvailable();//find motor

}

void Mediator::receiveNoDataProxy(const int nDeviceID)
{
    switch (nDeviceID) {
    case D_CHART_DATA_STATR:
        emit startNewPeriodChart();
        break;
    default:
        break;
    }
}

qreal Mediator::getValue(MotorForm::Motor_Data_Id nDataId) const
{
    MotorForm * pMotor = MotorMgr::getInstance()->getCurSelected();
    if(pMotor)
        return pMotor->getValue(nDataId);
    return -1;
}

void Mediator::setValueByUser(MotorForm::Motor_Data_Id nDataId, qreal value, bool bSendProxy)
{
//    MotorForm * pMotor = MotorMgr::getInstance()->getCurSelected();
//    if(pMotor)
//        pMotor->setValueByUser(nDataId,value);
    QVector<MotorForm *> selMotors = MotorMgr::getInstance()->allSelectedMotors();
    foreach (MotorForm * pMotor, selMotors)
    {
       pMotor->setValueByUser(nDataId,value,bSendProxy);
    }
}

void Mediator::sendProxy(const quint8 nDeviceId, const quint32 proxyId)
{
    MotorForm * pMotor = MotorMgr::getInstance()->getMotorByDeviceId(nDeviceId);
    if(pMotor)
        InnfosProxy::SendProxy(nDeviceId,proxyId);
}

void Mediator::setTriggerMode(quint8 mode)
{
    QVector<MotorForm *> selMotors = MotorMgr::getInstance()->allSelectedMotors();
    foreach (MotorForm * pMotor, selMotors)
    {
       InnfosProxy::SendProxy(pMotor->deviceId(),D_SET_CUR_TRIGGER_MODE,mode);
    }
}

void Mediator::activeMode(const int nMode)
{
//    MotorForm * pMotor = MotorMgr::getInstance()->getCurSelected();
//    if(pMotor)
//        pMotor->activeMode(nMode);
    QVector<MotorForm *> selMotors = MotorMgr::getInstance()->allSelectedMotors();
    foreach (MotorForm * pMotor, selMotors)
    {
       pMotor->activeMode(nMode);
    }
}

void Mediator::switchCalibrationVel(quint8 nValue)
{
    MotorForm * pMotor = MotorMgr::getInstance()->getCurSelected();
    if(pMotor)
        InnfosProxy::SendProxy(pMotor->deviceId(),D_SWITCH_CALIBRATION_VEL,nValue);
}

void Mediator::switchCalibration(quint8 nValue)
{
    MotorForm * pMotor = MotorMgr::getInstance()->getCurSelected();
    if(pMotor)
        InnfosProxy::SendProxy(pMotor->deviceId(),D_SWITCH_CALIBRATION,nValue);
}

void Mediator::startCalibration()
{
    MotorForm * pMotor = MotorMgr::getInstance()->getCurSelected();
    if(pMotor)
        InnfosProxy::SendProxy(pMotor->deviceId(),D_START_CALIBRATION);
}

MotorForm *Mediator::getCurMotor() const
{
    return MotorMgr::getInstance()->getCurSelected();
}

quint8 Mediator::getCurDeviceId() const
{
    MotorForm * pMotor = MotorMgr::getInstance()->getCurSelected();
    if(pMotor == nullptr)
    {
        pMotor = MotorMgr::getInstance()->getDefalutMotor();
    }

    if(pMotor == nullptr)
        return 0;

    return pMotor->deviceId();
}

void Mediator::saveDataToDevice()
{
//    MotorForm * pMotor = getCurMotor();
//    if(pMotor)
//        InnfosProxy::SendProxy(pMotor->deviceId(),D_SAVE_PARAM);
    QVector<MotorForm *> selMotors = MotorMgr::getInstance()->allSelectedMotors();
    foreach (MotorForm * pMotor, selMotors)
    {
       InnfosProxy::SendProxy(pMotor->deviceId(),D_SAVE_PARAM);
    }
}

void Mediator::setChartFrequency(quint16 nFrequency)
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        pMotor->setValueByUser(MotorForm::CHART_FREQUENCY,nFrequency);
    }
}

void Mediator::setChartThreshold(qreal value)
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        pMotor->setValueByUser(MotorForm::CHART_THRESHOLD,value);
    }
}

int Mediator::curErrorId() const
{

    MotorForm * pMotor = this->getCurMotor();
    if(pMotor)
    {
        return pMotor->currentError();
    }
    return -1;
}

void Mediator::clearDeviceError()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        InnfosProxy::SendProxy(pMotor->deviceId(),D_CLEAR_ERROR);
    }

}

void Mediator::checkError()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        InnfosProxy::SendProxy(pMotor->deviceId(),D_ERROR);
    }
}

void Mediator::homingSet()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        InnfosProxy::SendProxy(pMotor->deviceId(),D_READ_MAX_POS);
        InnfosProxy::SendProxy(pMotor->deviceId(),D_READ_MIN_POS);
    }
}

void Mediator::clearHoming()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        InnfosProxy::SendProxy(pMotor->deviceId(),D_CLEAR_HOMING);
    }
}

void Mediator::clearHomingSuccessfully()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        pMotor->setValueByUser(MotorForm::POS_MAX_POS,0,false);
        pMotor->setValueByUser(MotorForm::POS_MIN_POS,0,false);
        pMotor->setValueByUser(MotorForm::POS_HOMING,0,false);
    }
}

void Mediator::setHomingOperation(quint8 nId)
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        InnfosProxy::SendProxy(pMotor->deviceId(),D_SET_HOMING_OPERATION,nId);
    }
}

MotorForm::Motor_Mode Mediator::curDeviceMode() const
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        return pMotor->currentMode();
    }
    return MotorForm::Mode_None;
}

QVector<qint16> Mediator::errorHistory() const
{
    QVector<qint16> errors;
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        errors = pMotor->errorHistory();
    }
    return errors;
}

void Mediator::deviceDataChange(quint8 nDeviceId, MotorForm::Motor_Data_Id Id)
{
    dataChange(Id);
}

void Mediator::deviceOnlineStatus(quint8 nDeviceId, bool bOnline)
{
    onlineChange(bOnline);
}

void Mediator::playAction(quint32 nMac, MotorForm::Motor_Mode mode, qreal value)
{
    //MotorForm * pMotor = MotorMgr::getInstance()->getMotorByMac(nMac);
    MotorForm * pMotor = MotorMgr::getInstance()->getMotorByDeviceId(nMac);
    if(pMotor)
    {
        if(pMotor->currentMode() != mode)
        {
            pMotor->activeMode(mode);
        }
        else
        {
            pMotor->changeDemand(value);
        }

    }
}

void Mediator::openChart()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        pMotor->showGraph();
        InnfosProxy::SendProxy(pMotor->deviceId(),D_CHART_OPEN);
    }
}

void Mediator::closeChart()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        pMotor->closeGraph();
        InnfosProxy::SendProxy(pMotor->deviceId(),D_CHART_CLOSE);
    }
}

void Mediator::openChannel(const int nChannelId)
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        int nProxyId[InnfosChartWidget::channel_cnt] = {D_CHANNEL1_OPEN,D_CHANNEL2_OPEN,D_CHANNEL3_OPEN,D_CHANNEL4_OPEN};
        InnfosProxy::SendProxy(pMotor->deviceId(),nProxyId[nChannelId]);
    }
}

void Mediator::closeChannel(const int nChannelId)
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        int nProxyId[InnfosChartWidget::channel_cnt] = {D_CHANNEL1_CLOSE,D_CHANNEL2_CLOSE,D_CHANNEL3_CLOSE,D_CHANNEL4_CLOSE};
        InnfosProxy::SendProxy(pMotor->deviceId(),nProxyId[nChannelId]);
    }
}

void Mediator::setLeftLimit()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        InnfosProxy::SendProxy(pMotor->deviceId(),D_SET_HOMING_MIN);
    }
}

void Mediator::setRightLimit()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        InnfosProxy::SendProxy(pMotor->deviceId(),D_SET_HOMING_MAX);
    }
}

qreal Mediator::curCurrentScale()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        return pMotor->getValue(MotorForm::CURRENT_SCALE);
    }
    return curScale;
}

qreal Mediator::currentScale(int nDeviceId)
{
    MotorForm * pMotor = MotorMgr::getInstance()->getMotorByDeviceId(nDeviceId);
    if(pMotor)
    {
        return pMotor->getValue(MotorForm::CURRENT_SCALE);
    }
    return curScale;
}

qreal Mediator::curVelocityScale()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        return pMotor->getValue(MotorForm::VELOCITY_SCALE);
    }
    return velScale;
}

qreal Mediator::velocityScale(int nDeviceId)
{
    MotorForm * pMotor = MotorMgr::getInstance()->getMotorByDeviceId(nDeviceId);
    if(pMotor)
    {
        return pMotor->getValue(MotorForm::VELOCITY_SCALE);
    }
    return velScale;
}

void Mediator::checkServosStatus()
{
    MotorMgr::getInstance()->CheckServosSwitch();
}

void Mediator::response(const QByteArray &buf)
{
    ProxyParser::getInstance()->parse(buf);

}

void Mediator::timeout(QString str)
{
    qDebug() << str;
    QMessageBox::warning(nullptr,tr("Warning"),str,QMessageBox::Ok);
}

void Mediator::loadDataFromFile()
{
    getCurMotor()->loadData();
}

void Mediator::saveDataToFile()
{
    getCurMotor()->saveData();
}

void Mediator::reconnectDevice()
{
    MotorForm * pMotor = getCurMotor();
    if(pMotor)
    {
        pMotor->reconnect();
    }
}

Mediator::Mediator(QObject *parent):
    QObject(parent)
{
    connect(Communication::getInstance(),&Communication::response,this,&Mediator::response);
    connect(Communication::getInstance(),&Communication::timeout,this,&Mediator::timeout);
}


