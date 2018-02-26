#ifndef WAVETRIGGERWIDGET_H
#define WAVETRIGGERWIDGET_H

#include <QWidget>

class QTimer;
class QPushButton;

class WaveTriggerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WaveTriggerWidget(bool bResetAfterStop = true,QWidget *parent = 0);

signals:
    void valueChanged(qreal value);
public slots:
    void onMinChanged();
    void onMaxChanged();
    void onInterValChanged();
    void onTrigger(bool bStart);
    void onSendValue();
public:
    void stop();
private:
    QTimer * m_pTimer;
    qreal m_minValue;
    qreal m_maxValue;
    bool m_bNeedSendMax;
    quint32 m_nInterval;
    QPushButton * m_pBtn;
    bool m_bResetAfterStop;
};

#endif // WAVETRIGGERWIDGET_H
