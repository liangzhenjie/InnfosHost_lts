#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>
//#define ACTION_DATA_OUT

class TimeRuler;
class QTreeView;
class QTimeLine;
class QLabel;
class QPushButton;
class QStandardItemModel;
class ActionTreeView;
class QUndoStack;
class KeyItem;
class QLineEdit;
class QXmlStreamReader;
class QFile;

class EditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditorWidget(QWidget *parent = 0);
    void addItems(const int nMacId);
    bool eventFilter(QObject *watched, QEvent *event);
signals:

public slots:
    void setValue(qreal value,const QModelIndex &index);
    QPushButton * creatBtn(QString normal,QString disable,QSize size);
    void onKeyPressed(KeyItem * pItem,quint16 keyId);
    void onKeyReleased(KeyItem * pItem,quint16 keyId);
    void onCurrentTimeChange(quint16 nTime);
    void saveAction();
    void readAction();
    void playBtnCallback(bool bPlay);
protected slots:

    void calcValueByTime(int time);
    void deleteKey();
protected:
    void paintEvent(QPaintEvent *event);
    void initUndo(QWidget *parent);
    void clearAllActions();//when user load action from file,need clear all actions first;
private:
    void saveActionData(QString fileName);
    void readActionData(QString fileName);
    void readActions(QXmlStreamReader *reader);
private:
    TimeRuler * m_pRuler;
    ActionTreeView * m_pActionTree;
    QStandardItemModel * m_pModel;
    QTimeLine * m_pPlayLine;
    QLineEdit * m_pTotalTime;
    QLabel * m_pCurTime;
    QUndoStack * m_pUndoStack;
    //move key relate
    KeyItem * m_pKeyItem;
    quint16 m_nKeyId;
    quint16 m_nKeyOldTime;
    QFile *m_pActionDataFile;
    quint16 m_nPrevious;
};

#endif // EDITORWIDGET_H
