﻿#ifndef MOTORFORM_H
#define MOTORFORM_H

#include <QWidget>
#include <QMap>
#include <QVector>

namespace Ui {
class MotorForm;
}

class QButtonGroup;
class InnfosWindow;
class QTimer;
class QXmlStreamReader;
class QPushButton;
class QLineEdit;
class QFile;
class InnfosWindow;
class EditorWidget;
class AngleClock;
class QLCDNumber;

//#define RECORD_DATA

class MotorForm : public QWidget
{
    Q_OBJECT

public:
    enum Motor_Mode{
        Mode_None,
        Mode_Cur,
        Mode_Vel,
        Mode_Pos,
        Mode_Teaching,
        Mode_Profile_Pos=6,
        Mode_Profile_Vel,
        Mode_Homing,
    };
    enum Motor_status{
        Status_Normal=0x01,
        Status_Select=0x02,
        Status_Error=0x04,
        Status_Offline=0x08,
    };

    Q_DECLARE_FLAGS(MotorStatus,Motor_status)

    enum Motor_Data_Id{
        CUR_IQ_SET,
        CUR_PROPORTIONAL,
        CUR_INTEGRAL,
        CUR_ID_SET,
        CUR_MINIMAL,
        CUR_MAXIMUM,
        CUR_NOMINAL,
        CUR_OUTPUT,
        CUR_MAXSPEED,
        CUR_ACTURAL,
        CUR_DEMAND,
        VEL_SET,
        VEL_PROPORTIONAL,
        VEL_INTEGRAL,
        VEL_MINIMAL,
        VEL_MAXIMUM,
        VEL_ACTURAL,
        VEL_DEMAND,
        POS_SET,
        POS_PROPORTIONAL,
        POS_INTEGRAL,
        POS_DIFFERENTIAL,
        POS_MINIMAL,
        POS_MAXIMUM,
        POS_STEP_ADD,
        POS_MIN_POS,
        POS_MAX_POS,
        POS_HOMING,
        POS_ACTURAL,
        POS_DEMAND,
        PROFILE_POS_MAX_SPEED,
        PROFILE_POS_ACC,
        PROFILE_POS_DEC,
        PROFILE_VEL_MAX_SPEED,
        PROFILE_VEL_ACC,
        PROFILE_VEL_DEC,
        CHART_FREQUENCY,
        CHART_THRESHOLD,
        POS_OFFSET,
        VOLTAGE,
        HOMING_LIMIT,//if
        HOMING_CUR_MAX,
        HOMING_CUR_MIN,
        CURRENT_SCALE,
        VELOCITY_SCALE,
        FILTER_C_STATUS,
        FILTER_C_VALUE,
        FILTER_V_STATUS,
        FILTER_V_VALUE,
        FILTER_P_STATUS,
        FILTER_P_VALUE,
        INERTIA,
        LOCK_ENERGY,
        TEMP_MOTOR,
        TEMP_INVERTER,
        TEMP_PROTECT,
        TEMP_RECOVERY,
        CALIBRATION_SWITCH,
        CALIBRATION_ANGLE,
        MOTOR_SWITCH,
        MOTOR_VERSION,//
        MOTOR_MODE,
        CURRENT_MAX_VEL,
        DATA_CNT,
        DATA_CHART,//special data,don't need to save
        DATA_INVALID,
    };

    explicit MotorForm(const quint8 nDeviceId,const quint32 nDeviceMac,QWidget *parent = 0);
    quint8 deviceId() const;
    quint32 deviceMac()const{
        return m_deviceMac;
    }
    QString motorCode()const;
    void setValueByProxy(const int nProxyId,const qreal value);//save value that recive
    void requestAllValue();
    void setPosPercent(const qreal xPercent,const qreal yPercent);
    void getPosPercent(qreal &xPercent,qreal &yPercent);
    void changeStatus(MotorStatus status);
    void selectMotor();
    void unselectMotor();
    qreal getValue(Motor_Data_Id nDataId)const;
    void setValueByUser(Motor_Data_Id nDataId,qreal value,bool bSendProxy=true);//set value by user, will send proxy or not
    void responseHeart(bool bSuccess);
    bool isOnline()const;
    bool isSelected()const;
    void errorOccur(const int nErrorId);
    void clearError();
    void changeDeviceId(quint8 nNewDeviceId);
    QVector<qint16> errorHistory()const{
        return m_errorHistory;
    }
    qint16 currentError()const{
        return m_curErrorId;
    }
    Motor_Mode currentMode()const{
        return (Motor_Mode)((int)m_motorData[MOTOR_MODE]);
    }
    ~MotorForm();
    void setDeviceIdSuccessfully();
    qint16 getOldDeviceId()const{
        return m_oldDeviceId;
    }
    bool deviceIdIsAvailable()const;
    void setDetailWnd(InnfosWindow * pDetail);
    void changeDemand(qreal value);//demand value change
    void showGraph();
    void closeGraph();
    void setRequestActual(bool bRequest){
        m_bRequestActual = bRequest;
    }

protected:
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void focusInEvent(QFocusEvent *event)Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event)Q_DECL_OVERRIDE;
    void showVersion();
    void switchMotorSuccessfully(bool bOn);
public slots:
    void buttonChanged(const int nMode);
    void activeMode(const int nMode);
    void activeModeSuccessfully();
    void detailClosed();
    void saveData();
    void loadData();
    void reconnect();

private slots:
    void refreshCurShow(Motor_Data_Id Id);
    void logData(Motor_Data_Id Id);
    void enableChildren(bool bEnable);
    void requestActualValue();//request value initiatively
    void setOnlineStatus(bool bOnline);
    void on_macEdit_returnPressed();

private:
    void saveDataToFile(QString fileName);
    void readDataFromFile(QString fileName);
    void readParams(QXmlStreamReader *reader);
    void changeColor();
    void getLimit(qreal & min, qreal & max);
signals:
    void dataChange(Motor_Data_Id Id);
    void acturalVauleChange(Motor_Mode Id,QVector<qreal> values);//only use by chart
    void beSelected(bool bSelected);
    void onlineChange(bool bOnline);
    void currentErrorChange(const int nErrorId);
private:
    Ui::MotorForm *ui;
private:
    QPoint m_ptDragPoint;
    QString m_motorCode;//no use
    int m_localId;//local id no use
    quint8 m_deviceId;//device id
    qint16 m_oldDeviceId;//old device id,when new device id is available,is -1
    quint32 m_deviceMac;//mac,never change
    //int m_modeId;
    int m_requestModeId;
    qreal m_xPercent;//positon relative to parent
    qreal m_yPercent;
    MotorStatus m_status;

    qreal m_motorData[DATA_CNT];

    QVector<qreal> m_acturalCurs;
    QVector<qreal> m_acturalVels;
    QVector<qreal> m_acturalPoss;
    QVector<qreal> m_chartCurs;
    QVector<qreal> m_chartVels;
    QVector<qreal> m_chartPoss;
    bool m_bPressed;
    InnfosWindow * m_pDetail;
    int m_nHeartFailCnt;//
    QTimer * m_pHeartTimer;
    QTimer * m_pValueTimer;
    QVector<qint16> m_errorHistory;
    qint16 m_curErrorId;
    bool m_bRequestActual;
#ifdef RECORD_DATA
    //log file
    QFile * m_pFileLog;
#endif
    bool m_bCalibrating;
};

#define motorMgrInst MotorMgr::getInstance()
class MotorMgr : public QWidget
{
    Q_OBJECT
public:
    static MotorMgr * getInstance();
    static void autoDestroy();
    static void initAllMotors(QMap<quint8,quint32>info);
	explicit MotorMgr(QWidget * parent=0);
    virtual ~MotorMgr() {}
    void AddMotor(const quint32 nDeviceMac, const quint8 nDeviceId);
    void RmMotorFromMap(const quint8 nDeviceId);
    void AddMotorToMap(MotorForm * pMotor);
    MotorForm * getCurSelected();
    MotorForm * getMotorByDeviceId(const int nDeviceId)const;
    MotorForm * getMotorByMac(const quint32 nMac)const;
    MotorForm * getDefalutMotor()const;//if no motor selected, return the defalut motor from map
    quint8 findAvailavleDeviceId();
    bool deviceIdHasExist(const quint8 nDeviceId);
    QVector<MotorForm *> allSelectedMotors();
    void setRequestActual(bool bRequest);
    void enterDetailWindow();
    QPoint availablePos(const QPoint oriPos,const QSize availableSize);//return available pos that motors can move to avoid motors out of boundins.
    bool isMultiSelected(MotorForm * pSelect)const;
    void CheckServosSwitch(quint32 nDelaymsec=100);
protected:
    //void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
    void saveLayoutToFile(QString fileName);
    void readLayoutFromFile(QString fileName);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    void changeSwitchBtnStatus(bool bOn);

public slots:
    void AdjustLayout();
    void updateMotorGeometry(MotorForm *pMotor);
    void updateMotorPosPercent(MotorForm *pMotor);
    void selectedChange();
    void errorOccur(const int nDeviceId,const qint32 nMsgId);
    void clearError(const int nDeviceId);
    void saveMotorsLayout();
    void readMotorsLayout();
    void onVoltageChange(MotorForm::Motor_Data_Id id);
signals:
    void countChange();
private:
    QMap<quint8,MotorForm *> m_motors;
    static MotorMgr * m_pMotorsMgr;
    MotorForm * m_pCurSelected;

    //test use
    QTimer * m_pTestTimer;
    QPushButton * m_pTestButton;
    QPushButton * m_pCurButton;
    QPushButton * m_pVeltButton;
    QPushButton * m_pPosButton;
    QPushButton * m_pSwitch;
    QLineEdit * m_pMin;
    QLineEdit * m_pMax;
    QLineEdit * m_pInterval;
    int m_nMode;
    qreal m_min;
    qreal m_max;
    int m_interval;
    bool m_bMin;
    //test end
    bool m_bCtrlPressed;
    EditorWidget * m_pEditor;
    QLCDNumber * m_pVoltage;
private slots:
    void buttonCallback1(bool bChecked);
public:
    void triggerSend();
};

#endif // MOTORFORM_H
