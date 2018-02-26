#ifndef INNFOSTOOLBAR_H
#define INNFOSTOOLBAR_H

#include <QWidget>

class QPushButton;
class QLabel;

class InnfosToolbar : public QWidget
{
    Q_OBJECT
public:
    InnfosToolbar(QWidget *parent = 0);
    ~InnfosToolbar();
    void SetBtnHide(int nParam);
    void setIconSize(QSize s);
    QPushButton * getCloseBtn()const;
    enum HideBtn{
        Hide_Min=0x01,
        Hide_Max=0x02,
        Hide_Prew=0x04,
        Hide_None=0x08
    };
    void setToolBarIcon(const QPixmap& icon);
protected:
    //double click to minimal or maximum the window
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    //drag the window
    //virtual void mousePressEvent(QMouseEvent *event);
    //event filter
    virtual bool eventFilter(QObject *watched, QEvent *event);
signals:

public slots:

private slots:
    //minimal,maximum or close opreation
    void onClicked();
private:
    //maximum or reset
    void updateMaximize();
private:
    QLabel * m_pIconLabel;
    QLabel * m_pTittleLabel;
    QPushButton * m_pMinimizeButton;
    QPushButton * m_pMaximizeButton;
    QPushButton * m_pCloseButton;
    QPushButton * m_pPreview;
};

#endif // INNFOSTOOLBAR_H
