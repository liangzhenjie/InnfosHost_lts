#ifndef MSGBOX_H
#define MSGBOX_H

#include <QWidget>
#include <QString>

class QEventLoop;
class QGridLayout;

class MsgBox : public QWidget
{
    Q_OBJECT
public:
    explicit MsgBox(const QString &text,QWidget *parent = 0,int nMsgId = -1);
    ~MsgBox();
    int exec();
    static int Tip(QWidget * pParent,const QString &tittle,const QString &text);
    static int Error(QWidget * pParent,const QString &tittle,const QString &text,int nErrorId);
    static int Warning(QWidget * pParent,const QString &tittle,const QString &text);
    void setVisible(bool visible);
    QGridLayout * getContentLayout(){
        return m_pContentLayout;
    }
signals:
private:
    static QList<int> * m_msgIdList;//to avoid multi popup same messageboxs.
    int m_nMsgId;
public slots:
private:
    QEventLoop * m_loop;
    QGridLayout * m_pContentLayout;
};

#endif // MSGBOX_H
