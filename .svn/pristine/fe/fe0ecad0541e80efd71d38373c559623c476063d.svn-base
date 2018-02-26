#ifndef INNFOSCHARTWIDGET_H
#define INNFOSCHARTWIDGET_H

#include <QWidget>
#include "mediator.h"
#include <QVector>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QMutex>
#include <QLineEdit>

QT_CHARTS_BEGIN_NAMESPACE

QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class QFile;
class MyDoubleSpinBox;
class QPushButton;


class InnfosChartWidget : public QWidget
{
    Q_OBJECT
public:
    enum{
        channel_1,
        channel_2,
        channel_3,
        channel_4,
        channel_cnt
    };
    explicit InnfosChartWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    static InnfosChartWidget * creatChartWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);

signals:
    void closeItSelf();
public slots:
    virtual void addValues(const int nChannelId, qreal values);
    virtual void updateChart();
    void ManualAdjust();
    virtual void paramChanged(){}
    virtual void clearChart();
    void onDemandChanged(MotorForm::Motor_Data_Id dataId);
protected:
    //void closeEvent(QCloseEvent *event);
    void AxisXChanged();
    void closeEvent(QCloseEvent *event);
protected:
    void DropPoints(const int nCount);//
    void AutoAdjust();
    virtual void initInNewThread()=0;//
    MotorForm::Motor_Mode m_modeId;
    QVector<qreal> m_newValues;
    QChart * m_pChart;
    QLineSeries * m_series;
    QLineSeries * m_demandSeries;
    qint32 m_nAxisXMax;
    qint32 m_nCurCnt;
    QVector<QPointF> m_vPoints;//
    QMutex m_mutex;
    bool m_pause;
    bool m_canReceiveData;
    qreal m_rThreshold;//
};

class PosChartWidget : public InnfosChartWidget
{
    Q_OBJECT
public:
    PosChartWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    virtual ~PosChartWidget() {}
public slots:
    virtual void updateChart();
    virtual void paramChanged();
protected:
    virtual void initInNewThread();
    bool ThroughThreshold(const qreal oriValue);
    qreal GetRelativeValue(const qreal oriValue);

private:
    int m_nFrequency;
    QVector <int> m_perPeriodPointCount;//
    qreal m_preValue;//
    qint32 m_nBufferSize;

    QLineEdit * m_pPosFrequency;
    QLineEdit * m_pBufSize;
    QLineEdit * m_pThreshold;

};

class VelChartWidget : public InnfosChartWidget
{
    Q_OBJECT
public:
    VelChartWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    virtual ~VelChartWidget() {}
public slots:
    virtual void updateChart();
    virtual void paramChanged();
protected:
    virtual void initInNewThread();//
private:
private:

    QVector<qreal> m_YposFront;//最前面的点数组
    QVector<qreal> m_YposAfter;//紧随其后的点数组
    qint32 m_nInterval;//间隔

    int m_nFrequency;

    QLineEdit * m_pSpeedFrequency;
    QLineEdit * m_pSpan;//检测距离
    QLineEdit * m_pThreshold;//

};

class CurChartWidget : public InnfosChartWidget
{
    Q_OBJECT
public:
    CurChartWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    virtual ~CurChartWidget() {}
public slots:
    virtual void updateChart();
    virtual void paramChanged();
    virtual void clearChart();
protected:
    virtual void initInNewThread();//
private:
private:

    QLineEdit * m_pCurrentFrequency;
    QLineEdit * m_pThreshold;//
    int m_nFrequency;
    //log file
    QFile * m_pFileLog;
};

//this chart only used by testing
class CombineChartWidget : public InnfosChartWidget
{
    Q_OBJECT
public:
    CombineChartWidget(MotorForm::Motor_Mode modeId,QWidget *parent = 0);
    virtual ~CombineChartWidget();
    void ManualAdjust();
    void onModeChange(int nMode);
public slots:
    virtual void addValues(const int nChannelId, qreal values);
    virtual void paramChanged();
    virtual void clearChart();
    void OffSetChanged(double offset);
    void ScaleChanged(double scale);
    void onChannelSwitch(bool bChecked);
    void autoFullScreen();
protected slots:
    void saveChannelData();
    void channelFullScreen(const int nChannelId);
protected:
    virtual void initInNewThread();//
    void saveParams(QString fileName);
    void readParams(QString fileName);
    void AutoAdjust();
    void refresh(const int id);
    void switchChannel(const int nChannelId,bool bOff);
private:
private:

    QLineEdit * m_pCurrentFrequency;
    QLineEdit * m_pThreshold;//
    int m_nFrequency;
    //log file
    QFile * m_pFileLog;
    QVector<qreal> m_combineValues[channel_cnt];
    QVector<QPointF> m_combinePoints[channel_cnt];
    QLineSeries * m_combineSeries[channel_cnt];
    int m_ncombineCnt[channel_cnt];
    float m_fOffset[channel_cnt];
    float m_fScale[channel_cnt];
    float m_fOffsetChange;
    float m_fScaleChange;
    bool m_bShow[channel_cnt];
    MyDoubleSpinBox * m_pSpinOff[channel_cnt];
    MyDoubleSpinBox * m_pSpinScale[channel_cnt];
    QPushButton * m_pOffBtn[channel_cnt];
    float m_fThresholdScale;
};

#endif // INNFOSCHARTWIDGET_H
