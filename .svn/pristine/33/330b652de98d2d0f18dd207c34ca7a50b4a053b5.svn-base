#include "inertiamove.h"
#include <QEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QtMath>
#include <QDebug>

const int calInterval = 10;

InertiaMove::~InertiaMove()
{
    delete m_pMouseTimer;
}

InertiaMove::InertiaMove(QObject *parent) :
    QObject(parent),
    m_pressPos(QPointF()),
    m_pCalcTimer(nullptr),
    m_pMouseTimer(nullptr),
    m_fAcc(1000)
{
    //parent->installEventFilter(this);
    m_pCalcTimer = new QTimer(this);
    connect(m_pCalcTimer,&QTimer::timeout,this,&InertiaMove::calcMove);
    m_pCalcTimer->setInterval(calInterval);

    m_pMouseTimer = new QElapsedTimer();
}

bool InertiaMove::eventFilter(QObject *watched, QEvent *event)
{

    switch (event->type())
    {
    case QEvent::MouseButtonPress:
    {
        //qDebug() << "press"<< m_pressPos;
        stop();
        QMouseEvent * moustEvt = static_cast<QMouseEvent*>(event);
        m_pressPos = moustEvt->globalPos();
        m_curPos = m_pressPos;
        m_pMouseTimer->restart();
        //return true;
    }
        break;
    case QEvent::MouseMove:
    {
        QMouseEvent * moustEvt = static_cast<QMouseEvent*>(event);

        if(!m_pressPos.isNull())
        {
            //qDebug() << "move"<< m_pressPos << m_curPos << moustEvt->globalPos();
            movePos(moustEvt->globalPos()-m_curPos);

            m_curPos = moustEvt->globalPos();

        }
        //return true;
    }
        break;
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent * moustEvt = static_cast<QMouseEvent*>(event);
        m_posSpeed = (moustEvt->globalPos()-m_pressPos)*1000/m_pMouseTimer->elapsed();
        qreal angle = qAtan2(m_posSpeed.y(),m_posSpeed.x());
        m_posAcc.setX(m_fAcc*qCos(angle));
        m_posAcc.setY(m_fAcc*qSin(angle));

        m_pressPos = QPointF();
        m_pCalcTimer->start();
        //return true;
    }
        break;
    default:
        break;
    }
    return /*QObject::eventFilter(watched,event)*/false;
}

void InertiaMove::setAcc(qreal acc)
{
    m_fAcc = acc;
}

void InertiaMove::stop()
{
    m_pressPos = QPointF();
    m_pCalcTimer->stop();
    m_posSpeed = QPointF();
    m_posAcc = QPointF();
}

void InertiaMove::calcMove()
{
    if(!m_posSpeed.isNull())
    {
        QPointF pos;
        pos.setX(m_posSpeed.x()*calInterval/1000);
        pos.setY(m_posSpeed.y()*calInterval/1000);
        movePos(pos);
        qDebug() << "timer" << pos;
        m_posSpeed.setX(m_posSpeed.x()-m_posAcc.x()*calInterval/1000);
        m_posSpeed.setY(m_posSpeed.y()-m_posAcc.y()*calInterval/1000);
        if(qAbs(m_posSpeed.x()) < qAbs(m_posAcc.x()*calInterval/1000))
        {
            stop();
        }
    }

}
