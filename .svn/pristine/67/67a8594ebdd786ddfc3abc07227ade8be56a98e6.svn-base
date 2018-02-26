#include "motordata.h"
#include "paramwidget.h"
#include "motorform.h"

const int nTotalCount = 10;
MotorData::MotorData(const int nModeId,QObject *parent) :
    QObject(parent),
    m_nModeIdx(nModeId)
{

}


NormalModeData::NormalModeData(const int nRegulationCnt, const int nParamCnt, const int nActualCnt, const int nModeIdx, QObject *parent):
    MotorData(nModeIdx,parent)
{
    for (int i=0;i<nRegulationCnt;++i)
        m_regulation.push_back(0);
    for (int i=0;i<nParamCnt;++i)
        m_param.push_back(0);
    for (int i=0;i<nActualCnt;++i)
        m_actural.push_back(0);
}

void NormalModeData::setRegulation(int nId, const qreal value)
{
    if (nId>=0 && nId < m_regulation.size())
        m_regulation[nId] = value;
}

qreal NormalModeData::getRegulation(int nId, bool *bOK)
{
    if (nId>=0 && nId < m_regulation.size())
    {
        if(bOK)
          *bOK = true;
        return m_regulation.at(nId);
    }

    if(bOK)
        *bOK = false;
    return -1;
}

void NormalModeData::setParam(int nId, const qreal value)
{
    if (nId>=0 && nId < m_param.size())
        m_param[nId] = value;
}

qreal NormalModeData::getParam(int nId, bool *bOK)
{
    if (nId>=0 && nId < m_param.size())
    {
        if(bOK)
          *bOK = true;
        return m_param.at(nId);
    }

    if(bOK)
        *bOK = false;
    return -1;
}

void NormalModeData::setActual(int nId, const qreal value)
{
    if (nId>=0 && nId < m_actural.size())
    {
        if(nId == 0)
        {
            m_acturalCurrent.push_back(value);
            if (m_acturalCurrent.size() >= nTotalCount)
            {
                qreal total = 0;
                foreach (qreal perValue, m_acturalCurrent) {
                    total += perValue;
                }
                m_actural[nId] = total/nTotalCount;
                m_acturalCurrent.clear();
            }
        }
        else
        {
            m_actural[nId] = value;
        }

    }

}

qreal NormalModeData::getActual(int nId, bool *bOK)
{
    if (nId>=0 && nId < m_actural.size())
    {
        if(bOK)
          *bOK = true;
        return m_actural.at(nId);
    }

    if(bOK)
        *bOK = false;
    return -1;
}

//data manager
DataMgr::DataMgr(const int nCurIdx, QObject *parent):
    QObject(parent),
    m_nCurIdx(nCurIdx)
{

}

qreal DataMgr::getRegulationValue(const int nId, bool *bOK)
{
    return getRegulationValue(m_nCurIdx,nId,bOK);
}

void DataMgr::setRegulationValue(const int nId, qreal value)
{
    setRegulationValue(m_nCurIdx,nId,value);
}

qreal DataMgr::getRegulationValue(const int nModeId, const int nId, bool *bOK)
{
    MotorData * pData = getData(nModeId);
    if(pData)
        return pData->getRegulation(nId,bOK);
    return -1;
}

void DataMgr::setActualValue(const int nModeId, const int nId, qreal value)
{
    MotorData * pData = getData(nModeId);
    if(pData)
        pData->setActual(nId,value);
}

qreal DataMgr::getActualValue(const int nId, bool *bOK)
{
    return getActualValue(m_nCurIdx,nId,bOK);
}

void DataMgr::setActualValue(const int nId, qreal value)
{
    setActualValue(m_nCurIdx,nId,value);
}

qreal DataMgr::getActualValue(const int nModeId, const int nId, bool *bOK)
{
    MotorData * pData = getData(nModeId);
    if(pData)
        return pData->getActual(nId,bOK);
    return -1;
}

void DataMgr::setParamValue(const int nModeId, const int nId, qreal value)
{
    MotorData * pData = getData(nModeId);
    if(pData)
        pData->setParam(nId,value);
}

qreal DataMgr::getParamValue(const int nId, bool *bOK)
{
    return getParamValue(m_nCurIdx,nId,bOK);
}

void DataMgr::setParamValue(const int nId, qreal value)
{
    setParamValue(m_nCurIdx,nId,value);
}

qreal DataMgr::getParamValue(const int nModeId, const int nId, bool *bOK)
{
    MotorData * pData = getData(nModeId);
    if(pData)
        return pData->getParam(nId,bOK);
    return -1;
}

void DataMgr::setRegulationValue(const int nModeId, const int nId, qreal value)
{
    MotorData * pData = getData(nModeId);
    if(pData)
        pData->setRegulation(nId,value);
}

void DataMgr::changeCurIdx(const int nCurIdx)
{
    m_nCurIdx = nCurIdx;
}

MotorData *DataMgr::getData(const int nIdx)
{
    MotorData * pData = nullptr;
    foreach (MotorData *tmp, m_vDatas) {
        if(tmp->GetModeId() == nIdx)
            return tmp;
    }
    return pData;
}
