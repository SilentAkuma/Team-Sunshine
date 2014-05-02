#ifndef SENSORS_H
#define SENSORS_H

#include "IntervalTimer.h"
#include "Arduino.h"

#define SENSOR_D_PERIOD 10000
#define SENSOR_D_EMIT_TIME 100

class Sensors
{
protected:
    uint8_t m_sensor_ct;
    uint8_t *m_tx_pins, *m_rx_pins;
    uint8_t m_sensor_index;

    uint32_t m_period;
    uint32_t m_emit_time;

    uint16_t *m_sensor_data_internal;
    uint16_t *m_sensor_data_external;
    bool *m_sensor_data_lock;

    typedef enum
    {
        IDLE,
        READING
    } sensor_state_t;

    sensor_state_t m_sensor_state;
    IntervalTimer sensorTimer;
public:
    Sensors();
    ~Sensors();
    void init(uint16_t* sensor_data, uint8_t *tx_pins, uint8_t *rx_pins, uint8_t sensor_ct, uint32_t period = SENSOR_D_PERIOD, uint32_t emit_time = SENSOR_D_EMIT_TIME);
    void updateData(void);

    friend void sensorISR(void);
};

extern Sensors sensors;

#endif // SENSORS_H
