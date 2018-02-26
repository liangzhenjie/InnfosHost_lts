#ifndef INERTIAMOVE_H
#define INERTIAMOVE_H

#include <QObject>
#include <QPointF>

class QTimer;
class QElapsedTimer;

class InertiaMove : public QObject
{
    Q_OBJECT
public:
    ~InertiaMove();
    explicit InertiaMove(QObject *parent);
    virtual bool eventFilter(QObject *watched, QEvent *event);
    void setAcc(qreal acc);
    void stop();
signals:
    void movePos(QPointF pos);
public slots:
protected slots:
    void calcMove();
private:
    qreal m_fAcc;
    QPointF m_posSpeed;//x,y,speed
    QPointF m_posAcc;//x,y,acc
    QPointF m_pressPos;
    QPointF m_curPos;
    QTimer * m_pCalcTimer;
    QElapsedTimer * m_pMouseTimer;

};

#endif // INERTIAMOVE_H
