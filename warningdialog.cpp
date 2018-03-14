#include "warningdialog.h"
#include "innfostoolbar.h"
#include "framelesshelper.h"
#include "innfosutil.h"
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include "mediator.h"
#include <QApplication>

WarningDialog::WarningDialog(ErrorType type,QWidget *parent) :
    QWidget(parent)
{
    quint32 nTitleHeight = 40;
    quint32 nWindowSize = 600;
    setWindowFlags(Qt::FramelessWindowHint | windowFlags() /*| Qt::WindowStaysOnTopHint*/);
    setFixedSize(nWindowSize,nWindowSize-180);
    modifyPalette(this,QPalette::Window,QColor(255,255,255));

    InnfosToolbar * titleBar = new InnfosToolbar(this);
    modifyPalette(titleBar,QPalette::Window,QColor(232,50,39));
    titleBar->SetBtnHide(InnfosToolbar::Hide_Min | InnfosToolbar::Hide_Max | InnfosToolbar::Hide_Prew);
    titleBar->setFixedHeight(40);
    installEventFilter(titleBar);
    setWindowTitle(tr("Error"));
    //setWindowIcon(QIcon(QPixmap(":/images/logo.png").scaled(150,30)));
    titleBar->setToolBarIcon(QPixmap(":/images/logo.png").scaled(150,30));
    titleBar->setFixedHeight(nTitleHeight);
    titleBar->setGeometry(0,0,nWindowSize,nTitleHeight);

    FramelessHelper * pHelper = new FramelessHelper(this);
    pHelper->activateOn(this);
    pHelper->setTitleHeight(titleBar->height());
    pHelper->setWidgetMovable(true);  //设置窗体可移动
    //pHelper->setRubberBandOnMove(true);  //设置橡皮筋效果-可移动

    QGridLayout * pLayout = new QGridLayout(this);
    pLayout->setGeometry(QRect(110,nTitleHeight,nWindowSize,nWindowSize-nTitleHeight));
    pLayout->setColumnStretch(1,1);
    pLayout->setContentsMargins(20,50,20,20);

    QVBoxLayout * pVLayout = new QVBoxLayout();
    pVLayout->setContentsMargins(10,10,10,10);
    QLabel * pLogo = new QLabel();
    pLogo->setPixmap(QPixmap(":/images/micro1.png"));
    pVLayout->addWidget(pLogo);

    const char * tip[ERROR_CNT] = {"USB communication failed,\n please check your USB connection and try again!",
                                  "CAN communication failed,\n please check your CAN connection and try again!"};
    QLabel * pPlease = new QLabel(tr(tip[type]));
    pVLayout->addWidget(pPlease);

    QLabel * pDetail = new QLabel();
    pDetail->setScaledContents(true);
    pDetail->setPixmap(QPixmap(":/images/warning.png"));
    pDetail->setFixedSize(540,233);
    pVLayout->addWidget(pDetail,0,Qt::AlignHCenter);
    QPoint pos[ERROR_CNT] = {QPoint(340,90),QPoint(170,90)};
    QLabel * pError = new QLabel(pDetail);
    pError->setScaledContents(true);
    pError->setPixmap(QPixmap(":/images/tanhao.png"));
    pError->setFixedSize(30,30);
    pError->move(pos[type]);

    pLayout->addLayout(pVLayout,0,0,1,5,Qt::AlignHCenter|Qt::AlignVCenter);

    QLabel * pLabel = new QLabel(this);
    pLabel->setPixmap(QPixmap(":/images/www.png"));
    pLayout->addWidget(pLabel,1,0,1,1);

    m_pRetry = new QPushButton(tr("Retry"),this);
    pLayout->addWidget(m_pRetry,1,3,1,1);
    m_pRetry->setFixedSize(100,20);
    connect(m_pRetry,&QPushButton::clicked,this,&WarningDialog::retry);

    m_pCancel = new QPushButton(tr("Cancel"),this);
    pLayout->addWidget(m_pCancel,1,4,1,1);
    m_pCancel->setFixedSize(100,20);
    connect(m_pCancel,&QPushButton::clicked,[=]{QApplication::quit();});

    connect(titleBar->getCloseBtn(),&QPushButton::clicked,[=]{QApplication::quit();});
}

void WarningDialog::retry()
{
    Mediator::getInstance()->autoRecognize(true);
    close();
}
