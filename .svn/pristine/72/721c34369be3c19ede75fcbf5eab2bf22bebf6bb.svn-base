#ifndef INNFOSWIZARD_H
#define INNFOSWIZARD_H

#include <QWidget>
#include <QMap>

class InnfosWizardPage;
class QPushButton;

class InnfosWizard : public QWidget
{
    Q_OBJECT
public:
    typedef QMap<int,InnfosWizardPage *> WizardsMap;
    explicit InnfosWizard(QWidget *parent = 0);
    void addPage(InnfosWizardPage * page);
    virtual ~InnfosWizard() {}
signals:

public slots:
    void FindAvailablePort(int nConnectStatus);
    void back();
    void next();
    void updateBtnStatus();
protected:
    void resizeEvent(QResizeEvent *event);
private:
    bool isFinal();
    void Finished();
private:
    int m_nCurPageId;
    WizardsMap m_mPages;
    QPushButton * m_pBack;
    QPushButton * m_pNext;
    QPushButton * m_pCancel;
};

class InnfosWizardPage : public QWidget
{
    Q_OBJECT
public:
    InnfosWizardPage(QWidget *parent = 0);
    virtual ~InnfosWizardPage() {}
    virtual bool isComplete();
    static InnfosWizardPage * createPage1();
    static InnfosWizardPage * createPage2();

public slots:
    void SetComplete(bool bComplete);
signals:
    void isCompleteChange();
private:
    bool m_bIsComplete;
};



#endif // INNFOSWIZARD_H
