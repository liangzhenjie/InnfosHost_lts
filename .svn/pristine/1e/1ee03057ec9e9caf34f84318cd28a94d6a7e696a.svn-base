#ifndef ACTIONITEM_H
#define ACTIONITEM_H

#include <QStandardItem>

/*
 * the motor action item, include three mode items
*/
class ActionItem : public QStandardItem
{
public:
    enum{
        ActionType = UserType+1
    };
    explicit ActionItem(const quint32 nMac);
    virtual int type()const{
        return ActionType;
    }
    virtual void setData(const QVariant &value, int role);
    virtual QVariant data(int role) const;
    quint32 mac()const;
    bool isChecked()const;
signals:

public slots:
private:
    quint32 m_nMac;
    bool m_bChekced;
};

struct KeyStruct{
    quint16 time;//time is in millisecond
    qreal value;//
    qint16 m_nStatus;
};
Q_DECLARE_METATYPE(KeyStruct)

enum KeyStatus{
    Key_None = 0x00,
    Has_Left=0x01,
    Has_Right=0x02,
    Cur_Select=0x04
};

/*
 * the key item which include all key points in a certain mode
*/
class KeyItem : public QStandardItem
{
public:
    typedef QMap<quint16,KeyStruct> KeyMap;
    enum{
        KeyType = UserType+2
    };
    enum{
        Key_Normal,
        Key_Select,
        Key_Hover
    };
    explicit KeyItem();
    virtual int type()const{
        return KeyType;
    }
    virtual void setData(const QVariant &value, int role);
    virtual QVariant data(int role) const;
    quint16 addKey(quint16 time,qreal value);
    void removeKey(quint16 id);
    void setKeyTime(quint16 id,quint16 time);
    void setKeyValue(quint16 id,qreal value);
    void selectKey(quint16 id);
    void setKeyStatus(quint16 id, quint16 nStatus);
    void unselectKey();
    quint16 selectId()const;
    quint16 keyTime(quint16 id);
    qreal keyValue(quint16 id);
    quint16 getIdByTime(const quint16 time)const;
    qreal getValueByTime(const quint16 time);
    void sortKeys();
    int keyPosStatus(const quint16 time);
    QList<quint16> getAllKeysId()const;
    qint32 getKeyIdDuring(const quint16 previous,const quint16 current);
private:
    quint16 getAvailableId()const;
    QVector<qreal> m_sortValues;
    QVector<quint16> m_sortTimes;


signals:

public slots:
private:
    KeyMap keyFrames;
};

/*
 * the mode item,there has mode,checked,unchecked info,if it is unchecked,this mode action would not affect
*/
class ModeItem : public QStandardItem
{
public:
    typedef QMap<quint16,KeyStruct> KeyMap;
    enum{
        ModeType = UserType+3
    };

    enum ModeData{
        KeyRole = Qt::UserRole+1,
        ModeRole = Qt::UserRole+2,
    };
    explicit ModeItem(int modeId);
    virtual int type()const{
        return ModeType;
    }
    virtual void setData(const QVariant &value, int role);
    virtual QVariant data(int role) const;
    void setKeyStatus(int nStatus);
    int modeId()const{
        return m_nModeId;
    }
    bool isChecked()const{
        return m_bChecked;
    }
private:
    QVariant ModeName()const;
signals:

public slots:
private:
    int m_nModeId;
    int m_nKeyStatus;
    bool m_bChecked;
    qreal m_value;
};


#endif // ACTIONITEM_H
