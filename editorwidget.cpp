#include "editorwidget.h"
#include "timeruler.h"
#include "actiondelegate.h"
#include "actionitem.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QStandardItemModel>
#include "actiondelegate.h"
#include "motorform.h"
#include <QTimeLine>
#include <QDebug>
#include "mediator.h"
#include "actiontree.h"
#include <QWheelEvent>
#include <QUndoStack>
#include <QAction>
#include "actioncmds.h"
#include <QMenu>
#include <QMenuBar>
#include <QShortcut>
#include <QLineEdit>
#include <QFileDialog>
#include <QXmlStreamWriter>
#include "communication.h"
#include <QFile>
#include <QTextStream>

const int nModeItemCnt = 1;

EditorWidget::EditorWidget(QWidget *parent) :
    QWidget(parent),
    m_pActionTree(nullptr),
    m_pRuler(nullptr),
    m_pPlayLine(nullptr),
    m_pTotalTime(nullptr),
    m_pCurTime(nullptr),
    m_pUndoStack(nullptr),
    m_pKeyItem(nullptr),
    m_nKeyId(0),
    m_nKeyOldTime(0),
    m_pActionDataFile(nullptr),
    m_nPrevious(0)
{
    QVBoxLayout * pVLayout = new QVBoxLayout(this);
    pVLayout->setSpacing(0);
    QHBoxLayout * pBtnLayout = new QHBoxLayout();
    QHBoxLayout * pRulerLayout = new QHBoxLayout();
    pVLayout->addLayout(pBtnLayout);
    pVLayout->addLayout(pRulerLayout);

    m_pActionTree = new ActionTreeView(this);
    m_pActionTree->setHeaderHidden(true);
    m_pActionTree->setMouseTracking(true);
    pVLayout->addWidget(m_pActionTree);
    m_pModel = new QStandardItemModel();
    m_pModel->setColumnCount(3);
    m_pActionTree->setModel(m_pModel);
    m_pActionTree->setColumnWidth(0,150);
    m_pActionTree->setColumnWidth(1,50);
    connect(m_pModel,&QStandardItemModel::itemChanged,[=](QStandardItem *item){
        m_pActionTree->update(item->index());
        switch (item->type()) {
        case KeyItem::KeyType:
            onCurrentTimeChange(m_pRuler->curMillisecond());
            break;
        case ModeItem::ModeType:
        {
            QModelIndex idx = m_pModel->indexFromItem(item);
            if(idx.data(Qt::CheckStateRole) == Qt::Checked)
            {
                for (int i=0;i<nModeItemCnt;++i)
                {
                    if(i != idx.row())
                    {
                        QModelIndex sibling = idx.sibling(i,idx.column());
                        if(sibling.isValid())
                        {
                            m_pModel->itemFromIndex(sibling)->setData(Qt::Unchecked,Qt::CheckStateRole);
                        }

                    }
                }
            }
        }
            break;
        default:
            break;
        }


    });
    installEventFilter(m_pActionTree);

    m_pTotalTime = new QLineEdit(tr("%1").arg(0),this);
    m_pTotalTime->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    m_pTotalTime->setValidator(new QIntValidator(0,1000000));
    pBtnLayout->addWidget(m_pTotalTime);
    pBtnLayout->addSpacerItem(new QSpacerItem(10,10,QSizePolicy::Expanding));

    QPushButton * pReset = creatBtn(":/images/play2.png",":/images/play1.png",QSize(26,15));
    pBtnLayout->addWidget(pReset);
    connect(pReset,&QPushButton::clicked,[=]{m_pPlayLine->setCurrentTime(0);});

    QPushButton * pPlay = creatBtn(":/images/play2.png",":/images/play1.png",QSize(26,15));
    pBtnLayout->addWidget(pPlay);
    pPlay->setCheckable(true);
    connect(pPlay,&QPushButton::clicked,this,&EditorWidget::playBtnCallback);
    QPushButton * pRecircle = creatBtn(":/images/recircle2.png",":/images/recircle1.png",QSize(26,15));
    pBtnLayout->addWidget(pRecircle);
    pRecircle->setCheckable(true);
    QPushButton * pExport = creatBtn(":/images/export2.png",":/images/export1.png",QSize(26,15));
    pBtnLayout->addWidget(pExport);
    connect(pExport,&QPushButton::clicked,this,&EditorWidget::saveAction);
    QPushButton * pImport = creatBtn(":/images/import2.png",":/images/import1.png",QSize(26,15));
    pBtnLayout->addWidget(pImport);
    connect(pImport,&QPushButton::clicked,this,&EditorWidget::readAction);
    pBtnLayout->setSpacing(8);

    m_pCurTime = new QLabel(tr("Duration: 00:00:00"),this);
    m_pCurTime->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    m_pCurTime->setFixedWidth(200);
    pRulerLayout->addWidget(m_pCurTime);

    m_pRuler = new TimeRuler();
    m_pRuler->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    m_pRuler->setGeometry(0,0,500,20);
    m_pRuler->setMaxValue(30);
    m_pTotalTime->setText(tr("%1").arg(30000));

    RulerContainner * pContainner = new RulerContainner(this);
    pContainner->addRuler(m_pRuler);
    pRulerLayout->addWidget(pContainner);
    connect(pContainner,&RulerContainner::rulerGeometryChange,m_pActionTree,static_cast<void(QWidget:: *)()>(&QWidget::update));


    ActionDelegate * pDelegate = new ActionDelegate(m_pRuler->maxMillisecond(),m_pActionTree);
    m_pActionTree->setItemDelegate(pDelegate);
    connect(pDelegate,&ActionDelegate::valueChange,this,&EditorWidget::setValue);
    connect(m_pRuler,&TimeRuler::visualValueChanged,pDelegate,&ActionDelegate::changeTime);
    connect(pDelegate,&ActionDelegate::keyPress,this,&EditorWidget::onKeyPressed);
    connect(pDelegate,&ActionDelegate::keyMoved,this,&EditorWidget::onKeyReleased);
    connect(pDelegate,&ActionDelegate::movePos,pContainner,&RulerContainner::moveRuler);

    m_pPlayLine = new QTimeLine(m_pRuler->maxMillisecond(),this);
    m_pPlayLine->setCurveShape(QTimeLine::LinearCurve);
    connect(m_pPlayLine,&QTimeLine::frameChanged,this,&EditorWidget::calcValueByTime);
    m_pPlayLine->setUpdateInterval(1);
    connect(pRecircle,&QPushButton::clicked,[=](bool bChecked){
        int nCount = bChecked?0:1;
        m_pPlayLine->setLoopCount(nCount);
    });
    connect(m_pPlayLine,&QTimeLine::finished,[=]{m_nPrevious = 0;});

    //qRegisterMetaType<QWheelEvent>("QWheelEvent");
    connect(m_pRuler,&TimeRuler::valueChanged,m_pActionTree,static_cast<void(QWidget:: *)()>(&QWidget::update));
    connect(m_pRuler,&TimeRuler::valueChanged,[=](quint16 value){
        m_pCurTime->setText(tr("Duration: %1:%2:%3").arg(value/60000,2,10,QChar('0'))\
                            .arg(value/1000,2,10,QChar('0')).arg((value%1000)/10,2,10,QChar('0')));
    });
    connect(m_pRuler,&TimeRuler::valueChanged,this,&EditorWidget::onCurrentTimeChange);
    connect(m_pTotalTime,&QLineEdit::editingFinished,[=]{
       int nTime = m_pTotalTime->text().toInt();
       m_pRuler->setMaxValue(nTime/1000.0);
    });
    connect(m_pActionTree,&ActionTreeView::wheelScale,m_pRuler,&TimeRuler::onWheelScale);
    initUndo(this);
#ifdef ACTION_DATA_OUT
    m_pActionDataFile = new QFile("./action.txt",this);
    if(!m_pActionDataFile->open(QFile::WriteOnly|QFile::Text|QFile::Truncate))
    {
        m_pActionDataFile->close();
        m_pActionDataFile = nullptr;
    }
#endif
}

void EditorWidget::addItems(const int nMacId)
{
    ActionItem * pItem = new ActionItem(nMacId);

    m_pModel->appendRow(QList<QStandardItem*>()<<pItem);
    int nModeId[nModeItemCnt] = {MotorForm::Mode_Profile_Pos/*,MotorForm::Mode_Profile_Vel,MotorForm::Mode_Cur*/};
    for(int i=0;i<nModeItemCnt;++i)
    {
        ModeItem * modeItem = new ModeItem(nModeId[i]);
        modeItem->setKeyStatus(7);
        QStandardItem * pValueItem = new QStandardItem();
        pValueItem->setData(QVariant(0),Qt::EditRole);
        pValueItem->setData(QColor(11,101,84),Qt::TextColorRole);
        KeyItem * pKey = new KeyItem();
        QList<QStandardItem *>list;
        list <<modeItem <<pValueItem << pKey;
        pItem->appendRow(list);
        if(i == 0)
            modeItem->setData(Qt::Checked,Qt::CheckStateRole);
        else
            modeItem->setData(Qt::Unchecked,Qt::CheckStateRole);
    }

    int nCol = m_pModel->columnCount();
    for (int i=0;i<nCol;++i)
    {
        QModelIndex index = m_pModel->index(m_pModel->rowCount()-1,i);
        m_pModel->setData(index,QColor(41,60,75),Qt::BackgroundColorRole);
    }


}

bool EditorWidget::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Paint:
    {
        QPaintEvent * pPaintEvt = static_cast<QPaintEvent *>(event);
        paintEvent(pPaintEvt);
    }
        return true;
        break;
    default:
        break;
    }
    return QObject::eventFilter(watched,event);
}

void EditorWidget::setValue(qreal value, const QModelIndex &index)
{
    QModelIndex keyIndex = index.sibling(index.row(),index.column()+1);
    if(!keyIndex.isValid())
        return;
    KeyItem * pItem = static_cast<KeyItem *>(m_pModel->itemFromIndex(keyIndex));
    if(pItem)
    {
        quint16 id = pItem->selectId();
        if(id > 0)
            //pItem->setKeyValue(id,value);
            m_pUndoStack->push(new KeyEditCmds(pItem,id,pItem->keyValue(id),value));
        else
        {
            quint16 current = pItem->getIdByTime(m_pRuler->curMillisecond());
            if(current > 0)
                //pItem->setKeyValue(id,value);
                m_pUndoStack->push(new KeyEditCmds(pItem,id,pItem->keyValue(id),value));
            else
//                pItem->addKey(m_pRuler->curMillisecond(),value);
                m_pUndoStack->push(new AddCmds(pItem,m_pRuler->curMillisecond(),value));
        }
    }
}

QPushButton *EditorWidget::creatBtn(QString normal, QString disable, QSize size)
{
    QIcon icon;
    icon.addFile(normal,QSize(),QIcon::Normal);
    icon.addFile(disable,QSize(),QIcon::Active,QIcon::On);
    icon.addFile(normal,QSize(),QIcon::Active,QIcon::Off);
    QPushButton * pBtn = new QPushButton(icon,"");
    pBtn->setIconSize(size);
    pBtn->setFixedSize(size);
    return pBtn;
}

void EditorWidget::onKeyPressed(KeyItem *pItem, quint16 keyId)
{
    m_pKeyItem = pItem;
    m_nKeyId = keyId;
    m_nKeyOldTime = pItem->keyTime(keyId);
}

void EditorWidget::onKeyReleased(KeyItem *pItem, quint16 keyId)
{
    quint16 newTime = pItem->keyTime(keyId);
    if(m_pKeyItem == pItem && m_nKeyId==keyId && m_nKeyOldTime!=newTime)
    {
        m_pUndoStack->push(new KeyMoveCmds(pItem,keyId,m_nKeyOldTime,newTime));
    }
    m_pKeyItem = nullptr;
    m_nKeyId = 0;
    m_nKeyOldTime = 0;
}

void EditorWidget::onCurrentTimeChange(quint16 nTime)
{
    if(m_pPlayLine->state() == QTimeLine::Running)
        return;
    int nRowCnt = m_pModel->rowCount();
    for(int i=0;i<nRowCnt;++i)
    {
        QModelIndex actionIdx = m_pModel->index(i,0);
        if(actionIdx.isValid())
        {
            int nModeCnt = m_pModel->rowCount(actionIdx);
            for(int nMode=0;nMode<nModeCnt;++nMode)
            {
                QModelIndex modeIdx = m_pModel->index(nMode,0,actionIdx);
                ModeItem * modeItem = static_cast<ModeItem *>(m_pModel->itemFromIndex(modeIdx));
                QModelIndex keyIdx = modeIdx.sibling(modeIdx.row(),modeIdx.column()+2);
                if(!keyIdx.isValid())
                    continue;
                KeyItem * pKeyItem = static_cast<KeyItem *>(m_pModel->itemFromIndex(keyIdx));
                modeItem->setKeyStatus(pKeyItem->keyPosStatus(nTime));
            }
        }
    }
}

void EditorWidget::saveAction()
{
    QFileDialog dialog(this,tr("Save as"),QDir::currentPath());
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Innfos(*.action)"));
    if(dialog.exec() == QDialog::Accepted)
    {
        QString path = dialog.selectedFiles().first();
        if(path.size() > 0)
        {
            saveActionData(path);
        }

    }
}

void EditorWidget::readAction()
{
    QFileDialog dialog(this,tr("Load"),QDir::currentPath());
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Innfos(*.action)"));
    if(dialog.exec() == QDialog::Accepted)
    {
        QString path = dialog.selectedFiles().first();
        if(path.size() > 0)
        {
            readActionData(path);
        }

    }
}

void EditorWidget::playBtnCallback(bool bPlay)
{
    if(bPlay)
    {
        m_pPlayLine->setFrameRange(0,m_pRuler->maxMillisecond());
        m_pPlayLine->setDuration(m_pRuler->maxMillisecond());
        if(m_pPlayLine->state() == QTimeLine::NotRunning)
        {
            m_pPlayLine->start();
        }
        else
        {
            m_pPlayLine->resume();
        }
        //Communication::getInstance()->m_bRecieve = false;
        //MotorMgr::getInstance()->setRequestActual(false);
    }
    else
    {
        m_pPlayLine->setPaused(true);
        //Communication::getInstance()->m_bRecieve = true;
        MotorMgr::getInstance()->setRequestActual(true);
    }
}

void EditorWidget::calcValueByTime(int time)
{
    m_pRuler->setCurValue(time/1000.0);
    int nRowCnt = m_pModel->rowCount();
#ifdef ACTION_DATA_OUT
    QTextStream out(m_pActionDataFile);
#endif
    for(int i=0;i<nRowCnt;++i)
    {

        QModelIndex actionIdx = m_pModel->index(i,0);
        if(actionIdx.isValid())
        {
            ActionItem * pActionItem = static_cast<ActionItem *>(m_pModel->itemFromIndex(actionIdx));
            if(pActionItem->isChecked())
            {
                quint32 nMac = pActionItem->mac();
                int nModeCnt = m_pModel->rowCount(actionIdx);
                for(int nMode=0;nMode<nModeCnt;++nMode)
                {
                    QModelIndex modeIdx = m_pModel->index(nMode,0,actionIdx);
                    if(modeIdx.data(Qt::CheckStateRole).toInt() == Qt::Checked)
                    {
                        ModeItem * modeItem = static_cast<ModeItem *>(m_pModel->itemFromIndex(modeIdx));
                        QModelIndex keyIdx = modeIdx.sibling(modeIdx.row(),modeIdx.column()+2);
                        if(!keyIdx.isValid())
                            continue;
                        KeyItem * pKeyItem = static_cast<KeyItem *>(m_pModel->itemFromIndex(keyIdx));
                        qreal value = pKeyItem->getValueByTime(time);

                        //qDebug()<<pKeyItem->getValueByTime(time)<<modeItem->modeId();
#ifdef ACTION_DATA_OUT
                        out << value << " ";
#else
                        if(pKeyItem->getKeyIdDuring(m_nPrevious,time) >= 0)
                        {
                            Mediator::getInstance()->playAction(nMac,(MotorForm::Motor_Mode)modeItem->modeId(),value);

                        }

#endif
                    }

                }
            }
        }
    }
    m_nPrevious = time;
#ifdef ACTION_DATA_OUT
    endl(out);
#endif
}

void EditorWidget::deleteKey()
{
    int nRowCnt = m_pModel->rowCount();
    for(int i=0;i<nRowCnt;++i)
    {
        QModelIndex actionIdx = m_pModel->index(i,0);
        if(actionIdx.isValid())
        {
            int nModeCnt = m_pModel->rowCount(actionIdx);
            for(int nMode=0;nMode<nModeCnt;++nMode)
            {
                QModelIndex modeIdx = m_pModel->index(nMode,0,actionIdx);
                QModelIndex keyIdx = modeIdx.sibling(modeIdx.row(),modeIdx.column()+2);
                KeyItem * pKeyItem = static_cast<KeyItem *>(m_pModel->itemFromIndex(keyIdx));
                if(pKeyItem->selectId() > 0)
                {
                    m_pUndoStack->push(new RemoveCmds(pKeyItem,pKeyItem->selectId()));
                }
            }
        }
    }
}

void EditorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
//    QPainter painter(this);
//    painter.save();
    QPoint sliderPos = m_pRuler->sliderPosToParent();
    if(sliderPos.x() >= 0)
    {
        RulerContainner * pContainner = qobject_cast<RulerContainner *>(m_pRuler->parent());
        if(pContainner)
        {
//            painter.setPen(QColor(255,0,0));
            QPoint lineBegin = m_pActionTree->mapFromParent(pContainner->mapToParent(sliderPos));
            QPoint lineEnd = QPoint(lineBegin.x(),lineBegin.y()+m_pActionTree->height());
            m_pActionTree->drawLine(lineBegin,lineEnd);
//            painter.drawLine(lineBegin,lineEnd);
//            qDebug() << lineBegin << lineEnd;
        }
    }
    else{
        m_pActionTree->drawLine(QPointF(0,0),QPointF(0,0));
    }
//    painter.restore();
}

void EditorWidget::initUndo(QWidget *parent)
{
    m_pUndoStack = new QUndoStack(parent);

    QShortcut *pUndo = new QShortcut(QKeySequence(tr("Ctrl+Z")),this);
    connect(pUndo,&QShortcut::activated,m_pUndoStack,&QUndoStack::undo);
    QShortcut *pRedo = new QShortcut(QKeySequence(tr("Ctrl+Y")),this);
    connect(pRedo,&QShortcut::activated,m_pUndoStack,&QUndoStack::redo);

    QShortcut * pDelete = new QShortcut(QKeySequence("Del"),this);
    connect(pDelete,&QShortcut::activated,this,&EditorWidget::deleteKey);

    connect(m_pPlayLine,&QTimeLine::stateChanged,[=](QTimeLine::State newState){
        bool bEnable = true;
        if(newState == QTimeLine::Running)
            bEnable = false;
        pUndo->setEnabled(bEnable);
        pRedo->setEnabled(bEnable);
        pDelete->setEnabled(bEnable);
        m_pActionTree->setEnabled(bEnable);
    });
//    QAction * pUndoAction = m_pUndoStack->createUndoAction(parent,tr("&Undo"));
//    pUndoAction->setShortcut(QKeySequence::Undo);

//    QAction * pRedoAction = m_pUndoStack->createRedoAction(parent,tr("&Redo"));
//    pRedoAction->setShortcut(QKeySequence::Redo);

//    QAction * pDeleteAction = new QAction(tr("&Delete"),parent);
//    pDeleteAction->setShortcut(tr("Del"));
//    connect(pDeleteAction,&QAction::triggered,this,&EditorWidget::deleteKey);
//    QMenuBar * pBar = new QMenuBar(this);
//    pBar->hide();
//    QMenu * pMenu = pBar->addMenu(tr("&Item"));
//    pMenu->addAction(pUndoAction);
//    pMenu->addAction(pRedoAction);
//    pMenu->addAction(pDeleteAction);
    //    pMenu->hide();
}

void EditorWidget::clearAllActions()
{
    int nRowCnt = m_pModel->rowCount();
    m_pModel->removeRows(0,nRowCnt);
}

void EditorWidget::saveActionData(QString fileName)
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
    writer.writeStartElement("ActionData");
    writer.writeAttribute("Version","1.0");
    int nRowCnt = m_pModel->rowCount();
    for (int i=0;i<nRowCnt;++i)
    {
        QModelIndex actionIdx = m_pModel->index(i,0);
        ActionItem * pActionItem = static_cast<ActionItem *>(m_pModel->itemFromIndex(actionIdx));
        quint32 mac = pActionItem->mac();
        bool bChecked = pActionItem->isChecked();
        writer.writeStartElement(tr("Mac%1").arg(mac));
        writer.writeAttribute("Check",tr("%1").arg(bChecked));
        int nModeCnt = pActionItem->rowCount();
        writer.writeAttribute("modeCnt",tr("%1").arg(nModeCnt));
        for(int nMode=0;nMode<nModeCnt;++nMode)
        {
            QModelIndex modeIdx = m_pModel->index(nMode,0,actionIdx);
            ModeItem *pModeItem = static_cast<ModeItem *>(m_pModel->itemFromIndex(modeIdx));
            int modeId = pModeItem->modeId();
            bool bModeChecked = pModeItem->isChecked();
            writer.writeStartElement(tr("modeId%1").arg(modeId));
            writer.writeAttribute("Check",tr("%1").arg(bModeChecked));
            QModelIndex keyIdx = modeIdx.sibling(modeIdx.row(),modeIdx.column()+2);
            KeyItem * pKeyItem = static_cast<KeyItem *>(m_pModel->itemFromIndex(keyIdx));
            QList<quint16> keysId = pKeyItem->getAllKeysId();
            writer.writeAttribute("KeyCnt",tr("%1").arg(keysId.size()));
            foreach (quint16 keyId, keysId)
            {
                writer.writeTextElement("Id",tr("%1").arg(keyId));
                writer.writeTextElement("Value",tr("%1").arg(pKeyItem->keyValue(keyId)));
                writer.writeTextElement("Time",tr("%1").arg(pKeyItem->keyTime(keyId)));
            }

            writer.writeEndElement();
        }
        writer.writeEndElement();

    }
//    for(int i=0;i<DATA_CNT;++i)
//    {
//        writer.writeTextElement(tr("Attr%1").arg(i),tr("%1").arg(m_motorData[i]));
//    }
    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
}

void EditorWidget::readActionData(QString fileName)
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
                if(strElementName == "ActionData")
                {
                    QXmlStreamAttributes attributes = reader.attributes();
                    if(attributes.hasAttribute("Version") && attributes.value("Version").toString() == "1.0")
                    {
                        readActions(&reader);
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

void EditorWidget::readActions(QXmlStreamReader *reader)
{
    clearAllActions();
    while (!reader->atEnd()) {
        reader->readNext();
        if(reader->isStartElement())
        {
            QString strElementName = reader->name().toString();
            int nMacIdx = strElementName.indexOf("Mac");

            if(nMacIdx >= 0)
            {
                int nMac = strElementName.right(strElementName.size()-3).toInt();
                //add action item
                ActionItem * pAction = new ActionItem(nMac);
                m_pModel->appendRow(QList<QStandardItem*>()<<pAction);
                int nModeCnt = reader->attributes().value("modeCnt").toInt();
                Qt::CheckState motorChecked = reader->attributes().value("Check").toInt()==1?Qt::Checked:Qt::Unchecked;
                pAction->setCheckState(motorChecked);
                for(int nMode = 0;nMode < nModeCnt;++nMode)
                {
                    reader->readNextStartElement();
                    strElementName = reader->name().toString();
                    int nModeIdx = strElementName.right(strElementName.size()-6).toInt();
                    QXmlStreamAttributes attrs = reader->attributes();
                    Qt::CheckState checked = attrs.value("Check").toInt()==1?Qt::Checked:Qt::Unchecked;
                    //add mode item
                    ModeItem * modeItem = new ModeItem(nModeIdx);
                    modeItem->setKeyStatus(7);
                    modeItem->setCheckState(checked);
                    QStandardItem * pValueItem = new QStandardItem();
                    pValueItem->setData(QVariant(0),Qt::EditRole);
                    pValueItem->setData(QColor(11,101,84),Qt::TextColorRole);
                    KeyItem * pKey = new KeyItem();
                    QList<QStandardItem *>list;
                    list <<modeItem <<pValueItem << pKey;
                    pAction->appendRow(list);

                    int nKeyCnt = attrs.value("KeyCnt").toInt();
                    for(int i=0;i<nKeyCnt;++i)
                    {
                        reader->readNextStartElement();
                        quint16 id = reader->readElementText().toInt();
                        reader->readNextStartElement();
                        qreal value = reader->readElementText().toDouble();
                        reader->readNextStartElement();
                        quint32 time = reader->readElementText().toInt();
                        pKey->addKey(time,value);
                    }
                    reader->readNextStartElement();//skip the end of element
                }

            }

        }
    }
}
