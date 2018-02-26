#ifndef ACTIONDELEGATE_H
#define ACTIONDELEGATE_H
#include <QStyledItemDelegate>
#include <QScopedPointer>
#include <QPushButton>

//class QPushButton;
class QTreeView;
class KeyItem;
class InertiaMove;
class ActionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ActionDelegate(quint16 nMaxTime,QTreeView * pView=0,QObject *parent = 0);
    virtual ~ActionDelegate();
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index)const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
signals:
    void valueChange(const qreal value,const QModelIndex &index)const;
    void keyPress(KeyItem * pItem,quint16 keyId);//press key dot
    void keyMoved(KeyItem * pItem,quint16 keyId);//release key dot after moving
    void movePos(QPointF pos);
public slots:
    void changeTime(quint16 minTime,quint16 maxTime);
protected:
    void paintKeysItem(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index)const;
    void paintModeItem(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index)const;
private:
    QScopedPointer<QPushButton > m_pKeyBtn;
    QScopedPointer<QPushButton > m_pLeftArrow;
    QScopedPointer<QPushButton > m_pRightArrow;
    QScopedPointer<QPushButton > m_pStatusBtn;
    QTreeView * m_pView;
    int m_nPressKeyId;
    bool m_bMousePressed;
    quint16 m_nMaxTime;
    quint16 m_nMinTime;
    InertiaMove * m_pMove;//
};

#endif // ACTIONDELEGATE_H
