#ifndef MYDOUBLESPINBOX_H
#define MYDOUBLESPINBOX_H

#include <QDoubleSpinBox>

class MyDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit MyDoubleSpinBox(QWidget *parent = 0);
    virtual void stepBy(int steps);
    virtual QString textFromValue(double val) const;
    void setEmitFinishedOnLoseFocus(bool bEmit);//prevent eimt emitFinished signal if bEmit is false.
signals:
    void stepByChange();
    void myValueChanged(double value);
public slots:
private:
    bool m_bEmitFinishedOnLoseFocus;
};

#endif // MYDOUBLESPINBOX_H
