#ifndef FRAMELESSHELPER_H
#define FRAMELESSHELPER_H

#include <QObject>
#include <QEvent>
#include <QMouseEvent>

class QWidget;
class FramelessHelperPrivate;
class WidgetData;
class QRubberBand;

class FramelessHelper : public QObject
{
    Q_OBJECT
public:
    explicit FramelessHelper(QObject *parent = 0);
    ~FramelessHelper();
    //activate window
    void activateOn(QWidget * topLevelWidget);
    //remove window
    void removeFrom(QWidget * topLevelWidget);
    //
    void setWidgetMovable(bool bMovable);
    //
    void setWidgetResizable(bool bResizable);
    void setRubberBandOnMove(bool movable);
    void setRubberBandOnResize(bool resizable);
    void setBorderWidth(const quint32 width);
    void setTitleHeight(const quint32 height);
    bool widgetResizable();
    bool widgetMovable();
    bool rubberBandOnMove();
    bool rubberBandOnResisze();
    uint borderWidth();
    uint titleHeight();
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    FramelessHelperPrivate * d;
signals:

public slots:
};

class FramelessHelperPrivate
{
public:
    QHash<QWidget * ,WidgetData * > m_widgetDataHash;
    bool m_bWidgetMovable        : true;
    bool m_bWidgetResizable      : true;
    bool m_bRubberBandOnResize   : true;
    bool m_bRubberBandOnMove     : true;
};

class CursorPosCalculator
{
public:
    explicit CursorPosCalculator();
    void reset();
    void recalculate(const QPoint &globalMousePos, const QRect &frameRect);

public:
    bool m_bOnEdges              : true;
    bool m_bOnLeftEdge           : true;
    bool m_bOnRightEdge          : true;
    bool m_bOnTopEdge            : true;
    bool m_bOnBottomEdge         : true;
    bool m_bOnTopLeftEdge        : true;
    bool m_bOnBottomLeftEdge     : true;
    bool m_bOnTopRightEdge       : true;
    bool m_bOnBottomRightEdge    : true;

    static int m_nBorderWidth;
    static int m_nTitleHeight;
};

/**
  *WidgetData
  *更新鼠标样式、移动、缩放窗体
*/

class WidgetData
{
public:
    explicit WidgetData(FramelessHelperPrivate *_d,QWidget * pTopLevelWidget);
    ~WidgetData();
    QWidget * widget();
    //handle mouse event- press hover release move;
    void handleWidgetEvent(QEvent * event);
    //update rubber band status
    void updateRubberBandStatus();
private:
    void updateCursorShape(const QPoint & gMousePos);
    void resizeWidget(const QPoint &gMousePos);
    void moveWidget(const QPoint &gMousePos);
    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleLeaveEvent(QEvent * event);
    void handleHoverMoveEvent(QHoverEvent * event);
private:
    FramelessHelperPrivate * d;
    QRubberBand * m_pRubberBand;
    QWidget * m_pWidget;
    QPoint m_ptDragPos;
    CursorPosCalculator m_pressMousePos;
    CursorPosCalculator m_moveMousePos;
    bool m_bLeftButtonPressed;
    bool m_bCursorShapeChanged;
    bool m_bLeftButtonTittlePressed;
    Qt::WindowFlags m_windowFlags;
};

#endif // FRAMELESSHELPER_H
