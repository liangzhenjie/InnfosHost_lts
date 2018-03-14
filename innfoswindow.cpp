#include "innfoswindow.h"
#include "innfostoolbar.h"
#include <QIcon>
#include <QVBoxLayout>
#include <QPushButton>
#include <QButtonGroup>
#include "innfosutil.h"
#include "paramwidget.h"
#include "framelesshelper.h"
#include <QStackedWidget>
#include <QLabel>
#include <QPixmap>
#include "motorform.h"
#include "innfosproxy.h"
#include "mediator.h"
#include "innfoschartwidget.h"
#include "filter.h"

InnfosWindow::InnfosWindow(quint8 nDeviceId,QWidget *parent, int nMode, QString nodeName)
    : QWidget(parent),
      m_pStackedWidget(nullptr),
      m_pGraph(nullptr),
      m_nCurMode(nMode),
      m_nDeviceId(nDeviceId)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags()/* | Qt::WindowStaysOnTopHint*/);
    resize(900,660);
    modifyPalette(this,QPalette::Window,QColor(29,45,60));
    InnfosToolbar * titleBar = new InnfosToolbar(this);
    installEventFilter(titleBar);

    FramelessHelper * pHelper = new FramelessHelper(this);
    pHelper->activateOn(this);
    pHelper->setTitleHeight(titleBar->height());
    pHelper->setWidgetMovable(true);  //设置窗体可移动
    pHelper->setWidgetResizable(true);  //设置窗体可缩放
    //pHelper->setRubberBandOnResize(true);  //设置橡皮筋效果-可缩
    //pHelper->setRubberBandOnMove(true);  //设置橡皮筋效果-可移动
    setWindowTitle(tr("UserInterface Version0.2 /%1").arg(nodeName));
//#ifdef MAXSION_DEVICE
//    setWindowTitle(tr("UserInterface Version2.0 /%1").arg(nodeName));
//#else
//    setWindowTitle(tr("UserInterface Version1.0 /%1").arg(nodeName));
//#endif
    //setWindowIcon(QIcon(QPixmap(":/images/logo.png").scaled(210,55)));
    titleBar->setToolBarIcon(QPixmap(":/images/logo.png").scaled(210,55));
    QGridLayout * pLayout = new QGridLayout(this);

    pLayout->setContentsMargins(0,0,0,0);
    pLayout->addWidget(titleBar,0,0,1,2);

    QButtonGroup * pBtnGroup = new QButtonGroup(this);

    QPushButton * pCur = createInnfosBtn(":/images/icon_cur_t.png",":/images/icon_cur.png",tr("Current Regulation    "),"cur_mode");
    pBtnGroup->addButton(pCur,1);
    pLayout->addWidget(pCur,1,0,1,1);

    QPushButton * pVel = createInnfosBtn(":/images/icon_vel_t.png",":/images/icon_vel.png",tr("Velocity Regulation   "),"vel_mode");
    pBtnGroup->addButton(pVel,2);
    pLayout->addWidget(pVel,2,0,1,1);

    QPushButton * pPos = createInnfosBtn(":/images/icon_pos_t.png",":/images/icon_pos.png",tr("Position Regulation   "),"pos_mode");
    pBtnGroup->addButton(pPos,3);
    pLayout->addWidget(pPos,3,0,1,1);

    QPushButton * pProfile = createInnfosBtn(":/images/icon_profile_t.png",":/images/icon_profile.png",tr("Profile Position Mode"),"profile_mode");
    pBtnGroup->addButton(pProfile,4);
    pLayout->addWidget(pProfile,4,0,1,1);

    QPushButton * pProfileVel = createInnfosBtn(":/images/icon_profile_v_t.png",":/images/icon_profile_v.png",tr("Profile Velocity Mode"),"homing_mode");
    pBtnGroup->addButton(pProfileVel,5);
    pLayout->addWidget(pProfileVel,5,0,1,1);

    QPushButton * pHoming = createInnfosBtn(":/images/icon_homing_t.png",":/images/icon_homing.png",tr("Homing Mode           "),"homing_mode");
    pBtnGroup->addButton(pHoming,6);
    pLayout->addWidget(pHoming,6,0,1,1);

    int nIdx[MotorForm::Mode_Homing]={0,1,2,3,0,0,4,5};
    if(nIdx[nMode] > 0)
    {
        QPushButton *pBtn = qobject_cast<QPushButton *>(pBtnGroup->button(nIdx[nMode]));
        if(pBtn)
        {
            pBtn->setChecked(true);//
        }

    }

    m_pStackedWidget = new QStackedWidget(this);
    CurWidget * pCurMode = new CurWidget(m_nDeviceId,MotorForm::Mode_Cur,this);
    m_pStackedWidget->addWidget(pCurMode);
    VelWidget * pVelMode = new VelWidget(m_nDeviceId,MotorForm::Mode_Vel,this);
    m_pStackedWidget->addWidget(pVelMode);
    PosWidget * pPosMode = new PosWidget(m_nDeviceId,MotorForm::Mode_Pos,this);
    m_pStackedWidget->addWidget(pPosMode);
    ProfilePosWidget * pProfilePosMode = new ProfilePosWidget(m_nDeviceId,MotorForm::Mode_Profile_Pos,this);
    m_pStackedWidget->addWidget(pProfilePosMode);

    ProfileVelWidget * pProfileVelMode = new ProfileVelWidget(m_nDeviceId,MotorForm::Mode_Profile_Vel,this);
    m_pStackedWidget->addWidget(pProfileVelMode);

    HomingWidget * pHomingMode = new HomingWidget(m_nDeviceId,MotorForm::Mode_Homing,this);
    m_pStackedWidget->addWidget(pHomingMode);

    pLayout->addWidget(m_pStackedWidget,1,1,11,1);

    if(nIdx[nMode] > 0)
    {
        m_pStackedWidget->setCurrentIndex(nIdx[nMode]-1);
    }

    connect(pBtnGroup,static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),[=](int nId){
        m_pStackedWidget->setCurrentIndex(nId-1);});
    QPushButton * pLoad = new QPushButton(tr("Download"));
    pLoad->setObjectName("download");
    pLoad->setFixedWidth(130);
    connect(pLoad,&QPushButton::clicked,[=]{Mediator::getInstance()->saveDataToDevice();});
    pLayout->addWidget(pLoad,8,0,1,1,Qt::AlignHCenter);


    QPushButton * pImport = new QPushButton(tr("Import"));
    pImport->setObjectName("save");
    pImport->setFixedWidth(130);
    connect(pImport,&QPushButton::clicked,Mediator::getInstance(),&Mediator::loadDataFromFile);
    pLayout->addWidget(pImport,9,0,1,1,Qt::AlignHCenter);

    QPushButton * pSave = new QPushButton(tr("Export"));
    pSave->setObjectName("save");
    pSave->setFixedWidth(130);
    connect(pSave,&QPushButton::clicked,Mediator::getInstance(),&Mediator::saveDataToFile);
    pLayout->addWidget(pSave,10,0,1,1,Qt::AlignHCenter);

    QPushButton * pFilter = new QPushButton(tr("Filter"));
    pFilter->setObjectName("save");
    pFilter->setFixedWidth(130);
    connect(pFilter,&QPushButton::clicked,[=]{
        Filter::ShowWindowIfNotExist(m_nDeviceId);
    });
    pLayout->addWidget(pFilter,11,0,1,1,Qt::AlignHCenter);

    connect(Mediator::getInstance(),&Mediator::modeChange,[=](quint8 nDeviceId,int nMode){
        if(nDeviceId == m_nDeviceId)
        {
            enableMode(nMode);
        }
    });

    pLayout->setRowStretch(7,6);
    pLayout->setRowStretch(9,1);
    pLayout->setRowStretch(11,2);
    pLayout->setColumnMinimumWidth(0,209);
    pLayout->setRowMinimumHeight(9,60);
    pLayout->setColumnStretch(1,11);
    pLayout->setContentsMargins(0,0,0,0);
    pLayout->setSpacing(0);
    //setLayout(pLayout);
    enableMode(nMode);
}

InnfosWindow::~InnfosWindow()
{
    if(m_pGraph)
    {
        closeGraph();
    }
}

void InnfosWindow::enableMode(int nMode)
{
    int nCount = m_pStackedWidget->count();
    m_nCurMode = nMode;
//    bool bShowChart = false;
//    QRect rc(0,0,-1,-1);
//    for(int i=0;i<nCount;++i)
//    {
//        ParamWidget * pWidget = qobject_cast<ParamWidget *>(m_pStackedWidget->widget(i));
//        if(pWidget && pWidget->ChartShowing())
//        {
//            bShowChart = true;
//            rc = pWidget->chartGeometry();
//            break;
//        }
//    }
    for(int i=0;i<nCount;++i)
    {
        ParamWidget * pWidget = qobject_cast<ParamWidget *>(m_pStackedWidget->widget(i));
        if(pWidget)
        {
            if(pWidget->getModeId() == nMode)
            {
                pWidget->enableMode(true);
//                if(bShowChart)
//                {
//                    if (rc.isValid())
//                    {
//                        pWidget->showGraphWithRect(rc);
//                    }
//                    else
//                    {
//                        pWidget->showGraph();
//                    }

//                }
            }
            else
            {
                pWidget->enableMode(false);
                //pWidget->closeGraph();
            }

        }
    }
    CombineChartWidget * pChart = qobject_cast<CombineChartWidget*>(m_pGraph);
    if(pChart)
        pChart->onModeChange(nMode);
//    if(bShowChart)
//        Mediator::getInstance()->openChart();//notify monitor chart has opened
}

void InnfosWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    //int a = 0;
    deleteLater();
    ParamWidget * pWidget = qobject_cast<ParamWidget *>(m_pStackedWidget->currentWidget());
    if(pWidget)
    {
        switch (pWidget->getModeId()) {
        case MotorForm::Mode_Profile_Pos:
        case MotorForm::Mode_Pos:
            Mediator::getInstance()->activeMode(MotorForm::Mode_Profile_Pos);
            break;
        case MotorForm::Mode_Profile_Vel:
            Mediator::getInstance()->activeMode(MotorForm::Mode_Vel);
            break;
        default:
            break;
        }
    }
    Filter::closeFilter();
}

void InnfosWindow::showGraph()
{
    if(!m_pGraph)
    {
        m_pGraph = new CombineChartWidget((MotorForm::Motor_Mode)m_nCurMode);
        m_pGraph->show();
    }
    else {
        m_pGraph->raise();
    }

}

void InnfosWindow::closeGraph()
{
    if(m_pGraph)
    {
        m_pGraph->close();
        m_pGraph = nullptr;
    }
}


QPushButton * InnfosWindow::createInnfosBtn(const QString & iconNormal, const QString & iconSelect, const QString & text, const QString &objName)
{
    QIcon curIcon = QIcon(iconNormal);
    curIcon.addFile(iconSelect,QSize(),QIcon::Active,QIcon::On);
    QPushButton * pCur = new QPushButton(curIcon,text,this);
    pCur->setIconSize(QSize(30,30));
    pCur->setFixedHeight(80);
    pCur->setCheckable(true);
    //pCur->setChecked(true);
    pCur->setObjectName(objName);

    return pCur;
}
