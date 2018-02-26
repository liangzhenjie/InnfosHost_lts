#include "innfoschartwidget.h"
#include "innfosutil.h"
#include "innfostoolbar.h"
#include "framelesshelper.h"
#include <QThread>
#include <QGridLayout>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QPushButton>
#include <QIntValidator>
#include <QLabel>
#include <QSpacerItem>
#include "innfosutil.h"
#include <QDebug>
#include "innfoschartview.h"
#include <QGroupBox>
#include "mydoublespinbox.h"
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#define COMBINE_CHART

const qreal percent = 0.05;//

InnfosChartWidget::InnfosChartWidget(MotorForm::Motor_Mode modeId, QWidget *parent) :
    QWidget(parent),
    m_modeId(modeId),
    m_pChart(nullptr),
    m_series(nullptr),
    m_demandSeries(nullptr),
    m_nAxisXMax(200),
    m_nCurCnt(0),
    m_pause(false),
    m_canReceiveData(true),
    m_rThreshold(0)
{
    //setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint | windowFlags() | Qt::Tool/*| Qt::WindowStaysOnTopHint*/);
    setAttribute(Qt::WA_DeleteOnClose);
    resize(800,500);
    modifyPalette(this,QPalette::Window,QColor(29,45,60));
    InnfosToolbar * titleBar = new InnfosToolbar(this);
    titleBar->SetBtnHide(InnfosToolbar::Hide_Prew);
    titleBar->setFixedHeight(35);
    installEventFilter(titleBar);
    titleBar->setIconSize(QSize(30,30));
    FramelessHelper * pHelper = new FramelessHelper(this);
    pHelper->activateOn(this);
    pHelper->setTitleHeight(titleBar->height());
    pHelper->setWidgetMovable(true);  //设置窗体可移动
    pHelper->setWidgetResizable(true);  //设置窗体可缩放
    //pHelper->setRubberBandOnResize(true);  //设置橡皮筋效果-可缩
    //pHelper->setRubberBandOnMove(true);  //设置橡皮筋效果-可移动
    connect(Mediator::getInstance(),&Mediator::startNewPeriodChart,this,&InnfosChartWidget::clearChart);
    QGridLayout * pLayout = new QGridLayout(this);
    pLayout->addWidget(titleBar,0,0,1,1);
    pLayout->setContentsMargins(0,0,0,0);
    connect(Mediator::getInstance(),&Mediator::chartVauleChange,this,&InnfosChartWidget::addValues);
}

InnfosChartWidget *InnfosChartWidget::creatChartWidget(MotorForm::Motor_Mode modeId, QWidget *parent)
{
    InnfosChartWidget * pWidget = nullptr;
#ifdef COMBINE_CHART
    pWidget = new CombineChartWidget(modeId,parent);
    return pWidget;
#endif
    switch (modeId) {
    case MotorForm::Mode_Pos:
    case MotorForm::Mode_Profile_Pos:
    {
        pWidget = new PosChartWidget(modeId,parent);
//        QThread * thread = new QThread();
//        pWidget->moveToThread(thread);
//        connect(pWidget,&InnfosChartWidget::destroyed,thread,&QThread::quit);
//        connect(thread,&QThread::started,pWidget,&InnfosChartWidget::updateChart);
//        connect(thread,&QThread::finished,thread,&QThread::deleteLater);
//        thread->start();
    }
        break;
    case MotorForm::Mode_Cur:
    {
        pWidget = new CurChartWidget(modeId,parent);
    }
        break;
    case MotorForm::Mode_Vel:
    case MotorForm::Mode_Profile_Vel:
    {
        pWidget = new VelChartWidget(modeId,parent);
    }
        break;
    default:
        break;
    }
    return pWidget;
}

void InnfosChartWidget::addValues(const int nChannelId, qreal values)
{
    if(/*nChannelId != m_modeId || */!m_canReceiveData)
        return;
    //QMutexLocker locker(&m_mutex);
    m_newValues.append(values);
    if(m_newValues.size() >= 10)//decrease update rate
    {
        updateChart();
    }

}

void InnfosChartWidget::updateChart()
{
    QVector<qreal> newValues;
    if(m_newValues.size() > 0)
    {
       newValues = m_newValues;
       m_newValues.clear();
    }
    //m_mutex.unlock();
    bool bChange = false;
    foreach (qreal pos, newValues)
    {
        m_vPoints.append(QPointF(m_nCurCnt++, pos));
        bChange = true;
//        if(m_nCurCnt > 200)
//            DropPoints(200);
    }
    if(bChange)
    {
        AutoAdjust();
        m_series->replace(m_vPoints);
    }
}

void InnfosChartWidget::DropPoints(const int nCount)
{
    if(nCount < 0)
        return;
    QVector<QPointF> points;
    for (int i = nCount; i < m_vPoints.count(); ++i)
    {
        points.append(QPointF(i - nCount, m_vPoints.at(i).y()));
    }
    m_vPoints = points;
    m_nCurCnt = m_vPoints.size();
}

void InnfosChartWidget::ManualAdjust()
{
    if (!m_pChart)
    {
        return;
    }
    QValueAxis * pivotX = dynamic_cast<QValueAxis *>(m_pChart->axisX());
    QValueAxis * pivotY = dynamic_cast<QValueAxis *>(m_pChart->axisY());
    if (pivotX && pivotY)
    {
        qreal  yMin, yMax;
// 		xMin = pivotX->min();
// 		xMax = pivotX->max();
        yMin = pivotY->max();//这里要注意最大最小值的初始化，这样才能找到最合适的值
        yMax = pivotY->min();
        foreach (QPointF point , m_vPoints)
        {
            qreal dis = point.y()*percent;
            if (dis < 0)
            {
                dis = -dis;
            }

            if (point.y() + dis > yMax)
            {
                yMax = point.y() + dis;
            }

            if (point.y() - dis < yMin)
            {
                yMin = point.y() - dis;
            }
        }
        pivotY->setMin(yMin);
        pivotY->setMax(yMax);
    }
}

void InnfosChartWidget::clearChart()
{
    if(!m_pause)
    {
        DropPoints(m_vPoints.size());
        m_newValues.clear();
        m_canReceiveData = true;
    }
    else
    {
        m_canReceiveData = false;
    }
}

void InnfosChartWidget::onDemandChanged(MotorForm::Motor_Data_Id dataId)
{
    if(dataId!=MotorForm::CUR_ID_SET && dataId!=MotorForm::POS_SET && dataId!=MotorForm::VEL_SET)
        return;
    qreal value = Mediator::getInstance()->getValue(dataId);
    if(/*value <= m_rThreshold*/value == 0)
    {
        m_demandSeries->clear();
        return;
    }

//    if(m_demandSeries)
//    {
//        m_demandSeries->clear();
//        m_demandSeries->append(0,0);
//        m_demandSeries->append(0,value);
//        m_demandSeries->append(m_nAxisXMax,value);
//        //scale y axis
//        QValueAxis * pivotY = dynamic_cast<QValueAxis *>(m_pChart->axisY());
//        qreal max = value*(1+percent);
//        qreal min = m_rThreshold*(1-percent);
//        if(max > pivotY->max())
//            pivotY->setMax(max);
//        if(min < pivotY->min())
//            pivotY->setMin(min);
//    }
}

void InnfosChartWidget::AxisXChanged()
{
    if(m_nAxisXMax > 0)
    {
        QValueAxis * pivotX = dynamic_cast<QValueAxis *>(m_pChart->axisX());
        pivotX->setMax(m_nAxisXMax);
    }
}

void InnfosChartWidget::closeEvent(QCloseEvent *event)
{
    emit closeItSelf();
    QWidget::closeEvent(event);
}

void InnfosChartWidget::AutoAdjust()
{
    if (!m_pChart)
    {
        return;
    }
    QValueAxis * pivotX = dynamic_cast<QValueAxis *>(m_pChart->axisX());
    QValueAxis * pivotY = dynamic_cast<QValueAxis *>(m_pChart->axisY());
    if (pivotX && pivotY)
    {
        qreal minY = pivotY->min();
        qreal maxY = pivotY->max();
        bool bChange = false;
        foreach (QPointF point , m_vPoints)
        {

            qreal dis = point.y()*percent;
            if (dis < 0)
            {
                dis = -dis;
            }
            if (point.y() + dis > maxY)
            {
                maxY = point.y() + dis;
                bChange = true;
            }
            else if (point.y() - dis < minY)
            {
                minY = point.y() - dis;
                bChange = true;
            }
        }
        if(bChange)
        {
            pivotY->setMax(maxY);
            pivotY->setMin(minY);
        }
    }
}

//void InnfosChartWidget::closeEvent(QCloseEvent *event)
//{
//    Q_UNUSED(event);
//    deleteLater();
//}


PosChartWidget::PosChartWidget(MotorForm::Motor_Mode modeId, QWidget *parent):
    InnfosChartWidget(modeId,parent),
    m_nBufferSize(200),
    m_preValue(0)
{
    m_nFrequency = Mediator::getInstance()->getValue(MotorForm::CHART_FREQUENCY);
    m_rThreshold = Mediator::getInstance()->getValue(MotorForm::CHART_THRESHOLD);
    setWindowTitle(tr("Position chart"));
    setWindowIcon(QIcon(":/images/icon_pos_t.png"));
    initInNewThread();
}

void PosChartWidget::updateChart()
{
    InnfosChartWidget::updateChart();
}

void PosChartWidget::paramChanged()
{
    QLineEdit * pEdit = qobject_cast<QLineEdit *>(sender());
    if(pEdit == m_pPosFrequency)
    {
        quint16 nFrequency = pEdit->text().toInt();
        Mediator::getInstance()->setChartFrequency(nFrequency);
        //AxisXChanged();
    }
    else if(pEdit == m_pThreshold)
    {
        m_rThreshold = pEdit->text().toDouble();
        Mediator::getInstance()->setChartThreshold(m_rThreshold);
    }
}

void PosChartWidget::initInNewThread()
{
    m_series = new QLineSeries;
    //m_series->setUseOpenGL(true);



    QGridLayout * pLayout = qobject_cast<QGridLayout *>(layout());
    m_pChart = new QChart;
    QBrush bru = m_pChart->plotAreaBackgroundBrush();
    bru.setStyle(Qt::SolidPattern);
    bru.setColor(QColor(255,0,0,0));
    QPen pen(Qt::SolidLine);
    pen.setColor(QColor(255,0,0));
    m_pChart->setPlotAreaBackgroundPen(pen);
    m_pChart->setPlotAreaBackgroundBrush(bru);
    m_pChart->setPlotAreaBackgroundVisible();
    m_pChart->setBackgroundBrush(bru);
    m_pChart->setBackgroundVisible();
    m_pChart->addSeries(m_series);
    m_pChart->setAcceptHoverEvents(true);
    //m_pChart->setAcceptTouchEvents(true);

    m_pChart->legend()->hide();
    QValueAxis * axisX = new QValueAxis;
    axisX->setRange(0, m_nAxisXMax);
    axisX->setLinePenColor(QColor(135,114,98));
    axisX->setGridLineColor(QColor(135,114,98));
    m_pChart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis * axisY = new QValueAxis;
    axisY->setRange(-128, 128);
    axisY->setLinePenColor(QColor(135,114,98));
    axisY->setGridLineColor(QColor(135,114,98));
    m_pChart->addAxis(axisY, Qt::AlignLeft);
    m_series->attachAxis(axisX);
    m_series->attachAxis(axisY);

    m_demandSeries = new QLineSeries;
    m_demandSeries->setPen(QPen(QBrush(QColor(255,0,0)),2));
    m_pChart->addSeries(m_demandSeries);
    m_demandSeries->attachAxis(axisX);
    m_demandSeries->attachAxis(axisY);

    InnfosChartView * pPosView = new InnfosChartView(m_pChart);
    //pPosView->setEnabled(false);
    bru.setColor(QColor(29,45,60));
    pPosView->setBackgroundBrush(bru);
    connect(m_series,&QXYSeries::clicked,pPosView,&InnfosChartView::keepCallout);
    connect(m_demandSeries,&QXYSeries::clicked,pPosView,&InnfosChartView::keepCallout);
    connect(m_series,&QXYSeries::hovered,pPosView,&InnfosChartView::tooltip);
    connect(m_demandSeries,&QXYSeries::hovered,pPosView,&InnfosChartView::tooltip);
    connect(m_series,&QLineSeries::pointsReplaced,pPosView,&InnfosChartView::clearAllCallouts);
    m_pPosFrequency = new QLineEdit;
    m_pPosFrequency->setMaximumWidth(100);
    m_pPosFrequency->setText(tr("%1").arg(m_nFrequency));
    m_pPosFrequency->setValidator(new QIntValidator(1,200,this));
    connect(m_pPosFrequency, &QLineEdit::editingFinished, this, &InnfosChartWidget::paramChanged);

    QPushButton * pPosAutoAdjust = new QPushButton;
    pPosAutoAdjust->setMinimumWidth(100);
    pPosAutoAdjust->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    pPosAutoAdjust->setText("Auto"); 
    connect(pPosAutoAdjust, &QPushButton::clicked, this, &InnfosChartWidget::ManualAdjust);

    QGridLayout * pDownLayout = new QGridLayout;
    pDownLayout->setContentsMargins(10,0,10,0);
    pLayout->addLayout(pDownLayout,1,0,1,1);
    pDownLayout->addWidget(new QLabel("prescalar"),0,0,1,1);
    pDownLayout->addWidget(m_pPosFrequency,0,1,1,1);

    pDownLayout->addWidget(new QLabel("trig_value"),0,4,1,1);
    m_pThreshold = new QLineEdit;
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    m_pThreshold->setText(QString::number(m_rThreshold,'f',6).replace(rx,""));
    m_pThreshold->setValidator(new QDoubleValidator(0,1,6,this));
    connect(m_pThreshold, &QLineEdit::editingFinished, this, &InnfosChartWidget::paramChanged);
    pDownLayout->addWidget(m_pThreshold,0,5,1,1);

    pDownLayout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding, QSizePolicy::Minimum),0,6,1,1);
    pDownLayout->addWidget(pPosAutoAdjust,0,7,1,1);
    QPushButton * pPause = new QPushButton;
    pPause->setMinimumWidth(100);
    pPause->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    pPause->setText("Pause");
    connect(pPause,&QPushButton::clicked,[=](bool bPause){m_pause = bPause;});
    pPause->setCheckable(true);
    pDownLayout->addWidget(pPause,0,8,1,1);



    pDownLayout->addWidget(pPosView,1,0,1,9);
    pDownLayout->setRowStretch(1,5);
    //pLayout->setHorizontalSpacing(10);
}

bool PosChartWidget::ThroughThreshold(const qreal oriValue)
{
    return true;
    qreal positive[8];
    qreal negative[8];
    for (int i = 0; i < 8; ++i)
    {
        positive[i] = -96 + i * 32 + m_rThreshold;
        negative[i] = -112 + i * 32 - m_rThreshold;
    }
    bool bRet = false;
    if (oriValue < m_preValue)
    {
        for (int i = 0; i < 8; ++i)
        {
            if (oriValue < negative[i] && m_preValue >= negative[i])
            {
                bRet = true;
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < 8; ++i)
        {
            if (oriValue > positive[i] && m_preValue <= positive[i])
            {
                bRet = true;
                break;
            }
        }
    }
    m_preValue = oriValue;
    return bRet;
}

qreal PosChartWidget::GetRelativeValue(const qreal oriValue)
{
    qreal ret = 0;
    for (int i = 0; i < 16; ++i)
    {
        if (oriValue >= -128 + i * 16 && oriValue <= -128 + (i + 1) * 16)
        {
            ret = oriValue - (-128 + i * 16);
            ret = i % 2 == 1 ? ret / 16.0 - 1 : ret / 16.0;
            break;
        }
    }
    return ret;
}

/*velocity chart*/
VelChartWidget::VelChartWidget(MotorForm::Motor_Mode modeId, QWidget *parent):
    InnfosChartWidget(modeId,parent),
    m_nInterval(50)
{
    m_nFrequency = Mediator::getInstance()->getValue(MotorForm::CHART_FREQUENCY);
    m_rThreshold = Mediator::getInstance()->getValue(MotorForm::CHART_THRESHOLD);
    setWindowTitle(tr("Velocity chart"));
    setWindowIcon(QIcon(":/images/icon_vel_t.png"));
    initInNewThread();
}

void VelChartWidget::updateChart()
{
    InnfosChartWidget::updateChart();

}

void VelChartWidget::paramChanged()
{
    QLineEdit * pEdit = qobject_cast<QLineEdit *>(sender());
    if(pEdit == m_pSpeedFrequency)
    {
        quint16 nFrequency = pEdit->text().toInt();
        Mediator::getInstance()->setChartFrequency(nFrequency);
    }
    else if(pEdit == m_pThreshold)
    {
        m_rThreshold = pEdit->text().toDouble();
        Mediator::getInstance()->setChartThreshold(m_rThreshold);
    }
}

void VelChartWidget::initInNewThread()
{
    m_series = new QLineSeries;

    QGridLayout * pLayout = qobject_cast<QGridLayout *>(layout());
    m_pChart = new QChart;
    QBrush bru = m_pChart->plotAreaBackgroundBrush();
    bru.setStyle(Qt::SolidPattern);
    bru.setColor(QColor(255,0,0,0));
    QPen pen(Qt::SolidLine);
    pen.setColor(QColor(255,0,0));
    m_pChart->setPlotAreaBackgroundPen(pen);
    m_pChart->setPlotAreaBackgroundBrush(bru);
    m_pChart->setPlotAreaBackgroundVisible();
    m_pChart->setBackgroundBrush(bru);
    m_pChart->setBackgroundVisible();
    m_pChart->addSeries(m_series);
    m_pChart->legend()->hide();
    QValueAxis * axisX = new QValueAxis;
    axisX->setRange(0, m_nAxisXMax);
    axisX->setLinePenColor(QColor(135,114,98));
    axisX->setGridLineColor(QColor(135,114,98));
    m_pChart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis * axisY = new QValueAxis;
    axisY->setRange(-1, 1);
    axisY->setLinePenColor(QColor(135,114,98));
    axisY->setGridLineColor(QColor(135,114,98));
    m_pChart->addAxis(axisY, Qt::AlignLeft);
    m_series->attachAxis(axisX);
    m_series->attachAxis(axisY);

    m_demandSeries = new QLineSeries;
    m_demandSeries->setPen(QPen(QColor(255,0,0)));
    m_pChart->addSeries(m_demandSeries);
    m_demandSeries->attachAxis(axisX);
    m_demandSeries->attachAxis(axisY);

    InnfosChartView * pPosView = new InnfosChartView(m_pChart);
    //pPosView->setEnabled(false);
    bru.setColor(QColor(29,45,60));
    pPosView->setBackgroundBrush(bru);
    connect(m_series,&QXYSeries::clicked,pPosView,&InnfosChartView::keepCallout);
    connect(m_demandSeries,&QXYSeries::clicked,pPosView,&InnfosChartView::keepCallout);
    connect(m_series,&QXYSeries::hovered,pPosView,&InnfosChartView::tooltip);
    connect(m_demandSeries,&QXYSeries::hovered,pPosView,&InnfosChartView::tooltip);
    connect(m_series,&QLineSeries::pointsReplaced,pPosView,&InnfosChartView::clearAllCallouts);
    m_pSpeedFrequency = new QLineEdit;
    m_pSpeedFrequency->setMaximumWidth(100);
    m_pSpeedFrequency->setText(tr("%1").arg(m_nFrequency));
    m_pSpeedFrequency->setValidator(new QIntValidator(0,200,this));
    QPushButton * pPosAutoAdjust = new QPushButton;
    pPosAutoAdjust->setMinimumWidth(100);
    pPosAutoAdjust->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    pPosAutoAdjust->setText("Auto");
    connect(m_pSpeedFrequency, &QLineEdit::editingFinished, this, &InnfosChartWidget::paramChanged);
    connect(pPosAutoAdjust, &QPushButton::clicked, this, &InnfosChartWidget::ManualAdjust);

    QGridLayout * pDownLayout = new QGridLayout;
    pDownLayout->setContentsMargins(10,0,10,0);
    pLayout->addLayout(pDownLayout,1,0,1,1);
    pDownLayout->addWidget(new QLabel("prescalar"),0,0,1,1);
    pDownLayout->addWidget(m_pSpeedFrequency,0,1,1,1);

    pDownLayout->addWidget(new QLabel("trig_value"),0,4,1,1);
    m_pThreshold = new QLineEdit;
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    m_pThreshold->setText(QString::number(m_rThreshold,'f',6).replace(rx,""));
    m_pThreshold->setValidator(new QDoubleValidator(0,1,6,this));
    connect(m_pThreshold, &QLineEdit::editingFinished, this, &InnfosChartWidget::paramChanged);
    pDownLayout->addWidget(m_pThreshold,0,5,1,1);

    pDownLayout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding, QSizePolicy::Minimum),0,6,1,1);
    pDownLayout->addWidget(pPosAutoAdjust,0,7,1,1);
    QPushButton * pPause = new QPushButton;
    pPause->setMinimumWidth(100);
    pPause->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    pPause->setText("Pause");
    connect(pPause,&QPushButton::clicked,[=](bool bPause){m_pause = bPause;});
    pPause->setCheckable(true);
    pDownLayout->addWidget(pPause,0,8,1,1);

    pDownLayout->addWidget(pPosView,1,0,1,9);
    pDownLayout->setRowStretch(1,5);
}

/*current chart*/
CurChartWidget::CurChartWidget(MotorForm::Motor_Mode modeId, QWidget *parent):
    InnfosChartWidget(modeId,parent)
{
    m_nFrequency = Mediator::getInstance()->getValue(MotorForm::CHART_FREQUENCY);
    m_rThreshold = Mediator::getInstance()->getValue(MotorForm::CHART_THRESHOLD);
    setWindowTitle(tr("Current chart"));
    setWindowIcon(QIcon(":/images/icon_cur_t.png"));
    initInNewThread();
#ifdef LOG_DEBUG
    m_pFileLog = new QFile("chart.txt",this);
    if(!m_pFileLog->open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
        m_pFileLog = nullptr;
#endif
}

void CurChartWidget::updateChart()
{

    //m_mutex.lock();
    QVector<qreal> newValues;
    if(m_newValues.size() >= 200)
    {
       newValues = m_newValues;
#ifdef LOG_DEBUG
       if(m_pFileLog)
       {
           for(int i=0;i<m_newValues.size();++i)
           {
               QTextStream stream(m_pFileLog);
               stream << m_newValues.at(i)/curScale << endl;
           }

       }
#endif
       m_newValues.clear();
    }
    //m_mutex.unlock();
    bool bChange = false;
    foreach (qreal pos, newValues)
    {
        if(m_vPoints.size() < 200)
        {
            m_vPoints.push_back(QPointF(m_nCurCnt++,pos));
        }
        else
        {
            int nIdx = (m_nCurCnt++)%200;
            if(m_vPoints.size() > nIdx)
            {
                m_vPoints[nIdx].setY(pos);
                //qDebug()<<"nIdx="<<nIdx<<"size="<<m_vPoints.size();
            }
        }

        bChange = true;
    }

//    while(m_nCurCnt > m_nAxisXMax)
//    {
//        int nCnt = m_nAxisXMax/4>0?m_nAxisXMax/4:1;
//        DropPoints(nCnt);
//        bChange = true;
//    }
    if(bChange)
    {
        AutoAdjust();
        m_series->replace(m_vPoints);
    }

}

void CurChartWidget::paramChanged()
{
    QLineEdit * pEdit = qobject_cast<QLineEdit *>(sender());
    if(pEdit == m_pCurrentFrequency)
    {
        quint16 nFrequency = pEdit->text().toInt();
        Mediator::getInstance()->setChartFrequency(nFrequency);
    }
    else if(pEdit == m_pThreshold)
    {
        m_rThreshold = pEdit->text().toDouble();
        Mediator::getInstance()->setChartThreshold(m_rThreshold);
    }


}

void CurChartWidget::clearChart()
{
    if(!m_pause)
    {
        m_canReceiveData = true;
        m_newValues.clear();
        m_nCurCnt = 0;
    }
    else
    {
        m_canReceiveData = false;
    }
}

void CurChartWidget::initInNewThread()
{
    m_series = new QLineSeries;

    QGridLayout * pLayout = qobject_cast<QGridLayout *>(layout());
    m_pChart = new QChart;
    QBrush bru = m_pChart->plotAreaBackgroundBrush();
    bru.setStyle(Qt::SolidPattern);
    bru.setColor(QColor(255,0,0,0));
    QPen pen(Qt::SolidLine);
    pen.setColor(QColor(255,0,0));
    m_pChart->setPlotAreaBackgroundPen(pen);
    m_pChart->setPlotAreaBackgroundBrush(bru);
    m_pChart->setPlotAreaBackgroundVisible();
    m_pChart->setBackgroundBrush(bru);
    m_pChart->setBackgroundVisible();
    m_pChart->addSeries(m_series);
    m_pChart->legend()->hide();
    QValueAxis * axisX = new QValueAxis;
    axisX->setRange(0, m_nAxisXMax);
    axisX->setLinePenColor(QColor(135,114,98));
    axisX->setGridLineColor(QColor(135,114,98));
    m_pChart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis * axisY = new QValueAxis;
    axisY->setRange(-1, 1);
    axisY->setLinePenColor(QColor(135,114,98));
    axisY->setGridLineColor(QColor(135,114,98));
    m_pChart->addAxis(axisY, Qt::AlignLeft);
    m_series->attachAxis(axisX);
    m_series->attachAxis(axisY);

    m_demandSeries = new QLineSeries;
    m_demandSeries->setPen(QPen(QColor(255,0,0)));
    m_pChart->addSeries(m_demandSeries);
    m_demandSeries->attachAxis(axisX);
    m_demandSeries->attachAxis(axisY);

    InnfosChartView * pPosView = new InnfosChartView(m_pChart);
    //pPosView->setEnabled(false);
    bru.setColor(QColor(29,45,60));
    pPosView->setBackgroundBrush(bru);
    connect(m_series,&QXYSeries::clicked,pPosView,&InnfosChartView::keepCallout);
    connect(m_demandSeries,&QXYSeries::clicked,pPosView,&InnfosChartView::keepCallout);
    connect(m_series,&QXYSeries::hovered,pPosView,&InnfosChartView::tooltip);
    connect(m_demandSeries,&QXYSeries::hovered,pPosView,&InnfosChartView::tooltip);
    connect(m_series,&QLineSeries::pointsReplaced,pPosView,&InnfosChartView::clearAllCallouts);
    m_pCurrentFrequency = new QLineEdit;
    m_pCurrentFrequency->setMaximumWidth(100);
    m_pCurrentFrequency->setText(tr("%1").arg(m_nFrequency));
    m_pCurrentFrequency->setValidator(new QIntValidator(0,200,this));
    QPushButton * pPosAutoAdjust = new QPushButton;
    pPosAutoAdjust->setMinimumWidth(100);
    pPosAutoAdjust->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    pPosAutoAdjust->setText("Auto");
    connect(m_pCurrentFrequency, &QLineEdit::editingFinished, this, &InnfosChartWidget::paramChanged);
    connect(pPosAutoAdjust, &QPushButton::clicked, this, &InnfosChartWidget::ManualAdjust);
    QGridLayout * pDownLayout = new QGridLayout;
    pDownLayout->setContentsMargins(10,0,10,0);
    pLayout->addLayout(pDownLayout,1,0,1,1);
    pDownLayout->addWidget(new QLabel("prescalar"),0,0,1,1);
    pDownLayout->addWidget(m_pCurrentFrequency,0,1,1,1);

    pDownLayout->addWidget(new QLabel("trig_value"),0,4,1,1);
    m_pThreshold = new QLineEdit;
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    m_pThreshold->setText(QString::number(m_rThreshold,'f',6).replace(rx,""));
    m_pThreshold->setValidator(new QDoubleValidator(0,1,6,this));
    connect(m_pThreshold, &QLineEdit::editingFinished, this, &InnfosChartWidget::paramChanged);
    pDownLayout->addWidget(m_pThreshold,0,5,1,1);

    pDownLayout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding, QSizePolicy::Minimum),0,6,1,1);
    pDownLayout->addWidget(pPosAutoAdjust,0,7,1,1);
    QPushButton * pPause = new QPushButton;
    pPause->setMinimumWidth(100);
    pPause->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    pPause->setText("Pause");
    connect(pPause,&QPushButton::clicked,[=](bool bPause){m_pause = bPause;});
    pPause->setCheckable(true);
    pDownLayout->addWidget(pPause,0,8,1,1);
    pDownLayout->addWidget(pPosView,1,0,1,9);
    pDownLayout->setRowStretch(1,5);
}


/*combine chart*/
CombineChartWidget::CombineChartWidget(MotorForm::Motor_Mode modeId, QWidget *parent):
    InnfosChartWidget(modeId,parent),
    m_fOffsetChange(0),
    m_fScaleChange(0),
    m_fThresholdScale(1)
{
    m_nFrequency = Mediator::getInstance()->getValue(MotorForm::CHART_FREQUENCY);
    m_rThreshold = Mediator::getInstance()->getValue(MotorForm::CHART_THRESHOLD);
    setWindowTitle(tr("Combine chart"));
    setWindowIcon(QIcon(":/images/icon_cur_t.png"));
    for(int i=0;i<channel_cnt;++i)
    {
        m_fOffset[i] = 0;
        m_fScale[i] = 1;
        m_bShow[i] = true;
    }
    initInNewThread();
    readParams("params.txt");
    onModeChange(modeId);

#ifdef LOG_DEBUG
    m_pFileLog = new QFile("chart.txt",this);
    if(!m_pFileLog->open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
        m_pFileLog = nullptr;
#endif
}

CombineChartWidget::~CombineChartWidget()
{
    saveParams("params.txt");
    Mediator::getInstance()->closeChart();
}

void CombineChartWidget::ManualAdjust()
{
    if (!m_pChart)
    {
        return;
    }
    QValueAxis * pivotX = dynamic_cast<QValueAxis *>(m_pChart->axisX());
    QValueAxis * pivotY = dynamic_cast<QValueAxis *>(m_pChart->axisY());
    if (pivotX && pivotY)
    {
        qreal  yMin, yMax;
// 		xMin = pivotX->min();
// 		xMax = pivotX->max();
        yMin = pivotY->max();//这里要注意最大最小值的初始化，这样才能找到最合适的值
        yMax = pivotY->min();
        for (int i=0;i<channel_cnt;++i)
        {
            foreach (QPointF point , m_combinePoints[i])
            {
                qreal dis = point.y()*percent;
                if (dis < 0)
                {
                    dis = -dis;
                }

                if (point.y() + dis > yMax)
                {
                    yMax = point.y() + dis;
                }

                if (point.y() - dis < yMin)
                {
                    yMin = point.y() - dis;
                }
            }
        }

        pivotY->setMin(yMin);
        pivotY->setMax(yMax);
    }
}

void CombineChartWidget::onModeChange(int nMode)
{
    switch (nMode) {
    case MotorForm::Mode_Cur:
        m_fThresholdScale = Mediator::getInstance()->curCurrentScale();
        break;
    case MotorForm::Mode_Vel:
    case MotorForm::Mode_Profile_Vel:
        m_fThresholdScale = Mediator::getInstance()->curVelocityScale();
        break;
    case MotorForm::Mode_Pos:
    case MotorForm::Mode_Profile_Pos:
        m_fThresholdScale = 128;
        break;
    default:
        break;
    }
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    m_pThreshold->setText(QString::number(m_rThreshold*m_fThresholdScale,'f',3).replace(rx,""));
    m_pThreshold->setValidator(new QDoubleValidator(-m_fThresholdScale,m_fThresholdScale,3,this));
}

void CombineChartWidget::addValues(const int nChannelId, qreal values)
{
    int nId = nChannelId;
    if(!m_canReceiveData || !m_bShow[nId])
        return;

    values = values*m_fScale[nId]+m_fOffset[nId];
    m_combineValues[nId].append(values);
    if(m_combineValues[nId].size() >= 10)//decrease update rate
    {
        QVector<qreal> newValues;

        newValues = m_combineValues[nId];
        m_combineValues[nId].clear();

        //m_mutex.unlock();
        bool bChange = false;
        foreach (qreal pos, newValues)
        {
            m_combinePoints[nId].append(QPointF(m_ncombineCnt[nId]++, pos));
            bChange = true;
    //        if(m_nCurCnt > 200)
    //            DropPoints(200);
        }
        if(bChange)
        {
            //AutoAdjust();
            m_combineSeries[nId]->replace(m_combinePoints[nId]);
        }
    }
}



void CombineChartWidget::paramChanged()
{
    QLineEdit * pEdit = qobject_cast<QLineEdit *>(sender());
    if(pEdit == m_pCurrentFrequency)
    {
        quint16 nFrequency = pEdit->text().toInt();
        m_nFrequency = nFrequency;
        Mediator::getInstance()->setChartFrequency(nFrequency);
    }
    else if(pEdit == m_pThreshold)
    {
        m_rThreshold = pEdit->text().toDouble()/m_fThresholdScale;
        Mediator::getInstance()->setChartThreshold(m_rThreshold);
    }


}

void CombineChartWidget::clearChart()
{
    if(!m_pause)
    {
        m_canReceiveData = true;
        for(int i=0;i<channel_cnt;++i)
        {
            m_combineValues[i].clear();
            m_ncombineCnt[i] = 0;
            m_combinePoints[i].clear();
        }
    }
    else
    {
        m_canReceiveData = false;
    }
}

void CombineChartWidget::OffSetChanged(double offset)
{
    QObject * pSender = sender();
    for(int i=0;i<channel_cnt;++i)
    {
        if(pSender == m_pSpinOff[i])
        {
            m_fOffsetChange = offset - m_fOffset[i];
            m_fOffset[i] = offset;
            refresh(i);
            break;
        }
    }
}

void CombineChartWidget::ScaleChanged(double scale)
{
    QObject * pSender = sender();
    for(int i=0;i<channel_cnt;++i)
    {
        if(pSender == m_pSpinScale[i])
        {
            m_fScaleChange = scale - m_fScale[i];
            m_fScale[i] = scale;
            refresh(i);
            break;
        }
    }
}

void CombineChartWidget::onChannelSwitch(bool bChecked)
{
    QObject * pSender = sender();
    for(int i=0;i<channel_cnt;++i)
    {
        if(pSender == m_pOffBtn[i])
        {
            switchChannel(i,bChecked);
            break;
        }
    }
}

void CombineChartWidget::autoFullScreen()
{
    for(int i=0;i<channel_cnt;++i)
    {
        channelFullScreen(i);
    }
}

void CombineChartWidget::channelFullScreen(const int nChannelId)
{
    if(m_combinePoints[nChannelId].size() < 3)
        return;
    QValueAxis * pivotX = dynamic_cast<QValueAxis *>(m_pChart->axisX());
    QValueAxis * pivotY = dynamic_cast<QValueAxis *>(m_pChart->axisY());
    if (pivotX && pivotY)
    {
        qreal  yMin, yMax;
        yMin = pivotY->min();
        yMax = pivotY->max();
        qreal channelMin = m_combinePoints[nChannelId].at(0).y();
        qreal channelMax = m_combinePoints[nChannelId].at(0).y();
        foreach (QPointF point , m_combinePoints[nChannelId])
        {
            if (point.y() > channelMax)
            {
                channelMax = point.y() ;
            }

            if (point.y() < channelMin)
            {
                channelMin = point.y();
            }
        }
        if(qAbs(channelMax-channelMin) > 0.002)//if this channel is straight line,any change should not happen.
        {
            qreal scale = (yMax-yMin)*0.95/(channelMax-channelMin)*m_fScale[nChannelId];
            if(scale > 1000)//if scale is too large,treat it as a straight line.
                return;
            qreal offset = -(channelMax+channelMin-m_fOffset[nChannelId]*2)/2*scale/m_fScale[nChannelId];

            m_pSpinScale[nChannelId]->setValue(scale);
            emit m_pSpinScale[nChannelId]->myValueChanged(scale);


            m_pSpinOff[nChannelId]->setValue(offset);
            emit m_pSpinOff[nChannelId]->myValueChanged(offset);
        }

    }
}


void CombineChartWidget::saveChannelData()
{
    QFile file("channel_data.csv");
    if(file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
    {
        QTextStream out(&file);
        int nCnt = m_combinePoints[channel_1].size();
        for(int i=channel_1;i<channel_cnt;++i)
        {
            if(nCnt < m_combinePoints[i].size())
                nCnt = m_combinePoints[i].size();
        }
        for(int i=0;i<nCnt;++i)
        {
            for(int nChannelId=0;nChannelId<channel_cnt;++nChannelId)
            {
                double ori = (m_combinePoints[nChannelId].at(i).y()-m_fOffset[nChannelId])/m_fScale[nChannelId];
                out <<  ori << ",";
            }
            endl(out);
        }
    }
    file.close();
}


void CombineChartWidget::initInNewThread()
{
    m_series = new QLineSeries;

    QGridLayout * pLayout = qobject_cast<QGridLayout *>(layout());
    m_pChart = new QChart;
    QBrush bru = m_pChart->plotAreaBackgroundBrush();
    bru.setStyle(Qt::SolidPattern);
    bru.setColor(QColor(255,0,0,0));
    QPen pen(Qt::SolidLine);
    pen.setColor(QColor(255,0,0));
    m_pChart->setPlotAreaBackgroundPen(pen);
    m_pChart->setPlotAreaBackgroundBrush(bru);
    m_pChart->setPlotAreaBackgroundVisible();
    m_pChart->setBackgroundBrush(bru);
    m_pChart->setBackgroundVisible();
    m_pChart->addSeries(m_series);
    m_pChart->legend()->hide();
    QValueAxis * axisX = new QValueAxis;
    axisX->setRange(0, m_nAxisXMax);
    axisX->setLinePenColor(QColor(50,50,50));
    axisX->setGridLineColor(QColor(50,50,50));
    axisX->setMinorGridLineColor(QColor(50,50,50));
    m_pChart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis * axisY = new QValueAxis();
    axisY->setRange(-1, 1);
    axisY->setLinePenColor(QColor(50,50,50));
    axisY->setGridLineColor(QColor(50,50,50));
    axisY->setMinorGridLineColor(QColor(30,30,30));
    axisY->setMinorTickCount(4);
    m_pChart->addAxis(axisY, Qt::AlignLeft);
    m_series->attachAxis(axisX);
    m_series->attachAxis(axisY);

    m_demandSeries = new QLineSeries;
    m_demandSeries->setPen(QPen(QColor(255,0,0)));
    //m_pChart->addSeries(m_demandSeries);
    m_demandSeries->attachAxis(axisX);
    m_demandSeries->attachAxis(axisY);

    QColor color[channel_cnt] = {QColor(255,0,255),QColor(255,255,0),QColor(0,255,0),QColor(0,255,255)};




    InnfosChartView * pPosView = new InnfosChartView(m_pChart);
    //pPosView->setEnabled(false);
    bru.setColor(QColor(0,0,0));
    pPosView->setBackgroundBrush(bru);
    for(int i=0;i<channel_cnt;++i)
    {
        m_combineSeries[i] = new QLineSeries;
        m_combineSeries[i]->setPen(QPen(color[i]));
        m_pChart->addSeries(m_combineSeries[i]);
        m_combineSeries[i]->attachAxis(axisX);
        m_combineSeries[i]->attachAxis(axisY);

        connect(m_combineSeries[i],&QXYSeries::clicked,pPosView,&InnfosChartView::keepCallout);
        connect(m_combineSeries[i],&QXYSeries::hovered,pPosView,&InnfosChartView::tooltip);
        connect(m_combineSeries[i],&QLineSeries::pointsReplaced,pPosView,&InnfosChartView::clearAllCallouts);
    }
    connect(m_series,&QXYSeries::clicked,pPosView,&InnfosChartView::keepCallout);
    connect(m_demandSeries,&QXYSeries::clicked,pPosView,&InnfosChartView::keepCallout);
    connect(m_series,&QXYSeries::hovered,pPosView,&InnfosChartView::tooltip);
    connect(m_demandSeries,&QXYSeries::hovered,pPosView,&InnfosChartView::tooltip);
    connect(m_series,&QLineSeries::pointsReplaced,pPosView,&InnfosChartView::clearAllCallouts);
    m_pCurrentFrequency = new QLineEdit;
    m_pCurrentFrequency->setMaximumWidth(100);
    m_pCurrentFrequency->setText(tr("%1").arg(m_nFrequency));
    m_pCurrentFrequency->setValidator(new QIntValidator(0,32767,this));
    QPushButton * pPosAutoAdjust = new QPushButton;
    pPosAutoAdjust->setMinimumWidth(100);
    pPosAutoAdjust->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    pPosAutoAdjust->setText("Save");
    connect(m_pCurrentFrequency, &QLineEdit::editingFinished, this, &InnfosChartWidget::paramChanged);
    connect(pPosAutoAdjust, &QPushButton::clicked, this, &CombineChartWidget::saveChannelData);
    QGridLayout * pDownLayout = new QGridLayout;
    pDownLayout->setContentsMargins(10,0,10,0);
    pLayout->addLayout(pDownLayout,1,0,1,1);
    pDownLayout->addWidget(new QLabel("prescalar"),0,0,1,1);
    pDownLayout->addWidget(m_pCurrentFrequency,0,1,1,1);

    pDownLayout->addWidget(new QLabel("trig_value"),0,4,1,1);
    m_pThreshold = new QLineEdit;
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    m_pThreshold->setText(QString::number(m_rThreshold*m_fThresholdScale,'f',3).replace(rx,""));
    m_pThreshold->setValidator(new QDoubleValidator(-m_fThresholdScale,m_fThresholdScale,3,this));
    connect(m_pThreshold, &QLineEdit::editingFinished, this, &InnfosChartWidget::paramChanged);
    pDownLayout->addWidget(m_pThreshold,0,5,1,1);

    pDownLayout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding, QSizePolicy::Minimum),0,6,1,1);
    pDownLayout->addWidget(pPosAutoAdjust,0,7,1,1);
    QPushButton * pPause = new QPushButton;
    pPause->setMinimumWidth(100);
    pPause->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    pPause->setText("Pause");
    connect(pPause,&QPushButton::clicked,[=](bool bPause){m_pause = bPause;});
    pPause->setCheckable(true);
    pDownLayout->addWidget(pPause,0,8,1,1);

    QPushButton * pAuto = new QPushButton;
    pAuto->setMinimumWidth(100);
    pAuto->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    pAuto->setText("Auto");
    connect(pAuto,&QPushButton::clicked,this,&CombineChartWidget::autoFullScreen);
    pDownLayout->addWidget(pAuto,0,9,1,1);

    pDownLayout->addWidget(pPosView,1,0,9,9);
    //pDownLayout->setRowStretch(1,5);
    for(int i=0;i<channel_cnt;++i)
    {
        QGroupBox * pGroup = new QGroupBox(tr("channel%1").arg(i+1));
        pGroup->setObjectName(tr("channel%1").arg(i+1));
        QGridLayout * pLayout = new QGridLayout(pGroup);

        QLabel * pLabelOff = new QLabel(tr("Offset"));
        m_pSpinOff[i] = new MyDoubleSpinBox();
        m_pSpinOff[i] ->setMaximum(20);
        m_pSpinOff[i] ->setMinimum(-20);
        m_pSpinOff[i] ->setSingleStep(0.1);
        m_pSpinOff[i] ->setValue(m_fOffset[i]);
        m_pSpinOff[i] ->setMinimumWidth(60);
        connect(m_pSpinOff[i] ,static_cast<void(MyDoubleSpinBox:: *)(double)>(&MyDoubleSpinBox::myValueChanged),this,&CombineChartWidget::OffSetChanged);
        pLayout->addWidget(pLabelOff,0,0,1,1);
        pLayout->addWidget(m_pSpinOff[i],0,1,1,1);

        QLabel * pLabelScale = new QLabel(tr("Scale"));
        m_pSpinScale[i] = new MyDoubleSpinBox();
        m_pSpinScale[i]->setMaximum(1000);
        m_pSpinScale[i]->setMinimum(0);
        m_pSpinScale[i]->setSingleStep(0.1);
        m_pSpinScale[i]->setValue(m_fScale[i]);
        m_pSpinScale[i]->setMinimumWidth(60);
        connect(m_pSpinScale[i] ,static_cast<void(MyDoubleSpinBox:: *)(double)>(&MyDoubleSpinBox::myValueChanged),this,&CombineChartWidget::ScaleChanged);
        pLayout->addWidget(pLabelScale,1,0,1,1);
        pLayout->addWidget(m_pSpinScale[i],1,1,1,1);

        m_pOffBtn[i] = new QPushButton(tr("Off"));
        m_pOffBtn[i]->setCheckable(true);
        pLayout->addWidget(m_pOffBtn[i],2,0,1,2);
        m_pOffBtn[i]->setObjectName("ChartOff");
        connect(m_pOffBtn[i],&QPushButton::clicked,this,&CombineChartWidget::onChannelSwitch);

        pDownLayout->addWidget(pGroup,i*2+1,9,2,1);
    }
}

void CombineChartWidget::saveParams(QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
    {
        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);
        writer.writeStartDocument("1.0",true);
        writer.writeStartElement("Channel_Params");
        writer.writeAttribute("Version","1.0");

        writer.writeTextElement("prescalar",tr("%1").arg(m_nFrequency));
        writer.writeTextElement("threshold",tr("%1").arg(m_rThreshold));
        for(int i=0;i<channel_cnt;++i)
        {
            writer.writeTextElement(tr("scale%1").arg(i),tr("%1").arg(m_fScale[i]));
            writer.writeTextElement(tr("offset%1").arg(i),tr("%1").arg(m_fOffset[i]));
        }

        writer.writeEndElement();
        writer.writeEndDocument();
    }
    file.close();
}

void CombineChartWidget::readParams(QString fileName)
{
    if(!QFile::exists(fileName))
        return;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QXmlStreamReader reader(&file);
        while (!reader.atEnd()) {
            QXmlStreamReader::TokenType type = reader.readNext();
            switch (type) {
            case QXmlStreamReader::StartElement:
            {
                QStringRef name = reader.name();
                if(name.contains("scale"))
                {
                    int nIdx = name.right(name.length()-5).toInt();
                    m_fScale[nIdx] = reader.readElementText().toDouble();
                    m_pSpinScale[nIdx]->setValue(m_fScale[nIdx]);
                }
                else if(name.contains("offset"))
                {
                    int nIdx = name.right(name.length()-6).toInt();
                    m_fOffset[nIdx] = reader.readElementText().toDouble();
                    m_pSpinOff[nIdx]->setValue(m_fOffset[nIdx]);
                }
                else if(name.contains("prescalar"))
                {
                    m_nFrequency = reader.readElementText().toInt();
                    m_pCurrentFrequency->setText(tr("%1").arg(m_nFrequency));
                    m_pCurrentFrequency->editingFinished();
                }
                else if(name.contains("threshold"))
                {
                    QRegExp rx;
                    rx.setPattern("(\\.){0,1}0+$");
                    m_rThreshold = reader.readElementText().toDouble();
                    m_pThreshold->setText(QString::number(m_rThreshold*m_fThresholdScale,'f',3).replace(rx,""));
                    m_pThreshold->editingFinished();
                }
            }
                break;
            default:
                break;
            }
        }

    }
    file.close();
}

void CombineChartWidget::AutoAdjust()
{
    if (!m_pChart)
    {
        return;
    }
    QValueAxis * pivotX = dynamic_cast<QValueAxis *>(m_pChart->axisX());
    QValueAxis * pivotY = dynamic_cast<QValueAxis *>(m_pChart->axisY());
    if (pivotX && pivotY)
    {
        qreal minY = pivotY->min();
        qreal maxY = pivotY->max();
        bool bChange = false;
        for (int i=0;i<channel_cnt; ++i)
        {
            foreach (QPointF point , m_combinePoints[i])
            {

                qreal dis = point.y()*percent;
                if (dis < 0)
                {
                    dis = -dis;
                }
                if (point.y() + dis > maxY)
                {
                    maxY = point.y() + dis;
                    bChange = true;
                }
                else if (point.y() - dis < minY)
                {
                    minY = point.y() - dis;
                    bChange = true;
                }
            }
        }

        if(bChange)
        {
            pivotY->setMax(maxY);
            pivotY->setMin(minY);
        }
    }
}

void CombineChartWidget::refresh(const int id)
{
    if(id <0 || id >= channel_cnt)
        return;
    int nSize = m_combinePoints[id].size();
    double oriOff = m_fOffset[id]-m_fOffsetChange;
    double oriScale = m_fScale[id]-m_fScaleChange;
    m_fOffsetChange = 0;
    m_fScaleChange = 0;
    for(int n=0;n<nSize;++n)
    {
        double ori = (m_combinePoints[id][n].y()-oriOff)/oriScale;
        m_combinePoints[id][n].setY(ori*m_fScale[id]+m_fOffset[id]);
    }

    m_combineSeries[id]->replace(m_combinePoints[id]);

    //ManualAdjust();
}

void CombineChartWidget::switchChannel(const int nChannelId, bool bOff)
{
    if(nChannelId <0 || nChannelId >= channel_cnt)
        return;
    m_bShow[nChannelId] = !bOff;
    if(bOff)
    {
        m_combineValues[nChannelId].clear();
        m_combinePoints[nChannelId].clear();
        m_combineSeries[nChannelId]->replace(m_combinePoints[nChannelId]);
        Mediator::getInstance()->closeChannel(nChannelId);
    }
    else
    {
        Mediator::getInstance()->openChannel(nChannelId);
    }
}
