#ifndef ANGLECLOCK_H
#define ANGLECLOCK_H

#include <QWidget>
#include "motorform.h"

class QTimer;
class AngleClock : public QWidget
{
    Q_OBJECT
public:
    explicit AngleClock(QWidget *parent = 0);
    ~AngleClock();
signals:

protected:
    void paintEvent(QPaintEvent * event);

public slots:
    void angleChange(qreal angle);// angle is in angle
    void angleChangeByCircle(qreal circle);
    void modeChange(MotorForm::Motor_Mode mode);
protected slots:
    void calculate();
private:
    bool needAdd(qreal angle);
private:
    qreal m_angle;
    qreal m_angleAcc;//accelaration
    QVector <qreal> m_waitList;//value list wait to show;
    QTimer * m_pTimer;
    QString m_tip;
};

#endif // ANGLECLOCK_H
