#include "actiontree.h"
#include <QWheelEvent>
#include <QDebug>
#include <QStandardItem>
#include <QStandardItemModel>
#include <actionitem.h>
#include <QPainter>

ActionTreeView::ActionTreeView(QWidget *parent) :
    QTreeView(parent),
    m_lineBegin(QPointF()),
    m_lineEnd(QPointF())
{

}

void ActionTreeView::drawLine(QPointF lineBegin, QPointF lineEnd, QColor color)
{
    m_lineBegin = viewport()->mapFromParent(lineBegin.toPoint());
    m_lineEnd = viewport()->mapFromParent(lineEnd.toPoint());
    m_lineColor = color;
    update();
}

void ActionTreeView::wheelEvent(QWheelEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    const QStandardItemModel * pModel = qobject_cast<const QStandardItemModel *>(index.model());
    if(pModel)
    {
        QStandardItem * pItem = pModel->itemFromIndex(index);
        if(pItem->type() == KeyItem::KeyType)
        {
            emit wheelScale(event);
            return;
        }
    }
    QTreeView::wheelEvent(event);
}

void ActionTreeView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QTreeView::paintEvent(event);
    QPainter painter(viewport());
    if(m_lineBegin != m_lineEnd)
    {
        painter.setPen(QPen(m_lineColor));
        painter.drawLine(m_lineBegin,m_lineEnd);
    }

}
