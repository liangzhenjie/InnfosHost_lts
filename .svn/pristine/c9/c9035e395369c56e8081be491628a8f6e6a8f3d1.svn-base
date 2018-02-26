#include "innfoswindow.h"
#include <QApplication>
#include <QFont>
#include <QFile>
#include "innfosWizard.h"
#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QDateTime>
#include <QDebug>
#include <QtMath>
#include "mediator.h"
#include "msgbox.h"
#include "filter.h"
#include <QDesktopWidget>

//#define LOG_TO_FILE

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    switch (type)
    {
        case QtDebugMsg:
                txt = QString("Debug: %1").arg(msg);
                break;
            case QtWarningMsg:
                txt = QString("Warning: %1").arg(msg);
                break;
            case QtCriticalMsg:
                txt = QString("Critical: %1").arg(msg);
                break;
            case QtFatalMsg:
                txt = QString("Fatal: %1").arg(msg);
                abort();
    }
    QFile outFile("debuglog.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append|QIODevice::Text);
    QTextStream ts(&outFile);
    ts << txt << endl;

    //输出到控制台
    QDateTime now = QDateTime::currentDateTime();
    std::cout << now.toString("hh:mm:ss.zzz").toStdString() << " "<< txt.toStdString() <<std::endl;
}

int main(int argc, char *argv[])
{
#ifdef LOG_TO_FILE
    qInstallMessageHandler (customMessageHandler);
#endif
    QApplication a(argc, argv);
    QFile qss(":/qss/myqss.qss");
    qss.open(QFile::ReadOnly);
    a.setStyleSheet(qss.readAll());
    qss.close();
    a.setFont(QFont(QString::fromLocal8Bit("微软雅黑")));
    InnfosWizard w;
    w.setObjectName("wizard");
    w.addPage(InnfosWizardPage::createPage1());
    w.addPage(InnfosWizardPage::createPage2());
    //const QRect rc = QApplication::desktop()->screenGeometry();
    //w.move((rc.width()-w.size().width())/2,(rc.height()-w.size().height())/2);
    w.show();
//      Filter w;
//      w.show();
//    WarningDialog d(WarningDialog::USB_ERROR);
//    d.show();
//    AngleClock w;
//    w.angleChange(30);
//    w.show();

//    WaveTriggerWidget w;
//    w.show();


//    EditorWidget widget;
//    widget.addItems(2222);
//    widget.show();



    if(a.exec() == 0)
    {
        Mediator::destroyAllStaticObjects();//remove all static objects before application to be destroyed;
        return 0;
    }
    return -1;
}
