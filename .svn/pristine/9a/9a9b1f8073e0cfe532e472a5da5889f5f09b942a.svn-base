#include "angleclock.h"
#include <QPainter>
#include <QPixmap>
#include <QTransform>
#include <QtMath>
#include <QImage>
#include <QTimer>
#include <QtMath>

AngleClock::AngleClock(QWidget *parent) :
    QWidget(parent),
    m_angle(0),
    m_pTimer(nullptr),
    m_angleAcc(0)
{
    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(1);
    connect(m_pTimer,&QTimer::timeout,this,&AngleClock::calculate);
    setFixedSize(75,75);
}

AngleClock::~AngleClock()
{

}

void AngleClock::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();
    QRect rc = rect();
    qreal x = rc.width()/2;
    qreal y = rc.height()/2;
    rc.moveCenter(QPoint(0,0));
    QImage bg(":/images/clockbg.png");
    bg = bg.scaled(rc.width(),rc.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    painter.translate(x,y);
    painter.drawImage(rc,bg);

    painter.setFont(QFont("微软雅黑",7));

    painter.setPen(QColor(255,255,255));
    QRect rcTxt = rc;
    rcTxt.moveCenter(QPoint(0,rc.height()/5.0));
    painter.drawText(rcTxt,m_tip,QTextOption(Qt::AlignCenter));

    QImage pointer(":/images/pointer.png");
    bg = bg.scaled(rc.width()*0.9,rc.height()*0.9,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    painter.rotate(m_angle);
    qreal marRmX = rc.width()*0.1;
    qreal marRmY = rc.height()*0.1;
    rc = rc.marginsRemoved(QMargins(marRmX,marRmY,marRmX,marRmY));
    painter.drawImage(rc,pointer);
    painter.restore();
}

void AngleClock::angleChange(qreal angle)
{
    if(needAdd(angle))
    {
        m_waitList.push_back(angle);
        if(!m_pTimer->isActive())
            m_pTimer->start();
    }
}

void AngleClock::angleChangeByCircle(qreal circle)
{
    if(needAdd(circle*360))
    {
        m_waitList.push_back(circle * 360);
        if(!m_pTimer->isActive())
            m_pTimer->start();
    }

}

void AngleClock::modeChange(MotorForm::Motor_Mode mode)
{
    switch (mode) {
    case MotorForm::Mode_Cur:
        m_tip = "C(x0.165A)";
        break;
    case MotorForm::Mode_Vel:
    case MotorForm::Mode_Profile_Vel:
        m_tip = "V(x600rpm)";
        break;
    case MotorForm::Mode_Pos:
    case MotorForm::Mode_Profile_Pos:
        m_tip = "P(x1R)";
        break;
    default:
        break;
    }
}

void AngleClock::calculate()
{
    if(m_waitList.size() > 0)
    {
        qreal value = m_waitList.at(0);
        if(m_angleAcc == 0)
            m_angleAcc = (value - m_angle)/5.0;
        if(qAbs(value - m_angle) < qAbs(m_angleAcc))
        {
            if(m_angleAcc != 0)
            {
                m_angle = value;
                m_angleAcc = 0;
                m_waitList.pop_front();
                update();
            }

        }
        else
        {
            if(m_angleAcc != 0)
            {
                m_angle += m_angleAcc;
                update();
            }
            else
            {
                if(m_waitList.size() > 0)
                    m_waitList.pop_front();
            }
        }
    }
    else
    {
        if(m_pTimer->isActive())
            m_pTimer->stop();
    }
}

bool AngleClock::needAdd(qreal angle)
{
    if(m_waitList.size()==0 || (m_waitList.size()>0 && qAbs(angle-m_waitList.at(m_waitList.size()-1)) > 0.1))
        return true;
    return false;
}
