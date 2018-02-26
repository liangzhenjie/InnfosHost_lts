#include "motorform.h"
#include "ui_motorform.h"
#include "innfosutil.h"
#include <QEvent>
#include <QMouseEvent>
#include "innfostoolbar.h"
#include <QGridLayout>
#include "framelesshelper.h"
#include <QLabel>
#include <QIcon>
#include <QButtonGroup>
#include "innfosproxy.h"
#include "innfoswindow.h"
#include "innfosutil.h"
#include "innfosproxy.h"
#include "motordata.h"
#include "dataUtil.h"
#include <QTimer>
#include "mediator.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFileDialog>
#include <QDebug>
#include <QApplication>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QTextStream>
#include "editorwidget.h"
#include "angleclock.h"
#include <QLCDNumber>
#include <QComboBox>
#include <QDesktopWidget>
#include "msgbox.h"
#include "launchwindow.h"
#include "filter.h"

#define MOTION_EDIT

const int nTotalCount = 1;
const int nChartCount = 1;
const int nMaxErrorCnt = 30;
MotorForm::MotorForm(const quint8 nDeviceId, const quint32 nDeviceMac, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotorForm),
    m_deviceId(nDeviceId),
    m_deviceMac(nDeviceMac),
    m_xPercent(0.5),
    m_yPercent(0.5),
    m_pBtnGroup(nullptr),
    m_status(Status_Normal),
    m_modeId(Mode_None),
    m_requestModeId(Mode_None),
    m_bPressed(false),
    m_pDetail(nullptr),
    m_nHeartFailCnt(0),
    m_motorCode(tr("innfos%1").arg(nDeviceMac)),
    m_oldDeviceId(-1),
    m_bCalibrating(false),
    m_bRequestActual(true),
    m_curErrorId(0)
{

    ui->setupUi(this);
    for(int i=CUR_IQ_SET;i<DATA_CNT;++i)
        m_motorData[i] = 0;
    m_motorData[CURRENT_SCALE]= curScale;
    m_motorData[VELOCITY_SCALE] = velScale;
    m_pBtnGroup = new QButtonGroup(this);
    m_pBtnGroup->addButton(ui->btn_cur,Mode_Cur);
    m_pBtnGroup->addButton(ui->btn_vel,Mode_Vel);
    m_pBtnGroup->addButton(ui->btn_pos,Mode_Profile_Pos);
    ui->demand_value->setValidator(new QDoubleValidator);
    connect(m_pBtnGroup,static_cast<void(QButtonGroup:: *)(int)>(&QButtonGroup::buttonClicked),this,&MotorForm::buttonChanged);
    connect(ui->horizontalSlider,&QSlider::sliderMoved,this,&MotorForm::sliderChange);
    connect(ui->demand_value,&QLineEdit::editingFinished,this,&MotorForm::demandInput);
    connect(this,&MotorForm::dataChange,this,&MotorForm::refreshCurShow);
    connect(this,&MotorForm::beSelected,this,&MotorForm::enableChildren);
    ui->local_Id->setValidator(new QIntValidator(0,99,this));
    ui->local_Id->setText(tr("%1").arg(m_deviceId,2,10,QLatin1Char('0')));
    ui->label->setText(tr("ID:%1").arg(m_deviceMac));
    connect(ui->local_Id,&QLineEdit::editingFinished,[=]{
        quint8 nId = ui->local_Id->text().toInt();
        if(!MotorMgr::getInstance()->deviceIdHasExist(nId))
        {
            changeDeviceId(nId);
        }
        else
        {
            //change id failed
            ui->local_Id->setText(tr("%1").arg(m_deviceId,2,10,QLatin1Char('0')));
        }

    });
    connect(this,&MotorForm::onlineChange,this,&MotorForm::setOnlineStatus);
    m_pHeartTimer = new QTimer(this);
    connect(m_pHeartTimer,&QTimer::timeout,[=]{
        if(!isOnline())
        {
            emit onlineChange(false);
            m_pHeartTimer->stop();
            m_pValueTimer->stop();
        }
        else
        {
            if(!m_bRequestActual)
                return;
            InnfosProxy::SendProxy(m_deviceId,D_HANDSHAKE);
            ++ m_nHeartFailCnt;
        }
    });

    m_pValueTimer = new QTimer(this);
    connect(m_pValueTimer,&QTimer::timeout,this,&MotorForm::requestActualValue);

    m_pClock = new AngleClock(this);
    m_pClock->setFixedSize(75,75);
    m_pClock->move(50,15);
    connect(this,&MotorForm::dataChange,[=](Motor_Data_Id Id){
        switch (m_modeId)
        {
        case Mode_Cur:
            if(Id == CUR_ACTURAL)
                m_pClock->angleChange(m_motorData[Id]/m_motorData[CURRENT_SCALE]*360);
            break;
        case Mode_Vel:
            if(Id == VEL_ACTURAL)
                m_pClock->angleChange(m_motorData[Id]/m_motorData[VELOCITY_SCALE]*360);
            break;
        case Mode_Pos:
            if(Id == POS_ACTURAL)
            {
                m_pClock->angleChange(m_motorData[Id]/128*360);
            }
            break;
        default:
            break;
        }

    });

    enableChildren(false);

#ifdef RECORD_DATA
    //log file
    m_pFileLog = new QFile(tr("./%1.csv").arg(m_deviceId),this);
    if(m_pFileLog->open(QFile::WriteOnly|QFile::Text|QFile::Truncate))
        connect(this,&MotorForm::dataChange,this,&MotorForm::logData);
#endif
    InnfosProxy::SendProxy(m_deviceId,D_READ_MOTORS_SWITCH);
    //InnfosProxy::SendProxy(m_deviceId,D_READ_VERSION);
}

MotorForm::~MotorForm()
{
#ifdef RECORD_DATA
    m_pFileLog->close();
#endif
    delete ui;
}

void MotorForm::setDeviceIdSuccessfully()
{
    m_oldDeviceId = -1;
}

bool MotorForm::deviceIdIsAvailable() const
{
    return m_oldDeviceId < 0;
}

void MotorForm::setDetailWnd(InnfosWindow *pDetail)
{
    if(pDetail)
    {
        m_pDetail = pDetail;
        connect(m_pDetail,&QWidget::destroyed,this,&MotorForm::detailClosed);
    }
}


void MotorForm::mouseDoubleClickEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    if(m_modeId != Mode_None)
    {
        selectMotor();//be sure that when enter detail layer,there has selected motor
        if(isOnline())
        {
//            if(!m_pDetail)
//            {
//                m_pDetail = new InnfosWindow(nullptr,m_modeId,tr("Node %1").arg(m_deviceId));
//                connect(m_pDetail,&QWidget::destroyed,this,&MotorForm::detailClosed);
//            }
//            const QRect rc = QApplication::desktop()->screenGeometry();

//            //m_pDetail->move((rc.width()-m_pDetail->size().width())/2,(rc.height()-m_pDetail->size().height())/2);
//            m_pDetail->show();
//            QSize size = m_pDetail->size();
//            MotorMgr::getInstance()->hide();
//            requestAllValue();
            MotorMgr::getInstance()->enterDetailWindow();
        }
        else
        {
            reconnect();
        }

    }
}

void MotorForm::mouseMoveEvent(QMouseEvent * event)
{
    if(m_bPressed)
        move(MotorMgr::getInstance()->availablePos(event->globalPos()-m_ptDragPoint,size()));
}

void MotorForm::mousePressEvent(QMouseEvent *event)
{
    m_bPressed = true;
    m_ptDragPoint = event->globalPos() - frameGeometry().topLeft();
}

void MotorForm::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    selectMotor();
    m_bPressed = false;
    MotorMgr::getInstance()->updateMotorPosPercent(this);
}

void MotorForm::focusInEvent(QFocusEvent *event)
{
    selectMotor();
    QWidget::focusInEvent(event);
}

void MotorForm::focusOutEvent(QFocusEvent *event)
{
    if(!MotorMgr::getInstance()->isMultiSelected())
        unselectMotor();
    QWidget::focusOutEvent(event);
}

void MotorForm::showVersion()
{
    int nVersionF = (int)(m_motorData[MOTOR_VERSION]/1000000);
    int nVersionS = ((int)m_motorData[MOTOR_VERSION])%1000000/1000;
    int nVersionT = ((int)m_motorData[MOTOR_VERSION])%1000;
    ui->version->setText(tr("Version: %1.%2.%3").arg(nVersionF).arg(nVersionS).arg(nVersionT));
}

void MotorForm::changeDemand(qreal value)
{
    switch (m_modeId) {
    case Mode_Cur:
//        m_motorData[CUR_DEMAND] = value;
//        emit dataChange(CUR_DEMAND);
        setValueByUser(CUR_IQ_SET,value);
        break;
    case Mode_Vel:
    case Mode_Profile_Vel:
//        m_motorData[VEL_DEMAND] = value;
//        emit dataChange(VEL_DEMAND);
        setValueByUser(VEL_SET,value);
        break;
    case Mode_Pos:
    case Mode_Profile_Pos:
//        m_motorData[POS_DEMAND] = value;
//        emit dataChange(POS_DEMAND);
        setValueByUser(POS_SET,value);
        break;
    default:
        break;
    }
}

void MotorForm::showGraph()
{
    if(m_pDetail)
        m_pDetail->showGraph();
}

void MotorForm::closeGraph()
{
    if(m_pDetail)
        m_pDetail->closeGraph();
}

void MotorForm::buttonChanged(const int nMode)
{
    selectMotor();
    activeMode(nMode);
}

void MotorForm::activeMode(const int nMode)
{
    if(nMode < Mode_None || nMode > Mode_Homing/* || nMode==m_modeId*/)
        return;
    m_requestModeId = nMode;
    //active mode
    quint8 uMode = m_requestModeId;
    InnfosProxy::SendProxy(m_deviceId,D_SET_MODE,uMode);

}

void MotorForm::activeModeSuccessfully()
{
    m_modeId = m_requestModeId;
    switch (m_modeId) {
    case Mode_Cur:
        ui->cur_tag->setText(tr("C"));
        setValueByUser(CUR_IQ_SET,0,false);//reset demand
        refreshCurShow(CUR_ACTURAL);
        refreshCurShow(CUR_IQ_SET);//refresh slider and lineedit
        m_pBtnGroup->button(Mode_Cur)->setChecked(true);
        break;
    case Mode_Vel:
        ui->cur_tag->setText(tr("V"));
        setValueByUser(VEL_SET,0,false);//reset demand
        refreshCurShow(VEL_ACTURAL);
        refreshCurShow(VEL_SET);//refresh slider and lineedit
        m_pBtnGroup->button(Mode_Vel)->setChecked(true);
        break;
    case Mode_Pos:
        ui->cur_tag->setText(tr("P"));
        //setValueByUser(POS_SET,0,false);//reset demand
        refreshCurShow(POS_ACTURAL);
        refreshCurShow(POS_SET);//refresh slider and lineedit
        //m_pBtnGroup->button(Mode_Pos)->setChecked(true);

        break;
    case Mode_Profile_Pos:
        ui->cur_tag->setText(tr("P"));
        //setValueByUser(POS_SET,0,false);//reset demand
        refreshCurShow(POS_ACTURAL);
        refreshCurShow(POS_SET);//refresh slider and lineedit
        m_pBtnGroup->button(Mode_Profile_Pos)->setChecked(true);
        break;
    case Mode_Profile_Vel:
        //setValueByUser(VEL_SET,0,false);//reset demand
        break;
    default:
        break;
    }

    if(m_pDetail)
        m_pDetail->enableMode(m_modeId);
    m_pClock->modeChange((Motor_Mode)m_modeId);
}

void MotorForm::detailClosed()
{
    m_pDetail = nullptr;
    MotorMgr::getInstance()->show();
}

void MotorForm::sliderChange(int nPercent)
{
    qreal min = 0;
    qreal max = 0;
    getLimit(min,max);

    if(min >= max)
        return;

    qreal value = (max-min)*nPercent/ui->horizontalSlider->maximum()+min;
    changeDemand(value);
    //ui->demand_value->setText(tr("%1").arg());
}

void MotorForm::demandInput()
{
    if (ui->demand_value->isModified())
    {
        ui->demand_value->setModified(false);
        qreal demand = ui->demand_value->text().toDouble();
        changeDemand(demand);
//        qreal min = 0;
//        qreal max = 0;
//        getLimit(min,max);
//        if(min >= max)
//            return;
//        qreal percent = -1;
//        percent = (demand-min)/(max-min);
//        if (percent >= 0)
//        {
//            ui->horizontalSlider->setValue(percent*ui->horizontalSlider->maximum());
//        }
    }
}

void MotorForm::refreshCurShow(MotorForm::Motor_Data_Id Id)
{
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");

    switch (Id) {
    case CUR_ACTURAL:
        ui->label_6->setText(QString::number(m_motorData[CUR_ACTURAL],'f',6).replace(rx,""));
        break;

    case VEL_ACTURAL:
        ui->label_7->setText(QString::number(m_motorData[VEL_ACTURAL],'f',6).replace(rx,""));
        break;

    case POS_ACTURAL:
        ui->label_8->setText(QString::number(m_motorData[POS_ACTURAL],'f',6).replace(rx,""));
        break;
    case MOTOR_VERSION:
        showVersion();
        break;
    default:
        break;
    }

    qreal min = 0;
    qreal max = 0;
    getLimit(min,max);
    Mediator::getInstance()->deviceDataChange(m_deviceId,Id);//notify mediator data has change
    switch (Id) {
    case CUR_ACTURAL:
        if(m_modeId == Mode_Cur)
//            ui->cur_value->setText(QString::number(m_motorData[CUR_ACTURAL],'f',6).replace(rx,""));
        break;
    case CUR_IQ_SET:
    case CUR_MINIMAL:
    case CUR_MAXIMUM:
        if(m_modeId == Mode_Cur)
            refreshShow(m_motorData[CUR_ACTURAL],m_motorData[CUR_IQ_SET],min,max);

        break;
    case VEL_ACTURAL:
        if(m_modeId == Mode_Vel)
//            ui->cur_value->setText(QString::number(m_motorData[VEL_ACTURAL],'f',6).replace(rx,""));
        break;
    case VEL_SET:
    case VEL_MINIMAL:
    case VEL_MAXIMUM:
        if(m_modeId == Mode_Vel)
            refreshShow(m_motorData[VEL_ACTURAL],m_motorData[VEL_SET],min,max);
        break;
    case POS_ACTURAL:
        if(m_modeId == Mode_Pos)
//            ui->cur_value->setText(QString::number(m_motorData[POS_ACTURAL],'f',6).replace(rx,""));
        break;
    case POS_SET:
    case POS_MINIMAL:
    case POS_MAXIMUM:
        if(m_modeId == Mode_Pos)
            refreshShow(m_motorData[POS_ACTURAL],m_motorData[POS_SET],min,max);
        break;
    default:
        break;
    }
}

void MotorForm::logData(MotorForm::Motor_Data_Id Id)
{
#ifdef RECORD_DATA
    if(Id == POS_ACTURAL)
    {
        QTextStream out(m_pFileLog);
        out<<tr("%1").arg(m_motorData[CUR_ACTURAL])<<","<<tr("%1").arg(m_motorData[VEL_ACTURAL])<<","<<tr("%1").arg(m_motorData[POS_ACTURAL])<<","
          <<tr("%1").arg(m_motorData[TEMP_MOTOR])<<","<<QTime::currentTime().toString("hh:mm:ss.zzz");
        endl(out);
        out.flush();
    }
#endif
}

void MotorForm::enableChildren(bool bEnable)
{
//    ui->btn_cur->setEnabled(bEnable);
//    ui->btn_vel->setEnabled(bEnable);
//    ui->btn_pos->setEnabled(bEnable);
    ui->local_Id->setEnabled(bEnable);
    ui->demand_value->setEnabled(bEnable);
    ui->horizontalSlider->setEnabled(false);
}

void MotorForm::requestActualValue()
{
    if(!m_bRequestActual)
        return;
    InnfosProxy::SendProxy(m_deviceId,D_READ_CUR_CURRENT);
    InnfosProxy::SendProxy(m_deviceId,D_READ_CUR_VELOCITY);
    InnfosProxy::SendProxy(m_deviceId,D_READ_CUR_POSITION);
    InnfosProxy::SendProxy(m_deviceId,D_READ_VOLTAGE);
    InnfosProxy::SendProxy(m_deviceId,D_READ_TEMP_INVERTER);
    InnfosProxy::SendProxy(m_deviceId,D_READ_TEMP_MOTOR);
    if(m_curErrorId == 0)//if error occur, do not check error again
        InnfosProxy::SendProxy(m_deviceId,D_ERROR);
}

void MotorForm::setOnlineStatus(bool bOnline)
{
    Mediator::getInstance()->deviceOnlineStatus(m_deviceId,bOnline);
    if(!bOnline)
    {
        changeStatus(Status_Offline);
    }
    else
    {
        m_status = m_status &(~Status_Offline);
        changeColor();
    }
}

void MotorForm::saveData()
{
    QFileDialog dialog(this,tr("Save as"),QDir::currentPath());
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Innfos(*.innfos)"));
    if(dialog.exec() == QDialog::Accepted)
    {
        QString path = dialog.selectedFiles().first();
        if(path.size() > 0)
        {
            saveDataToFile(path);
        }

    }
}

void MotorForm::loadData()
{
    QFileDialog dialog(this,tr("Load"),QDir::currentPath());
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Innfos(*.innfos)"));
    if(dialog.exec() == QDialog::Accepted)
    {
        QString path = dialog.selectedFiles().first();
        if(path.size() > 0)
        {
            readDataFromFile(path);
        }

    }
}

void MotorForm::reconnect()
{
    m_nHeartFailCnt = 0;
    if(m_pHeartTimer)
    {
        m_pHeartTimer->start();
    }
    if(m_pValueTimer)
    {
        m_pValueTimer->start();
    }
}

void MotorForm::changeDeviceId(quint8 nNewDeviceId)
{
    m_oldDeviceId = m_deviceId;
    InnfosProxy::SendProxy(m_deviceId,m_deviceMac,D_SET_DEVICE_ID,nNewDeviceId);
    m_deviceId = nNewDeviceId;
    ui->local_Id->setText(tr("%1").arg(m_deviceId,2,10,QLatin1Char('0')));
}

void MotorForm::saveDataToFile(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
    {
        qDebug() << tr("Cannot write file %1 %2").arg(fileName).arg(file.errorString());
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument("1.0",true);
    writer.writeStartElement("ServoData");
    writer.writeAttribute("Version","1.0");
    for(int i=0;i<DATA_CNT;++i)
    {
        writer.writeTextElement(tr("Attr%1").arg(i),tr("%1").arg(m_motorData[i]));
    }
    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
}

void MotorForm::readDataFromFile(QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QXmlStreamReader reader(&file);
        while(!reader.atEnd())
        {
            QXmlStreamReader::TokenType nType = reader.readNext();
            switch (nType)
            {
            case QXmlStreamReader::StartDocument:

                break;
            case QXmlStreamReader::StartElement:
            {
                QString strElementName = reader.name().toString();
                if(strElementName == "ServoData")
                {
                    QXmlStreamAttributes attributes = reader.attributes();
                    if(attributes.hasAttribute("Version") && attributes.value("Version").toString() == "1.0")
                    {
                        readParams(&reader);
                    }
                }
            }
                break;
            default:
                break;
            }
        }
        if(reader.hasError())
        {
            qDebug() << tr("Errorinfo:%1 line:%2 column:%3 character offset:%4").arg(reader.errorString()).arg(reader.lineNumber()).arg(reader.columnNumber()).arg(reader.characterOffset());
        }
        file.close();
    }


}

void MotorForm::readParams(QXmlStreamReader *reader)
{
    while (!reader->atEnd()) {
        reader->readNext();
        if(reader->isStartElement())
        {
            QString strElementName = reader->name().toString();
            int nIdx = strElementName.indexOf("Attr");
            if(nIdx >=0)
            {
                int nAttrId = strElementName.right(strElementName.size()-4).toInt();
                if(nAttrId>=0 && nAttrId<DATA_CNT)
                {
                    qreal readValue = reader->readElementText().toDouble();
                    if(m_motorData[nAttrId] != readValue)
                    {
                        m_motorData[nAttrId] = readValue;
                        emit dataChange((Motor_Data_Id)nAttrId);
                    }

                }
            }
        }
    }
}

void MotorForm::refreshShow(qreal acturalValue, qreal demand, qreal min, qreal max)
{
    if(min >= max)
        return;
    qreal percent = -1;
    percent = (demand-min)/(max-min);
    if (percent >= 0)
    {
        ui->demand_value->setText(tr("%1").arg(demand));
        //ui->horizontalSlider->setValue(percent*ui->horizontalSlider->maximum());
    }

}

quint8 MotorForm::deviceId() const
{
//    if(m_oldDeviceId >=0)
//        return m_oldDeviceId;
    return m_deviceId;
}

QString MotorForm::motorCode() const
{
    //return m_motorCode;
    return tr("%1").arg(m_deviceMac);
}

void MotorForm::setValueByProxy(const int nProxyId, const qreal value)
{
    //responseHeart(true);
    if(!isOnline())
        responseHeart(true);
    else
        m_nHeartFailCnt = 0;

    switch (nProxyId) {
    case D_READ_CUR_CURRENT:
        m_acturalCurs.push_back(value);
        if(m_acturalCurs.size() == nTotalCount)
        {
            qreal total = 0;
            foreach (qreal perValue, m_acturalCurs) {
                total += perValue;
            }
            m_motorData[CUR_ACTURAL] = total/nTotalCount;
            emit dataChange(CUR_ACTURAL);

            m_acturalCurs.clear();
        }
        break;
    case D_READ_CUR_VELOCITY:
        m_acturalVels.push_back(value);
        if(m_acturalVels.size() == nTotalCount)
        {
            qreal total = 0;
            foreach (qreal perValue, m_acturalVels) {
                total += perValue;
            }
            m_motorData[VEL_ACTURAL] = total/nTotalCount;
            emit dataChange(VEL_ACTURAL);
            m_acturalVels.clear();
        }
        break;
    case D_READ_CUR_POSITION:
        m_acturalPoss.push_back(value);
        if(m_acturalPoss.size() == nTotalCount)
        {
            qreal total = 0;
            foreach (qreal perValue, m_acturalPoss) {
                total += perValue;
            }
            m_motorData[POS_ACTURAL] = total/nTotalCount;
            emit dataChange(POS_ACTURAL);
            m_acturalPoss.clear();
        }
        break;
    case D_READ_CHANNEL_2:

        m_chartCurs.push_back(value);
        if(m_chartCurs.size() == nChartCount)
        {
            emit acturalVauleChange(Mode_Cur,m_chartCurs);
            m_chartCurs.clear();
        }
        break;
    case D_READ_CHANNEL_3:
        m_chartVels.push_back(value);
        if(m_chartVels.size() == nChartCount)
        {
            emit acturalVauleChange(Mode_Vel,m_chartVels);
            m_chartVels.clear();
        }
        break;
    case D_READ_CHANNEL_4:
        m_chartPoss.push_back(value);
        if(m_chartPoss.size() == nChartCount)
        {
            emit acturalVauleChange(Mode_Pos,m_chartPoss);
            m_chartPoss.clear();
        }
        break;
    case D_READ_MOTORS_SWITCH:
        if((int)value == 1 && (int)m_motorData[MOTOR_SWITCH]==0)
        {
            m_motorData[MOTOR_SWITCH] = value;
            emit dataChange(MOTOR_SWITCH);
            requestAllValue();
        }
        break;
    case D_READ_CUR_P:      
    case D_READ_CUR_I:
    case D_READ_VEL_P:
    case D_READ_VEL_I:
    case D_READ_POS_P:
    case D_READ_POS_I:
    case D_READ_POS_D:
    case D_READ_PROFILE_POS_ACC:
    case D_READ_PROFILE_POS_MAX_SPEED:
    case D_READ_PROFILE_POS_DEC:
    case D_READ_PROFILE_VEL_ACC:
    case D_READ_PROFILE_VEL_MAX_SPEED:
    case D_READ_PROFILE_VEL_DEC:
    case D_READ_CURRENT_PID_MAX:
    case D_READ_CURRENT_PID_MIN:
    case D_READ_VELOCITY_PID_MAX:
    case D_READ_VELOCITY_PID_MIN:
    case D_READ_POSITION_PID_MAX:
    case D_READ_POSITION_PID_MIN:
    case D_READ_CHART_FREQUENCY:
    case D_READ_CHART_THRESHOLD:
    {
        Directives proxy = (Directives)nProxyId;
        Motor_Data_Id id = DataUtil::convertToMotorDataId(proxy);
        if(id != DIRECTIVES_INVALID)
        {
            m_motorData[id] = value;
            emit dataChange(id);
        }
    }
        break;
    case D_READ_POS_OFFSET:
    {
        Directives proxy = (Directives)nProxyId;
        Motor_Data_Id id = DataUtil::convertToMotorDataId(proxy);
        if(id != DIRECTIVES_INVALID)
        {
            m_motorData[id] = value;
            emit dataChange(id);
        }
    }

        break;
    case D_READ_VERSION:
    {
        Directives proxy = (Directives)nProxyId;
        Motor_Data_Id id = DataUtil::convertToMotorDataId(proxy);
        if(id != DIRECTIVES_INVALID)
        {
            m_motorData[id] = value;
            showVersion();
            emit dataChange(id);
        }
    }
        break;
    case D_READ_MOTOR_MODE:
    {
        //这里读取模式，收到后就当做激活成功处理
        m_requestModeId = value;
        activeModeSuccessfully();
    }
        break;
    default:
    {
        Directives proxy = (Directives)nProxyId;
        Motor_Data_Id id = DataUtil::convertToMotorDataId(proxy);
        if(id != DIRECTIVES_INVALID)
        {
            m_motorData[id] = value;
            emit dataChange(id);
        }
    }
        break;
    }
}

void MotorForm::requestAllValue()
{
//    activeMode(Mode_Cur);
//    for (int i=D_READ_CUR_CURRENT;i<=D_READ_CUR_POSITION;++i)
//    {
//        InnfosProxy::SendProxy(m_deviceId,i);
//    }

    for (int i=D_READ_CUR_P;i<=D_READ_PROFILE_POS_DEC;++i)
    {
        InnfosProxy::SendProxy(m_deviceId,i);
    }

    for (int i=D_READ_PROFILE_VEL_MAX_SPEED;i<=D_READ_PROFILE_VEL_DEC;++i)
    {
        InnfosProxy::SendProxy(m_deviceId,i);
    }

    for (int i=D_READ_CURRENT_PID_MIN;i<=D_READ_POSITION_PID_MAX;++i)
    {
        InnfosProxy::SendProxy(m_deviceId,i);
    }

    for (int i=D_READ_CHART_THRESHOLD;i<=D_READ_CHART_FREQUENCY;++i)
    {
        InnfosProxy::SendProxy(m_deviceId,i);
    }
    InnfosProxy::SendProxy(m_deviceId,D_READ_VOLTAGE);
    InnfosProxy::SendProxy(m_deviceId,D_READ_CURRENT_SCALE);

    for (int i=D_READ_MAX_POS;i<=D_READ_MIN_POS;++i)
    {
        InnfosProxy::SendProxy(m_deviceId,i);
    }

    for (int i=D_READ_POS_OFFSET;i<=D_READ_HOMING_LIMIT;++i)
    {
        InnfosProxy::SendProxy(m_deviceId,i);
    }



    InnfosProxy::SendProxy(m_deviceId,D_READ_VERSION);
    InnfosProxy::SendProxy(m_deviceId,D_READ_HOMING_CUR_MIN);
    InnfosProxy::SendProxy(m_deviceId,D_READ_HOMING_CUR_MAX);

    InnfosProxy::SendProxy(m_deviceId,D_READ_FILTER_C_STATUS);
    InnfosProxy::SendProxy(m_deviceId,D_READ_FILTER_C_VALUE);
    InnfosProxy::SendProxy(m_deviceId,D_READ_FILTER_V_STATUS);
    InnfosProxy::SendProxy(m_deviceId,D_READ_FILTER_V_VALUE);
    InnfosProxy::SendProxy(m_deviceId,D_READ_FILTER_P_STATUS);
    InnfosProxy::SendProxy(m_deviceId,D_READ_FILTER_P_VALUE);
    InnfosProxy::SendProxy(m_deviceId,D_READ_INERTIA);
    InnfosProxy::SendProxy(m_deviceId,D_READ_LOCK_ENERGY);
    InnfosProxy::SendProxy(m_deviceId,D_READ_TEMP_MOTOR);
    InnfosProxy::SendProxy(m_deviceId,D_READ_TEMP_INVERTER);
    InnfosProxy::SendProxy(m_deviceId,D_READ_TEMP_PROTECT);
    InnfosProxy::SendProxy(m_deviceId,D_READ_TEMP_RECOVERY);
    InnfosProxy::SendProxy(m_deviceId,D_READ_CALIBRATION_SWITCH);
    InnfosProxy::SendProxy(m_deviceId,D_READ_CALIBRATION_ANGLE);
    InnfosProxy::SendProxy(m_deviceId,D_READ_MOTORS_SWITCH);
    InnfosProxy::SendProxy(m_deviceId,D_READ_CURRENT_MAX_VEL);
    InnfosProxy::SendProxy(m_deviceId,D_READ_MOTOR_MODE);
#ifndef NO_HEART_BEAT
    m_pHeartTimer->start(1000);
    m_pValueTimer->start(1000);
#endif
}

void MotorForm::setPosPercent(const qreal xPercent, const qreal yPercent)
{
    m_xPercent = xPercent;
    m_yPercent = yPercent;
}

void MotorForm::getPosPercent(qreal &xPercent, qreal &yPercent)
{
    xPercent = m_xPercent;
    yPercent = m_yPercent;
}

void MotorForm::changeStatus(MotorStatus status)
{
    if (status & m_status)
        return;
    if ((m_status&Status_Normal) && (status&Status_Select))
    {
        m_status = m_status &(~Status_Normal);
    }

    if ((status&Status_Normal) && (m_status&Status_Select))
    {
        m_status = m_status &(~Status_Select);
    }

    m_status |= status;
    changeColor();
}

void MotorForm::selectMotor()
{
    changeStatus(Status_Select);
    if(m_modeId == Mode_None/* && m_requestModeId==Mode_None*/)
    {
        activeMode(/*Mode_Cur*/m_pBtnGroup->checkedId());
    }

    emit beSelected(true);

}

void MotorForm::unselectMotor()
{
    changeStatus(Status_Normal);
    //to do deacitve mode
}

qreal MotorForm::getValue(MotorForm::Motor_Data_Id nDataId) const
{
    Q_ASSERT(nDataId < DATA_CNT);
    if(nDataId < DATA_CNT)
    {
        return m_motorData[nDataId];
    }
    return -1;
}

void MotorForm::setValueByUser(MotorForm::Motor_Data_Id nDataId, qreal value, bool bSendProxy)
{
    if (/*getValue(nDataId) != value*/true)
    {
        Directives pId = DataUtil::convertToSetProxyId(nDataId);
        m_motorData[nDataId] = value;
        if(pId != DIRECTIVES_INVALID)
        {           
            //
            if(bSendProxy)
            {
                switch (pId) {
                case D_SET_CHART_FREQUENCY:// these values are qint16
                {
                    qint16 nValue = value;
                    InnfosProxy::SendProxy(m_deviceId,pId,nValue);
                }
                    break;
                case D_SET_LOCK_ENERGY://
                {
                    qint32 nValue = value*75.225;
                    InnfosProxy::SendProxy(m_deviceId,pId,nValue);
                }
                    break;
                case D_SET_FILTER_C_VALUE:
                case D_SET_FILTER_V_VALUE:
                case D_SET_FILTER_P_VALUE:
                case D_SET_TEMP_PROTECT:
                case D_SET_TEMP_RECOVERY:
                {
                    qint16 nValue = value*(1<<8);
                    InnfosProxy::SendProxy(m_deviceId,pId,nValue);
                }
                    break;
                case D_SET_HOMING_LIMIT:
                case D_SET_FILTER_C_STATUS:
                case D_SET_FILTER_V_STATUS:
                case D_SET_FILTER_P_STATUS:
                case D_SWITCH_CALIBRATION:
                {
                    quint8 limit = value;
                    InnfosProxy::SendProxy(m_deviceId,pId,limit);
                }
                    break;
                default:
                    InnfosProxy::SendQrealProxy(m_deviceId,pId,value);
                    break;
                }
            }     
        }
        emit dataChange(nDataId);

    }
}

void MotorForm::responseHeart(bool bSuccess)
{
    if(bSuccess && isOnline())
    {
        m_nHeartFailCnt = 0;
        emit onlineChange(true);
    }

}

bool MotorForm::isOnline() const
{
#ifdef MY_DEBUG
    return true;
#endif
    return m_nHeartFailCnt < 10;
}

bool MotorForm::isSelected() const
{
    return m_status&Status_Select;
}

void MotorForm::errorOccur(const int nErrorId)
{
    if(nErrorId >= 0)
    {
        m_curErrorId = nErrorId;
        if(m_errorHistory.size() == nMaxErrorCnt)
            m_errorHistory.pop_back();
        m_errorHistory.push_front(nErrorId);
        emit Mediator::getInstance()->curErrorChange(m_curErrorId);
        changeStatus(Status_Error);
        // pop error messagebox
        QMap<int,QString> errorInfo;
        errorInfo.insert(0x01,QString::fromLocal8Bit("ID:%1 voltage:%2 Overvoltage error!").arg(m_deviceId).arg(m_motorData[VOLTAGE]));
        errorInfo.insert(0x02,QString::fromLocal8Bit("ID:%1 voltage:%2 Undervoltage error!").arg(m_deviceId).arg(m_motorData[VOLTAGE]));
        errorInfo.insert(0x04,QString::fromLocal8Bit("ID:%1 Locked-rotor error!").arg(m_deviceId));
        errorInfo.insert(0x08,QString::fromLocal8Bit("ID:%1 Temperture:%2 Overheating error!").arg(m_deviceId).arg(m_motorData[TEMP_MOTOR]));
        errorInfo.insert(0x10,QString::fromLocal8Bit("ID:%1 Read or write params error!").arg(m_deviceId));
        errorInfo.insert(0x20,QString::fromLocal8Bit("ID:%1 Multi-turn count error!").arg(m_deviceId));
        errorInfo.insert(0x40,QString::fromLocal8Bit("ID:%1 Temperature sensor error!").arg(m_deviceId));
        errorInfo.insert(0x80,QString::fromLocal8Bit("ID:%1 Can communication error!").arg(m_deviceId));
        errorInfo.insert(0x400,QString::fromLocal8Bit("ID:%1 Driver has been protected!").arg(m_deviceId));

        QList<int> keys = errorInfo.uniqueKeys();
        QString errorStr;
        for(int i=0;i<keys.size();++i)
        {
            if(nErrorId&keys.at(i))
            {
                errorStr += errorInfo.value(keys.at(i));
                errorStr += "\n";
            }
        }

        if(errorStr.length() > 0)
        {
            MsgBox::Error(nullptr,tr("Error"),errorStr,nErrorId);
        }

    }

}

void MotorForm::clearError()
{
    m_curErrorId = 0;
    emit Mediator::getInstance()->curErrorChange(m_curErrorId);
    m_status = m_status &(~Status_Error);
    changeColor();
    //when error clear,the mode change to current automatically;
    m_requestModeId = Mode_Cur;
    activeModeSuccessfully();
}

void MotorForm::changeColor()
{
    if(m_status & Status_Normal)
    {
        ui->frame->setStyleSheet("QFrame#frame{\
                                 background-color: rgb(25, 38, 54);\
                                 border: 2px solid rgb(25, 38, 54);\
                                 border-radius: 8px;\
                                 }");
        ui->leftwidget->setStyleSheet(QLatin1String("QWidget#leftwidget{\n"
            "background-color: rgb(17, 29, 43);\n"
            "border-left: 2px solid rgb(25, 38, 54);\n"
            "border-top: 2px solid rgb(25, 38, 54);\n"
            "border-bottom: 2px solid rgb(25, 38, 54);\n"
            "border-top-left-radius:8;\n"
            "border-bottom-left-radius:8;\n"
            "}"));
    }

    if(m_status & Status_Select)
    {
    ui->frame->setStyleSheet("QFrame#frame{\
                             background-color: rgb(25, 38, 54);\
                             border: 2px solid #068365;\
                             border-radius: 8px;\
                             }");
        ui->leftwidget->setStyleSheet(QLatin1String("QWidget#leftwidget{\n"
            "background-color: rgb(17, 29, 43);\n"
            "border-left: 2px solid #068365;\n"
            "border-top: 2px solid #068365;\n"
            "border-bottom: 2px solid #068365;\n"
            "border-top-left-radius:8;\n"
            "border-bottom-left-radius:8;\n"
            "}"));
        //ui->leftwidget->setStyleSheet(QLatin1String("background-color: rgb(0,160,112);"));
        //modifyPalette(ui->leftwidget,QPalette::Window,QColor(0,160,112));
    }

    if(m_status & Status_Error)
    {
        ui->frame->setStyleSheet("QFrame#frame{\
                                 background-color: rgb(25, 38, 54);\
                                 border: 2px solid rgb(232,50,39);\
                                 border-radius: 8px;\
                                 }");
        ui->leftwidget->setStyleSheet(QLatin1String("QWidget#leftwidget{\n"
            "background-color: rgb(17, 29, 43);\n"
            "border-left: 2px solid rgb(232,50,39);\n"
            "border-top: 2px solid rgb(232,50,39);\n"
            "border-bottom: 2px solid rgb(232,50,39);\n"
            "border-top-left-radius:8;\n"
            "border-bottom-left-radius:8;\n"
            "}"));
    }

    if(m_status & Status_Offline)
    {
        ui->frame->setStyleSheet("QFrame#frame{\
                                 background-color: rgb(25, 38, 54);\
                                 border: 2px solid rgb(255,255,127);\
                                 border-radius: 8px;\
                                 }");
        ui->leftwidget->setStyleSheet(QLatin1String("QWidget#leftwidget{\n"
            "background-color: rgb(17, 29, 43);\n"
            "border-left: 2px solid rgb(255,255,127);\n"
            "border-top: 2px solid rgb(255,255,127);\n"
            "border-bottom: 2px solid rgb(255,255,127);\n"
            "border-top-left-radius:8;\n"
            "border-bottom-left-radius:8;\n"
            "}"));
    }
}

void MotorForm::getLimit(qreal &min, qreal &max)
{
    switch (m_modeId) {
    case Mode_Cur:
        min = -m_motorData[CURRENT_SCALE];
        max = m_motorData[CURRENT_SCALE];
        break;
    case Mode_Vel:
        min = -m_motorData[VELOCITY_SCALE];
        max = m_motorData[VELOCITY_SCALE];
        break;
    case Mode_Pos:
        min = -128;
        max = 128;
        break;
    default:
        break;
    }
}

/*motor manager*/

MotorMgr * MotorMgr::m_pMotorsMgr = nullptr;


MotorMgr *MotorMgr::getInstance()
{
    if(!m_pMotorsMgr)
    {
        m_pMotorsMgr = new MotorMgr();
    }
    return m_pMotorsMgr;
}

void MotorMgr::autoDestroy()
{
    if(m_pMotorsMgr)
        delete m_pMotorsMgr;
    m_pMotorsMgr = nullptr;
}

void MotorMgr::initAllMotors(QMap<quint8, quint32> info)
{
    QMapIterator<quint8, quint32> it(info);
    while(it.hasNext())
    {
        it.next();
        MotorMgr::getInstance()->AddMotor(it.value(),it.key());
    }
    MotorMgr::getInstance()->show();
}

MotorMgr::MotorMgr(QWidget *parent):
    QWidget(parent),
    m_pCurSelected(nullptr),
    m_bCtrlPressed(false),
    m_pEditor(nullptr),
    m_pSwitch(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags() /*| Qt::WindowStaysOnTopHint*/);
    resize(900,700);
    modifyPalette(this,QPalette::Window,QColor(41,60,75));

    InnfosToolbar * titleBar = new InnfosToolbar(this);
    installEventFilter(titleBar);
    titleBar->SetBtnHide(InnfosToolbar::Hide_Prew);

    FramelessHelper * pHelper = new FramelessHelper(this);
    pHelper->activateOn(this);
    pHelper->setTitleHeight(titleBar->height());
    pHelper->setWidgetMovable(true);  //设置窗体可移动
    pHelper->setWidgetResizable(true);  //设置窗体可缩放
    //pHelper->setRubberBandOnResize(true);  //设置橡皮筋效果-可缩
    //pHelper->setRubberBandOnMove(true);  //设置橡皮筋效果-可移动

#ifdef MAXSION_DEVICE
    setWindowTitle(tr("UserInterface Version2.0"));
#else
    setWindowTitle(tr("UserInterface Version1.0"));
#endif
    //setWindowIcon(QIcon(QPixmap(":/images/logo.png").scaled(210,55)));
    titleBar->setToolBarIcon(QPixmap(":/images/logo.png").scaled(210,55));

    QGridLayout * pLayout = new QGridLayout(this);
    pLayout->setContentsMargins(0,0,0,0);
    pLayout->setRowStretch(1,5);
    pLayout->addWidget(titleBar,0,0,1,1);


    QWidget * pBtmBar = new QWidget(this);
    modifyPalette(pBtmBar,QPalette::Window,QColor(29,45,60));
    pBtmBar->setAutoFillBackground(true);
    pBtmBar->setFixedHeight(40);
    pBtmBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QHBoxLayout * pBtmLayout = new QHBoxLayout(pBtmBar);
    pBtmLayout->setContentsMargins(40,5,20,5);
    pBtmLayout->setSpacing(5);
    QLabel * pLabel = new QLabel(pBtmBar);
    pLabel->setPixmap(QPixmap(":/images/www.png"));
    pBtmLayout->addWidget(pLabel);
    pBtmLayout->addStretch(4);

#ifndef NO_CALIBRATION
    QPushButton * pFilterBtn = new QPushButton(tr("calibration"));
    pBtmLayout->addWidget(pFilterBtn);
    connect(pFilterBtn,&QPushButton::clicked,[=]{
        Filter::ShowWindowIfNotExist();
    });
#endif

    m_pSwitch = new QPushButton();
    m_pSwitch->setCheckable(true);
    pBtmLayout->addWidget(m_pSwitch);
    connect(m_pSwitch,&QPushButton::clicked,[=](bool bChecked){
        QMapIterator<quint8,MotorForm *>it(m_motors);
        quint8 nSwitch = bChecked?1:0;
        while(it.hasNext())
        {
            it.next();
            MotorForm * motor = it.value();
            motor->setValueByUser(MotorForm::MOTOR_SWITCH,nSwitch);
        }
        if(nSwitch == 1)//open
        {
            launchWindow::launchTip(nullptr);// pop a window to prevent any operations for several seconds
        }
        CheckServosSwitch();
    });
    changeSwitchBtnStatus(false);
    QComboBox * pVol = new QComboBox();
    pVol->setFixedWidth(80);
    pVol->addItem("42V");
    pVol->addItem("45V");
    pVol->addItem("48V");
    pVol->addItem("51V");
    pBtmLayout->addWidget(pVol);
    connect(pVol,static_cast<void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged),[=](int nIdx){
        quint8 nId = nIdx;
        InnfosProxy::SendProxy(0,D_SET_VOLTAGE,nId);
    });

    QPushButton * pImport = new QPushButton(tr("Import"),pBtmBar);
    pImport->setObjectName("Import");
    pImport->setFixedWidth(130);
    connect(pImport,&QPushButton::clicked,this,&MotorMgr::readMotorsLayout);
    pBtmLayout->addWidget(pImport);
    QPushButton * pExport = new QPushButton(tr("Export"),pBtmBar);
    connect(pExport,&QPushButton::clicked,this,&MotorMgr::saveMotorsLayout);
    pExport->setObjectName("Export");
    pExport->setFixedWidth(130);
    pBtmLayout->addWidget(pExport);

    pLayout->addWidget(pBtmBar,2,0,1,1);
    connect(this,&MotorMgr::countChange,this,&MotorMgr::AdjustLayout);
    connect(Mediator::getInstance(),&Mediator::ErrorMsg,this,&MotorMgr::errorOccur);
    connect(Mediator::getInstance(),&Mediator::clearErrorSuccess,this,&MotorMgr::clearError);
#ifdef MOTION_EDIT
    m_pEditor = new EditorWidget(this);
    pLayout->addWidget(m_pEditor,3,0,1,1);
    pLayout->setRowStretch(3,5);
    m_pEditor->raise();
#endif
#ifdef TEST_DEBUG
    m_bMin = true;
    QWidget * pTestAll = new QWidget(this);
    modifyPalette(pTestAll,QPalette::Window,QColor(129,45,60));
    pTestAll->setAutoFillBackground(true);
    pTestAll->setFixedHeight(40);
    pTestAll->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    QHBoxLayout * pTestLayout = new QHBoxLayout(pTestAll);
    m_pMin = new QLineEdit(pTestAll);
    pTestLayout->addWidget(m_pMin);
    m_pMax = new QLineEdit(pTestAll);
    pTestLayout->addWidget(m_pMax);
    m_pInterval = new QLineEdit(pTestAll);
    pTestLayout->addWidget(m_pInterval);

    m_pTestButton = new QPushButton("Test  ",pTestAll);
    m_pTestButton->setCheckable(true);
    pTestLayout->addWidget(m_pTestButton);

    m_pCurButton = new QPushButton("Cur  ",pTestAll);
    pTestLayout->addWidget(m_pCurButton);
    m_pVeltButton = new QPushButton("Vel  ",pTestAll);
    pTestLayout->addWidget(m_pVeltButton);
    m_pPosButton = new QPushButton("Pos  ",pTestAll);
    pTestLayout->addWidget(m_pPosButton);
    connect(m_pTestButton,&QPushButton::clicked,this,&MotorMgr::buttonCallback1);
    connect(m_pCurButton,&QPushButton::clicked,this,&MotorMgr::buttonCallback1);
    connect(m_pVeltButton,&QPushButton::clicked,this,&MotorMgr::buttonCallback1);
    connect(m_pPosButton,&QPushButton::clicked,this,&MotorMgr::buttonCallback1);
    m_pTestButton->setObjectName("test");
    m_pCurButton->setObjectName("test");
    m_pVeltButton->setObjectName("test");
    m_pPosButton->setObjectName("test");
    QButtonGroup * pGrup = new QButtonGroup(pTestAll);
    pGrup->addButton(m_pCurButton);
    pGrup->addButton(m_pVeltButton);
    pGrup->addButton(m_pPosButton);
    m_pCurButton->setCheckable(true);
    m_pVeltButton->setCheckable(true);
    m_pPosButton->setCheckable(true);

    m_pTestTimer = new QTimer(this);
    connect(m_pTestTimer,&QTimer::timeout,[=]{
        MotorForm::Motor_Data_Id id = MotorForm::DATA_INVALID;
        switch (m_nMode) {
        case MotorForm::Mode_Cur:
            id = MotorForm::CUR_IQ_SET;
            break;
        case MotorForm::Mode_Vel:
            id = MotorForm::VEL_SET;
            break;
        case MotorForm::Mode_Profile_Pos:
            id = MotorForm::POS_SET;
            break;
        default:
            break;
        }
        if(id != MotorForm::DATA_INVALID)
        {
            qreal value = m_min;
            if(!m_bMin)
                value = m_max;
            QMapIterator<quint8,MotorForm *>it(m_motors);
            while(it.hasNext())
            {
                it.next();
                MotorForm * motor = it.value();
                motor->setValueByUser(id,value);
            }
            m_bMin = !m_bMin;
        }
    });

    pLayout->addWidget(pTestAll,4,0,1,1);
#endif
//    m_pVoltage = new QLCDNumber(this);
//    pLayout->addWidget(m_pVoltage,5,0,1,1);
//    m_pVoltage->setDigitCount(4);
}

void MotorMgr::buttonCallback1(bool bChecked)
{
    QPushButton * pBtn = qobject_cast<QPushButton *>(sender());
    if(pBtn == m_pTestButton)
    {
        if(bChecked)
        {
            m_interval = m_pInterval->text().toInt();
            m_min = m_pMin->text().toDouble();
            m_max = m_pMax->text().toDouble();
            if(m_interval > 0)
            {
                m_pTestTimer->start(m_interval);
                triggerSend();
            }
        }
        else
        {
            m_pTestTimer->stop();
        }
    }
    else
    {
        int nMode = -1;
        if(pBtn == m_pCurButton)
            nMode = MotorForm::Mode_Cur;
        else if(pBtn == m_pVeltButton)
            nMode = MotorForm::Mode_Vel;
        else if(pBtn == m_pPosButton)
            nMode = MotorForm::Mode_Profile_Pos;
        if(nMode > -1)
        {
            m_nMode = nMode;
            QMapIterator<quint8,MotorForm *>it(m_motors);
            while(it.hasNext())
            {
                it.next();
                MotorForm * motor = it.value();
                motor->activeMode(m_nMode);
            }
        }
    }
}

void MotorMgr::triggerSend()
{
    {
            MotorForm::Motor_Data_Id id = MotorForm::DATA_INVALID;
            switch (m_nMode) {
            case MotorForm::Mode_Cur:
                id = MotorForm::CUR_IQ_SET;
                break;
            case MotorForm::Mode_Vel:
                id = MotorForm::VEL_SET;
                break;
            case MotorForm::Mode_Profile_Pos:
                id = MotorForm::POS_SET;
                break;
            default:
                break;
            }
            if(id != MotorForm::DATA_INVALID)
            {
                qreal value = m_min;
                if(!m_bMin)
                    value = m_max;
                QMapIterator<quint8,MotorForm *>it(m_motors);
                while(it.hasNext())
                {
                    it.next();
                    MotorForm * motor = it.value();
                    motor->setValueByUser(id,value);
                }
                m_bMin = !m_bMin;
            }
        }
}

void MotorMgr::AddMotor(const quint32 nDeviceMac, const quint8 nDeviceId)
{
    MotorForm * pWidget = new MotorForm(nDeviceId,nDeviceMac,this);
//    pWidget->requestAllValue();
    connect(pWidget,&MotorForm::beSelected,this,&MotorMgr::selectedChange);
    //pWidget->lower();
//    if(m_motors.size() == 0)
//    {
//        connect(pWidget,&MotorForm::dataChange,this,&MotorMgr::onVoltageChange);//motor voltage change,one motor is enough;
//    }
//    pWidget->show();
    if(!m_motors.contains(pWidget->deviceId()))
    {
        m_motors.insert(pWidget->deviceId(),pWidget);
#ifdef MOTION_EDIT
        m_pEditor->addItems(/*nDeviceMac*/nDeviceId);
#endif
    }
    else
    {
        pWidget->changeDeviceId(findAvailavleDeviceId());
        m_motors.insert(pWidget->deviceId(),pWidget);
    }

    //pWidget->requestAllValue();
    QTimer::singleShot(100,[=]{        
        pWidget->show();
        this->AdjustLayout();
        CheckServosSwitch();
    });
}

void MotorMgr::RmMotorFromMap(const quint8 nDeviceId)
{
    m_motors.remove(nDeviceId);
}

void MotorMgr::AddMotorToMap(MotorForm *pMotor)
{
    m_motors.insert(pMotor->deviceId(),pMotor);
}

MotorForm *MotorMgr::getCurSelected()
{
    return m_pCurSelected;
}

MotorForm *MotorMgr::getMotorByDeviceId(const int nDeviceId) const
{
    QMapIterator<quint8,MotorForm *>it(m_motors);
    while(it.hasNext())
    {
        it.next();
        MotorForm * motor = it.value();
        if (motor->deviceId() == nDeviceId)
            return motor;
    }
    return nullptr;
}

MotorForm *MotorMgr::getMotorByMac(const quint32 nMac) const
{
    QMapIterator<quint8,MotorForm *>it(m_motors);
    while(it.hasNext())
    {
        it.next();
        MotorForm * motor = it.value();
        if (motor->deviceMac() == nMac)
            return motor;
    }
    return nullptr;
}

MotorForm *MotorMgr::getDefalutMotor() const
{
    QMapIterator<quint8,MotorForm *>it(m_motors);
    while(it.hasNext())
    {
        it.next();
        MotorForm * motor = it.value();
        return motor;
    }
    return nullptr;
}

void MotorMgr::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    QMapIterator<quint8,MotorForm *>it(m_motors);
    while(it.hasNext())
    {
        it.next();
        MotorForm * motor = it.value();
        InnfosProxy::SendProxy(motor->deviceId(),D_SOFTWARE_CLOSE);//notify all motors that software will close;
        qDebug() << "close send";
    }
}



void MotorMgr::AdjustLayout()
{
    int nCnt = m_motors.size();
    if(nCnt > 0)
    {
        QGridLayout * pLayout = qobject_cast<QGridLayout *>(layout());
        QRect rc = pLayout->cellRect(1,0);
        MotorForm * motor = m_motors.first();
        QSize size = motor->size();
        int nMaxCol = rc.width()/size.width();
        int nRow = (nCnt-1)/nMaxCol+1;
        int nCol = nCnt<nMaxCol?nCnt:nMaxCol;

        int i=0;

        int nW = size.width();
        int nH = size.height();
        qreal xDis = (rc.width()-nW*nCol)/(nCol+1);
        qreal yDis = (rc.height()-nH*nRow)/(nRow+1);
        if(xDis < 0)
        {
            nW -= (nW*nCol-rc.width())/nCol;
            xDis = 0;
        }

        if(yDis < 0)
        {
            nH -= (nH*nRow-rc.height())/(nRow-1);
            yDis = 0;
        }

        QMapIterator<quint8,MotorForm *>it(m_motors);
        while(it.hasNext())
        {
            it.next();
            MotorForm * motor = it.value();
            motor->setGeometry((xDis+nW)*(i%nMaxCol)+xDis,(yDis+nH)*(i/nMaxCol)+yDis+rc.y(),size.width(),size.height());
            updateMotorPosPercent(motor);
            ++i;
        }
    }

}

void MotorMgr::updateMotorGeometry(MotorForm *pMotor)
{
    QGridLayout * pLayout = qobject_cast<QGridLayout *>(layout());
    if(pLayout)
    {
        QRect rc = pLayout->cellRect(1,0);
        QRect rcMotor = pMotor->geometry();
        qreal xPercent = 0;
        qreal yPercent = 0;

        pMotor->getPosPercent(xPercent,yPercent);
        int nX = rc.width()*xPercent;
        int nY = rc.height()*yPercent;
        int nW = rcMotor.width();
        int nH = rcMotor.height();
        pMotor->setGeometry(nX-nW/2,nY-nH/2,nW,nH);
    }
}

void MotorMgr::updateMotorPosPercent(MotorForm *pMotor)
{
    QGridLayout * pLayout = qobject_cast<QGridLayout *>(layout());
    if(pLayout)
    {
        QRect rc = pLayout->cellRect(1,0);
        QRect rcMotor = pMotor->geometry();
        QPoint motorCenter = rcMotor.center();
        qreal xPercent = motorCenter.x()*1.0/rc.width();
        qreal yPercent = motorCenter.y()*1.0/rc.height();

        pMotor->setPosPercent(xPercent,yPercent);

    }
}

void MotorMgr::selectedChange()
{

    MotorForm * pSelect = qobject_cast<MotorForm *>(sender());
    if (pSelect)
    {
        QMapIterator<quint8,MotorForm *>it(m_motors);
        while(it.hasNext())
        {
            if(m_bCtrlPressed)
                break;
            it.next();
            MotorForm * motor = it.value();
            if (motor != pSelect)
                motor->changeStatus(MotorForm::Status_Normal);
        }
        m_pCurSelected = pSelect;
    }

}

void MotorMgr::errorOccur(const int nDeviceId, const qint32 nMsgId)
{
    MotorForm *pMotor = getMotorByDeviceId(nDeviceId);
    if(pMotor)
        pMotor->errorOccur(nMsgId);
    if(m_pEditor)
        m_pEditor->playBtnCallback(false);//如果电机出错，停止动作
}

void MotorMgr::clearError(const int nDeviceId)
{
    MotorForm *pMotor = getMotorByDeviceId(nDeviceId);
    if(pMotor)
        pMotor->clearError();
}

void MotorMgr::saveMotorsLayout()
{
    QFileDialog dialog(this,tr("Save as"),QDir::currentPath());
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Layout(*.layout)"));
    if(dialog.exec() == QDialog::Accepted)
    {
        QString path = dialog.selectedFiles().first();
        if(path.size() > 0)
        {
            saveLayoutToFile(path);
        }

    }
}

void MotorMgr::readMotorsLayout()
{
    QFileDialog dialog(this,tr("Load"),QDir::currentPath());
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Layout(*.layout)"));
    if(dialog.exec() == QDialog::Accepted)
    {
        QString path = dialog.selectedFiles().first();
        if(path.size() > 0)
        {
            readLayoutFromFile(path);
        }

    }
}

void MotorMgr::onVoltageChange(MotorForm::Motor_Data_Id id)
{
    MotorForm * pMotor = qobject_cast<MotorForm *>(sender());
    if(pMotor)
    {
        if(id == MotorForm::VOLTAGE)
        {
            m_pVoltage->display(pMotor->getValue(id));
        }
    }

}

void MotorMgr::saveLayoutToFile(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
    {
        qDebug() << tr("Cannot write file %1 %2").arg(fileName).arg(file.errorString());
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument("1.0",true);
    QMapIterator<quint8,MotorForm *>it(m_motors);
    while(it.hasNext())
    {
        it.next();
        MotorForm * pMotor = it.value();
        qreal x = 0.5;
        qreal y = 0.5;
        pMotor->getPosPercent(x,y);
        writer.writeStartElement(pMotor->motorCode());
        writer.writeTextElement(tr("layoutX"),tr("%1").arg(x));
        writer.writeTextElement(tr("layoutY"),tr("%1").arg(y));
        writer.writeEndElement();
    }

    writer.writeEndDocument();
    file.close();
}

void MotorMgr::readLayoutFromFile(QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QXmlStreamReader reader(&file);
        while(!reader.atEnd())
        {
            QXmlStreamReader::TokenType nType = reader.readNext();
            switch (nType)
            {
            case QXmlStreamReader::StartDocument:

                break;
            case QXmlStreamReader::StartElement:
            {
                QString strElementName = reader.name().toString();
                QMapIterator<quint8,MotorForm *>it(m_motors);
                while(it.hasNext())
                {
                    it.next();
                    MotorForm * pMotor = it.value();
                    if(pMotor->motorCode() == strElementName)
                    {
                        reader.readNextStartElement();
                        qreal x = reader.readElementText().toDouble();
                        reader.readNextStartElement();
                        qreal y = reader.readElementText().toDouble();
                        pMotor->setPosPercent(x,y);
                        updateMotorGeometry(pMotor);
                        break;
                    }
                }
            }
                break;
            default:
                break;
            }
        }
        if(reader.hasError())
        {
            qDebug() << tr("Errorinfo:%1 line:%2 column:%3 character offset:%4").arg(reader.errorString()).arg(reader.lineNumber()).arg(reader.columnNumber()).arg(reader.characterOffset());
        }
        file.close();
    }
}

void MotorMgr::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    QMapIterator<quint8,MotorForm *>it(m_motors);
    AdjustLayout();//if size has changed,relayout all motors.
    while(it.hasNext())
    {
        it.next();
        MotorForm * motor = it.value();
        updateMotorGeometry(motor);
    }
}

void MotorMgr::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Control:
        m_bCtrlPressed = true;
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        if(/*!event->isAccepted()*/true)
        {
            enterDetailWindow();
        }

    }
    default:
        break;
    }
}

void MotorMgr::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Control:
        m_bCtrlPressed = false;
        break;

        break;
    default:
        QWidget::keyReleaseEvent(event);
        break;
    }
}

void MotorMgr::changeSwitchBtnStatus(bool bOn)
{
    if(bOn)
    {
        m_pSwitch->setChecked(true);
        m_pSwitch->setText("ON");
    }
    else
    {
        m_pSwitch->setChecked(false);
        m_pSwitch->setText("OFF");
    }
}

void MotorMgr::CheckServosSwitch(quint32 nDelaymsec)
{


    QTimer::singleShot(nDelaymsec,[=]{
        bool bSwitch = true;
        QMapIterator<quint8,MotorForm *>it(m_motors);
        while(it.hasNext())
        {
            it.next();
            MotorForm * motor = it.value();
            if(motor->getValue(MotorForm::MOTOR_SWITCH) == 0)
            {
                bSwitch = false;
                break;
            }
        }
        changeSwitchBtnStatus(bSwitch);
    });
}

quint8 MotorMgr::findAvailavleDeviceId()
{
    QList<quint8> keys = m_motors.keys();
    for(quint8 i=0;i<256;++i)
    {
        if(!keys.contains(i))
            return i;
    }
    return 0;
}

bool MotorMgr::deviceIdHasExist(const quint8 nDeviceId)
{
    QList<quint8> keys = m_motors.keys();
    if(keys.contains(nDeviceId))
        return true;
    return false;
}

QVector<MotorForm *> MotorMgr::allSelectedMotors()
{
    QVector<MotorForm *> motors;
    QMapIterator<quint8,MotorForm *>it(m_motors);
    while(it.hasNext())
    {
        it.next();
        MotorForm * motor = it.value();
        if(motor->isSelected())
            motors.push_back(motor);
    }
    return motors;
}

void MotorMgr::setRequestActual(bool bRequest)
{
    QMapIterator<quint8,MotorForm *>it(m_motors);
    while(it.hasNext())
    {
        it.next();
        MotorForm * motor = it.value();
        motor->setRequestActual(bRequest);
    }
}

void MotorMgr::enterDetailWindow()
{
    QVector<MotorForm *> selMotors = allSelectedMotors();
    if(selMotors.size() > 1)
    {
        MotorForm * pDefault = selMotors.at(0);
        QString str = tr("Node%1").arg(pDefault->deviceId());
        for(int i=1;i<selMotors.size();++i)
        {
            str += tr(",Node%1").arg(selMotors.at(i)->deviceId());
        }
        if(m_pCurSelected)
            pDefault = m_pCurSelected;
        InnfosWindow * pWnd = new InnfosWindow(nullptr,pDefault->currentMode(),str);
        pDefault->setDetailWnd(pWnd);
        pWnd->show();
        hide();
    }
    else if (selMotors.size() == 1)
    {
        MotorForm * pDefault = selMotors.at(0);
        if(!pDefault->hasFocus())
            return;
        QString str = tr("Node%1").arg(pDefault->deviceId());
        for(int i=1;i<selMotors.size();++i)
        {
            str += tr(",Node%1").arg(selMotors.at(i)->deviceId());
        }
        if(m_pCurSelected)
            pDefault = m_pCurSelected;
        InnfosWindow * pWnd = new InnfosWindow(nullptr,pDefault->currentMode(),str);
        pDefault->setDetailWnd(pWnd);
        pDefault->requestAllValue();
        pWnd->show();
        hide();
    }
}

QPoint MotorMgr::availablePos(const QPoint oriPos, const QSize availableSize)
{
    QPoint finalPos = oriPos;
    QGridLayout * pLayout = qobject_cast<QGridLayout *>(layout());
    QRect rc = pLayout->cellRect(1,0);
    if(oriPos.x() < rc.x())
        finalPos.setX(rc.x());
    if(oriPos.y() < rc.y())
        finalPos.setY(rc.y());
    if(oriPos.x()+availableSize.width() > rc.right())
        finalPos.setX(rc.right()-availableSize.width());
    if(oriPos.y()+availableSize.height() > rc.bottom())
        finalPos.setY(rc.bottom()-availableSize.height());
    return finalPos;
}

bool MotorMgr::isMultiSelected() const
{
    return m_bCtrlPressed;
}
