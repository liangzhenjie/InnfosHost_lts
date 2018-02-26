#ifndef ACTIONTREE_H
#define ACTIONTREE_H

#include <QTreeView>


class ActionTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit ActionTreeView(QWidget *parent = 0);
    void drawLine(QPointF lineBegin,QPointF lineEnd, QColor color=QColor(255,0,0));
protected:
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
signals:
    void wheelScale(QWheelEvent *event);
public slots:
private:
    QPointF m_lineBegin;
    QPointF m_lineEnd;
    QColor m_lineColor;
};

#endif // ACTIONTREE_H
