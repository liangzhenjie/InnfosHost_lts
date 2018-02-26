#include "framelesshelper.h"
#include <QRect>
#include <QWidget>
#include <QRubberBand>

FramelessHelper::FramelessHelper(QObject *parent) :
    QObject(parent),
    d(new FramelessHelperPrivate())
{
    d->m_bWidgetMovable = true;
    d->m_bWidgetResizable = true;
    d->m_bRubberBandOnMove = false;
    d->m_bRubberBandOnResize = false;
}

FramelessHelper::~FramelessHelper()
{
    QList<QWidget *> keys = d->m_widgetDataHash.keys();
    qint32 size = keys.size();
    for(int i=0;i<size;++i)
    {
        delete d->m_widgetDataHash.take(keys[i]);
    }
    delete d;
}

bool FramelessHelper::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::Leave:
    {
        WidgetData * data = d->m_widgetDataHash.value(static_cast<QWidget *>(watched));
        if(data)
        {
            data->handleWidgetEvent(event);
            return true;
        }
    }
        break;

    default:
        break;
    }
    return QObject::eventFilter(watched,event);
}

void FramelessHelper::activateOn(QWidget *topLevelWidget)
{
    if(!d->m_widgetDataHash.contains(topLevelWidget))
    {
        WidgetData * data = new WidgetData(d,topLevelWidget);
        d->m_widgetDataHash.insert(topLevelWidget,data);
        topLevelWidget->installEventFilter(this);
    }
}

void FramelessHelper::removeFrom(QWidget *topLevelWidget)
{
    WidgetData * data = d->m_widgetDataHash.take(topLevelWidget);
    if(data)
    {
        topLevelWidget->removeEventFilter(this);
        delete data;
    }
}

void FramelessHelper::setRubberBandOnMove(bool movable)
{
    d->m_bRubberBandOnMove = movable;

    QList <WidgetData *> list = d->m_widgetDataHash.values();
    foreach (WidgetData * data, list) {
        data->updateRubberBandStatus();
    }
}

void FramelessHelper::setRubberBandOnResize(bool resizable)
{
    d->m_bRubberBandOnResize = resizable;

    QList <WidgetData *> list = d->m_widgetDataHash.values();
    foreach (WidgetData * data, list) {
        data->updateRubberBandStatus();
    }
}

void FramelessHelper::setWidgetMovable(bool bMovable)
{
    d->m_bWidgetMovable = bMovable;
}

void FramelessHelper::setWidgetResizable(bool bResizable)
{
    d->m_bWidgetResizable = bResizable;
}

void FramelessHelper::setBorderWidth(const quint32 width)
{
    if(width > 0)
    {
        CursorPosCalculator::m_nBorderWidth = width;
    }
}

void FramelessHelper::setTitleHeight(const quint32 height)
{
    if(height > 0)
    {
        CursorPosCalculator::m_nTitleHeight = height;
    }
}

bool FramelessHelper::widgetMovable()
{
    return d->m_bWidgetMovable;
}

bool FramelessHelper::widgetResizable()
{
    return d->m_bWidgetResizable;
}

bool FramelessHelper::rubberBandOnMove()
{
    return d->m_bRubberBandOnMove;
}

bool FramelessHelper::rubberBandOnResisze()
{
    return d->m_bRubberBandOnResize;
}

quint32 FramelessHelper::borderWidth()
{
    return CursorPosCalculator::m_nBorderWidth;
}

quint32 FramelessHelper::titleHeight()
{
    return CursorPosCalculator::m_nTitleHeight;
}

/*CursorPosCalculator*/
qint32 CursorPosCalculator::m_nBorderWidth = 5;
qint32 CursorPosCalculator::m_nTitleHeight = 30;

CursorPosCalculator::CursorPosCalculator()
{
    reset();
}

void CursorPosCalculator::reset()
{
    m_bOnEdges = false;
    m_bOnLeftEdge = false;
    m_bOnRightEdge = false;
    m_bOnTopEdge = false;
    m_bOnBottomEdge = false;
    m_bOnTopLeftEdge = false;
    m_bOnBottomLeftEdge = false;
    m_bOnTopRightEdge  = false;
    m_bOnBottomRightEdge = false;
}

void CursorPosCalculator::recalculate(const QPoint &globalMousePos, const QRect &frameRect)
{
    qint32 globalMouseX = globalMousePos.x();
    qint32 globalMouseY = globalMousePos.y();

    qint32 frameX = frameRect.x();
    qint32 frameY = frameRect.y();
    qint32 frameWidth = frameRect.width();
    qint32 frameHeight = frameRect.height();

    m_bOnLeftEdge = (globalMouseX>=frameX && globalMousePos.x() <= frameX+m_nBorderWidth);
    m_bOnRightEdge = (globalMouseX>=frameX+frameWidth-m_nBorderWidth && globalMousePos.x() <= frameX+frameWidth);
    m_bOnTopEdge = (globalMouseY >= frameY && globalMouseY <= frameY + m_nBorderWidth);
    m_bOnBottomEdge = (globalMouseY >= frameY+frameHeight-m_nBorderWidth && globalMouseY <= frameY+frameHeight);

    m_bOnTopLeftEdge = m_bOnTopEdge && m_bOnLeftEdge;
    m_bOnBottomLeftEdge = m_bOnBottomEdge && m_bOnLeftEdge;
    m_bOnTopRightEdge = m_bOnTopEdge && m_bOnRightEdge;
    m_bOnBottomRightEdge = m_bOnBottomEdge && m_bOnRightEdge;

    m_bOnEdges = m_bOnLeftEdge || m_bOnRightEdge || m_bOnTopEdge || m_bOnBottomEdge;
}


/**WidgetData**/

WidgetData::WidgetData(FramelessHelperPrivate *_d, QWidget *pTopLevelWidget):
    d(_d),
    m_pWidget(pTopLevelWidget),
    m_pRubberBand(nullptr),
    m_windowFlags(pTopLevelWidget->windowFlags()),
    m_bCursorShapeChanged(false),
    m_bLeftButtonPressed(false),
    m_bLeftButtonTittlePressed(false)
{
    m_pWidget->setMouseTracking(true);
    m_pWidget->setAttribute(Qt::WA_Hover,true);

    updateRubberBandStatus();
}

WidgetData::~WidgetData()
{
    m_pWidget->setMouseTracking(false);
    m_pWidget->setWindowFlags(m_windowFlags);
    delete m_pRubberBand;
    m_pRubberBand = nullptr;
}

QWidget * WidgetData::widget()
{
    return m_pWidget;
}

void WidgetData::handleWidgetEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        handleMousePressEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::MouseButtonRelease:
        handleMouseReleaseEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::MouseMove:
        handleMouseMoveEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::Leave:
        handleLeaveEvent(event);
        break;
    case QEvent::HoverMove:
        handleHoverMoveEvent(static_cast<QHoverEvent *>(event));
        break;
    default:
        break;
    }
}

void WidgetData::updateRubberBandStatus()
{
    if(d->m_bRubberBandOnMove || d->m_bRubberBandOnResize)
    {
        if(!m_pRubberBand)
        {
            m_pRubberBand = new QRubberBand(QRubberBand::Rectangle);
        }
    }
    else
    {
        delete m_pRubberBand;
        m_pRubberBand = nullptr;
    }
}

void WidgetData::updateCursorShape(const QPoint &gMousePos)
{
    if(m_pWidget->isFullScreen() || m_pWidget->isMaximized())
    {
        if(m_bCursorShapeChanged)
        {
            m_pWidget->unsetCursor();
            m_bCursorShapeChanged = false; // add by lzj
        }
        return;
    }

    m_moveMousePos.recalculate(gMousePos,m_pWidget->frameGeometry());

    if(m_moveMousePos.m_bOnTopLeftEdge || m_moveMousePos.m_bOnBottomRightEdge)
    {
        m_pWidget->setCursor(Qt::SizeFDiagCursor);
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnTopRightEdge || m_moveMousePos.m_bOnBottomLeftEdge)
    {
        m_pWidget->setCursor(Qt::SizeBDiagCursor);
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnTopEdge || m_moveMousePos.m_bOnBottomEdge)
    {
        m_pWidget->setCursor(Qt::SizeVerCursor);
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnRightEdge || m_moveMousePos.m_bOnLeftEdge)
    {
        m_pWidget->setCursor(Qt::SizeHorCursor);
        m_bCursorShapeChanged = true;
    }
    else
    {
        if(m_bCursorShapeChanged)
        {
            m_pWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
    }
}

void WidgetData::resizeWidget(const QPoint &gMousePos)
{
    QRect origRect;
    if(d->m_bRubberBandOnResize)
    {
        origRect = m_pRubberBand->frameGeometry();
    }
    else
    {
        origRect = m_pWidget->frameGeometry();
    }

    int left = origRect.left();
    int top = origRect.top();
    int right = origRect.right();
    int bottom = origRect.bottom();
    origRect.getCoords(&left, &top, &right, &bottom);

    qint32 minWidth = m_pWidget->minimumWidth();
    qint32 minHeight = m_pWidget->minimumHeight();

    if(m_pressMousePos.m_bOnTopLeftEdge)
    {
        left = gMousePos.x();
        top = gMousePos.y();
    }
    else if(m_pressMousePos.m_bOnTopRightEdge)
    {
        right = gMousePos.x();
        top = gMousePos.y();
    }
    else if(m_pressMousePos.m_bOnBottomLeftEdge)
    {
        left = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if(m_pressMousePos.m_bOnBottomRightEdge)
    {
        right = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if(m_pressMousePos.m_bOnRightEdge)
    {
        right = gMousePos.x();
    }
    else if(m_pressMousePos.m_bOnLeftEdge)
    {
        left = gMousePos.x();
    }
    else if(m_pressMousePos.m_bOnBottomEdge)
    {
        bottom = gMousePos.y();
    }
    else if(m_pressMousePos.m_bOnTopEdge)
    {
        top = gMousePos.y();
    }

    QRect newRect(QPoint(left,top),QPoint(right,bottom));

    if(newRect.isValid())
    {
        if(minWidth > newRect.width())//如果比最小宽度还小，那就设置成最小宽度
        {
            if(left != origRect.left())
            {
                newRect.setLeft(origRect.left());
            }
            else
            {
                newRect.setRight(origRect.right());
            }
        }

        if(minHeight > newRect.height())//如果比最小高度还小，那就设置成最小高度
        {
            if(top != origRect.top())
            {
                newRect.setTop(origRect.top());
            }
            else
            {
                newRect.setBottom(origRect.bottom());
            }
        }

        if(d->m_bRubberBandOnResize)
        {
            m_pRubberBand->setGeometry(newRect);
        }
        else
        {
            m_pWidget->setGeometry(newRect);
        }
    }
}

void WidgetData::moveWidget(const QPoint &gMousePos)
{
    if(d->m_bRubberBandOnMove)
    {
        m_pRubberBand->move(gMousePos - m_ptDragPos);
    }
    else
    {
        m_pWidget->move(gMousePos - m_ptDragPos);
    }
}

void WidgetData::handleMousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = true;
        m_bLeftButtonTittlePressed = event->pos().y() < m_moveMousePos.m_nTitleHeight;

        QRect frameRect = m_pWidget->frameGeometry();
        m_pressMousePos.recalculate(event->globalPos(),frameRect);

        m_ptDragPos = event->globalPos() - frameRect.topLeft();

        if(m_pressMousePos.m_bOnEdges)
        {
            if(d->m_bRubberBandOnResize)
            {
                m_pRubberBand->setGeometry(frameRect);
                m_pRubberBand->show();
            }
        }
        else if(d->m_bRubberBandOnMove)
        {
            m_pRubberBand->setGeometry(frameRect);
            m_pRubberBand->show();
        }
    }
}

void WidgetData::handleMouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = false;
        m_bLeftButtonTittlePressed = false;
        m_pressMousePos.reset();
        if(m_pRubberBand && m_pRubberBand->isVisible())
        {
            m_pRubberBand->hide();
            m_pWidget->setGeometry(m_pRubberBand->frameGeometry());
        }
    }
}

void WidgetData::handleMouseMoveEvent(QMouseEvent *event)
{
    if(m_bLeftButtonPressed)
    {
        if(d->m_bWidgetResizable && m_pressMousePos.m_bOnEdges)
        {
            resizeWidget(event->globalPos());
        }
        else if(d->m_bWidgetMovable && m_bLeftButtonTittlePressed)
        {
            moveWidget(event->globalPos());
        }
    }
    else if(d->m_bWidgetResizable)
    {
        updateCursorShape(event->globalPos());
    }

}

void WidgetData::handleLeaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(!m_bLeftButtonPressed)
    {
        m_pWidget->unsetCursor();
    }
}

void WidgetData::handleHoverMoveEvent(QHoverEvent *event)
{
    if(d->m_bWidgetResizable)
    {
        updateCursorShape(m_pWidget->mapToGlobal(event->pos()));
    }
}
