#ifndef TIMERULER_H
#define TIMERULER_H
#include <QWidget>


class QTimeLine;
class InertiaMove;

class TimeRuler : public QWidget
{
    Q_OBJECT
public:
    TimeRuler(QWidget *parent = Q_NULLPTR);
    void paintEvent(QPaintEvent *event);
protected:
    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);
    //void wheelEvent(QWheelEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void hoverMoveEvent(QMouseEvent *event);
    void drawBg(QPainter *painter);
    void drawRule(QPainter *painter);
    void drawSlider(QPainter *painter);
    void drawTip(QPainter *painter);
    void changeValueByPos(QPointF pos);
public slots:
    void setMaxValue(qreal maxValue);
    void setCurValue(qreal curValue);
    void setSpace(int space);
    void setBgColor(QColor color);
    void setLineColor(QColor color);
    void setTipBgColor(QColor color);
    void onWheelScale(QWheelEvent * event);
signals:
    void valueChanged(quint16 value);
    void maxChanged(quint16 value);
    void visualValueChanged(quint16 minValue,quint16 maxValue);
protected:
    void updateVisualValue();
public:
    quint32 maxMillisecond()const{
        return m_maxValue*1000;
    }
    quint32 curMillisecond()const{
        return m_curValue*1000;
    }

    quint32 getMillisecond(qreal ratio)const;
    qreal getXRatio(quint32 millisecond)const;
    QPoint sliderPosToParent()const;
    bool isSliderPressed()const{
        return m_pressed;
    }
private:
    qreal m_curValue;
    qreal m_minValue;
    qreal m_maxValue;
    int m_longStep;
    int m_shortStep;
    int m_space;

    QColor m_bgColor;
    QColor m_lineColor;

    QColor m_tipBgColor;
    QColor m_tipTextColor;

    bool m_pressed;

    qreal m_longLineHeight;
    qreal m_shortLineHeight;

    QPointF m_sliderLastPot;

    QRectF m_sliderRect;
    QRectF m_tipRect;
    QPointF m_lineLeftPot;
    QPointF m_lineRightPot;

    qreal m_hoverRatio;
//    QTimeLine m_timeLine;
};

class RulerContainner : public QWidget
{
    Q_OBJECT
public:
    RulerContainner(QWidget *parent = Q_NULLPTR);
    virtual ~RulerContainner() {}
    void addRuler(TimeRuler * pRuler);
signals:
    void rulerGeometryChange();
public slots:
    void moveRuler(QPointF pos);
protected:
    void resizeEvent(QResizeEvent *event);
private:
    TimeRuler * m_pRuler;
    InertiaMove * m_pMover;
};

#endif // TIMERULER_H
