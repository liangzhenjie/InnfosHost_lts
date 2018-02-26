#ifndef MOTORDATA_H
#define MOTORDATA_H

#include <QObject>
#include "paramwidget.h"
#include <QVector>
#include <QMap>

class MotorData : public QObject
{
    Q_OBJECT
public:
    explicit MotorData(int nModeId,QObject *parent = 0);
    virtual void setRegulation(int nId,const qreal value)=0;
    virtual qreal getRegulation(int nId,bool * bOK=nullptr)=0;
    virtual void setParam(int nId,const qreal value)=0;
    virtual qreal getParam(int nId,bool * bOK=nullptr)=0;
    virtual void setActual(int nId,const qreal value)=0;
    virtual qreal getActual(int nId,bool * bOK=nullptr)=0;
    int GetModeId()const{
        return m_nModeIdx;
    }
signals:

public slots:
private:
    int m_nModeIdx;
};

class NormalModeData : public MotorData
{
    Q_OBJECT
public:
    explicit NormalModeData(const int nRegulationCnt,const int nParamCnt,const int nActualCnt,const int nModeIdx,QObject *parent = 0);
    virtual ~NormalModeData() {}
    virtual void setRegulation(int nId,const qreal value);
    virtual qreal getRegulation(int nId,bool * bOK=nullptr);
    virtual void setParam(int nId,const qreal value);
    virtual qreal getParam(int nId,bool * bOK=nullptr);
    virtual void setActual(int nId,const qreal value);
    virtual qreal getActual(int nId,bool * bOK=nullptr);
private:
    QVector<qreal> m_regulation;
    QVector<qreal> m_param;
    QVector<qreal> m_actural;
    QVector<qreal> m_acturalCurrent;//current value,use to calculate average value
};

//data manager
class DataMgr : public QObject
{
    Q_OBJECT
public:
    DataMgr(const int nCurIdx,QObject *parent = 0);
    virtual ~DataMgr() {}

    qreal getRegulationValue(const int nId,bool * bOK=nullptr);
    void setRegulationValue(const int nId,qreal value);
    qreal getRegulationValue(const int nModeId,const int nId,bool * bOK=nullptr);
    void setRegulationValue(const int nModeId,const int nId,qreal value);

    qreal getParamValue(const int nId,bool * bOK=nullptr);
    void setParamValue(const int nId,qreal value);
    qreal getParamValue(const int nModeId,const int nId,bool * bOK=nullptr);
    void setParamValue(const int nModeId,const int nId,qreal value);

    qreal getActualValue(const int nId,bool * bOK=nullptr);
    void setActualValue(const int nId,qreal value);
    qreal getActualValue(const int nModeId,const int nId,bool * bOK=nullptr);
    void setActualValue(const int nModeId,const int nId,qreal value);

    void changeCurIdx(const int nCurIdx);
protected:
    MotorData * getData(const int nIdx);
private:
    QVector<MotorData *> m_vDatas;
    int m_nCurIdx;
};



#endif // MOTORDATA_H
