#ifndef DATAUTIL_H
#define DATAUTIL_H
#include <QMap>
#include "innfosproxy.h"
#include "motorform.h"


class DataUtil
{
public:
    static Directives convertToReadProxyId(MotorForm::Motor_Data_Id id);
    static Directives convertToSetProxyId(MotorForm::Motor_Data_Id id);
    static MotorForm::Motor_Data_Id convertToMotorDataId(Directives id);
    static void autoDestroy();
private:
    DataUtil();
    QMap<Directives,MotorForm::Motor_Data_Id> readDataMap;
    QMap<Directives,MotorForm::Motor_Data_Id> setDataMap;
    static DataUtil * m_pInstance;
};
#endif // DATAUTIL_H
