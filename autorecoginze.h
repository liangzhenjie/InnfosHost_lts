#ifndef AUTORECOGINZE_H
#define AUTORECOGINZE_H

#include <QObject>
#include <QtSerialPort/QSerialPortInfo>
#include <QMap>

class AutoRecoginze : public QObject
{
    Q_OBJECT
public:
    enum{
        NO_CONNECT,
        USB_CONNECTED=0x02,
        MOTOR_CONNECTED=0x04,
    };
    static AutoRecoginze * getInstance();
    static void autoDestroy();
    bool startRecognize(bool bRetry = false);
    void setAvailable();
    void addMototInfo(quint8 nDeviceId,quint32 nDeviceMac);
    void setConnectStatus(int nStatus);
protected:
    explicit AutoRecoginze(QObject *parent = 0);
    void findAvailablePorts();
signals:

public slots:
    void openFailed();
    void waitTimeout();
    void onPortOpen(bool bSuccess);
private:
    static AutoRecoginze * m_pAutoRecognize;
    bool m_bFindAvaliable;
    bool m_bTryNext;
    QList <QSerialPortInfo> m_portList;
    QMap<quint8,quint32> m_motorsInfo;
    bool m_bHasSameId;
    int m_nConnectStatus;
};

#endif // AUTORECOGINZE_H
