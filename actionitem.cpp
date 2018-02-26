#include "actionitem.h"
#include "motorform.h"
#include <QtMath>
#include <QDebug>

const int nMaxFrameCnt = 1000;


ActionItem::ActionItem(const quint32 nMac):
    QStandardItem(),
    m_nMac(nMac),
    m_bChekced(true)
{
    setCheckable(true);
    setCheckState(Qt::Checked);
    QFont font("微软雅黑",10);
    setFont(font);
    setData(QColor(255,255,255),Qt::TextColorRole);
}

void ActionItem::setData(const QVariant &value, int role)
{
    switch (role) {
    case Qt::CheckStateRole:
        m_bChekced = (value.toInt()) == Qt::Checked;
        if(this->model())
            model()->itemChanged(this);
        break;
    default:
        QStandardItem::setData(value,role);
        break;
    }
//    if(role==Qt::DisplayRole && value.type()==QVariant::UInt)
//    {
//        m_nMac = value.toUInt();
//        if(this->model())
//            this->model()->itemChanged(this);
//    }
}

QVariant ActionItem::data(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return QString("ID: %1").arg(m_nMac);
        break;
    case Qt::CheckStateRole:
        return m_bChekced?Qt::Checked:Qt::Unchecked;
    default:
        return QStandardItem::data(role);
        break;
    }

    return QVariant();
}

quint32 ActionItem::mac() const
{
    return m_nMac;
}

bool ActionItem::isChecked() const
{
    return m_bChekced;
}


KeyItem::KeyItem():
    QStandardItem()
{
    setCheckable(true);
    setCheckState(Qt::Checked);
}

void KeyItem::setData(const QVariant &value, int role)
{
    switch (role) {
    case Qt::UserRole:
        break;
    default:
        QStandardItem::setData(value,role);
        break;
    }
}

QVariant KeyItem::data(int role) const
{
    switch (role) {
    case Qt::UserRole:
        return QVariant::fromValue(keyFrames);
        break;
    default:
        return QStandardItem::data(role);
        break;
    }
    return QVariant();
}

quint16 KeyItem::addKey(quint16 time, qreal value)
{
    quint16 id = getAvailableId();
    if(id > 0)
    {
        KeyStruct key;
        key.time = time;
        key.value = value;
        key.m_nStatus = Key_Normal;
        keyFrames.insert(id,key);
        if(this->model())
            model()->itemChanged(this);
        //model()->submit();
    }
    return id;
}

void KeyItem::removeKey(quint16 id)
{
    keyFrames.remove(id);
    if(this->model())
        model()->itemChanged(this);
    //model()->submit();
}

void KeyItem::setKeyTime(quint16 id, quint16 time)
{
    KeyMap::iterator it = keyFrames.find(id);
    if(it != keyFrames.end())
    {
        it.value().time = time;
        if(this->model())
            model()->itemChanged(this);
    }
}

void KeyItem::setKeyValue(quint16 id, qreal value)
{
    KeyMap::iterator it = keyFrames.find(id);
    if(it != keyFrames.end())
    {
        it.value().value = value;
        if(this->model())
            model()->itemChanged(this);
    }
}

void KeyItem::selectKey(quint16 id)
{
    unselectKey();
    KeyMap::iterator it = keyFrames.find(id);
    if(it != keyFrames.end())
    {
        it.value().m_nStatus = Key_Select;
        if(this->model())
            model()->itemChanged(this);
    }
}

void KeyItem::setKeyStatus(quint16 id, quint16 nStatus)
{
    KeyMap::iterator it = keyFrames.find(id);
    if(it != keyFrames.end())
    {
        if(nStatus <= Key_Hover && nStatus != it.value().m_nStatus)
        {
            it.value().m_nStatus = nStatus;
            if(this->model())
                model()->itemChanged(this);
        }
    }
}

void KeyItem::unselectKey()
{
    KeyMap::iterator it = keyFrames.find(selectId());
    if(it != keyFrames.end())
    {
        it.value().m_nStatus = Key_Normal;
        if(this->model())
            model()->itemChanged(this);
    }
}

quint16 KeyItem::selectId() const
{
    QMapIterator<quint16,KeyStruct> it(keyFrames);
    while (it.hasNext()) {
        it.next();
        if(it.value().m_nStatus == Key_Select)
            return it.key();
    }
    return 0;
}

quint16 KeyItem::keyTime(quint16 id)
{
    KeyMap::iterator it = keyFrames.find(id);
    if(it != keyFrames.end())
    {
        return it.value().time;
    }
    return 0;
}

qreal KeyItem::keyValue(quint16 id)
{
    KeyMap::iterator it = keyFrames.find(id);
    if(it != keyFrames.end())
    {
        return it.value().value;
    }
    return 0;
}

quint16 KeyItem::getIdByTime(const quint16 time) const
{
    QMapIterator<quint16,KeyStruct> it(keyFrames);
    while (it.hasNext()) {
        it.next();
        if(it.value().time == time)
            return it.key();
    }
    return 0;
}

qreal KeyItem::getValueByTime(const quint16 time)
{
    sortKeys();
    if(m_sortTimes.size() == 0)
        return 0;

    int nIdx = -1;
    for(int i=0;i<m_sortTimes.size();++i)
    {
        if(time <= m_sortTimes.at(i))
        {
            nIdx = i;
            break;
        }
    }

    switch (nIdx) {
    case 0:
    {
        qreal ratio = time*1.0/m_sortTimes.at(0);
        qreal sin = (qSin(ratio*M_PI-M_PI_2)+1)/2;
        return m_sortValues.at(0)*ratio;
    }
        break;
    case -1:
        return m_sortValues.at(m_sortValues.size()-1);
        break;
    default:
    {
        qreal ratio = (time-m_sortTimes.at(nIdx-1))/(qreal)(m_sortTimes.at(nIdx)-m_sortTimes.at(nIdx-1));

        return (m_sortValues.at(nIdx)-m_sortValues.at(nIdx-1))*ratio+m_sortValues.at(nIdx-1);
    }
        break;
    }

    return 0;
}

void KeyItem::sortKeys()
{
    m_sortTimes.clear();
    m_sortValues.clear();

    QMapIterator<quint16,KeyStruct> it(keyFrames);
    while (it.hasNext()) {
        it.next();
        quint16 time = it.value().time;
        qreal value = it.value().value;
        if(m_sortTimes.size() == 0)
        {
            m_sortTimes.push_back(time);
            m_sortValues.push_back(value);
            continue;
        }
        for(int i=0;i<m_sortTimes.size();++i)
        {
            if(time <= m_sortTimes.at(i))
            {
                m_sortTimes.insert(i,time);
                m_sortValues.insert(i,value);
                break;
            }

            if(i == m_sortTimes.size()-1)
            {
                m_sortTimes.push_back(time);
                m_sortValues.push_back(value);
            }
        }
    }
}

int KeyItem::keyPosStatus(const quint16 time)
{
    int nStatus = Key_None;
    QMapIterator<quint16,KeyStruct> it(keyFrames);
    while (it.hasNext()) {
        it.next();
        if(it.value().time == time)
            nStatus |= Cur_Select;
        if(it.value().time < time)
            nStatus |= Has_Left;
        if(it.value().time > time)
            nStatus |= Has_Right;
    }
    return nStatus;
}

QList<quint16> KeyItem::getAllKeysId() const
{
    return keyFrames.keys();
}

qint32 KeyItem::getKeyIdDuring(const quint16 previous, const quint16 current)
{
    QMapIterator<quint16,KeyStruct> it(keyFrames);
    while (it.hasNext()) {
        it.next();
        if(it.value().time>=previous && it.value().time<=current)
            return it.key();
    }
    return -1;
}

quint16 KeyItem::getAvailableId() const
{
    QList<quint16> keys = keyFrames.keys();
    for(quint16 i=1;i<nMaxFrameCnt;++i)
    {
        if(!keys.contains(i))
            return i;
    }
    return 0;
}



ModeItem::ModeItem(int modeId):
    QStandardItem(),
    m_nModeId(modeId),
    m_nKeyStatus(Key_None),
    m_value(0)
{
    setData(QColor(255,255,255),Qt::TextColorRole);
}

void ModeItem::setData(const QVariant &value, int role)
{
    switch (role) {
    case Qt::CheckStateRole:
        m_bChecked = (value.toInt()) == Qt::Checked;
        if(this->model())
            model()->itemChanged(this);
        break;
    case Qt::EditRole:
        m_value = value.toDouble();
        if(this->model())
            model()->itemChanged(this);
        break;
    default:
        QStandardItem::setData(value,role);
        break;
    }
//    if(role==Qt::DisplayRole && value.type()==QVariant::UInt)
//    {
//        m_nMac = value.toUInt();
//        if(this->model())
//            this->model()->itemChanged(this);
//    }
}

QVariant ModeItem::data(int role) const
{
    switch (role) {
    case Qt::CheckStateRole:
        return m_bChecked?Qt::Checked:Qt::Unchecked;
        break;
    case Qt::EditRole:
        return m_value;
        break;
    case KeyRole:
        return m_nKeyStatus;
        break;
    case ModeRole:
        return ModeName();
        break;
    default:
        return QStandardItem::data(role);
        break;
    }

    return QVariant();
}

void ModeItem::setKeyStatus(int nStatus)
{
    if(m_nKeyStatus != nStatus)
    {
        m_nKeyStatus = nStatus;
        if(model())
            model()->itemChanged(this);
    }

}

QVariant ModeItem::ModeName() const
{
    switch (m_nModeId) {
    case MotorForm::Mode_Cur:
        return QVariant("Current:");
        break;
    case MotorForm::Mode_Vel:
    case MotorForm::Mode_Profile_Vel:
        return QVariant("Velocity:");
        break;
    case MotorForm::Mode_Profile_Pos:
    case MotorForm::Mode_Pos:
        return QVariant("Position:");
        break;
    default:
        break;
    }
    return QVariant();
}
