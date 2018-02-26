#include "innfostoolbar.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPalette>

#ifdef Q_OS_WIN
#pragma comment(lib,"user32.lib")
#include <qt_windows.h>
#endif

InnfosToolbar::InnfosToolbar(QWidget *parent)
    : QWidget(parent),
      m_pCloseButton(nullptr),
      m_pIconLabel(nullptr),
      m_pMaximizeButton(nullptr),
      m_pMinimizeButton(nullptr),
      m_pTittleLabel(nullptr),
      m_pPreview(nullptr)
{
    setFixedHeight(55);
    QPalette pal(palette());
    pal.setColor(QPalette::Background,QColor(17,29,43));
    setAutoFillBackground(true);
    setPalette(pal);
    m_pIconLabel = new QLabel(this);

    m_pTittleLabel = new QLabel(this);
    QPalette pal2 = m_pTittleLabel->palette();
    pal2.setColor(QPalette::WindowText,QColor(255,255,255));
    m_pTittleLabel->setPalette(pal2);
    m_pPreview = new QPushButton(this);
    m_pMinimizeButton = new QPushButton(this);
    m_pMaximizeButton = new QPushButton(this);
    m_pCloseButton = new QPushButton(this);

    //m_pIconLabel->setFixedSize(20,20);
    m_pIconLabel->setScaledContents(true);

    m_pTittleLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    m_pMinimizeButton->setFixedSize(25,25);
    m_pMaximizeButton->setFixedSize(25,25);
    m_pCloseButton->setFixedSize(25,25);
    m_pPreview->setFixedSize(25,25);

    m_pTittleLabel->setObjectName("whiteLabel");
    m_pMaximizeButton->setObjectName("maximizeButton");
    m_pMinimizeButton->setObjectName("minimzieButton");
    m_pCloseButton->setObjectName("closeButton");
    m_pPreview->setObjectName("previewButton");

    m_pMinimizeButton->setToolTip("Minimize");
    m_pMaximizeButton->setToolTip("Maximize");
    m_pCloseButton->setToolTip("Close");
    m_pPreview->setToolTip("preview");

    QHBoxLayout * pLayout = new QHBoxLayout(this);
    pLayout->addWidget(m_pIconLabel);
    pLayout->addSpacing(5);
    pLayout->addWidget(m_pTittleLabel);
    pLayout->addWidget(m_pPreview);
    pLayout->addWidget(m_pMinimizeButton);
    pLayout->addWidget(m_pMaximizeButton);
    pLayout->addWidget(m_pCloseButton);
    pLayout->setSpacing(5);
    pLayout->setContentsMargins(0,0,15,0);
    setLayout(pLayout);

    connect(m_pMinimizeButton,SIGNAL(clicked(bool)),this,SLOT(onClicked()));
    connect(m_pMaximizeButton,SIGNAL(clicked(bool)),this,SLOT(onClicked()));
    connect(m_pCloseButton,SIGNAL(clicked(bool)),this,SLOT(onClicked()));
    connect(m_pPreview,SIGNAL(clicked(bool)),this,SLOT(onClicked()));
}

InnfosToolbar::~InnfosToolbar()
{

}

void InnfosToolbar::SetBtnHide(int nParam)
{
    if(nParam & Hide_None)
    {
        m_pMinimizeButton->setHidden(false);
        m_pMinimizeButton->setHidden(false);
        m_pMinimizeButton->setHidden(false);
    }
    if((nParam&Hide_Min) && m_pMinimizeButton)
        m_pMinimizeButton->setHidden(true);
    if((nParam&Hide_Max) && m_pMaximizeButton)
        m_pMaximizeButton->setHidden(true);
    if((nParam&Hide_Prew) && m_pPreview)
        m_pPreview->setHidden(true);
}

void InnfosToolbar::setIconSize(QSize s)
{
    m_pIconLabel->setFixedSize(s.width(),s.height());
}

QPushButton *InnfosToolbar::getCloseBtn() const
{
    return m_pCloseButton;
}

void InnfosToolbar::setToolBarIcon(const QPixmap &icon)
{
    m_pIconLabel->setPixmap(icon);
}

void InnfosToolbar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if(m_pMaximizeButton->isVisible())
    {
        emit m_pMaximizeButton->clicked();
    }

}

//void InnfosToolbar::mousePressEvent(QMouseEvent *event)
//{
//#ifdef Q_OS_WIN
//    if(ReleaseCapture())
//    {
//        QWidget * pWindow = this->window();
//        if(pWindow->isTopLevel())
//        {
//            SendMessage(HWND(pWindow->winId()),WM_SYSCOMMAND,SC_MOVE + HTCAPTION,0);
//        }
//    }
//    event->ignore();
//#else
//#endif
//}

bool InnfosToolbar::eventFilter(QObject *watched, QEvent *event)
{
    switch(event->type())
    {
    case QEvent::WindowTitleChange:
    {
        QWidget * pWidget = qobject_cast<QWidget *>(watched);
        if(pWidget)
        {
            m_pTittleLabel->setText(pWidget->windowTitle());
            return true;
        }

    }
    case QEvent::WindowIconChange:
    {
        QWidget * pWidget = qobject_cast<QWidget *>(watched);
        if(pWidget)
        {
            QIcon icon = pWidget->windowIcon();
            QList<QSize> sizes = icon.availableSizes();
            if(sizes.size() > 0)
                m_pIconLabel->setPixmap(icon.pixmap(sizes.at(0)));
            return true;
        }
    }
    case QEvent::WindowStateChange:
    case QEvent::Resize:
        updateMaximize();
        return false;//because other objects need these events,so return false
    }
    return QWidget::eventFilter(watched,event);
}

void InnfosToolbar::onClicked()
{
    QPushButton * pBtn = qobject_cast<QPushButton *>(sender());
    QWidget * pWindow = this->window();
    if(pWindow->isTopLevel())
    {
        if(pBtn == m_pMinimizeButton)
        {
            pWindow->showMinimized();
        }
        else if(pBtn == m_pMaximizeButton)
        {
            bool bMaximize = pWindow->isMaximized();
            if(bMaximize)
            {
                pWindow->showNormal();
            }
            else
            {
                pWindow->showMaximized();
            }

        }
        else if(pBtn == m_pCloseButton || pBtn == m_pPreview)
        {
            pWindow->close();
        }
    }
}

void InnfosToolbar::updateMaximize()
{
    QWidget * pWindow = this->window();
    if(pWindow->isTopLevel())
    {
        bool bMaximize = pWindow->isMaximized();
        if(bMaximize)
        {
            m_pMaximizeButton->setToolTip(tr("Restore"));
            m_pMaximizeButton->setProperty("maximizeProperty","restore");
        }
        else
        {
            m_pMaximizeButton->setToolTip(tr("Maximize"));
            m_pMaximizeButton->setProperty("maximizeProperty","maximize");
        }
        m_pMaximizeButton->setStyle(QApplication::style());
    }
}
