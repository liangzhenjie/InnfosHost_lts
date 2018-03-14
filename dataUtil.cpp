#include "dataUtil.h"
DataUtil * DataUtil::m_pInstance = nullptr;

Directives DataUtil::convertToReadProxyId(MotorForm::Motor_Data_Id id)
{
    if(!m_pInstance)
        m_pInstance = new DataUtil();
    QList<Directives> keys = m_pInstance->readDataMap.keys(id);

    if(keys.size() > 0)
        return keys.at(0);

    return DIRECTIVES_INVALID;
}

Directives DataUtil::convertToSetProxyId(MotorForm::Motor_Data_Id id)
{
    if(!m_pInstance)
        m_pInstance = new DataUtil();
    QList<Directives> keys = m_pInstance->setDataMap.keys(id);

    if(keys.size() > 0)
        return keys.at(0);

    return DIRECTIVES_INVALID;
}


MotorForm::Motor_Data_Id DataUtil::convertToMotorDataId(Directives id)
{
    if(!m_pInstance)
        m_pInstance = new DataUtil();

    if(m_pInstance->readDataMap.contains(id))
        return m_pInstance->readDataMap.value(id);

    if(m_pInstance->setDataMap.contains(id))
        return m_pInstance->setDataMap.value(id);

    return MotorForm::DATA_INVALID;
}

void DataUtil::autoDestroy()
{
    if(m_pInstance)
        delete m_pInstance;
    m_pInstance = nullptr;
}

DataUtil::DataUtil()
{
    readDataMap.insert(D_READ_CUR_CURRENT,MotorForm::CUR_ACTURAL);
    readDataMap.insert(D_READ_CUR_VELOCITY,MotorForm::VEL_ACTURAL);
    readDataMap.insert(D_READ_CUR_POSITION,MotorForm::POS_ACTURAL);

    readDataMap.insert(D_READ_CUR_P,MotorForm::CUR_PROPORTIONAL);
    readDataMap.insert(D_READ_CUR_I,MotorForm::CUR_INTEGRAL);
    readDataMap.insert(D_READ_VEL_P,MotorForm::VEL_PROPORTIONAL);
    readDataMap.insert(D_READ_VEL_I,MotorForm::VEL_INTEGRAL);
    readDataMap.insert(D_READ_POS_P,MotorForm::POS_PROPORTIONAL);
    readDataMap.insert(D_READ_POS_I,MotorForm::POS_INTEGRAL);
    readDataMap.insert(D_READ_POS_D,MotorForm::POS_DIFFERENTIAL);

    readDataMap.insert(D_READ_PROFILE_POS_MAX_SPEED,MotorForm::PROFILE_POS_MAX_SPEED);
    readDataMap.insert(D_READ_PROFILE_POS_ACC,MotorForm::PROFILE_POS_ACC);
    readDataMap.insert(D_READ_PROFILE_POS_DEC,MotorForm::PROFILE_POS_DEC);

    readDataMap.insert(D_READ_PROFILE_VEL_MAX_SPEED,MotorForm::PROFILE_VEL_MAX_SPEED);
    readDataMap.insert(D_READ_PROFILE_VEL_ACC,MotorForm::PROFILE_VEL_ACC);
    readDataMap.insert(D_READ_PROFILE_VEL_DEC,MotorForm::PROFILE_VEL_DEC);

    readDataMap.insert(D_READ_CURRENT_PID_MIN,MotorForm::CUR_MINIMAL);
    readDataMap.insert(D_READ_CURRENT_PID_MAX,MotorForm::CUR_MAXIMUM);
    readDataMap.insert(D_READ_VELOCITY_PID_MIN,MotorForm::VEL_MINIMAL);
    readDataMap.insert(D_READ_VELOCITY_PID_MAX,MotorForm::VEL_MAXIMUM);
    readDataMap.insert(D_READ_POSITION_PID_MIN,MotorForm::POS_MINIMAL);
    readDataMap.insert(D_READ_POSITION_PID_MAX,MotorForm::POS_MAXIMUM);

    setDataMap.insert(D_SET_CURRENT,MotorForm::CUR_IQ_SET);
    setDataMap.insert(D_SET_VELOCITY,MotorForm::VEL_SET);
    setDataMap.insert(D_SET_POSITION,MotorForm::POS_SET);
    setDataMap.insert(D_SET_CURRENT_ID,MotorForm::CUR_ID_SET);
    setDataMap.insert(D_SET_CURRENT_P,MotorForm::CUR_PROPORTIONAL);
    setDataMap.insert(D_SET_CURRENT_I,MotorForm::CUR_INTEGRAL);
    setDataMap.insert(D_SET_VELOCITY_P,MotorForm::VEL_PROPORTIONAL);
    setDataMap.insert(D_SET_VELOCITY_I,MotorForm::VEL_INTEGRAL);
    setDataMap.insert(D_SET_POSITION_P,MotorForm::POS_PROPORTIONAL);
    setDataMap.insert(D_SET_POSITION_I,MotorForm::POS_INTEGRAL);
    setDataMap.insert(D_SET_POSITION_D,MotorForm::POS_DIFFERENTIAL);

    setDataMap.insert(D_SET_PROFILE_POS_MAX_SPEED,MotorForm::PROFILE_POS_MAX_SPEED);
    setDataMap.insert(D_SET_PROFILE_POS_ACC,MotorForm::PROFILE_POS_ACC);
    setDataMap.insert(D_SET_PROFILE_POS_DEC,MotorForm::PROFILE_POS_DEC);

    setDataMap.insert(D_SET_PROFILE_VEL_MAX_SPEED,MotorForm::PROFILE_VEL_MAX_SPEED);
    setDataMap.insert(D_SET_PROFILE_VEL_ACC,MotorForm::PROFILE_VEL_ACC);
    setDataMap.insert(D_SET_PROFILE_VEL_DEC,MotorForm::PROFILE_VEL_DEC);

    setDataMap.insert(D_SET_CURRENT_PID_MIN,MotorForm::CUR_MINIMAL);
    setDataMap.insert(D_SET_CURRENT_PID_MAX,MotorForm::CUR_MAXIMUM);
    setDataMap.insert(D_SET_VELOCITY_PID_MIN,MotorForm::VEL_MINIMAL);
    setDataMap.insert(D_SET_VELOCITY_PID_MAX,MotorForm::VEL_MAXIMUM);
    setDataMap.insert(D_SET_POSITION_PID_MIN,MotorForm::POS_MINIMAL);
    setDataMap.insert(D_SET_POSITION_PID_MAX,MotorForm::POS_MAXIMUM);

    setDataMap.insert(D_SET_CHART_FREQUENCY,MotorForm::CHART_FREQUENCY);
    setDataMap.insert(D_SET_CHART_THRESHOLD,MotorForm::CHART_THRESHOLD);

    readDataMap.insert(D_READ_CHART_FREQUENCY,MotorForm::CHART_FREQUENCY);
    readDataMap.insert(D_READ_CHART_THRESHOLD,MotorForm::CHART_THRESHOLD);

    readDataMap.insert(D_READ_CHANNEL_2,MotorForm::DATA_CHART);
    readDataMap.insert(D_READ_CHANNEL_3,MotorForm::DATA_CHART);
    readDataMap.insert(D_READ_CHANNEL_4,MotorForm::DATA_CHART);

    readDataMap.insert(D_READ_MIN_POS,MotorForm::POS_MIN_POS);
    readDataMap.insert(D_READ_MAX_POS,MotorForm::POS_MAX_POS);

    setDataMap.insert(D_SET_MIN_POS,MotorForm::POS_MIN_POS);
    setDataMap.insert(D_SET_MAX_POS,MotorForm::POS_MAX_POS);

    setDataMap.insert(D_SET_HOMING_POS,MotorForm::POS_HOMING);

    setDataMap.insert(D_SET_POS_OFFSET,MotorForm::POS_OFFSET);
    readDataMap.insert(D_READ_POS_OFFSET,MotorForm::POS_OFFSET);

    readDataMap.insert(D_READ_VOLTAGE,MotorForm::VOLTAGE);

    readDataMap.insert(D_READ_HOMING_LIMIT,MotorForm::HOMING_LIMIT);
    setDataMap.insert(D_SET_HOMING_LIMIT,MotorForm::HOMING_LIMIT);

    setDataMap.insert(D_SET_HOMING_CUR_MIN,MotorForm::HOMING_CUR_MIN);
    readDataMap.insert(D_READ_HOMING_CUR_MIN,MotorForm::HOMING_CUR_MIN);
    setDataMap.insert(D_SET_HOMING_CUR_MAX,MotorForm::HOMING_CUR_MAX);
    readDataMap.insert(D_READ_HOMING_CUR_MAX,MotorForm::HOMING_CUR_MAX);

    readDataMap.insert(D_READ_CURRENT_SCALE,MotorForm::CURRENT_SCALE);

    setDataMap.insert(D_SET_FILTER_C_STATUS,MotorForm::FILTER_C_STATUS);
    readDataMap.insert(D_READ_FILTER_C_STATUS,MotorForm::FILTER_C_STATUS);
    setDataMap.insert(D_SET_FILTER_C_VALUE,MotorForm::FILTER_C_VALUE);
    readDataMap.insert(D_READ_FILTER_C_VALUE,MotorForm::FILTER_C_VALUE);

    setDataMap.insert(D_SET_FILTER_V_STATUS,MotorForm::FILTER_V_STATUS);
    readDataMap.insert(D_READ_FILTER_V_STATUS,MotorForm::FILTER_V_STATUS);
    setDataMap.insert(D_SET_FILTER_V_VALUE,MotorForm::FILTER_V_VALUE);
    readDataMap.insert(D_READ_FILTER_V_VALUE,MotorForm::FILTER_V_VALUE);

    setDataMap.insert(D_SET_FILTER_P_STATUS,MotorForm::FILTER_P_STATUS);
    readDataMap.insert(D_READ_FILTER_P_STATUS,MotorForm::FILTER_P_STATUS);
    setDataMap.insert(D_SET_FILTER_P_VALUE,MotorForm::FILTER_P_VALUE);
    readDataMap.insert(D_READ_FILTER_P_VALUE,MotorForm::FILTER_P_VALUE);

    setDataMap.insert(D_SET_INERTIA,MotorForm::INERTIA);
    readDataMap.insert(D_READ_INERTIA,MotorForm::INERTIA);

    setDataMap.insert(D_SET_LOCK_ENERGY,MotorForm::LOCK_ENERGY);
    readDataMap.insert(D_READ_LOCK_ENERGY,MotorForm::LOCK_ENERGY);

    readDataMap.insert(D_READ_TEMP_MOTOR,MotorForm::TEMP_MOTOR);
    readDataMap.insert(D_READ_TEMP_INVERTER,MotorForm::TEMP_INVERTER);
    readDataMap.insert(D_READ_TEMP_PROTECT,MotorForm::TEMP_PROTECT);
    readDataMap.insert(D_READ_TEMP_RECOVERY,MotorForm::TEMP_RECOVERY);

    setDataMap.insert(D_SET_TEMP_PROTECT,MotorForm::TEMP_PROTECT);
    setDataMap.insert(D_SET_TEMP_RECOVERY,MotorForm::TEMP_RECOVERY);

    setDataMap.insert(D_SWITCH_CALIBRATION,MotorForm::CALIBRATION_SWITCH);
    setDataMap.insert(D_SET_CALIBRATION_ANGLE,MotorForm::CALIBRATION_ANGLE);
    readDataMap.insert(D_READ_CALIBRATION_SWITCH,MotorForm::CALIBRATION_SWITCH);
    readDataMap.insert(D_READ_CALIBRATION_ANGLE,MotorForm::CALIBRATION_ANGLE);

    setDataMap.insert(D_SET_SWITCH_MOTORS,MotorForm::MOTOR_SWITCH);
    readDataMap.insert(D_READ_MOTORS_SWITCH,MotorForm::MOTOR_SWITCH);

    setDataMap.insert(D_SET_CURRENT_MAX_VEL,MotorForm::CURRENT_MAX_VEL);
    readDataMap.insert(D_READ_CURRENT_MAX_VEL,MotorForm::CURRENT_MAX_VEL);

    readDataMap.insert(D_READ_VERSION,MotorForm::MOTOR_VERSION);

    readDataMap.insert(D_READ_MOTOR_MODE,MotorForm::MOTOR_MODE);
    setDataMap.insert(D_SET_MODE,MotorForm::MOTOR_MODE);
}


