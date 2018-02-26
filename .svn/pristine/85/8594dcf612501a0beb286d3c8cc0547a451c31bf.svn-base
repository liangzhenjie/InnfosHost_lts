#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include <QWidget>
#include "motorform.h"

class QLineEdit;
class QPushButton;
class QTableWidget;
class QCheckBox;
class InnfosChartWidget;
class WaveTriggerWidget;
class AngleClock;
class MyDoubleSpinBox;
class QButtonGroup;
class QCheckBox;

class ParamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParamWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    virtual void initData()=0;
    virtual ~ParamWidget(){}
    bool isActivate();
    enum SPIN_ATTR{
        MIN_VALUE,
        MAX_VALUE,
        STEP_VALUE,
        ATTR_CNT
    };
public slots:
    virtual void valueChangeByUser()=0;//user modify data
    virtual void motorDataChange(int nId)=0;//motor data change acturally
    void activeMode(bool bActive);//active mode or not
    virtual void enableMode(bool bEnable)=0;//
    MotorForm::Motor_Mode getModeId()const{
        return m_modeId;
    }
    virtual void onlineStatus(bool bOnline);
    void currentErrorChange(qint16 nCurErrorId);
    void showGraphWithRect(QRect rc);
    void showGraph();
    void closeGraph();
    bool ChartShowing()const{return m_pGraph != nullptr;}
    QRect chartGeometry()const;
    void checkError();
protected:
    void showErrorHistroy();
    void errorBtnChange(bool bHasError);
protected:
    QTableWidget * m_pTableErrors;
    QPushButton * m_pClearErrors;
    QPushButton * m_pErrorInfos;
    QPushButton * m_pStatus;
    QPushButton * m_pBtnActiveMode;
    WaveTriggerWidget * m_pWaveWidget;
private:
    MotorForm::Motor_Mode m_modeId;
    InnfosChartWidget * m_pGraph;

};

class CurWidget : public ParamWidget
{
    Q_OBJECT
public:
    explicit CurWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    enum CurParams{
        IQ_SET,
        PROPORTIONAL,
        INTEGRAL,
        ID_SET,
        MINIMAL,
        MAXIMUM,
        NOMINAL,
        OUTPUT,
        MAXSPEED,

        CUR_PARAM_CNT,
    };

    enum CurActuals{
        ACTUAL,
        DEMAND,
        VELOCITY,
        POSITION,
        VOLTAGE,
        TEMP_MOTOR,
        TEMP_INVERTER,
        ACTUAL_CNT
    };

signals:
public:
    void initData();
public slots:
    void valueChangeByUser();
    void motorDataChange(int nId);
    virtual void enableMode(bool bEnable);
    void clock();
private:
    MyDoubleSpinBox * m_pParams[CUR_PARAM_CNT];
    QLineEdit * m_pActuals[ACTUAL_CNT];
    QLineEdit * m_pModeEdit;

    QPushButton * m_pBtnEffect;
    QPushButton * m_pBtnHalt;
    QPushButton * m_pViewGraph;
    int m_nParamsDataId[CUR_PARAM_CNT];
    int m_nActualsDataId[ACTUAL_CNT];
    AngleClock * m_pClock;
    QPushButton * m_pSwitch;
};

class VelWidget : public ParamWidget
{
    Q_OBJECT
public:
    explicit VelWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    enum VelParams{
        SET,
        PROPORTIONAL,
        INTEGRAL,
        MINIMAL,
        MAXIMUM,

        VEL_PARAM_CNT,
    };

    enum VelActual{
        ACTUAL,
        DEMAND,
        CURRENT,
        POSITION,
        VOLTAGE,
        TEMP_MOTOR,
        TEMP_INVERTER,
        ACTUAL_CNT
    };

signals:

public:
    void initData();
public slots:
    void valueChangeByUser();
    void motorDataChange(int nId);
    virtual void enableMode(bool bEnable);
    void clock();
private:
    MyDoubleSpinBox * m_pParams[VEL_PARAM_CNT];
    QLineEdit * m_pActuals[ACTUAL_CNT];
    QLineEdit * m_pModeEdit;
    //QPushButton * m_pBtnActiveMode;
    QPushButton * m_pBtnEffect;
    QPushButton * m_pBtnHalt;
    QPushButton * m_pViewGraph;
    int m_nParamsDataId[VEL_PARAM_CNT];
    int m_nActualsDataId[ACTUAL_CNT];
    AngleClock * m_pClock;
};

class PosWidget : public ParamWidget
{
    Q_OBJECT
public:
    explicit PosWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    enum PosParams{
        SET,
        PROPORTIONAL,
        INTEGRAL,
        DIFFERENTIAL,
        MINIMAL,
        MAXIMUM,
        STEP_ADD,
        MIN_POS,
        MAX_POS,
        POS_OFFSET,

        POS_PARAM_CNT,
    };

    enum{
        ACTUAL,
        DEMAND,
        CURRENT,
        VELOCITY,
        VOLTAGE,
        TEMP_MOTOR,
        TEMP_INVERTER,
        ACTUAL_CNT
    };

signals:

public:
    void initData();
public slots:
    void valueChangeByUser();
    void motorDataChange(int nId);
    virtual void enableMode(bool bEnable);
    void stepAdd();
    void stepMinus();
    void clock();
private:
    MyDoubleSpinBox * m_pParams[POS_PARAM_CNT];
    QLineEdit * m_pActuals[ACTUAL_CNT];
    QLineEdit * m_pModeEdit;
    //QPushButton * m_pBtnActiveMode;
    QPushButton * m_pBtnStepAdd;
    QPushButton * m_pBtnStepMinus;
    QPushButton * m_pBtnEffect;
//    QPushButton * m_pBtnCalibrate;
//    QPushButton * m_pBtnHoming;

    QPushButton * m_pViewGraph;
    QCheckBox * m_pMinEnable;
    QCheckBox * m_pMaxEnable;
    int m_nParamsDataId[POS_PARAM_CNT];
    int m_nActualsDataId[ACTUAL_CNT];
    AngleClock * m_pClock;
};


class ProfilePosWidget : public ParamWidget
{
    Q_OBJECT
public:
    explicit ProfilePosWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    enum ProfilePosParams{
        TARGET,
        PROFILE,
        ACCELERATE,
        DECELERATE,
        MAX_SPEED,
        STEP_ADD,
        MIN_POS,
        MAX_POS,
        POS_OFFSET,
        ACTUAL,
        DEMAND,
        CURRENT,
        VELOCITY,
        VOLTAGE,
        TEMP_MOTOR,
        TEMP_INVERTER,
        PROFILE_POS_PARAM_CNT,
    };

signals:

public:
    void initData();
public slots:
    void valueChangeByUser();
    void motorDataChange(int nId);
    virtual void enableMode(bool bEnable);
    void clock();
private:
    QWidget * m_pParams[PROFILE_POS_PARAM_CNT];
    QLineEdit * m_pModeEdit;
    //QPushButton * m_pBtnActiveMode;
    QPushButton * m_pBtnStepAdd;
    QPushButton * m_pBtnStepMinus;
    QPushButton * m_pBtnHalt;

    QPushButton * m_pViewGraph;
    int m_nDataId[PROFILE_POS_PARAM_CNT];
    AngleClock * m_pClock;
};

class ProfileVelWidget : public ParamWidget
{
    Q_OBJECT
public:
    explicit ProfileVelWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    enum ProfileVelParams{
        TARGET,
        PROFILE,
        ACCELERATE,
        DECELERATE,
        MAX_SPEED,
        ACTUAL,
        DEMAND,
        CURRENT,
        POSITION,
        VOLTAGE,
        TEMP_MOTOR,
        TEMP_INVERTER,
        PROFILE_VEL_PARAM_CNT,
    };

signals:

public:
    void initData();
public slots:
    void valueChangeByUser();
    void motorDataChange(int nId);
    virtual void enableMode(bool bEnable);
    void clock();
private:
    QWidget * m_pParams[PROFILE_VEL_PARAM_CNT];
    QLineEdit * m_pModeEdit;
    //QPushButton * m_pBtnActiveMode;
    QPushButton * m_pBtnAbsolute;
    QPushButton * m_pBtnRelative;
    QPushButton * m_pBtnHalt;

    QPushButton * m_pViewGraph;
    int m_nDataId[PROFILE_VEL_PARAM_CNT];
    AngleClock * m_pClock;
};

class HomingWidget : public ParamWidget
{
    Q_OBJECT
public:
    explicit HomingWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    enum HomingParams{
        MAX_POS,
        MIN_POS,
        VEL_SET,
        VEL_ADD,
        VEL_MIN,
        VEL_MAX,
        CURRENT,
        VELOCITY,
        POSITION,
        POS_OFFSET,
        INERTIA,
        LOCK_ENERGY,
        TEMP_PROTECT,
        TEMP_RECOVERY,
        HOMING_PARAM_CNT,
    };

signals:

public:
    void initData();
public slots:
    void valueChangeByUser();
    void motorDataChange(int nId);
    virtual void enableMode(bool bEnable);
    void homingCallback();
    void homingCallback2();
    void velAddCallback();
    void velMinusCallback();
protected slots:
    void operationModeChanged(int nChangeId);
private:
    void setVelocity(qreal vel);
private:
    QLineEdit * m_pParams[HOMING_PARAM_CNT];
    QLineEdit * m_pModeEdit;
    //QPushButton * m_pBtnActiveMode;
    QPushButton * m_pBtnCalibrate;
    QPushButton * m_pBtnHoming;
    QPushButton * m_pBtnVelAdd;
    QPushButton * m_pBtnVelMinus;
    QPushButton * m_pBtnHalt;
    QPushButton * m_pReset;
    QPushButton * m_pBtnAuto;
    QPushButton * m_pBtnManual;
    QCheckBox * m_pNeedLimit;
    QButtonGroup * m_pGroup;
    QPushButton * m_pSetMin;
    QPushButton * m_pSetMax;
    //QPushButton * m_pViewGraph;
    int m_nDataId[HOMING_PARAM_CNT];
};
#endif // PARAMWIDGET_H
