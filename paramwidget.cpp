﻿#include "paramwidget.h"
#include "innfosutil.h"
#include <qpalette.h>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QCheckBox>
#include "mediator.h"
#include <QStringList>
#include "innfosproxy.h"
#include "innfoschartwidget.h"
#include <QApplication>
#include <QComboBox>
#include "wavetriggerwidget.h"
#include "angleclock.h"
#include "mydoublespinbox.h"
#include <QButtonGroup>
#include <QCheckBox>
#include "innfoswindow.h"
#include "msgbox.h"
#include <QDebug>

ParamWidget::ParamWidget(quint8 nDeviceId, MotorForm::Motor_Mode modeId, QWidget *parent):
    QWidget(parent),
    m_modeId(modeId),
    m_pClearErrors(nullptr),
    m_pErrorInfos(nullptr),
    m_pTableErrors(nullptr),
    m_pGraph(nullptr),
    m_pBtnActiveMode(nullptr),
    m_pWaveWidget(nullptr),
    m_nDeviceId(nDeviceId)
{
    m_pTableErrors = new QTableWidget(0,1,this);
    m_pTableErrors->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStringList labels;
    labels << tr("ErrorCode");
    m_pTableErrors->setHorizontalHeaderLabels(labels);

    m_pClearErrors = new QPushButton(tr("No Errors"),this);
    m_pClearErrors->setObjectName("clearError");

    m_pStatus = new QPushButton(tr("offline"),this);
    m_pStatus->setObjectName("status");


    Mediator * pMediator = Mediator::getInstance();
    errorBtnChange(pMediator->curErrorId() > 0);
    qDebug() << "ErrorID"<<tr("%1").arg(pMediator->curErrorId());
    connect(pMediator,&Mediator::curErrorChange,this,&ParamWidget::currentErrorChange);
    connect(m_pClearErrors,&QPushButton::clicked,[=]{
        pMediator->clearDeviceError();
    });
    connect(m_pStatus,&QPushButton::clicked,pMediator,&Mediator::reconnectDevice);

    showErrorHistroy();
}

bool ParamWidget::isActivate()
{
    return Mediator::getInstance()->curDeviceMode() == m_modeId;
}

void ParamWidget::activeMode(bool bActive)
{
    if(bActive)
        Mediator::getInstance()->activeMode(m_modeId);
}

void ParamWidget::onlineStatus(bool bOnline)
{
    if(m_pBtnActiveMode)
    {
        m_pBtnActiveMode->setEnabled(bOnline);
    }
    if(bOnline)
    {
        if(m_pStatus->property("offline") == true && isActivate())//reconnect successfully
        {
            enableMode(bOnline);
        }
        m_pStatus->setText(tr("online"));
        m_pStatus->setProperty("offline",false);
        m_pStatus->setStyle(QApplication::style());
    }
    else
    {
        m_pStatus->setText(tr("offline"));
        m_pStatus->setProperty("offline",true);
        m_pStatus->setStyle(QApplication::style());
        enableMode(bOnline);

    }
}

void ParamWidget::currentErrorChange(qint16 nCurErrorId)
{
    if(nCurErrorId == 0)
    {
        errorBtnChange(false);
    }
    else
    {
        errorBtnChange(true);
        showErrorHistroy();
    }
}

void ParamWidget::showGraphWithRect(QRect rc)
{
    showGraph();
    if(m_pGraph)
    {
        if(rc.isValid())
        {
            m_pGraph->setGeometry(rc);
            m_pGraph->show();
        }
    }
}

void ParamWidget::showGraph()
{
    Mediator::getInstance()->openChart();
// below is separate to many graphs;
//    if(m_pGraph)
//    {
//        //m_pGraph->show();
//        return;
//    }
//    InnfosChartWidget * pWidget = InnfosChartWidget::creatChartWidget(m_modeId,nullptr);
//    if(pWidget)
//    {
//        m_pGraph = pWidget;

//        connect(Mediator::getInstance(),&Mediator::chartVauleChange,m_pGraph,&InnfosChartWidget::addValues);
//        connect(Mediator::getInstance(),&Mediator::dataChange,m_pGraph,&InnfosChartWidget::onDemandChanged);
//        m_pGraph->show();
//        connect(this,&QWidget::destroyed,m_pGraph,&QWidget::deleteLater);
//        connect(m_pGraph,&InnfosChartWidget::closeItSelf,[=]{m_pGraph=nullptr;Mediator::getInstance()->closeChart();});
//        Mediator::getInstance()->openChart();
//    }
}

void ParamWidget::closeGraph()
{
    if(m_pGraph)
    {
        m_pGraph->close();
        m_pGraph = nullptr;
        Mediator::getInstance()->closeChart();
    }
}

QRect ParamWidget::chartGeometry() const
{
    QRect rc(0,0,-1,-1);
    if(m_pGraph)
    {
        rc = m_pGraph->geometry();
    }

    return rc;
}

void ParamWidget::checkError()
{
    Mediator::getInstance()->checkError();
}

void ParamWidget::showErrorHistroy()
{
    QVector<qint16> errors = Mediator::getInstance()->errorHistory();
    int nCount = m_pTableErrors->rowCount();
    for(int i=nCount;--i>=0;)
        m_pTableErrors->removeRow(i);
    for (int i=0;i<errors.count();++i)
    {
        m_pTableErrors->insertRow(i);
        QString key = "0x";
        key += QString("%1").arg(errors.at(i),4,16,QLatin1Char('0'));
        QTableWidgetItem * pItem = new QTableWidgetItem(key);
        m_pTableErrors->setItem(i,0,pItem);
//        if(errors.count()-1 == i)
//            m_pTableErrors->scrollToItem(pItem);
    }
}

void ParamWidget::errorBtnChange(bool bHasError)
{
    if(m_pClearErrors)
    {
        if(bHasError)
        {
            m_pClearErrors->setText(tr("Clear Errors"));
            m_pClearErrors->setProperty("hasError",true);
            m_pClearErrors->setStyle(QApplication::style());
        }
        else
        {
            m_pClearErrors->setText(tr("No Errors"));
            m_pClearErrors->setProperty("hasError",false);
            m_pClearErrors->setStyle(QApplication::style());
        }
    }

}


CurWidget::CurWidget(quint8 nDeviceId,MotorForm::Motor_Mode modeId,QWidget *parent) :
    ParamWidget(nDeviceId,modeId,parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    modifyPalette(this,QPalette::Window,QColor(41,60,75));
    //resize(700,600);
    setAutoFillBackground(true);

    QGridLayout * pLayout = new QGridLayout(this);
    pLayout->setContentsMargins(40,5,40,5);
    pLayout->setVerticalSpacing(3);
    pLayout->setColumnStretch(0,1);

    QLabel * pDetail = new QLabel(this);
    pDetail->setPixmap(QIcon(":/images/detail_cur.png").pixmap(800,120));
    pLayout->addWidget(pDetail,0,0,1,2,Qt::AlignHCenter | Qt::AlignVCenter);

    QGroupBox * pOperationMode = new QGroupBox(tr("Operation Mode"),this);
    QHBoxLayout * pModeLayout = new QHBoxLayout();
    pModeLayout->setContentsMargins(0,5,0,0);
    pModeLayout->addStretch(3);
    pModeLayout->setSpacing(3);
    pModeLayout->addWidget(new QLabel(tr("Activate Mode")));
    m_pModeEdit = new QLineEdit(this);
    m_pModeEdit->setFixedWidth(155);
    m_pModeEdit->setPlaceholderText(tr("Unkonw Mode"));
    m_pModeEdit->setAlignment(Qt::AlignHCenter);
    m_pModeEdit->setReadOnly(true);
    pModeLayout->addWidget(m_pModeEdit);
    m_pBtnActiveMode = new QPushButton(tr("Active Current Mode"),this);
    pModeLayout->addWidget(m_pBtnActiveMode);
    connect(m_pBtnActiveMode,&QPushButton::clicked,this,&ParamWidget::activeMode);
    m_pBtnActiveMode->setFixedWidth(155);
    m_pBtnActiveMode->setCheckable(true);
    pOperationMode->setLayout(pModeLayout);
    pLayout->addWidget(pOperationMode,1,0,1,2);

    QGroupBox * pRegulation = new QGroupBox(tr("Regulation"),this);
    QGridLayout * pReguLayout = new QGridLayout();
    pReguLayout->setContentsMargins(0,5,0,0);
    pReguLayout->setVerticalSpacing(3);
    pReguLayout->setColumnStretch(2,5);
    const char * name[CUR_PARAM_CNT] = {"Iq Setting","Proportional","Integral","Id Setting","Minimal","Maximum",
                                       "Nominal","Output Current","Max Velocity"};
    qreal curScale = Mediator::getInstance()->curCurrentScale();
    qreal velScale = Mediator::getInstance()->curVelocityScale();
    double spinBoxAttr[CUR_PARAM_CNT][ATTR_CNT]={-curScale,curScale,0.1,
                                                 0,100,0.1,
                                                 0,10,0.01,
                                                 -curScale,curScale,0.01,
                                                 -1,0,0.1,
                                                 0,1,0.1,
                                                 -1,1,0.001,
                                                 -1,1,0.001,
                                                 -velScale,velScale,10
                                                };

    for(int i=0;i<NOMINAL;++i)
    {
        QLabel * pLabel = new QLabel(tr(name[i]));
        pLabel->setFixedWidth(90);
        pReguLayout->addWidget(pLabel,i,0,1,1);
        m_pParams[i] = new MyDoubleSpinBox(this);
        m_pParams[i]->setMinimum(spinBoxAttr[i][MIN_VALUE]);
        m_pParams[i]->setMaximum(spinBoxAttr[i][MAX_VALUE]);
        m_pParams[i]->setSingleStep(spinBoxAttr[i][STEP_VALUE]);
        m_pParams[i]->setDecimals(4);
        pReguLayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(180);
        if(i== ID_SET || i==IQ_SET)
        {
            QLabel * pTag = new QLabel(tr("A"));
            pReguLayout->addWidget(pTag,i,2,1,1);
            m_pParams[i]->setEmitFinishedOnLoseFocus(false);
            //m_pParams[i]->setSuffix("A");
        }
    }
    m_pBtnEffect = new QPushButton(tr("Set Current"),this);
    m_pBtnEffect->setFixedWidth(130);
    pReguLayout->addWidget(m_pBtnEffect,4,4,1,1);
    connect(m_pBtnEffect,&QPushButton::clicked,[=]{
        m_pParams[IQ_SET]->myValueChanged(m_pParams[IQ_SET]->value());
    });

    m_pBtnHalt = new QPushButton(tr("Halt"),this);
    m_pBtnHalt->setFixedWidth(130);
    connect(m_pBtnHalt,&QPushButton::clicked,[=]{
        m_pParams[IQ_SET]->setValue(0);
        m_pParams[IQ_SET]->myValueChanged(0);
    });
    pReguLayout->addWidget(m_pBtnHalt,5,4,1,1);
    m_pBtnHalt->setEnabled(false);
    pRegulation->setLayout(pReguLayout);
    pLayout->addWidget(pRegulation,2,0,1,2);

    QGroupBox * pParams = new QGroupBox(tr("Regulation"),this);
    QGridLayout * pParamLayout = new QGridLayout();
    pParamLayout->setContentsMargins(0,5,0,0);
    pParamLayout->setVerticalSpacing(3);
    pParamLayout->setColumnStretch(2,5);
    for(int i=NOMINAL;i<CUR_PARAM_CNT;++i)
    {
        QLabel * pLabel = new QLabel(tr(name[i]));
        pLabel->setFixedWidth(90);
        pParamLayout->addWidget(pLabel,i,0,1,1);
        m_pParams[i] = new MyDoubleSpinBox(this);
        m_pParams[i]->setMinimum(spinBoxAttr[i][MIN_VALUE]);
        m_pParams[i]->setMaximum(spinBoxAttr[i][MAX_VALUE]);
        m_pParams[i]->setSingleStep(spinBoxAttr[i][STEP_VALUE]);
        pParamLayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(180);
    }
    pParams->setLayout(pParamLayout);
    pLayout->addWidget(pParams,3,0,1,1);

    QGroupBox * pActuals = new QGroupBox(tr("Actual Values"),this);
    QGridLayout * pActualayout = new QGridLayout();
    pActualayout->setContentsMargins(0,5,0,0);
    pActualayout->setVerticalSpacing(3);
    const char * acturalName[ACTUAL_CNT] = {"Actual Current","Current Demand","Actual Velocity","Actual Position",
                                            "Actual Voltage","Motor Temperature","Inverter Temperature"};
    for(int i=ACTUAL;i<ACTUAL_CNT;++i)
    {
        pActualayout->addWidget(new QLabel(tr(acturalName[i])),i,0,1,1);
        m_pActuals[i] = new QLineEdit(this);
        pActualayout->addWidget(m_pActuals[i],i,1,1,1);
        m_pActuals[i]->setFixedWidth(115);
        m_pActuals[i]->setReadOnly(true);
        switch(i)
        {
        case ACTUAL:
        case DEMAND:
        {
            QLabel * pTag = new QLabel(tr("A"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case VELOCITY:
        {
            QLabel * pTag = new QLabel(tr("RPM"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case POSITION:
        {
            QLabel * pTag = new QLabel(tr("R"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case VOLTAGE:
        {
            QLabel * pTag = new QLabel(tr("V"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case TEMP_INVERTER:
        case TEMP_MOTOR:
        {
            QLabel * pTag = new QLabel(QString::fromLocal8Bit("℃"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        default:
            break;
        }

        //m_pActuals[i]->setUserData();
    }

    pActuals->setLayout(pActualayout);
    pLayout->addWidget(pActuals,3,1,1,1);

    QGroupBox * pError = new QGroupBox(tr("Error"),this);
    QGridLayout * pErrorLayout = new QGridLayout();
    pErrorLayout->setContentsMargins(0,5,0,0);
    m_pClearErrors->setFixedWidth(134);
    pErrorLayout->addWidget(m_pClearErrors,0,0,1,1);
    m_pErrorInfos = new QPushButton(tr("Error Info"),this);
    m_pErrorInfos->setFixedWidth(134);
    pErrorLayout->addWidget(m_pErrorInfos,1,0,1,1);
    //connect(m_pErrorInfos,&QPushButton::clicked,this,&ParamWidget::checkError);

    pErrorLayout->addWidget(m_pTableErrors,0,1,4,1);
    pError->setLayout(pErrorLayout);
    pLayout->addWidget(pError,4,0,1,2);

    QGroupBox * pEnd = new QGroupBox(this);
    QHBoxLayout * pEndLayout = new QHBoxLayout();
    pEndLayout->addWidget(new QLabel(tr("Status")));

    QGroupBox * pWaveGrid = new QGroupBox(tr("Wave Generator"),this);
    QHBoxLayout * pWaveLayout = new QHBoxLayout();
    pWaveLayout->setContentsMargins(0,5,0,0);
    m_pWaveWidget = new WaveTriggerWidget(this);
    pWaveLayout->addWidget(m_pWaveWidget);
    m_pSwitch = new QPushButton("IqTrigger");
    m_pSwitch->setCheckable(true);
    m_pSwitch->setFixedWidth(130);
    pWaveLayout->addWidget(m_pSwitch,0,Qt::AlignRight);
    connect(m_pSwitch,&QPushButton::clicked,[=](bool bChecked){
        quint8 nMode = bChecked?1:0;
        QString name = bChecked?"IdTrigger":"IqTrigger";
        m_pSwitch->setText(name);
        Mediator::getInstance()->setTriggerMode(nMode);
    });

    connect(m_pWaveWidget,&WaveTriggerWidget::valueChanged,[=](qreal value){
        if(m_pSwitch->isChecked())
        {
            m_pParams[ID_SET]->setValue(value);
            m_pParams[ID_SET]->myValueChanged(value);//emit signal
        }
        else
        {
            m_pParams[IQ_SET]->setValue(value);
            m_pParams[IQ_SET]->myValueChanged(value);//emit signal
        }

    });

    pWaveGrid->setLayout(pWaveLayout);
    pLayout->addWidget(pWaveGrid,5,0,1,2);

    m_pStatus->setFixedWidth(97);
    pEndLayout->addWidget(m_pStatus);
    pEndLayout->addStretch(4);
    m_pViewGraph = new QPushButton(tr("View Graph"),this);
    m_pViewGraph->setFixedWidth(157);
    pEndLayout->addWidget(m_pViewGraph);
    connect(m_pViewGraph,&QPushButton::clicked,this,&ParamWidget::showGraph);
    pEnd->setLayout(pEndLayout);
    pLayout->addWidget(pEnd,6,0,1,2);

    setLayout(pLayout);
    initData();
}

void CurWidget::initData()
{
    for(int i=IQ_SET;i<CUR_PARAM_CNT;++i)
        m_nParamsDataId[i] = i;
    m_nParamsDataId[MAXSPEED] = MotorForm::CURRENT_MAX_VEL;
    m_nActualsDataId[ACTUAL] = MotorForm::CUR_ACTURAL;
    m_nActualsDataId[DEMAND] = MotorForm::CUR_DEMAND;
    m_nActualsDataId[VELOCITY] = MotorForm::VEL_ACTURAL;
    m_nActualsDataId[POSITION] = MotorForm::POS_ACTURAL;
    m_nActualsDataId[VOLTAGE] = MotorForm::VOLTAGE;
    m_nActualsDataId[TEMP_MOTOR] = MotorForm::TEMP_MOTOR;
    m_nActualsDataId[TEMP_INVERTER] = MotorForm::TEMP_INVERTER;
    for(int i=IQ_SET;i<CUR_PARAM_CNT;++i)
    {
        //m_pParams[i]->setValidator(new QDoubleValidator);
        motorDataChange(m_nDeviceId,m_nParamsDataId[i]);
        connect(m_pParams[i],&MyDoubleSpinBox::myValueChanged,this,&CurWidget::valueChangeByUser);
        //connect(m_pParams[i],&MyDoubleSpinBox::stepByChange,this,&CurWidget::valueChangeByUser);
    }

    for(int i=ACTUAL; i<ACTUAL_CNT;++i)
    {
        motorDataChange(m_nDeviceId,m_nActualsDataId[i]);
        connect(m_pActuals[i],&QLineEdit::editingFinished,this,&CurWidget::valueChangeByUser);
    }

    connect(Mediator::getInstance(),&Mediator::dataChange,this,&CurWidget::motorDataChange);
    connect(Mediator::getInstance(),&Mediator::onlineChange,this,&CurWidget::onlineStatus);
//    m_pParams[DEMAND]->setReadOnly(true);
//    m_pParams[ACTUAL]->setReadOnly(true);
    connect(m_pParams[IQ_SET],static_cast<void(MyDoubleSpinBox::*)(const QString &)>(&MyDoubleSpinBox::valueChanged),m_pActuals[DEMAND],&QLineEdit::setText);
}

void CurWidget::valueChangeByUser()
{
    MyDoubleSpinBox * pSender = qobject_cast<MyDoubleSpinBox *>(sender());
    for(int i=IQ_SET;i<CUR_PARAM_CNT;++i)
    {
        if(m_pParams[i] == pSender)
        {
            qreal value = pSender->value();
            Mediator::getInstance()->setValueByUser((MotorForm::Motor_Data_Id)m_nParamsDataId[i],value);
        }
    }
}

void CurWidget::motorDataChange(quint8 nDeviceId, int nId)
{
    if(!isCurrentDevice(nDeviceId))
        return;
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    for(int i=ACTUAL;i<ACTUAL_CNT;++i)
    {
        if(m_nActualsDataId[i] == nId)
        {
            m_pActuals[i]->setText(QString::number(Mediator::getInstance()->getValue((MotorForm::Motor_Data_Id)nId),'f',6).replace(rx,""));
            break;
        }
    }

    for(int i=IQ_SET;i<CUR_PARAM_CNT;++i)
    {
        if(m_nParamsDataId[i] == nId)
        {
            m_pParams[i]->setValue(Mediator::getInstance()->getValue((MotorForm::Motor_Data_Id)nId));
            break;
        }
    }
}

void CurWidget::enableMode(bool bEnable)
{
    if(bEnable)
        m_pModeEdit->setText(tr("Current Mode"));
    else
    {
        m_pModeEdit->setText("");
        m_pWaveWidget->stop();
    }


    for(int i=IQ_SET;i<CUR_PARAM_CNT;++i)
    {
        m_pParams[i]->setEnabled(bEnable);
    }
    m_pBtnEffect->setEnabled(bEnable);
    m_pBtnHalt->setEnabled(bEnable);
    m_pViewGraph->setEnabled(bEnable);
    m_pBtnActiveMode->setChecked(bEnable);
    m_pWaveWidget->setEnabled(bEnable);
}

/*速度模式*/
VelWidget::VelWidget(quint8 nDeviceId,MotorForm::Motor_Mode modeId, QWidget *parent) :
    ParamWidget(nDeviceId,modeId,parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    modifyPalette(this,QPalette::Window,QColor(41,60,75));
    //resize(700,600);
    setAutoFillBackground(true);

    QGridLayout * pLayout = new QGridLayout(this);
    pLayout->setContentsMargins(40,5,40,5);
    pLayout->setVerticalSpacing(3);
    pLayout->setColumnStretch(0,1);

    QLabel * pDetail = new QLabel(this);
    pDetail->setPixmap(QIcon(":/images/detail_vel.png").pixmap(800,120));
    pLayout->addWidget(pDetail,0,0,1,2,Qt::AlignHCenter | Qt::AlignVCenter);

    QGroupBox * pOperationMode = new QGroupBox(tr("Operation Mode"),this);
    QHBoxLayout * pModeLayout = new QHBoxLayout();
    pModeLayout->setContentsMargins(0,5,0,0);
    pModeLayout->addStretch(5);
    pModeLayout->setSpacing(3);
    pModeLayout->addWidget(new QLabel(tr("Activate Mode")));
    m_pModeEdit = new QLineEdit(this);
    m_pModeEdit->setFixedWidth(155);
    m_pModeEdit->setPlaceholderText(tr("Unkonw Mode"));
    m_pModeEdit->setAlignment(Qt::AlignHCenter);
    m_pModeEdit->setReadOnly(true);
    pModeLayout->addWidget(m_pModeEdit);
    m_pBtnActiveMode = new QPushButton(tr("Active Velocity Mode"),this);
    pModeLayout->addWidget(m_pBtnActiveMode);
    connect(m_pBtnActiveMode,&QPushButton::clicked,this,&ParamWidget::activeMode);
    m_pBtnActiveMode->setFixedWidth(155);
    m_pBtnActiveMode->setCheckable(true);
    pOperationMode->setLayout(pModeLayout);
    pLayout->addWidget(pOperationMode,1,0,1,2);

    QGroupBox * pRegulation = new QGroupBox(tr("Regulation"),this);
    QGridLayout * pReguLayout = new QGridLayout();
    pReguLayout->setContentsMargins(0,5,0,0);
    pReguLayout->setVerticalSpacing(3);
    pReguLayout->setColumnStretch(2,5);
    const char * name[VEL_PARAM_CNT] = {"Setting","Proportional","Integral","Minimal","Maximum"};
    const char * actualName[ACTUAL_CNT] = {"Actual Velocity","Velocity Demand","Actual Current","Actual Position",
                                           "Actual Voltage","Motor Temperature","Inverter Temperature"};
    double spinBoxAttr[VEL_PARAM_CNT][ATTR_CNT]={-velScale,velScale,100,
                                                 0,100,0.1,
                                                 0,10,0.01,
                                                 -1,0,0.1,
                                                 0,1,0.1

                                                };

    for(int i=SET;i<VEL_PARAM_CNT;++i)
    {
        QLabel * pLabel = new QLabel(tr(name[i]));
        pLabel->setFixedWidth(110);
        pReguLayout->addWidget(pLabel,i,0,1,1);
        m_pParams[i] = new MyDoubleSpinBox(this);
        m_pParams[i]->setMinimum(spinBoxAttr[i][MIN_VALUE]);
        m_pParams[i]->setMaximum(spinBoxAttr[i][MAX_VALUE]);
        m_pParams[i]->setSingleStep(spinBoxAttr[i][STEP_VALUE]);
        m_pParams[i]->setDecimals(4);
        pReguLayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(180);
        if(i== SET)
        {
            QLabel * pTag = new QLabel(tr("RPM"));
            pReguLayout->addWidget(pTag,i,2,1,1);
            m_pParams[i]->setEmitFinishedOnLoseFocus(false);
        }
    }
    m_pBtnEffect = new QPushButton(tr("Set Velocity"),this);
    m_pBtnEffect->setFixedWidth(130);
    connect(m_pBtnEffect,&QPushButton::clicked,[=]{
        m_pParams[SET]->myValueChanged(m_pParams[SET]->value());
    });
    pReguLayout->addWidget(m_pBtnEffect,3,4,1,1);
    m_pBtnHalt = new QPushButton(tr("Halt"),this);
    m_pBtnHalt->setFixedWidth(130);
    connect(m_pBtnHalt,&QPushButton::clicked,[=]{
        m_pParams[SET]->setValue(0);
        m_pParams[SET]->myValueChanged(0);
    });
    pReguLayout->addWidget(m_pBtnHalt,4,4,1,1);
    m_pBtnHalt->setEnabled(false);
    pRegulation->setLayout(pReguLayout);
    pLayout->addWidget(pRegulation,2,0,1,2);

    QGroupBox * pActuals = new QGroupBox(tr("Actual Values"),this);
    QGridLayout * pActualayout = new QGridLayout();
    pActualayout->setContentsMargins(0,5,0,0);
    pActualayout->setVerticalSpacing(3);
    //pActualayout->setColumnStretch(2,5);
    for(int i=ACTUAL;i<ACTUAL_CNT;++i)
    {
        QLabel * pLabel = new QLabel(tr(actualName[i]));
        pLabel->setFixedWidth(110);
        pActualayout->addWidget(pLabel,i,0,1,1);
        m_pActuals[i] = new QLineEdit(this);
        pActualayout->addWidget(m_pActuals[i],i,1,1,1);
        m_pActuals[i]->setFixedWidth(180);
        m_pActuals[i]->setReadOnly(true);
        switch(i)
        {
        case CURRENT:
        {
            QLabel * pTag = new QLabel(tr("A"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case ACTUAL:
        case DEMAND:
        {
            QLabel * pTag = new QLabel(tr("RPM"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case POSITION:
        {
            QLabel * pTag = new QLabel(tr("R"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case VOLTAGE:
        {
            QLabel * pTag = new QLabel(tr("V"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case TEMP_INVERTER:
        case TEMP_MOTOR:
        {
            QLabel * pTag = new QLabel(QString::fromLocal8Bit("℃"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        default:
            break;
        }
    }
    pActuals->setLayout(pActualayout);
    pLayout->addWidget(pActuals,3,0,1,2);

    QGroupBox * pError = new QGroupBox(tr("Error"),this);
    QGridLayout * pErrorLayout = new QGridLayout();
    pErrorLayout->setContentsMargins(0,5,0,0);
    pErrorLayout->setVerticalSpacing(3);
    m_pClearErrors->setFixedWidth(134);
    pErrorLayout->addWidget(m_pClearErrors,0,0,1,1);
    m_pErrorInfos = new QPushButton(tr("Error Info"),this);
    m_pErrorInfos->setFixedWidth(134);
    pErrorLayout->addWidget(m_pErrorInfos,1,0,1,1);
    //connect(m_pErrorInfos,&QPushButton::clicked,this,&ParamWidget::checkError);

    pErrorLayout->addWidget(m_pTableErrors,0,1,4,1);
    pError->setLayout(pErrorLayout);
    pLayout->addWidget(pError,4,0,1,2);


    QGroupBox * pWave = new QGroupBox(tr("Wave Generator"),this);
    QHBoxLayout * pWaveLayout = new QHBoxLayout();
    pWaveLayout->setContentsMargins(0,5,0,0);
    m_pWaveWidget = new WaveTriggerWidget(this);
    pWaveLayout->addWidget(m_pWaveWidget);
    pWave->setLayout(pWaveLayout);
    pLayout->addWidget(pWave,5,0,1,2);
    connect(m_pWaveWidget,&WaveTriggerWidget::valueChanged,[=](qreal value){
        QRegExp rx;
        rx.setPattern("(\\.){0,1}0+$");
        m_pParams[SET]->setValue(value);
        m_pParams[SET]->myValueChanged(value);//emit signal
    });

    QGroupBox * pEnd = new QGroupBox(this);
    QHBoxLayout * pEndLayout = new QHBoxLayout();
    pEndLayout->addWidget(new QLabel(tr("Status")));
    pEndLayout->setContentsMargins(0,5,0,0);
    m_pStatus->setFixedWidth(97);
    pEndLayout->addWidget(m_pStatus);
    pEndLayout->addStretch(4);
    m_pViewGraph = new QPushButton(tr("View Graph"),this);
    m_pViewGraph->setFixedWidth(157);
    pEndLayout->addWidget(m_pViewGraph);
    connect(m_pViewGraph,&QPushButton::clicked,this,&ParamWidget::showGraph);
    pEnd->setLayout(pEndLayout);
    pLayout->addWidget(pEnd,6,0,1,2);

    setLayout(pLayout);
    initData();
}

void VelWidget::initData()
{
    for(int i=SET;i<VEL_PARAM_CNT;++i)
        m_nParamsDataId[i] = i+MotorForm::VEL_SET;
    m_nActualsDataId[ACTUAL] = MotorForm::VEL_ACTURAL;
    m_nActualsDataId[DEMAND] = MotorForm::VEL_DEMAND;
    m_nActualsDataId[CURRENT] = MotorForm::CUR_ACTURAL;
    m_nActualsDataId[POSITION] = MotorForm::POS_ACTURAL;
    m_nActualsDataId[VOLTAGE] = MotorForm::VOLTAGE;
    m_nActualsDataId[TEMP_MOTOR] = MotorForm::TEMP_MOTOR;
    m_nActualsDataId[TEMP_INVERTER] = MotorForm::TEMP_INVERTER;
    for(int i=SET;i<VEL_PARAM_CNT;++i)
    {
        //m_pParams[i]->setValidator(new QDoubleValidator);
        connect(m_pParams[i],&MyDoubleSpinBox::myValueChanged,this,&VelWidget::valueChangeByUser);
//        connect(m_pParams[i],&MyDoubleSpinBox::stepByChange,this,&VelWidget::valueChangeByUser);
        motorDataChange(m_nDeviceId,m_nParamsDataId[i]);
    }

    for(int i=ACTUAL; i<ACTUAL_CNT;++i)
    {
        motorDataChange(m_nDeviceId,m_nActualsDataId[i]);
        connect(m_pActuals[i],&QLineEdit::editingFinished,this,&VelWidget::valueChangeByUser);
    }

    connect(Mediator::getInstance(),&Mediator::dataChange,this,&VelWidget::motorDataChange);
    connect(Mediator::getInstance(),&Mediator::onlineChange,this,&VelWidget::onlineStatus);
    connect(m_pParams[SET],static_cast<void(MyDoubleSpinBox::*)(const QString &)>(&MyDoubleSpinBox::valueChanged),m_pActuals[DEMAND],&QLineEdit::setText);
}

void VelWidget::enableMode(bool bEnable)
{
    if(bEnable)
        m_pModeEdit->setText(tr("Velocity Mode"));
    else
    {
        m_pModeEdit->setText("");
        m_pWaveWidget->stop();
    }


    for(int i=SET;i<VEL_PARAM_CNT;++i)
    {
        m_pParams[i]->setEnabled(bEnable);
    }
    m_pBtnEffect->setEnabled(bEnable);
    m_pBtnHalt->setEnabled(bEnable);
    m_pViewGraph->setEnabled(bEnable);
    m_pBtnActiveMode->setChecked(bEnable);
    m_pWaveWidget->setEnabled(bEnable);
}

void VelWidget::valueChangeByUser()
{
    MyDoubleSpinBox * pSender = qobject_cast<MyDoubleSpinBox *>(sender());
    for(int i=SET;i<VEL_PARAM_CNT;++i)
    {
        if(m_pParams[i] == pSender)
        {
            qreal value = pSender->value();
            Mediator::getInstance()->setValueByUser((MotorForm::Motor_Data_Id)(m_nParamsDataId[i]),value);
            break;
        }
    }
}

void VelWidget::motorDataChange(quint8 nDeviceId, int nId)
{
    if(!isCurrentDevice(nDeviceId))
        return;
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    for(int i=ACTUAL;i<ACTUAL_CNT;++i)
    {
        if(m_nActualsDataId[i] == nId)
        {
            m_pActuals[i]->setText(QString::number(Mediator::getInstance()->getValue((MotorForm::Motor_Data_Id)nId),'f',6).replace(rx,""));
            break;
        }
    }

    for(int i=SET;i<VEL_PARAM_CNT;++i)
    {
        if(m_nParamsDataId[i] == nId)
        {
            m_pParams[i]->setValue(Mediator::getInstance()->getValue((MotorForm::Motor_Data_Id)nId));
            break;
        }
    }
}

/*位置模式*/
PosWidget::PosWidget(quint8 nDeviceId, MotorForm::Motor_Mode modeId, QWidget *parent) :
    ParamWidget(nDeviceId,modeId,parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    modifyPalette(this,QPalette::Window,QColor(41,60,75));
    //resize(700,600);
    setAutoFillBackground(true);

    QGridLayout * pLayout = new QGridLayout(this);
    pLayout->setContentsMargins(40,5,40,5);
    pLayout->setVerticalSpacing(3);
    pLayout->setColumnStretch(0,1);

    QLabel * pDetail = new QLabel(this);
    pDetail->setPixmap(QIcon(":/images/detail_pos.png").pixmap(800,120));
    pLayout->addWidget(pDetail,0,0,1,2,Qt::AlignHCenter | Qt::AlignVCenter);

    QGroupBox * pOperationMode = new QGroupBox(tr("Operation Mode"),this);
    QHBoxLayout * pModeLayout = new QHBoxLayout();
    pModeLayout->setContentsMargins(0,5,0,0);
    pModeLayout->addStretch(5);
    pModeLayout->setSpacing(3);
    pModeLayout->addWidget(new QLabel(tr("Activate Mode")));
    m_pModeEdit = new QLineEdit(this);
    m_pModeEdit->setFixedWidth(155);
    m_pModeEdit->setPlaceholderText(tr("Unkonw Mode"));
    m_pModeEdit->setAlignment(Qt::AlignHCenter);
    m_pModeEdit->setReadOnly(true);
    pModeLayout->addWidget(m_pModeEdit);
    m_pBtnActiveMode = new QPushButton(tr("Active Position Mode"),this);
    pModeLayout->addWidget(m_pBtnActiveMode);
    connect(m_pBtnActiveMode,&QPushButton::clicked,this,&ParamWidget::activeMode);
    m_pBtnActiveMode->setFixedWidth(155);
    m_pBtnActiveMode->setCheckable(true);
    pOperationMode->setLayout(pModeLayout);
    pLayout->addWidget(pOperationMode,1,0,1,2);

    QGroupBox * pRegulation = new QGroupBox(tr("Regulation"),this);
    QGridLayout * pReguLayout = new QGridLayout();
    pReguLayout->setContentsMargins(0,5,0,0);
    pReguLayout->setVerticalSpacing(3);
    pReguLayout->setColumnStretch(2,5);
    const char * name[POS_PARAM_CNT] = {"Setting","Proportional","Integral","Differential","Minimal","Maximum","Step Add",
                                  "Min Position","Max Position","Offset"};
    const char * actualsName[ACTUAL_CNT] = {"Actual Position","Position Demand","Actual Current","Actual Velocity",
                                            "Actual Voltage","Motor Temperature","Inverter Temperature"};
    double spinBoxAttr[POS_PARAM_CNT][ATTR_CNT]={-128,128,1,
                                                 0,100,0.1,
                                                 0,10,0.01,
                                                 0,100,0.01,
                                                 -1,0,0.1,
                                                 0,1,0.1,
                                                 -128,128,1,
                                                 -128,128,1,
                                                 -128,128,1,
                                                 -128,128,1,
                                                };
    for(int i=SET;i<STEP_ADD;++i)
    {
        QLabel * pLabel = new QLabel(tr(name[i]));
        pLabel->setFixedWidth(90);
        pReguLayout->addWidget(pLabel,i,0,1,1);
        m_pParams[i] = new MyDoubleSpinBox(this);
        m_pParams[i]->setMinimum(spinBoxAttr[i][MIN_VALUE]);
        m_pParams[i]->setMaximum(spinBoxAttr[i][MAX_VALUE]);
        m_pParams[i]->setSingleStep(spinBoxAttr[i][STEP_VALUE]);
        m_pParams[i]->setDecimals(5);
        pReguLayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(180);
        if(i== SET)
        {
            QLabel * pTag = new QLabel(tr("R"));
            pReguLayout->addWidget(pTag,i,2,1,1);
            m_pParams[i]->setEmitFinishedOnLoseFocus(false);
        }
    }
    m_pParams[STEP_ADD] = new MyDoubleSpinBox(this);
    m_pParams[STEP_ADD]->setMinimum(spinBoxAttr[STEP_ADD][MIN_VALUE]);
    m_pParams[STEP_ADD]->setMaximum(spinBoxAttr[STEP_ADD][MAX_VALUE]);
    m_pParams[STEP_ADD]->setSingleStep(spinBoxAttr[STEP_ADD][STEP_VALUE]);
    m_pParams[STEP_ADD]->setDecimals(4);
    m_pParams[STEP_ADD]->setFixedWidth(150);
    pReguLayout->addWidget(m_pParams[STEP_ADD],0,3,1,1,Qt::AlignRight);
    m_pBtnStepAdd = new QPushButton(tr("Step Add"),this);
    m_pBtnStepAdd->setFixedWidth(130);
    connect(m_pBtnStepAdd,&QPushButton::clicked,this,&PosWidget::stepAdd);
    pReguLayout->addWidget(m_pBtnStepAdd,0,4,1,1);
    m_pBtnStepMinus = new QPushButton(tr("Step Minus"),this);
    m_pBtnStepMinus->setFixedWidth(130);
    connect(m_pBtnStepMinus,&QPushButton::clicked,this,&PosWidget::stepMinus);
    pReguLayout->addWidget(m_pBtnStepMinus,1,4,1,1);
    m_pBtnEffect = new QPushButton(tr("Set Position"),this);
    m_pBtnEffect->setFixedWidth(130);
    pReguLayout->addWidget(m_pBtnEffect,4,4,1,1);
    connect(m_pBtnEffect,&QPushButton::clicked,[=]{
        m_pParams[SET]->myValueChanged(m_pParams[SET]->value());
    });
//    m_pBtnHalt = new QPushButton(tr("Halt"),this);
//    m_pBtnHalt->setFixedWidth(130);
//    pReguLayout->addWidget(m_pBtnHalt,5,4,1,1);
//    m_pBtnHalt->setEnabled(false);
    pRegulation->setLayout(pReguLayout);
    pLayout->addWidget(pRegulation,2,0,1,2);

    QGroupBox * pParams = new QGroupBox(tr("Regulation"),this);
    QGridLayout * pParamLayout = new QGridLayout();
    pParamLayout->setContentsMargins(0,5,0,0);
    pParamLayout->setVerticalSpacing(3);
    pParamLayout->setColumnStretch(3,5);
    for(int i=MIN_POS;i<POS_PARAM_CNT;++i)
    {
        QLabel * pLabel = new QLabel(tr(name[i]));
        pLabel->setFixedWidth(90);
        pParamLayout->addWidget(pLabel,i,0,1,1);
        m_pParams[i] = new MyDoubleSpinBox(this);
        m_pParams[i]->setMinimum(spinBoxAttr[i][MIN_VALUE]);
        m_pParams[i]->setMaximum(spinBoxAttr[i][MAX_VALUE]);
        m_pParams[i]->setSingleStep(spinBoxAttr[i][STEP_VALUE]);
        m_pParams[i]->setDecimals(4);
        pParamLayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(180);
    }
//    m_pMinEnable = new QCheckBox(tr("Enable"),this);
//    pParamLayout->addWidget(m_pMinEnable,0,2,1,1);
//    m_pMaxEnable = new QCheckBox(tr("Enable"),this);
//    pParamLayout->addWidget(m_pMaxEnable,1,2,1,1);

    pParams->setLayout(pParamLayout);
    pLayout->addWidget(pParams,3,0,1,1);

    QGroupBox * pActuals = new QGroupBox(tr("Actual Values"),this);
    QGridLayout * pActualayout = new QGridLayout();
    pActualayout->setContentsMargins(0,5,0,0);
    pActualayout->setVerticalSpacing(3);
    for(int i=ACTUAL;i<ACTUAL_CNT;++i)
    {
        pActualayout->addWidget(new QLabel(tr(actualsName[i])),i,0,1,1);
        m_pActuals[i] = new QLineEdit(this);
        pActualayout->addWidget(m_pActuals[i],i,1,1,1);
        m_pActuals[i]->setFixedWidth(115);
        m_pActuals[i]->setReadOnly(true);
        switch(i)
        {
        case CURRENT:
        {
            QLabel * pTag = new QLabel(tr("A"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case VELOCITY:
        {
            QLabel * pTag = new QLabel(tr("RPM"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case ACTUAL:
        case DEMAND:
        {
            QLabel * pTag = new QLabel(tr("R"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case VOLTAGE:
        {
            QLabel * pTag = new QLabel(tr("V"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case TEMP_INVERTER:
        case TEMP_MOTOR:
        {
            QLabel * pTag = new QLabel(QString::fromLocal8Bit("℃"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        default:
            break;
        }
    }


    pActuals->setLayout(pActualayout);
    pLayout->addWidget(pActuals,3,1,1,1);

    QGroupBox * pError = new QGroupBox(tr("Error"),this);
    QGridLayout * pErrorLayout = new QGridLayout();
    pErrorLayout->setContentsMargins(0,5,0,0);
    m_pClearErrors->setFixedWidth(134);
    pErrorLayout->addWidget(m_pClearErrors,0,0,1,1);
    m_pErrorInfos = new QPushButton(tr("Error Info"),this);
    m_pErrorInfos->setFixedWidth(134);
    pErrorLayout->addWidget(m_pErrorInfos,1,0,1,1);
    //connect(m_pErrorInfos,&QPushButton::clicked,this,&ParamWidget::checkError);

    pErrorLayout->addWidget(m_pTableErrors,0,1,4,1);
    pError->setLayout(pErrorLayout);
    pLayout->addWidget(pError,4,0,1,2);

    QGroupBox * pWave = new QGroupBox(tr("Wave Generator"),this);
    QHBoxLayout * pWaveLayout = new QHBoxLayout();
    pWaveLayout->setContentsMargins(0,5,0,0);
    m_pWaveWidget = new WaveTriggerWidget(false,this);
    pWaveLayout->addWidget(m_pWaveWidget);
    pWave->setLayout(pWaveLayout);
    pLayout->addWidget(pWave,5,0,1,2);
    connect(m_pWaveWidget,&WaveTriggerWidget::valueChanged,[=](qreal value){
        QRegExp rx;
        rx.setPattern("(\\.){0,1}0+$");
        m_pParams[SET]->setValue(value);
        m_pParams[SET]->myValueChanged(value);//emit signal
    });

    QGroupBox * pEnd = new QGroupBox(this);
    QHBoxLayout * pEndLayout = new QHBoxLayout();
    pEndLayout->setContentsMargins(0,5,0,0);
    pEndLayout->addWidget(new QLabel(tr("Status")));

    m_pStatus->setFixedWidth(97);
    pEndLayout->addWidget(m_pStatus);
    pEndLayout->addStretch(4);
    m_pViewGraph = new QPushButton(tr("View Graph"),this);
    m_pViewGraph->setFixedWidth(157);
    connect(m_pViewGraph,&QPushButton::clicked,this,&ParamWidget::showGraph);
    pEndLayout->addWidget(m_pViewGraph);
    pEnd->setLayout(pEndLayout);
    pLayout->addWidget(pEnd,6,0,1,2);

    setLayout(pLayout);
    initData();
}

void PosWidget::initData()
{
    for(int i=SET;i<POS_PARAM_CNT;++i)
        m_nParamsDataId[i] = i+MotorForm::POS_SET;
    m_nParamsDataId[POS_OFFSET] = MotorForm::POS_OFFSET;

    m_nActualsDataId[ACTUAL] = MotorForm::POS_ACTURAL;
    m_nActualsDataId[DEMAND] = MotorForm::POS_DEMAND;
    m_nActualsDataId[CURRENT] = MotorForm::CUR_ACTURAL;
    m_nActualsDataId[VELOCITY] = MotorForm::VEL_ACTURAL;
    m_nActualsDataId[VOLTAGE] = MotorForm::VOLTAGE;
    m_nActualsDataId[TEMP_MOTOR] = MotorForm::TEMP_MOTOR;
    m_nActualsDataId[TEMP_INVERTER] = MotorForm::TEMP_INVERTER;


    for(int i=SET;i<POS_PARAM_CNT;++i)
    {
        connect(m_pParams[i],&MyDoubleSpinBox::myValueChanged,this,&PosWidget::valueChangeByUser);
        //connect(m_pParams[i],&MyDoubleSpinBox::stepByChange,this,&PosWidget::valueChangeByUser);
        motorDataChange(m_nDeviceId,m_nParamsDataId[i]);
    }

    for(int i=ACTUAL; i<ACTUAL_CNT;++i)
    {
        motorDataChange(m_nDeviceId,m_nActualsDataId[i]);
        connect(m_pActuals[i],&QLineEdit::editingFinished,this,&PosWidget::valueChangeByUser);
    }
    connect(Mediator::getInstance(),&Mediator::dataChange,this,&PosWidget::motorDataChange);
    connect(Mediator::getInstance(),&Mediator::onlineChange,this,&PosWidget::onlineStatus);

    connect(m_pParams[SET],static_cast<void(MyDoubleSpinBox::*)(const QString &)>(&MyDoubleSpinBox::valueChanged),m_pActuals[DEMAND],&QLineEdit::setText);
    m_pParams[SET]->setValue(Mediator::getInstance()->getValue(MotorForm::POS_ACTURAL));//init position to actural pos
}

void PosWidget::enableMode(bool bEnable)
{
    if(bEnable)
        m_pModeEdit->setText(tr("Position Mode"));
    else
    {
        m_pModeEdit->setText("");
        m_pWaveWidget->stop();
    }

    for(int i=SET;i<POS_PARAM_CNT;++i)
    {
        m_pParams[i]->setEnabled(bEnable);
    }
    m_pBtnEffect->setEnabled(bEnable);
    m_pViewGraph->setEnabled(bEnable);
    m_pBtnActiveMode->setChecked(bEnable);
    m_pBtnStepAdd->setEnabled(bEnable);
    m_pBtnStepMinus->setEnabled(bEnable);
    m_pWaveWidget->setEnabled(bEnable);
}


void PosWidget::stepAdd()
{
    qreal add = m_pParams[STEP_ADD]->value();
    if(add != 0)
    {
        qreal ori = m_pParams[SET]->value();
        Mediator::getInstance()->setValueByUser((MotorForm::Motor_Data_Id)(MotorForm::POS_SET),ori+add);
    }
}

void PosWidget::stepMinus()
{
    qreal add = m_pParams[STEP_ADD]->value();
    if(add != 0)
    {
        qreal ori = m_pParams[SET]->text().toDouble();
        Mediator::getInstance()->setValueByUser((MotorForm::Motor_Data_Id)(MotorForm::POS_SET),ori-add);
    }
}


void PosWidget::valueChangeByUser()
{
    MyDoubleSpinBox * pSender = qobject_cast<MyDoubleSpinBox *>(sender());
    for(int i=SET;i<POS_PARAM_CNT;++i)
    {
        if(m_pParams[i] == pSender)
        {
            qreal value = pSender->value();
            Mediator::getInstance()->setValueByUser((MotorForm::Motor_Data_Id)(m_nParamsDataId[i]),value);
            break;
        }
    }
}

void PosWidget::motorDataChange(quint8 nDeviceId, int nId)
{
    if(!isCurrentDevice(nDeviceId))
        return;
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    for(int i=SET;i<POS_PARAM_CNT;++i)
    {
        if(m_nParamsDataId[i] == nId)
        {
            m_pParams[i]->setValue(Mediator::getInstance()->getValue(MotorForm::Motor_Data_Id(nId)));
        }
    }

    for(int i=ACTUAL;i<ACTUAL_CNT;++i)
    {
        if(m_nActualsDataId[i] == nId)
        {
            m_pActuals[i]->setText(QString::number(Mediator::getInstance()->getValue((MotorForm::Motor_Data_Id)nId),'f',6).replace(rx,""));
            break;
        }
    }
}

/*profile pos mode*/
ProfilePosWidget::ProfilePosWidget(quint8 nDeviceId, MotorForm::Motor_Mode modeId, QWidget *parent) :
    ParamWidget(nDeviceId,modeId,parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    modifyPalette(this,QPalette::Window,QColor(41,60,75));
    //resize(700,600);
    setAutoFillBackground(true);

    QGridLayout * pLayout = new QGridLayout(this);
    pLayout->setContentsMargins(40,5,40,5);
    pLayout->setVerticalSpacing(3);
    pLayout->setColumnStretch(0,1);
    QLabel * pDetail = new QLabel(this);
    pDetail->setPixmap(QIcon(":/images/detail_pos.png").pixmap(800,120));
    pLayout->addWidget(pDetail,0,0,1,2,Qt::AlignHCenter | Qt::AlignVCenter);

    QGroupBox * pOperationMode = new QGroupBox(tr("Operation Mode"),this);
    QHBoxLayout * pModeLayout = new QHBoxLayout();
    pModeLayout->setContentsMargins(0,5,0,0);
    pModeLayout->addStretch(5);
    pModeLayout->setSpacing(3);
    pModeLayout->addWidget(new QLabel(tr("Activate Mode")));
    m_pModeEdit = new QLineEdit(this);
    m_pModeEdit->setFixedWidth(155);
    m_pModeEdit->setPlaceholderText(tr("Unkonw Mode"));
    m_pModeEdit->setAlignment(Qt::AlignHCenter);
    m_pModeEdit->setReadOnly(true);
    pModeLayout->addWidget(m_pModeEdit);
    m_pBtnActiveMode = new QPushButton(tr("Active Profile Position Mode"),this);
    pModeLayout->addWidget(m_pBtnActiveMode);
    connect(m_pBtnActiveMode,&QPushButton::clicked,this,&ParamWidget::activeMode);
    m_pBtnActiveMode->setFixedWidth(155);
    m_pBtnActiveMode->setCheckable(true);
    pOperationMode->setLayout(pModeLayout);
    pLayout->addWidget(pOperationMode,1,0,1,2);

    QGroupBox * pRegulation = new QGroupBox(tr("Profile"),this);
    QGridLayout * pReguLayout = new QGridLayout();
    pReguLayout->setContentsMargins(0,5,0,0);
    pReguLayout->setVerticalSpacing(3);
    pReguLayout->setColumnStretch(2,5);
    const char * name[PROFILE_POS_PARAM_CNT] = {"Target","Profile","Accelerate","Decelerate","Max","Step Add","Min Position","Max Position","Offset",
                                                "Actual Position","Position Demand","Actual Current","Actual Velocity","Actual Voltage",
                                                "Motor Temperature","Inverter Temperature"};
    double spinBoxAttr[ACTUAL][ATTR_CNT]={-128,128,1,
                                         -1,1,0.001,
                                         0,100,0.1,
                                         -100,0,0.1,
                                         0,10,0.1,
                                         -128,128,1,
                                         -128,128,1,
                                         -128,128,1,
                                         -128,128,1
                                        };

    for(int i=TARGET;i<STEP_ADD;++i)
    {
        QLabel * pLabel = new QLabel(tr(name[i]));
        pLabel->setFixedWidth(90);
        pReguLayout->addWidget(pLabel,i,0,1,1);
        if(i == PROFILE)
        {
            QComboBox * pBox = new QComboBox(this);
            m_pParams[i] = pBox;
            QStringList profiles;
            profiles << "S-Curve";
            pBox->addItems(profiles);
        }
        else
        {
            MyDoubleSpinBox * pBox = new MyDoubleSpinBox(this);
            m_pParams[i] = pBox;
            pBox->setMinimum(spinBoxAttr[i][MIN_VALUE]);
            pBox->setMaximum(spinBoxAttr[i][MAX_VALUE]);
            pBox->setSingleStep(spinBoxAttr[i][STEP_VALUE]);
            pBox->setDecimals(4);
            if(i == TARGET)
                pBox->setEmitFinishedOnLoseFocus(false);
        }

        pReguLayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(180);

    }
    MyDoubleSpinBox * pAdd = new MyDoubleSpinBox(this);
    m_pParams[STEP_ADD] = pAdd;
    m_pParams[STEP_ADD]->setFixedWidth(150);

    pReguLayout->addWidget(m_pParams[STEP_ADD],0,3,1,1,Qt::AlignRight);
    m_pBtnStepAdd = new QPushButton(tr("Step Add"),this);
    m_pBtnStepAdd->setFixedWidth(130);
    pReguLayout->addWidget(m_pBtnStepAdd,0,4,1,1);
    connect(m_pBtnStepAdd,&QPushButton::clicked,[=]{
        MyDoubleSpinBox * pTarget = qobject_cast<MyDoubleSpinBox *>(m_pParams[TARGET]);
        MyDoubleSpinBox * pAdd = qobject_cast<MyDoubleSpinBox *>(m_pParams[STEP_ADD]);
        if(pTarget && pTarget)
        {
            qreal ori = pTarget->value();
            qreal add = pAdd->value();
            pTarget->setValue(ori+add);
            emit pTarget->editingFinished();
        }
    });
    m_pBtnStepMinus = new QPushButton(tr("Step Minus"),this);
    m_pBtnStepMinus->setFixedWidth(130);
    connect(m_pBtnStepMinus,&QPushButton::clicked,[=]{
        MyDoubleSpinBox * pTarget = qobject_cast<MyDoubleSpinBox *>(m_pParams[TARGET]);
        MyDoubleSpinBox * pAdd = qobject_cast<MyDoubleSpinBox *>(m_pParams[STEP_ADD]);
        if(pTarget && pTarget)
        {
            qreal ori = pTarget->value();
            qreal add = pAdd->value();
            pTarget->setValue(ori-add);
            emit pTarget->editingFinished();
        }
    });
    pReguLayout->addWidget(m_pBtnStepMinus,1,4,1,1);
    m_pBtnHalt = new QPushButton(tr("Halt"),this);
    m_pBtnHalt->setFixedWidth(130);
    pReguLayout->addWidget(m_pBtnHalt,2,4,1,1);

    pRegulation->setLayout(pReguLayout);
    pLayout->addWidget(pRegulation,2,0,1,2);

    QGroupBox * pParams = new QGroupBox(tr("Regulation"),this);
    QGridLayout * pParamLayout = new QGridLayout();
    pParamLayout->setContentsMargins(0,5,0,0);
    pParamLayout->setVerticalSpacing(3);
    pParamLayout->setColumnStretch(3,5);
    for(int i=MIN_POS;i<ACTUAL;++i)
    {
        QLabel * pLabel = new QLabel(tr(name[i]));
        pLabel->setFixedWidth(90);
        pParamLayout->addWidget(pLabel,i,0,1,1);
        MyDoubleSpinBox * pBox = new MyDoubleSpinBox(this);
        m_pParams[i] = pBox;
        pBox->setMinimum(spinBoxAttr[i][MIN_VALUE]);
        pBox->setMaximum(spinBoxAttr[i][MAX_VALUE]);
        pBox->setSingleStep(spinBoxAttr[i][STEP_VALUE]);
        pBox->setDecimals(4);
        pParamLayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(180);
//        m_pParams[i] = new QLineEdit(this);
//        pParamLayout->addWidget(m_pParams[i],i,1,1,1);
//        m_pParams[i]->setFixedWidth(180);
    }


    pParams->setLayout(pParamLayout);
    pLayout->addWidget(pParams,3,0,1,1);


    QGroupBox * pActuals = new QGroupBox(tr("Actual Values"),this);
    QGridLayout * pActualayout = new QGridLayout();
    pActualayout->setContentsMargins(0,5,0,0);
    pActualayout->setVerticalSpacing(3);
    for(int i=ACTUAL;i<PROFILE_POS_PARAM_CNT;++i)
    {
        pActualayout->addWidget(new QLabel(tr(name[i])),i,0,1,1);
        QLineEdit * pEdit = new QLineEdit(this);
        m_pParams[i] = pEdit;
        pActualayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(115);
        pEdit->setReadOnly(true);
        switch(i)
        {
        case CURRENT:
        {
            QLabel * pTag = new QLabel(tr("A"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case VELOCITY:
        {
            QLabel * pTag = new QLabel(tr("RPM"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case ACTUAL:
        case DEMAND:
        {
            QLabel * pTag = new QLabel(tr("R"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case VOLTAGE:
        {
            QLabel * pTag = new QLabel(tr("V"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case TEMP_INVERTER:
        case TEMP_MOTOR:
        {
            QLabel * pTag = new QLabel(QString::fromLocal8Bit("℃"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        default:
            break;
        }
    }
    pActuals->setLayout(pActualayout);
    pLayout->addWidget(pActuals,3,1,1,1);

    QGroupBox * pError = new QGroupBox(tr("Error"),this);
    QGridLayout * pErrorLayout = new QGridLayout();
    pErrorLayout->setContentsMargins(0,5,0,0);
    m_pClearErrors->setFixedWidth(134);
    pErrorLayout->addWidget(m_pClearErrors,0,0,1,1);
    m_pErrorInfos = new QPushButton(tr("Error Info"),this);
    m_pErrorInfos->setFixedWidth(134);
    pErrorLayout->addWidget(m_pErrorInfos,1,0,1,1);
    //connect(m_pErrorInfos,&QPushButton::clicked,this,&ParamWidget::checkError);

    pErrorLayout->addWidget(m_pTableErrors,0,1,4,1);
    pError->setLayout(pErrorLayout);
    pLayout->addWidget(pError,4,0,1,2);

    QGroupBox * pWave = new QGroupBox(tr("Wave Generator"),this);
    QHBoxLayout * pWaveLayout = new QHBoxLayout();
    pWaveLayout->setContentsMargins(0,5,0,0);
    m_pWaveWidget = new WaveTriggerWidget(false,pWave);
    pWaveLayout->addWidget(m_pWaveWidget);
    pWave->setLayout(pWaveLayout);
    pLayout->addWidget(pWave,5,0,1,2);
    connect(m_pWaveWidget,&WaveTriggerWidget::valueChanged,[=](qreal value){
        QRegExp rx;
        rx.setPattern("(\\.){0,1}0+$");
        MyDoubleSpinBox * pEdit = qobject_cast<MyDoubleSpinBox *>(m_pParams[TARGET]);
        pEdit->setValue(value);
        pEdit->myValueChanged(value);//emit signal
    });

    QGroupBox * pEnd = new QGroupBox(this);
    QHBoxLayout * pEndLayout = new QHBoxLayout();
    pEndLayout->setContentsMargins(0,5,0,0);
    pEndLayout->addWidget(new QLabel(tr("Status")));
    m_pStatus->setFixedWidth(97);
    pEndLayout->addWidget(m_pStatus);
    pEndLayout->addStretch(4);
    m_pViewGraph = new QPushButton(tr("View Graph"),this);
    m_pViewGraph->setFixedWidth(157);
    connect(m_pViewGraph,&QPushButton::clicked,this,&ParamWidget::showGraph);
    pEndLayout->addWidget(m_pViewGraph);
    pEnd->setLayout(pEndLayout);
    pLayout->addWidget(pEnd,6,0,1,2);

    setLayout(pLayout);
    initData();
}

void ProfilePosWidget::initData()
{
    m_nDataId[TARGET] = MotorForm::POS_SET;
    m_nDataId[PROFILE] = MotorForm::DATA_INVALID;
    m_nDataId[ACCELERATE] = MotorForm::PROFILE_POS_ACC;
    m_nDataId[DECELERATE] = MotorForm::PROFILE_POS_DEC;
    m_nDataId[MAX_SPEED] = MotorForm::PROFILE_POS_MAX_SPEED;
    m_nDataId[ACTUAL] = MotorForm::POS_ACTURAL;
    m_nDataId[STEP_ADD] = MotorForm::POS_STEP_ADD;
    m_nDataId[DEMAND] = MotorForm::POS_DEMAND;
    m_nDataId[CURRENT] = MotorForm::CUR_ACTURAL;
    m_nDataId[VELOCITY] = MotorForm::VEL_ACTURAL;

    m_nDataId[MIN_POS] = MotorForm::POS_MIN_POS;
    m_nDataId[MAX_POS] = MotorForm::POS_MAX_POS;
    m_nDataId[POS_OFFSET] = MotorForm::POS_OFFSET;
    m_nDataId[VOLTAGE] = MotorForm::VOLTAGE;
    m_nDataId[TEMP_INVERTER] = MotorForm::TEMP_INVERTER;
    m_nDataId[TEMP_MOTOR] = MotorForm::TEMP_MOTOR;


    for(int i=TARGET;i<PROFILE_POS_PARAM_CNT;++i)
    {
//        MotorForm::Motor_Data_Id id = (MotorForm::Motor_Data_Id)i;
//        m_pParams[i]->setText(tr("%1").arg(Mediator::getInstance()->getValue(id)));
        QLineEdit * pEdit = qobject_cast<QLineEdit *>(m_pParams[i]);
        QComboBox * pComboBox = qobject_cast<QComboBox *>(m_pParams[i]);
        MyDoubleSpinBox * pDoubleSpin = qobject_cast<MyDoubleSpinBox *>(m_pParams[i]);
        if(pEdit)
        {
            pEdit->setValidator(new QDoubleValidator);
            connect(pEdit,&QLineEdit::editingFinished,this,&ProfilePosWidget::valueChangeByUser);

        }
        else if(pComboBox)
        {
            connect(pComboBox,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&ProfilePosWidget::valueChangeByUser);
        }
        else if (pDoubleSpin)
        {
            connect(pDoubleSpin,&MyDoubleSpinBox::myValueChanged,this,&ProfilePosWidget::valueChangeByUser);
            //connect(pDoubleSpin,&MyDoubleSpinBox::stepByChange,this,&ProfilePosWidget::valueChangeByUser);
        }
        motorDataChange(m_nDeviceId,m_nDataId[i]);
    }

    connect(Mediator::getInstance(),&Mediator::dataChange,this,&ProfilePosWidget::motorDataChange);
    connect(Mediator::getInstance(),&Mediator::onlineChange,this,&ProfilePosWidget::onlineStatus);
    QLineEdit * pDemand = qobject_cast<QLineEdit *>(m_pParams[DEMAND]);
    QLineEdit * pTarget = qobject_cast<QLineEdit *>(m_pParams[TARGET]);
    if(pDemand && pTarget)
    {
        pDemand->setReadOnly(true);
        connect(pTarget,&QLineEdit::textChanged,pDemand,&QLineEdit::setText);
    }

    MyDoubleSpinBox * pTar = qobject_cast<MyDoubleSpinBox *>(m_pParams[TARGET]);
    if(pTar)
        pTar->setValue(Mediator::getInstance()->getValue(MotorForm::POS_ACTURAL));//init position to actural pos
}

void ProfilePosWidget::enableMode(bool bEnable)
{
    if(bEnable)
        m_pModeEdit->setText(tr("Profile Position Mode"));
    else
    {
        m_pModeEdit->setText("");
        m_pWaveWidget->stop();
    }


    for(int i=TARGET;i<PROFILE_POS_PARAM_CNT;++i)
    {
        m_pParams[i]->setEnabled(bEnable);
    }
    m_pBtnStepAdd->setEnabled(bEnable);
    m_pViewGraph->setEnabled(bEnable);
    m_pBtnActiveMode->setChecked(bEnable);
    m_pBtnStepMinus->setEnabled(bEnable);
    m_pBtnHalt->setEnabled(bEnable);
    m_pWaveWidget->setEnabled(bEnable);
}

void ProfilePosWidget::valueChangeByUser()
{
    QLineEdit * pSender = qobject_cast<QLineEdit *>(sender());
    QComboBox * pComboBox = qobject_cast<QComboBox *>(sender());
    MyDoubleSpinBox * pDoubleSpin = qobject_cast<MyDoubleSpinBox *>(sender());
    for(int i=TARGET;i<PROFILE_POS_PARAM_CNT;++i)
    {
        if(m_pParams[i] == pSender)
        {
            if(m_nDataId[i] != MotorForm::DATA_INVALID)
            {
                qreal value = pSender->text().toDouble();

                Mediator::getInstance()->setValueByUser((MotorForm::Motor_Data_Id)(m_nDataId[i]),value);
            }

            break;
        }
        else if(m_pParams[i] == pDoubleSpin)
        {
            qreal value = pDoubleSpin->value();
            Mediator::getInstance()->setValueByUser((MotorForm::Motor_Data_Id)(m_nDataId[i]),value);
        }
        else if(m_pParams[i] == pComboBox)
        {
            //combobox change
        }
    }
}

void ProfilePosWidget::motorDataChange(quint8 nDeviceId, int nId)
{
    if(!isCurrentDevice(nDeviceId))
        return;
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    switch (nId) {
    case MotorForm::DATA_INVALID:
        break;
    default:
    {
        for (int i=0;i<PROFILE_POS_PARAM_CNT;++i) {
            if(m_nDataId[i] == nId)
            {
                QLineEdit * pEdit = qobject_cast<QLineEdit *>(m_pParams[i]);
                MyDoubleSpinBox * pDoubleSpin = qobject_cast<MyDoubleSpinBox *>(m_pParams[i]);
                if(pEdit)
                {
                    pEdit->setText(QString::number(Mediator::getInstance()->getValue((MotorForm::Motor_Data_Id)nId),'f',6).replace(rx,""));
                }
                else if(pDoubleSpin)
                {
                    pDoubleSpin->setValue(Mediator::getInstance()->getValue((MotorForm::Motor_Data_Id)nId));
                }
                break;
            }
        }
    }
        break;
    }

}

/*profile pos mode*/
ProfileVelWidget::ProfileVelWidget(quint8 nDeviceId, MotorForm::Motor_Mode modeId, QWidget *parent) :
    ParamWidget(nDeviceId,modeId,parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    modifyPalette(this,QPalette::Window,QColor(41,60,75));
    //resize(700,600);
    setAutoFillBackground(true);

    QGridLayout * pLayout = new QGridLayout(this);
    pLayout->setContentsMargins(40,5,40,5);
    pLayout->setVerticalSpacing(3);
    pLayout->setColumnStretch(0,1);

    QLabel * pDetail = new QLabel(this);
    pDetail->setPixmap(QIcon(":/images/detail_vel.png").pixmap(800,120));
    pLayout->addWidget(pDetail,0,0,1,2,Qt::AlignHCenter | Qt::AlignVCenter);

    QGroupBox * pOperationMode = new QGroupBox(tr("Operation Mode"),this);
    QHBoxLayout * pModeLayout = new QHBoxLayout();
    pModeLayout->setContentsMargins(0,5,0,0);
    pModeLayout->addStretch(5);
    pModeLayout->setSpacing(3);
    pModeLayout->addWidget(new QLabel(tr("Activate Mode")));
    m_pModeEdit = new QLineEdit(this);
    m_pModeEdit->setFixedWidth(155);
    m_pModeEdit->setPlaceholderText(tr("Unkonw Mode"));
    m_pModeEdit->setAlignment(Qt::AlignHCenter);
    m_pModeEdit->setReadOnly(true);
    pModeLayout->addWidget(m_pModeEdit);
    m_pBtnActiveMode = new QPushButton(tr("Active Profile Velocity Mode"),this);
    pModeLayout->addWidget(m_pBtnActiveMode);
    connect(m_pBtnActiveMode,&QPushButton::clicked,this,&ParamWidget::activeMode);
    m_pBtnActiveMode->setFixedWidth(155);
    m_pBtnActiveMode->setCheckable(true);
    pOperationMode->setLayout(pModeLayout);
    pLayout->addWidget(pOperationMode,1,0,1,2);

    QGroupBox * pRegulation = new QGroupBox(tr("Profile"),this);
    QGridLayout * pReguLayout = new QGridLayout();
    pReguLayout->setContentsMargins(0,5,0,0);
    pReguLayout->setVerticalSpacing(3);
    pReguLayout->setColumnStretch(2,5);
    const char * name[PROFILE_VEL_PARAM_CNT] = {"Target","Profile","Accelerate","Decelerate","Max","Actual Velocity",
                                                "Velocity Demand","Actual Current","Actual Position","Actual Voltage",
                                                "Motor Temperature","Inverter Temperature"};
    qreal velScale = Mediator::getInstance()->curVelocityScale();
    double spinBoxAttr[ACTUAL][ATTR_CNT]={-velScale,velScale,100,
                                         -1,1,0.001,
                                         0,100,0.1,
                                         -100,0,0.1,
                                         0,10,0.1
                                        };

    for(int i=TARGET;i<ACTUAL;++i)
    {
        QLabel * pLabel = new QLabel(tr(name[i]));
        pLabel->setFixedWidth(110);
        pReguLayout->addWidget(pLabel,i,0,1,1);
        if(i == PROFILE)
        {
            QComboBox * pBox = new QComboBox(this);
            m_pParams[i] = pBox;
            QStringList profiles;
            profiles << "S-Curve";
            pBox->addItems(profiles);
        }
        else if(i < ACTUAL)
        {
            MyDoubleSpinBox * pBox = new MyDoubleSpinBox(this);
            pBox->setMinimum(spinBoxAttr[i][MIN_VALUE]);
            pBox->setMaximum(spinBoxAttr[i][MAX_VALUE]);
            pBox->setSingleStep(spinBoxAttr[i][STEP_VALUE]);
            pBox->setDecimals(4);
            if(i == TARGET)
                pBox->setEmitFinishedOnLoseFocus(false);
            m_pParams[i] = pBox;
        }
        else
        {
            m_pParams[i] = new QLineEdit(this);

        }
        pReguLayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(180);

    }

    m_pBtnAbsolute = new QPushButton(tr("Move Absolute"),this);
    m_pBtnAbsolute->setFixedWidth(130);
    pReguLayout->addWidget(m_pBtnAbsolute,0,4,1,1);
    m_pBtnRelative = new QPushButton(tr("Move Relative"),this);
    m_pBtnRelative->setFixedWidth(130);
//    connect(m_pBtnRelative,&QPushButton::clicked,[=]{
//        QLineEdit * pEdit = qobject_cast<QLineEdit *>(m_pParams[TARGET]);
//        if(pEdit)
//        {
//            pEdit->setText(tr("%1").arg(pEdit->text().toDouble()*2));
//            emit pEdit->editingFinished();
//        }
//    });
    pReguLayout->addWidget(m_pBtnRelative,1,4,1,1);
    m_pBtnHalt = new QPushButton(tr("Halt"),this);
    m_pBtnHalt->setFixedWidth(130);
    connect(m_pBtnHalt,&QPushButton::clicked,[=]{
        MyDoubleSpinBox * pEdit = qobject_cast<MyDoubleSpinBox *>(m_pParams[TARGET]);
        if(pEdit)
        {
            pEdit->setValue(0);
            emit pEdit->myValueChanged(0);
        }
    });
    pReguLayout->addWidget(m_pBtnHalt,2,4,1,1);

    pRegulation->setLayout(pReguLayout);
    pLayout->addWidget(pRegulation,2,0,1,2);


    QGroupBox * pActuals = new QGroupBox(tr("Actual Values"),this);
    QGridLayout * pActualayout = new QGridLayout();
    pActualayout->setContentsMargins(0,5,0,0);
    pActualayout->setVerticalSpacing(3);
    for(int i=ACTUAL;i<PROFILE_VEL_PARAM_CNT;++i)
    {
        QLabel * pLabel = new QLabel(tr(name[i]));
        pLabel->setFixedWidth(110);
        pActualayout->addWidget(pLabel,i,0,1,1);
        QLineEdit * pEdit = new QLineEdit(this);
        m_pParams[i] = pEdit;
        pActualayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(180);
        pEdit->setReadOnly(true);
        switch(i)
        {
        case CURRENT:
        {
            QLabel * pTag = new QLabel(tr("A"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case POSITION:
        {
            QLabel * pTag = new QLabel(tr("R"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case ACTUAL:
        case DEMAND:
        {
            QLabel * pTag = new QLabel(tr("RPM"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case VOLTAGE:
        {
            QLabel * pTag = new QLabel(tr("V"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case TEMP_INVERTER:
        case TEMP_MOTOR:
        {
            QLabel * pTag = new QLabel(QString::fromLocal8Bit("℃"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        default:
            break;
        }
    }

    pActuals->setLayout(pActualayout);
    pLayout->addWidget(pActuals,3,0,1,2);

    QGroupBox * pError = new QGroupBox(tr("Error"),this);
    QGridLayout * pErrorLayout = new QGridLayout();
    pError->setContentsMargins(0,5,0,0);
    m_pClearErrors->setFixedWidth(134);
    pErrorLayout->addWidget(m_pClearErrors,0,0,1,1);
    m_pErrorInfos = new QPushButton(tr("Error Info"),this);
    m_pErrorInfos->setFixedWidth(134);
    pErrorLayout->addWidget(m_pErrorInfos,1,0,1,1);
    //connect(m_pErrorInfos,&QPushButton::clicked,this,&ParamWidget::checkError);

    pErrorLayout->addWidget(m_pTableErrors,0,1,4,1);
    pError->setLayout(pErrorLayout);
    pLayout->addWidget(pError,4,0,1,2);

    QGroupBox * pWave = new QGroupBox(tr("Wave Generator"),this);
    QHBoxLayout * pWaveLayout = new QHBoxLayout();
    pWaveLayout->setContentsMargins(0,5,0,0);
    m_pWaveWidget = new WaveTriggerWidget(pWave);
    pWaveLayout->addWidget(m_pWaveWidget);
    pWave->setLayout(pWaveLayout);
    pLayout->addWidget(pWave,5,0,1,2);
    connect(m_pWaveWidget,&WaveTriggerWidget::valueChanged,[=](qreal value){
        QRegExp rx;
        rx.setPattern("(\\.){0,1}0+$");
        MyDoubleSpinBox * pEdit = qobject_cast<MyDoubleSpinBox *>(m_pParams[TARGET]);
        pEdit->setValue(value);
        pEdit->myValueChanged(value);//emit signal
    });

    QGroupBox * pEnd = new QGroupBox(this);
    QHBoxLayout * pEndLayout = new QHBoxLayout();
    pEndLayout->setContentsMargins(0,5,0,0);
    pEndLayout->addWidget(new QLabel(tr("Status")));

    m_pStatus->setFixedWidth(97);
    pEndLayout->addWidget(m_pStatus);
    pEndLayout->addStretch(4);
    m_pViewGraph = new QPushButton(tr("View Graph"),this);
    m_pViewGraph->setFixedWidth(157);
    connect(m_pViewGraph,&QPushButton::clicked,this,&ParamWidget::showGraph);
    pEndLayout->addWidget(m_pViewGraph);
    pEnd->setLayout(pEndLayout);
    pLayout->addWidget(pEnd,6,0,1,2);

    setLayout(pLayout);
    initData();
}

void ProfileVelWidget::initData()
{
    m_nDataId[TARGET] = MotorForm::VEL_SET;
    m_nDataId[PROFILE] = MotorForm::DATA_INVALID;
    m_nDataId[ACCELERATE] = MotorForm::PROFILE_VEL_ACC;
    m_nDataId[DECELERATE] = MotorForm::PROFILE_VEL_DEC;
    m_nDataId[MAX_SPEED] = MotorForm::PROFILE_VEL_MAX_SPEED;
    m_nDataId[ACTUAL] = MotorForm::VEL_ACTURAL;
    m_nDataId[DEMAND] = MotorForm::VEL_DEMAND;
    m_nDataId[CURRENT] = MotorForm::CUR_ACTURAL;
    m_nDataId[POSITION] = MotorForm::POS_ACTURAL;
    m_nDataId[VOLTAGE] = MotorForm::VOLTAGE;
    m_nDataId[TEMP_INVERTER] = MotorForm::TEMP_INVERTER;
    m_nDataId[TEMP_MOTOR] = MotorForm::TEMP_MOTOR;
    for(int i=TARGET;i<PROFILE_VEL_PARAM_CNT;++i)
    {
//        MotorForm::Motor_Data_Id id = (MotorForm::Motor_Data_Id)i;
//        m_pParams[i]->setText(tr("%1").arg(Mediator::getInstance()->getValue(id)));
        QLineEdit * pEdit = qobject_cast<QLineEdit *>(m_pParams[i]);
        QComboBox * pComboBox = qobject_cast<QComboBox *>(m_pParams[i]);
        MyDoubleSpinBox * pDoubleSpin = qobject_cast<MyDoubleSpinBox *>(m_pParams[i]);
        if(pEdit)
        {
            pEdit->setValidator(new QDoubleValidator);
            connect(pEdit,&QLineEdit::editingFinished,this,&ProfileVelWidget::valueChangeByUser);
        }
        else if(pComboBox)
        {
            connect(pComboBox,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&ProfileVelWidget::valueChangeByUser);
        }
        else if(pDoubleSpin)
        {
            connect(pDoubleSpin,&MyDoubleSpinBox::myValueChanged,this,&ProfileVelWidget::valueChangeByUser);
//            connect(pDoubleSpin,&MyDoubleSpinBox::stepByChange,this,&ProfileVelWidget::valueChangeByUser);
        }
        motorDataChange(m_nDeviceId,m_nDataId[i]);
    }

    connect(Mediator::getInstance(),&Mediator::dataChange,this,&ProfileVelWidget::motorDataChange);
    connect(Mediator::getInstance(),&Mediator::onlineChange,this,&ProfileVelWidget::onlineStatus);
    QLineEdit * pDemand = qobject_cast<QLineEdit *>(m_pParams[DEMAND]);
    MyDoubleSpinBox * pTarget = qobject_cast<MyDoubleSpinBox *>(m_pParams[TARGET]);
    if(pDemand && pTarget)
    {
        pDemand->setReadOnly(true);
        connect(pTarget,static_cast<void(MyDoubleSpinBox::*)(const QString &)>(&MyDoubleSpinBox::valueChanged),pDemand,&QLineEdit::setText);
    }

}

void ProfileVelWidget::enableMode(bool bEnable)
{
    if(bEnable)
        m_pModeEdit->setText(tr("Profile Velocity Mode"));
    else
    {
        m_pModeEdit->setText("");
        m_pWaveWidget->stop();
    }

    for(int i=TARGET;i<PROFILE_VEL_PARAM_CNT;++i)
    {
        m_pParams[i]->setEnabled(bEnable);
    }
    m_pBtnAbsolute->setEnabled(bEnable);
    m_pViewGraph->setEnabled(bEnable);
    m_pBtnActiveMode->setChecked(bEnable);
    m_pBtnRelative->setEnabled(bEnable);
    m_pBtnHalt->setEnabled(bEnable);
    m_pWaveWidget->setEnabled(bEnable);
}


void ProfileVelWidget::valueChangeByUser()
{
    QLineEdit * pSender = qobject_cast<QLineEdit *>(sender());
    QComboBox * pComboBox = qobject_cast<QComboBox *>(sender());
    MyDoubleSpinBox * pDoubleSpin = qobject_cast<MyDoubleSpinBox *>(sender());
    for(int i=TARGET;i<PROFILE_VEL_PARAM_CNT;++i)
    {
        if(m_pParams[i] == pSender)
        {
            qreal value = pSender->text().toDouble();

            Mediator::getInstance()->setValueByUser((MotorForm::Motor_Data_Id)(m_nDataId[i]),value);
            break;
        }
        else if( m_pParams[i] == pDoubleSpin)
        {
            qreal value = pDoubleSpin->value();
            Mediator::getInstance()->setValueByUser((MotorForm::Motor_Data_Id)(m_nDataId[i]),value);
            break;
        }
        else if(m_pParams[i] == pComboBox)
        {
            //combobox change
        }
    }
}

void ProfileVelWidget::motorDataChange(quint8 nDeviceId, int nId)
{
    if(!isCurrentDevice(nDeviceId))
        return;
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    switch (nId) {
    case MotorForm::DATA_INVALID:
        break;
    default:
    {
        for (int i=0;i<PROFILE_VEL_PARAM_CNT;++i) {
            if(m_nDataId[i] == nId)
            {
                QLineEdit * pEdit = qobject_cast<QLineEdit *>(m_pParams[i]);
                MyDoubleSpinBox * pDoubleSpin = qobject_cast<MyDoubleSpinBox *>(m_pParams[i]);
                if(pEdit)
                {
                    pEdit->setText(QString::number(Mediator::getInstance()->getValue((MotorForm::Motor_Data_Id)nId),'f',6).replace(rx,""));
                }
                else if(pDoubleSpin)
                {
                    pDoubleSpin->setValue(Mediator::getInstance()->getValue((MotorForm::Motor_Data_Id)nId));
                }
                break;
            }
        }
    }
        break;
    }

}


HomingWidget::HomingWidget(quint8 nDeviceId, MotorForm::Motor_Mode modeId, QWidget *parent):
    ParamWidget(nDeviceId,modeId,parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    modifyPalette(this,QPalette::Window,QColor(41,60,75));
    //resize(700,600);
    setAutoFillBackground(true);

    QGridLayout * pLayout = new QGridLayout(this);
    pLayout->setContentsMargins(40,5,40,0);
    pLayout->setVerticalSpacing(3);
    pLayout->setColumnStretch(0,1);

//    QLabel * pDetail = new QLabel(this);
//    pDetail->setPixmap(QIcon(":/images/detail_vel.png").pixmap(535,92));
//    pLayout->addWidget(pDetail,0,0,1,2,Qt::AlignHCenter | Qt::AlignVCenter);

    QGroupBox * pOperationMode = new QGroupBox(tr("Operation Mode"),this);
    QHBoxLayout * pModeLayout = new QHBoxLayout();
    pModeLayout->setContentsMargins(0,5,0,0);
    pModeLayout->addStretch(5);
    pModeLayout->setSpacing(3);
    pModeLayout->addWidget(new QLabel(tr("Activate Mode")));
    m_pModeEdit = new QLineEdit(this);
    m_pModeEdit->setFixedWidth(155);
    m_pModeEdit->setPlaceholderText(tr("Unkonw Mode"));
    m_pModeEdit->setAlignment(Qt::AlignHCenter);
    m_pModeEdit->setReadOnly(true);
    pModeLayout->addWidget(m_pModeEdit);
    m_pBtnActiveMode = new QPushButton(tr("Active Homing Mode"),this);
    pModeLayout->addWidget(m_pBtnActiveMode);
    connect(m_pBtnActiveMode,&QPushButton::clicked,this,&ParamWidget::activeMode);
    m_pBtnActiveMode->setFixedWidth(155);
    m_pBtnActiveMode->setCheckable(true);
    pOperationMode->setLayout(pModeLayout);
    pLayout->addWidget(pOperationMode,1,0,1,2);

    QGroupBox * pRegulation = new QGroupBox(tr("Profile"),this);
    QGridLayout * pReguLayout = new QGridLayout();
    pReguLayout->setContentsMargins(0,5,0,0);
    pReguLayout->setVerticalSpacing(3);
    pReguLayout->setColumnStretch(2,5);
    const char * name[HOMING_PARAM_CNT] = {"Max Pos","Min Pos","Velocity","Vel Change","Minimal","Maximal","Current","Velocity",""
                                           "Position","Offset","Inertia","Lock Energy","Protected Temperature","Recovery Temperature"};

    pRegulation->setLayout(pReguLayout);
    pLayout->addWidget(pRegulation,2,0,1,2);

    for(int i=0;i<CURRENT;++i)
    {
        QLabel * pLabel = new QLabel(tr(name[i]));
        pLabel->setFixedWidth(130);
        pReguLayout->addWidget(pLabel,i,0,1,1);
        m_pParams[i] = new QLineEdit(this);
        pReguLayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(180);
    }

    m_pBtnCalibrate = new QPushButton(tr("Clear"),this);
    m_pBtnCalibrate->setFixedWidth(130);
    pReguLayout->addWidget(m_pBtnCalibrate,0,4,1,1);
    connect(m_pBtnCalibrate,&QPushButton::clicked,[=]{
        Mediator::getInstance()->clearHoming();
    });
    m_pBtnHoming = new QPushButton(tr("Homing"),this);
    m_pBtnHoming->setFixedWidth(130);
    pReguLayout->addWidget(m_pBtnHoming,1,4,1,1);
    connect(m_pBtnHoming,&QPushButton::clicked,this,&HomingWidget::homingCallback);

    m_pBtnVelAdd = new QPushButton(tr("Vel Add"),this);
    m_pBtnVelAdd->setFixedWidth(130);
    pReguLayout->addWidget(m_pBtnVelAdd,2,4,1,1);
    connect(m_pBtnVelAdd,&QPushButton::clicked,this,&HomingWidget::velAddCallback);

    m_pBtnVelMinus = new QPushButton(tr("Vel Minus"),this);
    m_pBtnVelMinus->setFixedWidth(130);
    pReguLayout->addWidget(m_pBtnVelMinus,3,4,1,1);
    connect(m_pBtnVelMinus,&QPushButton::clicked,this,&HomingWidget::velMinusCallback);

    m_pSetMin = new QPushButton(tr("Min Set"),this);
    m_pSetMin->setFixedWidth(130);
    pReguLayout->addWidget(m_pSetMin,4,4,1,1);
    connect(m_pSetMin,&QPushButton::clicked,[]{Mediator::getInstance()->setLeftLimit();});

    m_pSetMax = new QPushButton(tr("Max Set"),this);
    m_pSetMax->setFixedWidth(130);
    pReguLayout->addWidget(m_pSetMax,5,4,1,1);
    connect(m_pSetMax,&QPushButton::clicked,[]{Mediator::getInstance()->setRightLimit();});

    m_pBtnAuto = new QPushButton(tr("Auto"),this);
    m_pBtnAuto->setFixedWidth(130);
    m_pBtnAuto->setCheckable(true);
    //m_pBtnAuto->setChecked(true);
    pReguLayout->addWidget(m_pBtnAuto,0,5,1,1);

    m_pBtnManual = new QPushButton(tr("Manual"),this);
    m_pBtnManual->setFixedWidth(130);
    m_pBtnManual->setCheckable(true);
    pReguLayout->addWidget(m_pBtnManual,1,5,1,1);

    m_pGroup = new QButtonGroup(this);
    m_pGroup->addButton(m_pBtnAuto,2);
    m_pGroup->addButton(m_pBtnManual,1);
    connect(m_pGroup,static_cast<void(QButtonGroup:: *)(int)>(&QButtonGroup::buttonClicked),this,&HomingWidget::operationModeChanged);

    m_pBtnHalt = new QPushButton(tr("Halt"),this);
    m_pBtnHalt->setFixedWidth(130);
    pReguLayout->addWidget(m_pBtnHalt,2,5,1,1);
    connect(m_pBtnHalt,&QPushButton::clicked,[=]{setVelocity(0);});

    m_pReset = new QPushButton(tr("Reset"),this);
    m_pReset->setFixedWidth(130);
    pReguLayout->addWidget(m_pReset,3,5,1,1);
    connect(m_pReset,&QPushButton::clicked,[=]{homingCallback2();});

    m_pNeedLimit = new QCheckBox(tr("Limit"),this);
    m_pNeedLimit->setFixedWidth(130);
    pReguLayout->addWidget(m_pNeedLimit,4,5,1,1);

    QGroupBox * pActuals = new QGroupBox(tr("Actual Values"),this);
    QGridLayout * pActualayout = new QGridLayout();
    pActualayout->setContentsMargins(0,5,0,0);
    pActualayout->setVerticalSpacing(3);
    pActualayout->setColumnStretch(2,5);
    for(int i=CURRENT;i<HOMING_PARAM_CNT;++i)
    {
        QLabel * pLabel = new QLabel(tr(name[i]));
        pLabel->setFixedWidth(130);
        pActualayout->addWidget(pLabel,i,0,1,1);
        m_pParams[i] = new QLineEdit(this);
        pActualayout->addWidget(m_pParams[i],i,1,1,1);
        m_pParams[i]->setFixedWidth(180);
        switch(i)
        {
        case CURRENT:
        {
            QLabel * pTag = new QLabel(tr("A"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case VELOCITY:
        {
            QLabel * pTag = new QLabel(tr("RPM"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case POSITION:
        {
            QLabel * pTag = new QLabel(tr("R"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        case TEMP_PROTECT:
        case TEMP_RECOVERY:
        {
            QLabel * pTag = new QLabel(QString::fromLocal8Bit("℃"));
            pActualayout->addWidget(pTag,i,2,1,1);
        }
            break;
        default:
            break;
        }
    }
    m_pParams[CURRENT]->setReadOnly(true);
    m_pParams[VELOCITY]->setReadOnly(true);
    m_pParams[POSITION]->setReadOnly(true);

    pActuals->setLayout(pActualayout);
    pLayout->addWidget(pActuals,3,0,1,2);

    QGroupBox * pError = new QGroupBox(tr("Error"),this);
    QGridLayout * pErrorLayout = new QGridLayout();
    pError->setContentsMargins(0,5,0,0);
    m_pClearErrors->setFixedWidth(134);
    pErrorLayout->addWidget(m_pClearErrors,0,0,1,1);
    m_pErrorInfos = new QPushButton(tr("Error Info"),this);
    m_pErrorInfos->setFixedWidth(134);
    pErrorLayout->addWidget(m_pErrorInfos,1,0,1,1);
    //connect(m_pErrorInfos,&QPushButton::clicked,this,&ParamWidget::checkError);

    pErrorLayout->addWidget(m_pTableErrors,0,1,4,1);
    pError->setLayout(pErrorLayout);
    pLayout->addWidget(pError,4,0,1,2);


    QGroupBox * pEnd = new QGroupBox(this);
    QHBoxLayout * pEndLayout = new QHBoxLayout();
    pEndLayout->setContentsMargins(0,5,0,0);
    pEndLayout->addWidget(new QLabel(tr("Status")));

    m_pStatus->setFixedWidth(97);
    pEndLayout->addWidget(m_pStatus);
    pEndLayout->addStretch(4);

    pEnd->setLayout(pEndLayout);
    pLayout->addWidget(pEnd,5,0,1,2);

    initData();
}

void HomingWidget::initData()
{
    m_nDataId[MAX_POS] = MotorForm::POS_MAX_POS;
    m_nDataId[MIN_POS] = MotorForm::POS_MIN_POS;
    m_nDataId[VEL_SET] = MotorForm::VEL_SET;
    m_nDataId[VEL_ADD] = MotorForm::DATA_INVALID;
    m_nDataId[VEL_MIN] = MotorForm::HOMING_CUR_MIN;
    m_nDataId[VEL_MAX] = MotorForm::HOMING_CUR_MAX;
    m_nDataId[CURRENT] = MotorForm::CUR_ACTURAL;
    m_nDataId[VELOCITY] = MotorForm::VEL_ACTURAL;
    m_nDataId[POSITION] = MotorForm::POS_ACTURAL;
    m_nDataId[POS_OFFSET] = MotorForm::POS_OFFSET;
    m_nDataId[INERTIA] = MotorForm::INERTIA;
    m_nDataId[LOCK_ENERGY] = MotorForm::LOCK_ENERGY;
    m_nDataId[TEMP_PROTECT] = MotorForm::TEMP_PROTECT;
    m_nDataId[TEMP_RECOVERY] = MotorForm::TEMP_RECOVERY;

    for(int i=0;i<HOMING_PARAM_CNT;++i)
    {
        if(i == VEL_SET)
        {
            m_pParams[i]->setValidator(new QDoubleValidator);
        }
        if(i == LOCK_ENERGY || i == TEMP_PROTECT || i == TEMP_RECOVERY)
        {
            connect(m_pParams[i],&QLineEdit::returnPressed,this,&HomingWidget::valueChangeByUser);
        }
        else
        {
            connect(m_pParams[i],&QLineEdit::editingFinished,this,&HomingWidget::valueChangeByUser);
        }


        motorDataChange(m_nDeviceId,m_nDataId[i]);
    }
    motorDataChange(m_nDeviceId,MotorForm::HOMING_LIMIT);
    connect(m_pNeedLimit,&QCheckBox::clicked,[=](bool bCheck){
        quint8 value = bCheck?1:0;
        Mediator::getInstance()->setValueByUser(MotorForm::HOMING_LIMIT,value);
    });
    m_pParams[VEL_ADD]->setText(tr("%1").arg(100));
    connect(Mediator::getInstance(),&Mediator::dataChange,this,&HomingWidget::motorDataChange);
    connect(Mediator::getInstance(),&Mediator::onlineChange,this,&HomingWidget::onlineStatus);
}

void HomingWidget::valueChangeByUser()
{
    QLineEdit * pSender = qobject_cast<QLineEdit *>(sender());

    for(int i=0;i<HOMING_PARAM_CNT;++i)
    {
        if(m_pParams[i] == pSender && m_nDataId[i] != MotorForm::DATA_INVALID)
        {
            qreal value = pSender->text().toDouble();
            if(i == TEMP_PROTECT || i == TEMP_RECOVERY)
            {
                qreal protectTemp = m_pParams[TEMP_PROTECT]->text().toDouble();
                qreal recoveryTemp = m_pParams[TEMP_RECOVERY]->text().toDouble();
                if(protectTemp - recoveryTemp < 5)
                {

                    MsgBox::Tip(nullptr,tr("Warning"),QString::fromLocal8Bit("Protect temperature must be 5℃ or more above \nthe recovery temperature!"));
                    motorDataChange(m_nDeviceId,m_nDataId[i]);
                    return;
                }
            }

            Mediator::getInstance()->setValueByUser((MotorForm::Motor_Data_Id)(m_nDataId[i]),value);
            break;
        }

    }
}

void HomingWidget::motorDataChange(quint8 nDeviceId, int nId)
{
    if(!isCurrentDevice(nDeviceId))
        return;
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    switch (nId) {
    case MotorForm::DATA_INVALID:
        break;
    case MotorForm::HOMING_LIMIT:
        m_pNeedLimit->setChecked(Mediator::getInstance()->getValue((MotorForm::Motor_Data_Id)nId)>0);
        break;
    default:
    {
        for (int i=0;i<HOMING_PARAM_CNT;++i) {
            if(m_nDataId[i] == nId)
            {
                m_pParams[i]->setText(QString::number(Mediator::getInstance()->getValue((MotorForm::Motor_Data_Id)nId),'f',6).replace(rx,""));
                break;
            }
        }
    }
        break;
    }
}

void HomingWidget::enableMode(bool bEnable)
{
    if(bEnable)
    {
        m_pModeEdit->setText(tr("Homing Mode"));

    }
    else
        m_pModeEdit->setText("");

    for(int i=MAX_POS;i<HOMING_PARAM_CNT;++i)
    {
        m_pParams[i]->setEnabled(bEnable);
    }
    m_pParams[LOCK_ENERGY]->setEnabled(true);
    m_pParams[TEMP_PROTECT]->setEnabled(true);
    m_pParams[TEMP_RECOVERY]->setEnabled(true);
    m_pBtnCalibrate->setEnabled(bEnable);
    m_pBtnHoming->setEnabled(bEnable);
    m_pBtnActiveMode->setChecked(bEnable);
    m_pBtnVelAdd->setEnabled(bEnable);
    m_pBtnVelMinus->setEnabled(bEnable);
    m_pBtnHalt->setEnabled((bEnable));
    m_pBtnAuto->setEnabled(bEnable);
    m_pBtnManual->setEnabled(bEnable);
    m_pSetMax->setEnabled(bEnable);
    m_pSetMin->setEnabled(bEnable);
    if(bEnable)
        m_pBtnManual->click();//after all set, if is enable ,manual toggled;
}

void HomingWidget::homingCallback()
{
    setVelocity(0);
    qreal hominValue = Mediator::getInstance()->getValue(MotorForm::POS_ACTURAL)/*-Mediator::getInstance()->getValue(MotorForm::POS_MIN_POS)*/;
    Mediator::getInstance()->setValueByUser(MotorForm::POS_HOMING,hominValue);
    Mediator::getInstance()->homingSet();
    //
    //Mediator::getInstance()->setValueByUser(MotorForm::POS_HOMING,0,false);
//    Mediator::getInstance()->setValueByUser(MotorForm::POS_MAX_POS,Mediator::getInstance()->getValue(MotorForm::POS_MAX_POS)-hominValue,false);
    //    Mediator::getInstance()->setValueByUser(MotorForm::POS_MIN_POS,Mediator::getInstance()->getValue(MotorForm::POS_MIN_POS)-hominValue,false);
}

void HomingWidget::homingCallback2()
{
    setVelocity(0);
    qreal hominValue = Mediator::getInstance()->getValue(MotorForm::POS_ACTURAL)/*-Mediator::getInstance()->getValue(MotorForm::POS_MIN_POS)*/;
    qreal minPos = Mediator::getInstance()->getValue(MotorForm::POS_MIN_POS);
    qreal maxPos = Mediator::getInstance()->getValue(MotorForm::POS_MAX_POS);

    Mediator::getInstance()->setValueByUser(MotorForm::POS_HOMING,hominValue);
    Mediator::getInstance()->setValueByUser(MotorForm::POS_MAX_POS,maxPos);
    Mediator::getInstance()->setValueByUser(MotorForm::POS_MIN_POS,minPos);
    Mediator::getInstance()->homingSet();
}

void HomingWidget::velAddCallback()
{
    qreal add = m_pParams[VEL_ADD]->text().toDouble();
    qreal ori = Mediator::getInstance()->getValue(MotorForm::VEL_ACTURAL);
    setVelocity(ori+add);
}

void HomingWidget::velMinusCallback()
{
    qreal add = m_pParams[VEL_ADD]->text().toDouble();
    qreal ori = Mediator::getInstance()->getValue(MotorForm::VEL_ACTURAL);
    setVelocity(ori-add);
}

void HomingWidget::operationModeChanged(int nChangeId)
{
    switch (nChangeId) {
    case 2:
        m_pBtnVelAdd->setEnabled(true);
        m_pBtnVelMinus->setEnabled(true);
        m_pSetMax->setEnabled(false);
        m_pSetMin->setEnabled(false);
        break;
    case 1:
        m_pBtnVelAdd->setEnabled(false);
        m_pBtnVelMinus->setEnabled(false);
        m_pSetMax->setEnabled(true);
        m_pSetMin->setEnabled(true);
        break;
    default:
        break;
    }
    Mediator::getInstance()->setHomingOperation(nChangeId);
}

void HomingWidget::setVelocity(qreal vel)
{
    m_pParams[VEL_SET]->setText(tr("%1").arg(vel));
    emit m_pParams[VEL_SET]->editingFinished();
}
