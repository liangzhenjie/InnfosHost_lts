#include "filter.h"
#include "ui_filter.h"
#include "motorform.h"
#include "mediator.h"
#include <QtWidgets>
#include "innfosproxy.h"

//#define NO_CALIBRATION

Filter::Filter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Filter)
{
    ui->setupUi(this);
    setWindowRole("Filter");
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
    ui->comboBox->addItem("Disable");
    ui->comboBox->addItem("Low Pass");

    ui->comboBox_2->addItem("Disable");
    ui->comboBox_2->addItem("Low Pass");

    ui->comboBox_3->addItem("Disable");
    ui->comboBox_3->addItem("Low Pass");
    initDataReference();

    for(int i=0;i<DATA_CNT;++i)
        motorDataChange(m_nDataRef[i]); //change combobox idx by motor's values;
    //setWindowFlags(Qt::Tool);
    connect(mediator,&Mediator::dataChange,this,&Filter::motorDataChange);
#ifndef NO_CALIBRATION
    connect(ui->start,&QPushButton::clicked,[=]{mediator->startCalibration();});
#else
    ui->tabWidget->removeTab(1);
#endif
}

Filter::~Filter()
{
    delete ui;
}

void Filter::initDataReference()
{
    for(int i=0;i<=P_VALUE;++i)
        m_nDataRef[i] = MotorForm::FILTER_C_STATUS+i;

    m_nDataRef[CALIBRATION_SWITCH] = MotorForm::CALIBRATION_SWITCH;
    m_nDataRef[CALIBRATION_ANGLE] = MotorForm::CALIBRATION_ANGLE;


}

void Filter::valueChangeByUser()
{

}

void Filter::motorDataChange(int nId)
{
    int nStatus = Mediator::getInstance()->getValue(MotorForm::Motor_Data_Id(nId));
    qreal value = Mediator::getInstance()->getValue(MotorForm::Motor_Data_Id(nId));
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    switch (nId)
    {
    case MotorForm::FILTER_C_STATUS:
        ui->comboBox->setCurrentIndex(nStatus);
        break;
    case MotorForm::FILTER_V_STATUS:
        ui->comboBox_2->setCurrentIndex(nStatus);
        break;
    case MotorForm::FILTER_P_STATUS:
        ui->comboBox_3->setCurrentIndex(nStatus);
        break;
    case MotorForm::FILTER_C_VALUE:
        ui->lineEdit->setText(QString::number(value,'f',6).replace(rx,""));
        break;
    case MotorForm::FILTER_V_VALUE:
        ui->lineEdit_2->setText(QString::number(value,'f',6).replace(rx,""));
        break;
    case MotorForm::FILTER_P_VALUE:
        ui->lineEdit_3->setText(QString::number(value,'f',6).replace(rx,""));
        break;
#ifndef NO_CALIBRATION
    case MotorForm::CALIBRATION_SWITCH:
        ui->calibration->setChecked(value>0);
        break;
    case MotorForm::CALIBRATION_ANGLE:
        ui->angle_offset->setText(tr("%1").arg(value));
        break;
#endif
    default:
        break;
    }
}

void Filter::ShowWindowIfNotExist()
{
    QWidgetList allWidgets = QApplication::allWidgets();
    Filter * pFilter = nullptr;
    for(int i=0;i<allWidgets.size();++i)
    {
        QWidget * pWidget = allWidgets.at(i);
        if(pWidget->objectName() == "Filter")
        {
            pFilter = qobject_cast<Filter*>(pWidget);
            break;
        }
    }

    if(pFilter == nullptr)
    {
        pFilter = new Filter();
    }
    pFilter->show();
}

void Filter::onTypeChange(int nIdx)
{
    QComboBox * pComboBox = qobject_cast<QComboBox *>(sender());
    if(pComboBox == ui->comboBox)
    {
        Mediator::getInstance()->setValueByUser(MotorForm::FILTER_C_STATUS,nIdx);
    }
    else if(pComboBox == ui->comboBox_2)
    {
        Mediator::getInstance()->setValueByUser(MotorForm::FILTER_V_STATUS,nIdx);
    }
    else if (pComboBox == ui->comboBox_3)
    {
        Mediator::getInstance()->setValueByUser(MotorForm::FILTER_P_STATUS,nIdx);
    }
}

void Filter::onValueChange()
{
    QLineEdit * pEdit = qobject_cast<QLineEdit *>(sender());
#ifndef NO_CALIBRATION
    QCheckBox * pCheckBox = qobject_cast<QCheckBox *>(sender());
#endif
    if(pEdit)
    {
        qreal value = pEdit->text().toDouble();
        if(pEdit == ui->lineEdit)
        {
            Mediator::getInstance()->setValueByUser(MotorForm::FILTER_C_VALUE,value);
        }
        else if(pEdit == ui->lineEdit_2)
        {
            Mediator::getInstance()->setValueByUser(MotorForm::FILTER_V_VALUE,value);
        }
        else if (pEdit == ui->lineEdit_3)
        {
            Mediator::getInstance()->setValueByUser(MotorForm::FILTER_P_VALUE,value);
        }
#ifndef NO_CALIBRATION
        else if(pEdit == ui->angle_offset)
        {
            mediator->setValueByUser(MotorForm::CALIBRATION_ANGLE,value);
        }
#endif
    }
#ifndef NO_CALIBRATION
    else if(pCheckBox)
    {
        qreal value = pCheckBox->isChecked() ? 1:0;
        if(pCheckBox == ui->calibration)
        {
            mediator->switchCalibration(value);
        }
        else if(pCheckBox = ui->velocity)
        {
            mediator->switchCalibrationVel(value);
        }
    }
#endif
}

void Filter::on_readAngle_clicked()
{
    mediator->sendProxy(mediator->getCurDeviceId(),D_READ_CALIBRATION_ANGLE);
}
