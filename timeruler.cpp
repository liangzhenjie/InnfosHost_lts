#include "timeruler.h"
#include <QPainter>
#include <QFontMetrics>
#include <QPixmap>
#include <QDebug>
#include <QWheelEvent>
#include "inertiamove.h"

TimeRuler::TimeRuler(QWidget *parent):
    QWidget(parent),
    m_curValue(0),
    m_minValue(0),
    m_maxValue(10),
    m_shortStep(1),
    m_longStep(5),
    m_pressed(false),
    m_space(0),
    m_bgColor(QColor(29,45,60)),
    m_lineColor(QColor(110,117,127)),
    m_tipBgColor(QColor(29,45,60)),
    m_tipTextColor(QColor(110,117,127)),
    m_shortLineHeight(7),
    m_longLineHeight(10),
    m_hoverRatio(0)
{
    m_sliderLastPot = QPointF(m_space,0);
    QPixmap slider(":/images/slider.png");
    m_sliderRect = QRectF(m_sliderLastPot-QPointF(slider.width()/2,0),slider.size());
    setMouseTracking(true);
    QFont font("微软雅黑",10);
    setFont(font);
}

void TimeRuler::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter.setRenderHint(QPainter::Antialiasing);
    drawBg(&painter);
    drawRule(&painter);
    drawSlider(&painter);
    drawTip(&painter);
}



quint32 TimeRuler::getMillisecond(qreal ratio) const
{
    return m_maxValue*ratio*1000;
}

qreal TimeRuler::getXRatio(quint32 millisecond) const
{
    return millisecond/(m_maxValue*1000);
}

QPoint TimeRuler::sliderPosToParent() const
{
    QPoint pos = mapToParent((m_sliderLastPot+QPointF(0,m_sliderRect.height())).toPoint());
    return pos;
}

void TimeRuler::resizeEvent(QResizeEvent *event)
{
    //Q_UNUSED(event);
    setCurValue(m_curValue);
    updateVisualValue();
    QWidget::resizeEvent(event);
}

void TimeRuler::moveEvent(QMoveEvent *event)
{
    //Q_UNUSED(event);
    updateVisualValue();
    QWidget::moveEvent(event);
}

//void TimeRuler::wheelEvent(QWheelEvent *event)
//{
//    m_hoverRatio = (event->pos().x()-m_lineLeftPot.x())/(m_lineRightPot.x()-m_lineLeftPot.x());
//    resize(size().width()+event->delta()*0.5,size().height());
//    move(-event->delta()*0.5*m_hoverRatio,0);
//    update();
//}

void TimeRuler::mouseMoveEvent(QMouseEvent *event)
{
    if(m_pressed)
    {
        qDebug() << "timer" << event->globalPos();
        QPointF pos = event->pos();
        changeValueByPos(pos);
    }
}

void TimeRuler::mousePressEvent(QMouseEvent *event)
{
    //m_pressed = true;
    if(event->button() & Qt::LeftButton)
    {
        if(m_sliderRect.contains(event->pos()))
        {
            m_pressed = true;
            event->accept();
            update();
        }
        else
        {
            changeValueByPos(event->pos());
        }
    }
}

void TimeRuler::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressed = false;
    update();
}

void TimeRuler::hoverMoveEvent(QMouseEvent *event)
{
    m_hoverRatio = (event->pos().x()-m_lineLeftPot.x())/(m_lineRightPot.x()-m_lineLeftPot.x());
}

void TimeRuler::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_bgColor);
    painter->drawRect(rect());
    painter->restore();
}

void TimeRuler::drawRule(QPainter *painter)
{
    painter->save();
    painter->setPen(m_lineColor);
    qreal initX = m_space;

    qreal initY = (qreal)size().height();

    m_lineLeftPot = QPointF(initX,initY);
    m_lineRightPot = QPointF(size().width()-initX,initY);
    painter->drawLine(m_lineLeftPot,m_lineRightPot);

    qreal increment = (size().width()-2*m_space)/(m_maxValue-m_minValue);

    for(int i=m_minValue;i<=m_maxValue;i+=m_shortStep)
    {
        if(i%m_longStep == 0)
        {
            QPointF btm(i*increment,initY);
            QPointF top(i*increment,initY-m_longLineHeight);
            painter->drawLine(btm,top);

            QString strValue = QString("%1s").arg((qreal)i,0,'f',0);
            qreal textW = QFontMetrics(font()).width(strValue);
            qreal textH = QFontMetrics(font()).height();
            QPointF textPot(i*increment-textW/2,initY-m_longLineHeight-3);
            painter->drawText(textPot,strValue);
        }
        else
        {
            QPointF btm(i*increment,initY);
            QPointF top(i*increment,initY-m_shortLineHeight);
            painter->drawLine(btm,top);
        }

        if(width() > 500)
        {
            int minCnt = 10;
            qreal minIncrement = increment/minCnt;
            for(int min=1;min<10;++min)
            {
                QPointF btm(i*increment+min*minIncrement,initY);
                QPointF top(i*increment+min*minIncrement,initY-m_shortLineHeight/2);
                painter->drawLine(btm,top);
            }
        }
    }

    painter->restore();
}

void TimeRuler::drawSlider(QPainter *painter)
{
    painter->save();
    QPixmap slider(":/images/slider.png");
    painter->drawPixmap(m_sliderLastPot+QPointF(-slider.width()/2,0),slider);
    painter->restore();
}

void TimeRuler::drawTip(QPainter *painter)
{
    if(!m_pressed)
        return;
    painter->save();
    painter->setPen(m_tipTextColor);
    painter->setBrush(m_tipBgColor);

    QString strValue = QString("%1s").arg(m_curValue,0,'f',1);
    qreal textW = QFontMetrics(font()).width(strValue);
    qreal textH = QFontMetrics(font()).height();
    m_tipRect = QRectF(m_sliderLastPot+QPointF(20,-10),QSizeF(textW,textH));
    painter->drawRect(m_tipRect);
    painter->drawText(m_tipRect,strValue);
    painter->restore();
}

void TimeRuler::changeValueByPos(QPointF pos)
{
    if(pos.x() >= m_lineLeftPot.x() && pos.x() <= m_lineRightPot.x())
    {
        qreal radio = (pos.x()-m_lineLeftPot.x())/(m_lineRightPot.x()-m_lineLeftPot.x());
        m_sliderLastPot.setX(pos.x());

        m_curValue = (m_maxValue-m_minValue)*radio;
        setCurValue(m_curValue);
//            emit valueChanged(m_curValue);
//            update();
    }
}

void TimeRuler::setMaxValue(qreal maxValue)
{
    if(maxValue <= m_minValue)
        return;
    m_maxValue = maxValue;
    setCurValue(m_minValue*1000);
}

void TimeRuler::setCurValue(qreal curValue)
{
    if(curValue<m_minValue || curValue>m_maxValue)
        return;
    m_curValue = curValue;
    qreal lineW = size().width()-2*m_space;
    qreal x = curValue/(m_maxValue-m_minValue)*lineW+m_space;
    qreal y = (qreal)size().height()-m_sliderRect.height();
    m_sliderLastPot = QPointF(x,y);
    m_sliderRect = QRectF(m_sliderLastPot-QPointF(m_sliderRect.size().width()/2,0),m_sliderRect.size());
    emit valueChanged(curValue*1000);
    update();
}

void TimeRuler::setSpace(int space)
{
    m_space = space;
    update();
}

void TimeRuler::setBgColor(QColor color)
{

}

void TimeRuler::setLineColor(QColor color)
{

}

void TimeRuler::setTipBgColor(QColor color)
{

}

void TimeRuler::onWheelScale(QWheelEvent *event)
{
    QPoint pos = mapFromGlobal(event->globalPos());
    m_hoverRatio = (pos.x()-m_lineLeftPot.x())/(m_lineRightPot.x()-m_lineLeftPot.x());
    qreal change = event->delta()*0.5;
    qreal finalW = size().width()+change;
    if(-change*m_hoverRatio+geometry().x() > 0)
    {
        change = geometry().x()/m_hoverRatio;
        finalW = size().width()+change;
    }
    QWidget *pParent = qobject_cast<QWidget *>(parent());
    if(-change*m_hoverRatio+geometry().x()+finalW < pParent->size().width())
    {
//        change = -(pParent->size().width()-finalW-geometry().x())/m_hoverRatio;
        finalW = pParent->size().width()+change*m_hoverRatio-geometry().x();
    }

    resize(finalW,size().height());
    move(-change*m_hoverRatio+geometry().x(),0);
    update();
}

void TimeRuler::updateVisualValue()
{
    QPoint min = mapFromParent(QPoint(0,0));
    QPoint max = mapFromParent(QPoint(parentWidget()->size().width(),0));

    qreal lineW = size().width()-2*m_space;
    quint16 minValue = (min.x()-m_space)*(m_maxValue-m_minValue)*1000/lineW;
    quint16 maxValue = (max.x()-m_space)*(m_maxValue-m_minValue)*1000/lineW;
    emit visualValueChanged(minValue,maxValue);
}

RulerContainner::RulerContainner(QWidget *parent):
    QWidget(parent),
    m_pRuler(nullptr),
    m_pMover(nullptr)
{
    setFixedHeight(30);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
//    m_pMover = new InertiaMove(this);
//    connect(m_pMover,&InertiaMove::movePos,this,&RulerContainner::moveRuler);

}

void RulerContainner::addRuler(TimeRuler *pRuler)
{
    m_pRuler = pRuler;
    m_pRuler->setParent(this);
    m_pRuler->resize(size());
    m_pRuler->setMinimumSize(this->size());
//    m_pRuler->installEventFilter(m_pMover);
}

void RulerContainner::moveRuler(QPointF pos)
{
    if(m_pRuler->isSliderPressed())
        return;
    qDebug() << pos.x() << m_pRuler->x() << size().width() << m_pRuler->size().width();
    m_pRuler->move(m_pRuler->x()+pos.x(),0);//only x move
    if(m_pRuler->x() < size().width()-m_pRuler->size().width())
    {
        m_pRuler->move((size().width()-m_pRuler->size().width()),0);
        //m_pMover->stop();
    }

    if(m_pRuler->x() > 0)
    {
        m_pRuler->move(0,0);
        //m_pMover->stop();
    }
    //update();
    emit rulerGeometryChange();
}

void RulerContainner::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if(m_pRuler->size().width() < size().width())
    {
        m_pRuler->resize(size());
        m_pRuler->setMinimumSize(this->size());
    }
}
