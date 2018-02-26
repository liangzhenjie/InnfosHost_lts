#ifndef LAUNCHWINDOW_H
#define LAUNCHWINDOW_H

#include <QFrame>

namespace Ui {
class launchWindow;
}
class QEventLoop;

class launchWindow : public QFrame
{
    Q_OBJECT

public:
    static int launchTip(QWidget * pParent);
    explicit launchWindow(QWidget *parent = 0);
    ~launchWindow();
protected:
    int exec();
    void startLaunchTimer();
private:
    Ui::launchWindow *ui;
    QEventLoop * m_pLoop;
};

#endif // LAUNCHWINDOW_H
