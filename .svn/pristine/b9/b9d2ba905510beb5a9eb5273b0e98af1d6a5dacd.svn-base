#include "msgbox.h"
#include "innfostoolbar.h"
#include "framelesshelper.h"
#include "innfosutil.h"
#include <QEventLoop>
#include <QIcon>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QList>

QList<int> * MsgBox::m_msgIdList = new QList<int>;
MsgBox::MsgBox(const QString &text, QWidget *parent, int nMsgId) :
    QWidget(parent),
    m_loop(nullptr),
    m_pContentLayout(nullptr),
    m_nMsgId(nMsgId)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags() /*| Qt::WindowStaysOnTopHint*/);
    //setFixedSize(nWindowSize,nWindowSize-180);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout * pLayout = new QVBoxLayout(this);
    pLayout->setContentsMargins(0,0,0,10);
    InnfosToolbar * titleBar = new InnfosToolbar(this);
    modifyPalette(titleBar,QPalette::Window,QColor(232,50,39));
    titleBar->SetBtnHide(InnfosToolbar::Hide_Min | InnfosToolbar::Hide_Max | InnfosToolbar::Hide_Prew);
    titleBar->setFixedHeight(40);
    installEventFilter(titleBar);
    //setWindowIcon(QIcon(QPixmap(":/images/logo.png").scaled(150,30)));
    titleBar->setToolBarIcon(QPixmap(":/images/logo.png").scaled(150,30));
    pLayout->addWidget(titleBar);


    m_pContentLayout = new QGridLayout();
    pLayout->addLayout(m_pContentLayout);
    //m_pContentLayout->setContentsMargins(10,20,10,10);


}

MsgBox::~MsgBox()
{
    m_msgIdList->removeOne(m_nMsgId);
}

int MsgBox::exec()
{
    setAttribute(Qt::WA_ShowModal,true);
    show();
    QEventLoop loop;
    m_loop = &loop;
    return loop.exec();
}

int MsgBox::Tip(QWidget *pParent, const QString &tittle, const QString &text)
{
    MsgBox box(text,pParent);
    box.setObjectName("tip");
    modifyPalette(&box,QPalette::Window,QColor(29,45,60));
    box.setWindowTitle(tittle);
    box.setFixedSize(450,220);
    QGridLayout * pLayout = box.getContentLayout();
    if(pLayout)
    {
        pLayout->setContentsMargins(20,10,20,0);
        QLabel * pContent = new QLabel(text);
        pLayout->addWidget(pContent,0,0,1,2,Qt::AlignCenter);
        pLayout->setRowStretch(0,2);

        QLabel * pLabel = new QLabel();
        pLabel->setPixmap(QPixmap(":/images/www.png"));
        pLayout->addWidget(pLabel,1,0,1,1);

        QPushButton * pBtn = new QPushButton("Ok");
        pBtn->setFixedWidth(100);
        connect(pBtn,&QPushButton::clicked,&box,&MsgBox::close);
        pLayout->addWidget(pBtn,1,1,1,1,Qt::AlignRight);

    }

    return box.exec();
}

int MsgBox::Error(QWidget *pParent, const QString &tittle, const QString &text, int nErrorId)
{
    if(m_msgIdList->contains(nErrorId))
        return 0;
    m_msgIdList->append(nErrorId);
    MsgBox box(text,pParent,nErrorId);
    box.setObjectName("tip");
    modifyPalette(&box,QPalette::Window,QColor(29,45,60));
    box.setWindowTitle(tittle);
    box.setFixedSize(450,220);
    QGridLayout * pLayout = box.getContentLayout();
    if(pLayout)
    {
        pLayout->setContentsMargins(20,10,20,0);
        QLabel * pContent = new QLabel(text);
        pLayout->addWidget(pContent,0,0,1,2,Qt::AlignCenter);
        pLayout->setRowStretch(0,2);

        QLabel * pLabel = new QLabel();
        pLabel->setPixmap(QPixmap(":/images/www.png"));
        pLayout->addWidget(pLabel,1,0,1,1);

        QPushButton * pBtn = new QPushButton("Ok");
        pBtn->setFixedWidth(100);
        connect(pBtn,&QPushButton::clicked,&box,&MsgBox::close);
        pLayout->addWidget(pBtn,1,1,1,1,Qt::AlignRight);

    }

    return box.exec();
}

int MsgBox::Warning(QWidget *pParent, const QString &tittle, const QString &text)
{
    MsgBox box(text,pParent);
    box.setObjectName("warning");
    modifyPalette(&box,QPalette::Window,QColor(255,255,255));
    box.setWindowTitle(tittle);
    box.setFixedSize(450,220);
    QGridLayout * pLayout = box.getContentLayout();
    if(pLayout)
    {
        QLabel * pLogo = new QLabel();
        pLogo->setPixmap(QPixmap(":/images/micro1.png").scaled(120,20));
        pLayout->addWidget(pLogo,0,0,1,1,Qt::AlignLeft);

        pLayout->setContentsMargins(20,20,20,0);
        QLabel * pContent = new QLabel(text);
        pLayout->addWidget(pContent,1,0,1,2,Qt::AlignLeft|Qt::AlignTop);
        pLayout->setRowStretch(1,1);

        QLabel * pIcon = new QLabel();
        pIcon->setScaledContents(true);
        pIcon->setPixmap(QPixmap(":/images/tanhao.png").scaled(40,40));
        pLayout->addWidget(pIcon,2,0,1,1,Qt::AlignLeft);
        //pLayout->setRowStretch(2,1);

        QLabel * pLabel = new QLabel();
        pLabel->setPixmap(QPixmap(":/images/www.png"));
        pLayout->addWidget(pLabel,3,0,1,1);
        pLayout->setRowStretch(3,2);

        QPushButton * pBtn = new QPushButton("Ok");
        pBtn->setFixedWidth(100);
        connect(pBtn,&QPushButton::clicked,&box,&MsgBox::close);
        pLayout->addWidget(pBtn,3,1,1,1,Qt::AlignRight);

    }
    return box.exec();
}

void MsgBox::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    if(!visible)
    {
        m_loop->exit();
    }
}
