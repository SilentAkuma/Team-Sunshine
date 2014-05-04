#ifndef SENSORS_H
#define SENSORS_H

#include "IntervalTimer.h"
#include "Arduino.h"

#define SENSOR_D_PERIOD 1000
#define SENSOR_D_EMIT_TIME 50

#define SENSOR_NUM 4

class Sensors
{
protected:
    uint8_t m_sensor_ct;
    uint8_t *m_tx_pins, *m_rx_pins;
    uint8_t m_sensor_index;

    uint32_t m_period;
    uint32_t m_emit_time;

    uint16_t m_sensor_data_internal[SENSOR_NUM];
    uint16_t *m_sensor_data_external;
    int16_t m_sensor_offsets[SENSOR_NUM];
    bool m_sensor_data_lock[SENSOR_NUM];

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
    void calibrateSensors(uint16_t iterations, uint8_t delay_ms);
    void init(uint16_t* sensor_data, uint8_t *tx_pins, uint8_t *rx_pins, uint32_t period = SENSOR_D_PERIOD, uint32_t emit_time = SENSOR_D_EMIT_TIME);
    void updateData(void);

    friend void sensorISR(void);
};

extern Sensors sensors;

#endif // SENSORS_H
