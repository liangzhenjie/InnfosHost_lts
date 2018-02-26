#ifndef MEDIATOR_H
#define MEDIATOR_H
#include <QObject>
#include "motorform.h"

#define TEST_DEBUG
#define LOG_DEBUG
//#define NO_HEART_BEAT
//#define WARNING_DEBUG
#define MAXSION_DEVICE

#define mediator Mediator::getInstance()

const qreal velScale = 6000;
#ifdef MAXSION_DEVICE
const qreal curScale = 8.25;
#else
const qreal curScale = 16.5;
#endif

class Mediator : public QObject
{
    Q_OBJECT
public:
    static Mediator * getInstance();
    static void destroyAllStaticObjects();
    bool autoRecognize(bool bRetry = false);//auto recognize motor

    void RefreshPosMode(int param[]);
    void RefreshMode(int param[], const quint32 nMode);

    void SendParams(const QByteArray & buf);
    void SendRequest(const QByteArray & buf);
    void SetPosModeEnable(bool bEnable);
    void SetModeEnable(bool bEnable,const quint32 nMode);
    //void ClosePort();
    void Handshake(quint32 nDeviceId,bool bSuccess);
    void SetCurParam(const int nDeviceID,const qreal value, const int nProxyId);//set motor param values
    void SetSucceed(const quint8 nDeviceId,const int nParam);//
    void SetFailed(const int nParam);//设置下位机参数fail
    void activeModeSuccessfully(const int nDeviceID);
    void revertSwitch(const quint8 nDeviceId);
    //void NullChartPointer();

    void reciveMotorInfo(const quint32 nDeviceMac, const quint8 nDeviceId);
    void receiveNoDataProxy(const int nDeviceID);

    qreal getValue(MotorForm::Motor_Data_Id nDataId)const;
    void setValueByUser(MotorForm::Motor_Data_Id nDataId,qreal value,bool bSendProxy=true);
    void sendProxy(const quint8 nDeviceId,const quint32 proxyId);
    void setTriggerMode(quint8 mode);
    void activeMode(const int nMode);
    void switchCalibrationVel(quint8 nValue);
    void switchCalibration(quint8 nValue);
    void startCalibration();
    MotorForm * getCurMotor()const;
    quint8  getCurDeviceId()const;

    void saveDataToDevice();
    void setChartFrequency(quint16 nFrequency);
    void setChartThreshold(qreal value);
    int curErrorId()const;
    void clearDeviceError();
    void checkError();
    void homingSet();
    void clearHoming();
    void clearHomingSuccessfully();
    void setHomingOperation(quint8 nId);
    MotorForm::Motor_Mode curDeviceMode()const;
    QVector<qint16> errorHistory()const;
    void deviceDataChange(quint8 nDeviceId,MotorForm::Motor_Data_Id Id);
    void deviceOnlineStatus(quint8 nDeviceId,bool bOnline);
    void playAction(quint32 nMac,MotorForm::Motor_Mode mode,qreal value);
    void openChart();
    void closeChart();
    void openChannel(const int nChannelId);
    void closeChannel(const int nChannelId);
    void setLeftLimit();
    void setRightLimit();
    qreal curCurrentScale();// current device's current scale
    qreal currentScale(int nDeviceId);//device's current scale with specified id;
    qreal curVelocityScale();// current device's current scale
    qreal velocityScale(int nDeviceId);//device's current scale with specified id;
    void checkServosStatus();//check servos are on or off
public slots:
    void response(const QByteArray & buf);
    void timeout(QString str);
    void loadDataFromFile();
    void saveDataToFile();
    void reconnectDevice();
signals:
    void findAvailablePort(int nConnectStatus);
    void ErrorMsg(const int nDeviceId,const qint32 nMsgId);
    void clearErrorSuccess(const int nDeviceId);
    void startNewPeriodChart();

    void curErrorChange(int nErrorId);
    void chartVauleChange(const int nChannelId,qreal values);//only use by chart
    void dataChange(MotorForm::Motor_Data_Id Id);
    void onlineChange(bool bOnline);
protected:
    Mediator(QObject * parent=0);
private:
    static Mediator *m_pInstance;
};

#endif // MEDIATOR_H
