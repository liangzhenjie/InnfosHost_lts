#include "mydoublespinbox.h"

MyDoubleSpinBox::MyDoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent),
    m_bEmitFinishedOnLoseFocus(true)
{
    connect(this,&MyDoubleSpinBox::editingFinished,[=]{
        if(hasFocus() || m_bEmitFinishedOnLoseFocus)
            emit myValueChanged(value());
    });
    connect(this,&MyDoubleSpinBox::stepByChange,[=]{emit myValueChanged(value());});
}

void MyDoubleSpinBox::stepBy(int steps)
{
    QDoubleSpinBox::stepBy(steps);
    emit stepByChange();
}

QString MyDoubleSpinBox::textFromValue(double val) const
{
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    //QString str = locale().toString(val, 'f', d->decimals);
    QString str = QString::number(val,'f',6).replace(rx,"");
    QDoubleSpinBox::textFromValue(val);

    return str;
}

void MyDoubleSpinBox::setEmitFinishedOnLoseFocus(bool bEmit)
{
    m_bEmitFinishedOnLoseFocus = bEmit;
}


