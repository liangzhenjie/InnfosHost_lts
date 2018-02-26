#include "actioncmds.h"
#include "actionitem.h"

AddCmds::AddCmds(KeyItem *pItem, quint16 nTime,qreal value,QUndoCommand *pParent):
    QUndoCommand(pParent),
    m_pItem(pItem),
    m_nKeyId(0),
    m_nTime(nTime),
    m_value(value)
{

}

void AddCmds::undo()
{
    m_pItem->removeKey(m_nKeyId);
}

void AddCmds::redo()
{
    m_nKeyId = m_pItem->addKey(m_nTime,m_value);
}

RemoveCmds::RemoveCmds(KeyItem *pItem, quint16 keyId, QUndoCommand *pParent):
    QUndoCommand(pParent),
    m_pItem(pItem),
    m_nSelKeyId(keyId),
    m_nTime(pItem->keyTime(keyId)),
    m_value(pItem->keyValue(keyId))
{

}

void RemoveCmds::undo()
{
    m_nSelKeyId = m_pItem->addKey(m_nTime,m_value);
}

void RemoveCmds::redo()
{
    m_pItem->removeKey(m_nSelKeyId);
}

KeyMoveCmds::KeyMoveCmds(KeyItem *pItem, quint16 keyId, quint16 oldTime, quint16 newTime, QUndoCommand *pParent):
    QUndoCommand(pParent),
    m_pItem(pItem),
    m_nSelKeyId(keyId),
    m_oldTime(oldTime),
    m_newTime(newTime)
{

}

void KeyMoveCmds::undo()
{
    m_pItem->setKeyTime(m_nSelKeyId,m_oldTime);
}

void KeyMoveCmds::redo()
{
    m_pItem->setKeyTime(m_nSelKeyId,m_newTime);
}

KeyEditCmds::KeyEditCmds(KeyItem *pItem, quint16 keyId, qreal oldValue, qreal newValue, QUndoCommand *pParent):
    QUndoCommand(pParent),
    m_pItem(pItem),
    m_nSelKeyId(keyId),
    m_oldValue(oldValue),
    m_newValue(newValue)
{

}

void KeyEditCmds::undo()
{
    m_pItem->setKeyValue(m_nSelKeyId,m_oldValue);
}

void KeyEditCmds::redo()
{
    m_pItem->setKeyValue(m_nSelKeyId,m_newValue);
}
