#ifndef WARNINGDIALOG_H
#define WARNINGDIALOG_H

#include <QWidget>
class QPushButton;

class WarningDialog : public QWidget
{
    Q_OBJECT
public:
    enum ErrorType{
        USB_ERROR,
        SERVO_ERROR,
        ERROR_CNT
    };
    explicit WarningDialog(ErrorType type,QWidget *parent = 0);

signals:

public slots:
    void retry();
private:
    QPushButton * m_pCancel;
    QPushButton * m_pRetry;
};

#endif // WARNINGDIALOG_H
