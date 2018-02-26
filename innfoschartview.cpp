#include "innfoschartview.h"
#include "callout.h"
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QDebug>

InnfosChartView::InnfosChartView(QChart *pChart, QWidget *parent):
    QGraphicsView(new QGraphicsScene,parent),
    m_pCurTooltip(nullptr),
    m_pChart(pChart)
{
    setDragMode(QGraphicsView::NoDrag);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setMouseTracking(true);
    scene()->addItem(m_pChart);

//    QAbstractSeries * series = m_pChart->series().at(0);
//    connect(series, SIGNAL(clicked(QPointF)), this, SLOT(keepCallout()));
//    connect(series, SIGNAL(hovered(QPointF, bool)), this, SLOT(tooltip(QPointF,bool)));
}

void InnfosChartView::keepCallout()
{
    m_callouts.append(m_pCurTooltip);
    m_pCurTooltip = new Callout(m_pChart);
    m_pCurTooltip->hide();
}

void InnfosChartView::tooltip(QPointF point, bool state)
{
    if(!m_pCurTooltip)
        m_pCurTooltip = new Callout(m_pChart);

    if(state)
    {
        m_pCurTooltip->setText(QString("X:%1 \nY:%2 ").arg(point.x()).arg(point.y()));
        m_pCurTooltip->setAnchor(point);
        m_pCurTooltip->setZValue(11);
        m_pCurTooltip->updateGeometry();
        m_pCurTooltip->show();
    }
    else
    {
        m_pCurTooltip->hide();
    }
}

void InnfosChartView::clearAllCallouts()
{
    foreach (Callout * callout, m_callouts) {
        if(scene())
        {
            scene()->removeItem(callout);
        }
    }
    if(m_pCurTooltip)
    {
        scene()->removeItem(m_pCurTooltip);
        m_pCurTooltip = nullptr;
    }
}

void InnfosChartView::resizeEvent(QResizeEvent *event)
{
    if(scene())
    {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        m_pChart->resize(event->size());
        foreach (Callout * callout, m_callouts) {
            callout->updateGeometry();
        }
    }
    QGraphicsView::resizeEvent(event);
}

void InnfosChartView::mouseMoveEvent(QMouseEvent *event)
{
//    QList<QGraphicsItem *> items = scene()->items(event->pos());
//    qDebug() << items <<endl;
    QGraphicsView::mouseMoveEvent(event);
}

