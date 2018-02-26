#include "actiondelegate.h"
#include <QApplication>
#include <QLineEdit>
#include "actionitem.h"
#include <QStyleOptionButton>
#include <QCheckBox>
#include <QDebug>
#include <QMouseEvent>
#include <QTreeView>
#include <QPainter>
#include <QValidator>
#include "inertiamove.h"

const int CUR_VALUE_COLUMN = 1;


ActionDelegate::ActionDelegate(quint16 nMaxTime, QTreeView *pView, QObject *parent):
    QStyledItemDelegate(parent),
    m_pKeyBtn(new QPushButton()),
    m_pLeftArrow(new QPushButton()),
    m_pRightArrow(new QPushButton()),
    m_pStatusBtn(new QPushButton()),
    m_pView(pView),
    m_nPressKeyId(0),
    m_bMousePressed(false),
    m_nMaxTime(nMaxTime),
    m_nMinTime(0),
    m_pMove(nullptr)
{
    m_pKeyBtn.data()->setStyleSheet("QPushButton {border: none; background-color: transparent; image:url(:/images/keyframe_unable.png);} \
                                    QPushButton:hover {image:url(:/images/left_arrow.png);} \
                                    QPushButton:pressed,QPushButton:checked{image:url(:/images/keyframe_enable.png);}");

    m_pLeftArrow.data()->setStyleSheet("QPushButton {border: none; background-color: transparent; image:url(:/images/left_arrow.png);}");
    m_pRightArrow.data()->setStyleSheet("QPushButton {border: none; background-color: transparent; image:url(:/images/right_arrow.png);}");

    m_pMove = new InertiaMove(this);
    connect(m_pMove,&InertiaMove::movePos,this,&ActionDelegate::movePos);
}

ActionDelegate::~ActionDelegate()
{

}

QWidget *ActionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.parent()!=QModelIndex() && index.column()==CUR_VALUE_COLUMN)
    {
        QLineEdit * pEdit = new QLineEdit(parent);
        pEdit->setValidator(new QDoubleValidator());
        return pEdit;
    }
    return nullptr;
}

void ActionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QRegExp rx;
    rx.setPattern("(\\.){0,1}0+$");
    qreal value = index.model()->data(index,Qt::EditRole).toDouble();
    QLineEdit * pEdit = qobject_cast<QLineEdit*>(editor);
    if(pEdit)
    {
        pEdit->setText(QString::number(value,'f').replace(rx,""));
    }
}

void ActionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QStandardItemModel * pModel = qobject_cast<QStandardItemModel *>(model);
    if(pModel)
    {
        QStandardItem * pItem = pModel->itemFromIndex(index);
        if(index.column() == CUR_VALUE_COLUMN)
        {
            QLineEdit * pEdit = qobject_cast<QLineEdit*>(editor);
            qreal value = pEdit->text().toDouble();
            pItem->setData(value,Qt::EditRole);
            emit valueChange(value,index);
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor,model,index);
    }
}

void ActionDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor,option,index);
}

void ActionDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(/*index.parent() != QModelIndex()*/true)
    {
        QStyleOptionViewItem viewOption(option);
        initStyleOption(&viewOption,index);
        if(viewOption.state.testFlag(QStyle::State_HasFocus))
            viewOption.state = viewOption.state ^ QStyle::State_HasFocus;
        const QStandardItemModel * pModel = qobject_cast<const QStandardItemModel *>(index.model());
        //qDebug() <<index;
        if(pModel)
        {
            QStandardItem * pItem = pModel->itemFromIndex(index);
            switch (pItem->type()) {
            case KeyItem::KeyType:
                paintKeysItem(painter,option,index);
                break;
            case ModeItem::ModeType:
                paintModeItem(painter,option,index);
                break;
            default:
                QStyledItemDelegate::paint(painter,option,index);
                break;
            }

        }

    }
    else
    {
        QStyledItemDelegate::paint(painter,option,index);
    }

}

bool ActionDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent * pEvt = static_cast<QMouseEvent*>(event);
    if(pEvt)
    {
        QStandardItemModel * pModel = qobject_cast<QStandardItemModel *>(model);
        if(!pModel)
            return false;
        int itemType = pModel->itemFromIndex(index)->type();
        switch (pEvt->type()) {
        case QEvent::MouseButtonPress:
        {
            m_bMousePressed = true;
            if(itemType == ModeItem::ModeType)
            {
                QRect rc(option.rect.x()+33,option.rect.y()+(option.rect.height()-36)/2,36,36);
                if(rc.contains(pEvt->pos()))
                {
                    int checked = index.data(Qt::CheckStateRole).toInt();
                    model->setData(index,checked==Qt::Checked?Qt::Unchecked:Qt::Checked,Qt::CheckStateRole);
                    return true;
                }
            }
            else if(itemType == KeyItem::KeyType)
            {
                m_pMove->stop();
                KeyItem::KeyMap frames = index.data(Qt::UserRole).value<KeyItem::KeyMap>();
                KeyItem* pItem = static_cast<KeyItem *>(pModel->itemFromIndex(index));
                QMapIterator<quint16,KeyStruct>it(frames);
                while(it.hasNext())
                {
                    it.next();
                    qreal ratio = (it.value().time-m_nMinTime)/(qreal)(m_nMaxTime-m_nMinTime);
                    QSize s(10,10);
                    QRect rect = QRect(option.rect.x()+option.rect.width()*ratio-s.width()/2,option.rect.y()+(option.rect.height()-s.height())/2,s.width(),s.height());

                    if(rect.contains(pEvt->pos()))
                    {
                        m_nPressKeyId = it.key();
                        pItem->selectKey(m_nPressKeyId);
                        emit keyPress(pItem,m_nPressKeyId);
                        pModel->itemFromIndex(index.sibling(index.row(),index.column()-1))->setData(it.value().value,Qt::EditRole);//update edit data
                        return true;
                    }

                }
                m_pMove->eventFilter(this,event);
                return true;
            }

        }
            break;
        case QEvent::MouseMove:
        {
            if(itemType == KeyItem::KeyType)
            {
                if(m_nPressKeyId > 0)//drag
                {
                    QRect rc = option.rect;
                    KeyItem * pItem = static_cast<KeyItem *>(pModel->itemFromIndex(index));
                    int time = (pEvt->pos().x()-rc.x())*1.0/rc.width()*(m_nMaxTime-m_nMinTime)+m_nMinTime;
                    if(time >= 0 && time <= m_nMaxTime)
                    {
                        pItem->setKeyTime(m_nPressKeyId,time);

                    }
                }
                else if(!m_bMousePressed)//hover
                {
                    KeyItem::KeyMap frames = index.data(Qt::UserRole).value<KeyItem::KeyMap>();
                    KeyItem* pItem = static_cast<KeyItem *>(pModel->itemFromIndex(index));
                    QMapIterator<quint16,KeyStruct>it(frames);
                    while(it.hasNext())
                    {
                        it.next();
                        qreal ratio = (it.value().time-m_nMinTime)/(qreal)(m_nMaxTime-m_nMinTime);
                        QSize s(10,10);
                        QRect rect = QRect(option.rect.x()+option.rect.width()*ratio-s.width()/2,option.rect.y()+(option.rect.height()-s.height())/2,s.width(),s.height());

                        if(rect.contains(pEvt->pos()))
                        {
                            if(it.value().m_nStatus==KeyItem::Key_Normal)
                                pItem->setKeyStatus(it.key(),KeyItem::Key_Hover);
                        }
                        else if(it.value().m_nStatus == KeyItem::Key_Hover)
                        {
                            pItem->setKeyStatus(it.key(),KeyItem::Key_Normal);
                        }
                    }
                }
                else
                {
                    m_pMove->eventFilter(this,event);
                }

                return true;
            }
        }
            break;
        case QEvent::MouseButtonRelease:
        {
            m_bMousePressed = false;
            if(m_nPressKeyId == 0)
            {
                if(itemType == KeyItem::KeyType)
                {
                    KeyItem* pItem = static_cast<KeyItem *>(pModel->itemFromIndex(index));
                    pItem->unselectKey();
                }
                m_pMove->eventFilter(this,event);
            }
            else
            {
                if(itemType == KeyItem::KeyType)
                {
                    KeyItem* pItem = static_cast<KeyItem *>(pModel->itemFromIndex(index));
                    emit keyMoved(pItem,m_nPressKeyId);
                }
            }

            m_nPressKeyId = 0;
        }
        default:
            break;
        }



        if(pEvt->type() == QEvent::Wheel)
        {
            m_pView->setColumnWidth(0,m_pView->columnWidth(0)+10);
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize ActionDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 0)
        return QSize(350,20);
    return QStyledItemDelegate::sizeHint(option,index);
}

void ActionDelegate::changeTime(quint16 minTime,quint16 maxTime)
{
    if(maxTime > minTime && m_nMaxTime != maxTime)
    {
        m_nMaxTime = maxTime;
        m_nMinTime = minTime;
    }

}

void ActionDelegate::paintKeysItem(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    KeyItem::KeyMap frames = index.data(Qt::UserRole).value<KeyItem::KeyMap>();
    QMapIterator<quint16,KeyStruct>it(frames);
    while(it.hasNext())
    {
        it.next();
        qreal ratio = (it.value().time-m_nMinTime)/(qreal)(m_nMaxTime-m_nMinTime);
        QSize s(10,10);
        QStyleOptionButton button;
        switch (it.value().m_nStatus) {
        case KeyItem::Key_Select:
            button.state |= QStyle::State_Sunken;
            break;
        case KeyItem::Key_Hover:
            button.state |= QStyle::State_MouseOver;
            break;
        default:
            break;
        }
        button.rect = QRect(option.rect.x()+option.rect.width()*ratio-s.width()/2,option.rect.y()+(option.rect.height()-s.height())/2,s.width(),s.height());
        if(option.rect.contains(button.rect,true))
            QApplication::style()->drawControl(QStyle::CE_PushButton,&button,painter,m_pKeyBtn.data());
    }

}

void ActionDelegate::paintModeItem(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int nKeyStatus = index.data(ModeItem::KeyRole).toInt();

    QSize arrowSize(4,6);
    QSize dotSize(6,6);

    if(nKeyStatus != Key_None)//
    {

        if(nKeyStatus & Has_Left)
        {
            QStyleOptionButton button;
            button.rect = QRect(option.rect.x(),option.rect.y()+(option.rect.height()-arrowSize.height())/2,arrowSize.width(),arrowSize.height());
            QApplication::style()->drawControl(QStyle::CE_PushButton,&button,painter,m_pLeftArrow.data());
        }

        if(nKeyStatus & Cur_Select)
        {
            QStyleOptionButton button;
            button.rect = QRect(option.rect.x()+arrowSize.width(),option.rect.y()+(option.rect.height()-dotSize.height())/2,dotSize.width(),dotSize.height());
            QApplication::style()->drawControl(QStyle::CE_PushButton,&button,painter,m_pKeyBtn.data());
        }

        if(nKeyStatus & Has_Right)
        {
            QStyleOptionButton button;
            button.rect = QRect(option.rect.x()+arrowSize.width()+dotSize.width(),option.rect.y()+(option.rect.height()-arrowSize.height())/2,arrowSize.width(),arrowSize.height());
            QApplication::style()->drawControl(QStyle::CE_PushButton,&button,painter,m_pRightArrow.data());
        }
    }

    QStyleOptionButton checkBoxStyle;
    checkBoxStyle.state = index.data(Qt::CheckStateRole)==Qt::Checked?QStyle::State_On:QStyle::State_Off;
    checkBoxStyle.state |= QStyle::State_Enabled;
    checkBoxStyle.iconSize = QSize(15,15);
    checkBoxStyle.rect = QRect(option.rect.x()+33,option.rect.y()+(option.rect.height()-15)/2,15,15);
    QCheckBox checkbox;
    QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox,&checkBoxStyle,painter,&checkbox);

    painter->save();
    painter->setFont(QFont("微软雅黑",8,QFont::Bold));
    QPalette p;
    p.setColor(QPalette::WindowText,QColor(92,96,106));
    QApplication::style()->drawItemText(painter,option.rect,Qt::AlignRight|Qt::AlignVCenter,p,false,
                                        index.data(ModeItem::ModeRole).toString(),QPalette::WindowText);
    painter->restore();
}
