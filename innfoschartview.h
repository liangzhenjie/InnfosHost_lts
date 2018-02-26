#ifndef INNFOSCHARTVIEW_H
#define INNFOSCHARTVIEW_H

#include <QtWidgets/QGraphicsView>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

class Callout;
QT_CHARTS_USE_NAMESPACE

class InnfosChartView : public QGraphicsView
{
    Q_OBJECT
public:
    InnfosChartView(QChart *pChart,QWidget * parent=nullptr);
public slots:
    void keepCallout();
    void tooltip(QPointF point,bool state);
    void clearAllCallouts();
protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private:
    QGraphicsSimpleTextItem * m_pCoordX;
    QGraphicsSimpleTextItem * m_pCoordY;
    QChart * m_pChart;
    Callout * m_pCurTooltip;
    QList<Callout*> m_callouts;
};


#endif // INNFOSCHARTVIEW_H
