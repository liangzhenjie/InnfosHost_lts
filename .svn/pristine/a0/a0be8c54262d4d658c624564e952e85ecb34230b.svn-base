#include "launchwindow.h"
#include "ui_launchwindow.h"
#include <QTimer>
#include "mediator.h"


launchWindow::launchWindow(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::launchWindow),
    m_pLoop(nullptr)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
}

launchWindow::~launchWindow()
{
    delete ui;
}

int launchWindow::exec()
{

    setAttribute(Qt::WA_ShowModal,true);
    show();
    QEventLoop loop;
    m_pLoop = &loop;
    return loop.exec();
}

void launchWindow::startLaunchTimer()
{
    QTimer::singleShot(3000,[=]{
        //mediator->checkServosStatus();
        m_pLoop->exit();
        this->close();
    });
}

int launchWindow::launchTip(QWidget *pParent)
{
    launchWindow * pWindow = new launchWindow(pParent);
    pWindow->setFixedSize(450,120);
    pWindow->startLaunchTimer();
    return pWindow->exec();
}
