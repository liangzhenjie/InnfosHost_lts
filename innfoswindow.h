#ifndef INNFOSWINDOW_H
#define INNFOSWINDOW_H

#include <QWidget>

class QStackedWidget;
class QPushButton;
class InnfosChartWidget;
class Filter;

class InnfosWindow : public QWidget
{
    Q_OBJECT

public:
    InnfosWindow(QWidget *parent = 0,int nMode=0,QString nodeName="Node 1");
    ~InnfosWindow();
    void enableMode(int nMode);
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void showGraph();
    void closeGraph();
private:
    QPushButton * createInnfosBtn(const QString & iconNormal,const QString & iconSelect,const QString & text,const QString & objName);
private:
    QStackedWidget * m_pStackedWidget;
    InnfosChartWidget * m_pGraph;
    int m_nCurMode;
    Filter * m_pFilter;
};

#endif // INNFOSWINDOW_H
