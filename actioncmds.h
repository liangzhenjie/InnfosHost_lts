#ifndef ACTIONCMDS_H
#define ACTIONCMDS_H

#include <QUndoCommand>

class KeyItem;
class AddCmds : public QUndoCommand
{
public:
    AddCmds(KeyItem * pItem,quint16 nTime,qreal value,QUndoCommand * pParent=nullptr);
    void undo();
    void redo();
private:
    KeyItem * m_pItem;
    quint16 m_nKeyId;
    quint16 m_nTime;
    qreal m_value;
};

class RemoveCmds : public QUndoCommand
{
public:
    RemoveCmds(KeyItem * pItem,quint16 keyId,QUndoCommand * pParent=nullptr);
    void undo();
    void redo();
private:
    KeyItem * m_pItem;
    quint16 m_nSelKeyId;
    quint16 m_nTime;
    qreal m_value;
};

class KeyMoveCmds : public QUndoCommand
{
public:
    KeyMoveCmds(KeyItem * pItem,quint16 keyId,quint16 oldTime,quint16 newTime,QUndoCommand * pParent=nullptr);
    void undo();
    void redo();
private:
    KeyItem * m_pItem;
    quint16 m_nSelKeyId;
    quint16 m_oldTime;
    quint16 m_newTime;
};

class KeyEditCmds : public QUndoCommand
{
public:
    KeyEditCmds(KeyItem * pItem,quint16 keyId,qreal oldValue,qreal newValue,QUndoCommand * pParent=nullptr);
    void undo();
    void redo();
private:
    KeyItem * m_pItem;
    quint16 m_nSelKeyId;
    qreal m_oldValue;
    qreal m_newValue;
};

#endif // ACTIONCMDS_H
