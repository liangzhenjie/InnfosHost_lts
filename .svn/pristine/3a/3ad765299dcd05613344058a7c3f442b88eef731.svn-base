#ifndef FILTER_H
#define FILTER_H

#include <QDialog>

namespace Ui {
class Filter;
}

class Filter : public QDialog
{
    Q_OBJECT

public:
    explicit Filter(QWidget *parent = 0);
    ~Filter();

protected:
    enum
    {
        C_STATUS,
        C_VALUE,
        V_STATUS,
        V_VALUE,
        P_STATUS,
        P_VALUE,
        CALIBRATION_SWITCH,
        CALIBRATION_ANGLE,
        DATA_CNT,
    };
    void initDataReference();
    void valueChangeByUser();

public:
    static void ShowWindowIfNotExist();
public slots:
    void onTypeChange(int nIdx);
    void onValueChange();
    void motorDataChange(int nId);
private slots:
    void on_readAngle_clicked();

private:
    Ui::Filter *ui;
    int m_nDataRef[DATA_CNT];
};

#endif // FILTER_H
