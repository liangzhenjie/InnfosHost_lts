#include "wavetriggerwidget.h"
#include <QTimer>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QValidator>

WaveTriggerWidget::WaveTriggerWidget(bool bResetAfterStop, QWidget *parent) :
    QWidget(parent),
    m_minValue(0),
    m_maxValue(0),
    m_bNeedSendMax(true),
    m_nInterval(1000),
    m_pBtn(nullptr),
    m_bResetAfterStop(bResetAfterStop)
{
    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(m_nInterval);
    connect(m_pTimer,&QTimer::timeout,this,&WaveTriggerWidget::onSendValue);
    QHBoxLayout * pLayout = new QHBoxLayout(this);
    QLabel * pMinName = new QLabel(tr("Value1:"));
    QLineEdit * pMinEdit = new QLineEdit(tr("%1").arg(m_minValue));
    pMinEdit->setValidator(new QDoubleValidator());
    connect(pMinEdit,&QLineEdit::editingFinished,this,&WaveTriggerWidget::onMinChanged);
    pLayout->addWidget(pMinName);
    pLayout->addWidget(pMinEdit);

    QLabel * pMaxName = new QLabel(tr("Value2:"));
    QLineEdit * pMaxEdit = new QLineEdit(tr("%1").arg(m_maxValue));
    pMaxEdit->setValidator(new QDoubleValidator());
    connect(pMaxEdit,&QLineEdit::editingFinished,this,&WaveTriggerWidget::onMaxChanged);
    pLayout->addWidget(pMaxName);
    pLayout->addWidget(pMaxEdit);

    QLabel * pIntervalName = new QLabel(tr("Interval:"));
    QLineEdit * pIntervalEdit = new QLineEdit(tr("%1").arg(m_nInterval));
    pIntervalEdit->setValidator(new QIntValidator());
    connect(pIntervalEdit,&QLineEdit::editingFinished,this,&WaveTriggerWidget::onInterValChanged);
    pLayout->addWidget(pIntervalName);
    pLayout->addWidget(pIntervalEdit);

    m_pBtn = new QPushButton(tr("Start"));
    m_pBtn->setCheckable(true);
    m_pBtn->setFixedWidth(130);
    connect(m_pBtn,&QPushButton::clicked,this,&WaveTriggerWidget::onTrigger);

    pLayout->addWidget(m_pBtn);
}

void WaveTriggerWidget::onMinChanged()
{
    QLineEdit * pEdit = qobject_cast<QLineEdit *>(sender());
    if(pEdit)
    {
        m_minValue = pEdit->text().toDouble();
    }
}

void WaveTriggerWidget::onMaxChanged()
{
    QLineEdit * pEdit = qobject_cast<QLineEdit *>(sender());
    if(pEdit)
    {
        m_maxValue = pEdit->text().toDouble();
    }
}

void WaveTriggerWidget::onInterValChanged()
{
    QLineEdit * pEdit = qobject_cast<QLineEdit *>(sender());
    if(pEdit)
    {
        quint32 value = pEdit->text().toUInt();
        if(value > 0)
            m_pTimer->setInterval(value);
    }

}

void WaveTriggerWidget::onTrigger(bool bStart)
{
    if(bStart)
    {
        m_pTimer->start();
        onSendValue();//send once immediately
        m_pBtn->setText(tr("Stop"));
    }
    else
    {
        m_pBtn->setText(tr("Start"));
        m_pTimer->stop();
        if(m_bResetAfterStop)
            emit valueChanged(0);//stop the device
    }

}

void WaveTriggerWidget::onSendValue()
{
    if(m_bNeedSendMax)
        emit valueChanged(m_maxValue);
    else
        emit valueChanged(m_minValue);
    m_bNeedSendMax = !m_bNeedSendMax;
}

void WaveTriggerWidget::stop()
{
    if(m_pTimer->isActive())
    {
        onTrigger(false);
        m_pBtn->setChecked(false);
    }
}
