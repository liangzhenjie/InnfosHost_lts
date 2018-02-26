#include "innfosWizard.h"
#include <QMap>
#include "innfostoolbar.h"
#include "innfosutil.h"
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include "mediator.h"
#include <QMessageBox>
#include <QDebug>
#include "framelesshelper.h"
#include <QSettings>
#include <QProcess>
#include <QApplication>
#include "actionitem.h"
#include "warningdialog.h"
#include "autorecoginze.h"

InnfosWizard::InnfosWizard(QWidget *parent) :
    QWidget(parent),
    m_nCurPageId(0)
{
    quint32 nTitleHeight = 40;
    quint32 nWindowSize = 600;
    setWindowFlags(Qt::FramelessWindowHint | windowFlags() /*| Qt::WindowStaysOnTopHint*/);
    setFixedSize(nWindowSize,nWindowSize);
    modifyPalette(this,QPalette::Window,QColor(29,45,60));

    InnfosToolbar * titleBar = new InnfosToolbar(this);
    modifyPalette(titleBar,QPalette::Window,QColor(232,50,39));
    titleBar->SetBtnHide(InnfosToolbar::Hide_Min | InnfosToolbar::Hide_Max | InnfosToolbar::Hide_Prew);
    titleBar->setFixedHeight(40);
    installEventFilter(titleBar);
    setWindowTitle(tr("Start Wizard"));
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

    QLabel * pLabel = new QLabel(this);
    pLabel->setPixmap(QPixmap(":/images/www.png"));
    pLayout->addWidget(pLabel,1,0,1,1);

    m_pBack = new QPushButton(tr("Back"),this);
    pLayout->addWidget(m_pBack,1,2,1,1);
    m_pBack->setFixedSize(100,20);

    m_pNext = new QPushButton(tr("Next"),this);
    pLayout->addWidget(m_pNext,1,3,1,1);
    m_pNext->setFixedSize(100,20);

    m_pCancel = new QPushButton(tr("Cancel"),this);
    pLayout->addWidget(m_pCancel,1,4,1,1);
    m_pCancel->setFixedSize(100,20);

    connect(m_pCancel,&QPushButton::clicked,this,&InnfosWizard::close);
    connect(m_pNext,&QPushButton::clicked,this,&InnfosWizard::next);
    connect(m_pBack,&QPushButton::clicked,this,&InnfosWizard::back);

    updateBtnStatus();

}

void InnfosWizard::addPage(InnfosWizardPage *page)
{
    if(page)
    {
        if(m_mPages.size() > 0)
        {
            page->hide();
        }
        m_mPages.insert(m_mPages.size(),page);
        QGridLayout * pLayout = qobject_cast<QGridLayout *>(layout());
        if (pLayout)
        {
            pLayout->addWidget(page,0,0,1,5,Qt::AlignHCenter|Qt::AlignVCenter);
        }
        connect(page,&InnfosWizardPage::isCompleteChange,this,&InnfosWizard::updateBtnStatus);
        updateBtnStatus();
    }
}

void InnfosWizard::FindAvailablePort(int nConnectStatus)
{
    if(!(nConnectStatus&AutoRecoginze::USB_CONNECTED))
    {
        WarningDialog * d = new WarningDialog(WarningDialog::USB_ERROR);
        d->show();
    }
    else if(!(nConnectStatus&AutoRecoginze::MOTOR_CONNECTED))
    {
        WarningDialog * d = new WarningDialog(WarningDialog::SERVO_ERROR);
        d->show();
    }
    else
    {
        close();
    }
//    if (nConnectStatus)
//    {
//        close();
//    }
//    else
//    {
//        //warning
//        int nRet = QMessageBox::warning(this,tr("Warning"),tr("The communication setting is not correct!"),QMessageBox::Ok);
//        if(nRet == QMessageBox::Ok)
//        {
//            QApplication::quit();
//        }

//    }
}

void InnfosWizard::back()
{
    if(m_nCurPageId > 0)
    {
        InnfosWizardPage * oldPage = m_mPages.value(m_nCurPageId);
        oldPage->hide();
        --m_nCurPageId;
        InnfosWizardPage * newPage = m_mPages.value(m_nCurPageId);
        newPage->show();
        updateBtnStatus();
    }


}

void InnfosWizard::next()
{
    if(m_nCurPageId < m_mPages.size()-1)
    {
        InnfosWizardPage * oldPage = m_mPages.value(m_nCurPageId);
        oldPage->hide();
        ++m_nCurPageId;
        InnfosWizardPage * newPage = m_mPages.value(m_nCurPageId);
        newPage->show();
        updateBtnStatus();

    }
    else if (m_nCurPageId == m_mPages.size()-1)
    {
        connect(Mediator::getInstance(),&Mediator::findAvailablePort,this,&InnfosWizard::FindAvailablePort);
        Mediator::getInstance()->autoRecognize();
        m_pNext->setEnabled(false);
        m_pCancel->setEnabled(false);
        m_pBack->setEnabled(false);
    }
}

void InnfosWizard::updateBtnStatus()
{
    if(m_mPages.size()==0)
    {
        m_pBack->setEnabled(false);
        m_pNext->setEnabled(false);
    }
    else
    {
        m_pBack->setEnabled(m_nCurPageId > 0);
        InnfosWizardPage * curPage = m_mPages.value(m_nCurPageId);
        if(curPage)
        {
            if(curPage->isComplete())
            {
                m_pNext->setEnabled(true);
//                if (isFinal())
//                {
//                    m_pNext->setText(tr("Finish"));
//                }
            }
            else
            {
                m_pNext->setEnabled(false);
            }
        }
    }

}

void InnfosWizard::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
}

bool InnfosWizard::isFinal()
{
    return m_nCurPageId == m_mPages.size()-1;
}

void InnfosWizard::Finished()
{

}

/*wizard page*/

InnfosWizardPage::InnfosWizardPage(QWidget *parent):
    QWidget(parent),
    m_bIsComplete(true)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags() | Qt::WindowStaysOnTopHint);
    setFixedSize(550,500);
    modifyPalette(this,QPalette::Window,QColor(41,60,75));
    setAutoFillBackground(true);
}

bool InnfosWizardPage::isComplete()
{
    return m_bIsComplete;
}

InnfosWizardPage *InnfosWizardPage::createPage1()
{
    InnfosWizardPage * pPage = new InnfosWizardPage();
    QVBoxLayout * pLayout = new QVBoxLayout(pPage);
    pLayout->addStretch(10);
    QLabel * pIcon = new QLabel();
    pIcon->setPixmap(QIcon(":/images/innfos.png").pixmap(100,130));
    pLayout->addWidget(pIcon,0,Qt::AlignHCenter);


    QLabel * pPlease = new QLabel(tr("please read the Getting started document \nfor a correct hardware installation!"));
    pPlease->setAlignment(Qt::AlignHCenter);
    pPlease->setObjectName("please");
    pLayout->addWidget(pPlease,0,Qt::AlignHCenter);

    QPushButton * pReadBtn = new QPushButton(tr("confirm that you've read the document!"),pPage);
    pReadBtn->setObjectName("read_doc");
    pReadBtn->setFixedWidth(260);
    pLayout->addWidget(pReadBtn,0,Qt::AlignHCenter);
    pReadBtn->setCheckable(true);

    QPushButton * pShowBtn = new QPushButton(tr("show the document!"),pPage);
    pLayout->addWidget(pShowBtn,0,Qt::AlignHCenter);
    pShowBtn->setFixedWidth(260);
    pLayout->addStretch(10);
    connect(pShowBtn,&QPushButton::clicked,[=]{
        QSettings set("\\HKEY_CLASSES_ROOT\\.pdf",QSettings::NativeFormat);
        //QStringList list = set.childKeys();
        qDebug() << set.value(".").toString();
        QString path("\\HKEY_CLASSES_ROOT\\");
        path += set.value(".").toString();
        path += ("\\shell\\open\\command");
        QSettings pdfSet(path,QSettings::NativeFormat);
        QString exePath = pdfSet.value(".").toString();
        if(exePath.size() > 1)
        {
            QStringList list = exePath.split('"');
            QStringList list2;
            QString appPath = QCoreApplication::applicationDirPath();
            appPath += "\\intro.pdf";
            list2 << appPath;
            QProcess * process = new QProcess(pPage);
            process->start(list.value(1),list2);
            qDebug()<<process->errorString();
        }
    });

    QLabel * pDownload = new QLabel();
    QString strHTML = QString("<html> \
                               <head> \
                               <style> \
                               font{color:#878088;} \
                               </style> \
                               </head> \
                               <body>\
                               <font>%1</font>\
                               <br/> \
                               <a href = \"%2\">%3</a>\
                                <br/> \
                                <font>%4</font>\
                               </body> \
                               </html>").arg("All documentations are available on the").arg("www.innfos.com")\
            .arg("http://www.innfos.com").arg("\n in the section \"Service & Downloads\"");
    pDownload->setText(strHTML);
    pDownload->setOpenExternalLinks(true);
    pDownload->setAlignment(Qt::AlignCenter);
    pLayout->addWidget(pDownload);
    //pShowBtn->setEnabled(false);
    pLayout->addStretch(10);

    pPage->SetComplete(false);
    connect(pReadBtn,&QPushButton::clicked,pPage,&InnfosWizardPage::SetComplete);

    return pPage;
}

InnfosWizardPage *InnfosWizardPage::createPage2()
{
    InnfosWizardPage * pPage = new InnfosWizardPage();
    QVBoxLayout * pLayout = new QVBoxLayout(pPage);
    pLayout->setContentsMargins(20,20,20,20);
    QLabel * pLogo = new QLabel(pPage);
    pLogo->setPixmap(QPixmap(":/images/micro.png"));
    pLayout->addWidget(pLogo);

    QLabel * pPlease = new QLabel(tr("Please connect the hardwre correctly according \nto the instruction chart before debugging."),pPage);
    pLayout->addWidget(pPlease);

    QLabel * pDetail = new QLabel(pPage);
    pDetail->setPixmap(QPixmap(":/images/connect.png"));
    pLayout->addWidget(pDetail,0,Qt::AlignHCenter);

    return pPage;
}

void InnfosWizardPage::SetComplete(bool bComplete)
{
    m_bIsComplete = bComplete;
    emit isCompleteChange();
}
