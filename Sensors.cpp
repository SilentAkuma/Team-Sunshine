#include "Sensors.h"

void sensorISR(void);

Sensors::Sensors()
{

}

Sensors::~Sensors()
{
    free(m_sensor_data_lock);
    free(m_sensor_data_internal);
    free(m_sensor_offsets);
}

void Sensors::init(uint16_t* sensor_data, uint8_t *tx_pins, uint8_t *rx_pins, uint8_t sensor_ct, uint32_t period, uint32_t emit_time)
{
    m_sensor_data_lock = (bool*)calloc(sensor_ct, sizeof(bool));
    m_sensor_data_internal = (uint16_t*)malloc(sizeof(uint16_t) * sensor_ct);
    m_sensor_offsets = (int16_t*)calloc(sensor_ct, sizeof(int16_t));
    m_sensor_data_external = sensor_data;
    m_tx_pins = tx_pins;
    m_rx_pins = rx_pins;
    m_sensor_ct = sensor_ct;
    m_sensor_index = 0;
    m_sensor_state = IDLE;

    m_period = period;
    m_emit_time = emit_time;

    for(uint8_t i = 0; i < sensor_ct; i++)
    {
        pinMode(tx_pins[i], OUTPUT);
        digitalWrite(tx_pins[i], LOW);
    }

    sensorTimer.begin(sensorISR, m_period);
}

void Sensors::updateData(void)
{
    for(uint8_t i = 0; i < m_sensor_ct; i++)
    {
        if(!m_sensor_data_lock[i])
        {
            m_sensor_data_external[i] = m_sensor_data_internal[i] + m_sensor_offsets[i];
        }
    }
}

Sensors sensors;

void sensorISR(void)
{
    // Stop the timer
    sensors.sensorTimer.end();

    switch(sensors.m_sensor_state)
    {
    case Sensors::sensor_state_t::IDLE:
    {
        // Schedule the next interrupt
        sensors.sensorTimer.begin(sensorISR, sensors.m_emit_time);
        sensors.m_sensor_state = Sensors::sensor_state_t::READING;

        // Turn on the IR LED
        digitalWrite(sensors.m_tx_pins[sensors.m_sensor_index], HIGH);
        break;
    }
    case Sensors::sensor_state_t::READING:
    {
        // Schedule the next interrupt
        sensors.sensorTimer.begin(sensorISR, sensors.m_period - sensors.m_emit_time);
        sensors.m_sensor_state = Sensors::sensor_state_t::IDLE;

        // Lock the data channel and read the sensor
        sensors.m_sensor_data_lock[sensors.m_sensor_index] = true;
        sensors.m_sensor_data_internal[sensors.m_sensor_index] = analogRead(sensors.m_rx_pins[sensors.m_sensor_index]);
        sensors.m_sensor_data_lock[sensors.m_sensor_index] = false;

        // Turn off the emitter
        digitalWrite(sensors.m_tx_pins[sensors.m_sensor_index], LOW);

        // Switch to a different sensor
        sensors.m_sensor_index++;

        if(sensors.m_sensor_index == sensors.m_sensor_ct)
        {
            sensors.m_sensor_index = 0;
        }
        break;
    }
    }
}

void Sensors::calibrateSensors(uint16_t iter, uint8_t delay_ms)
{
    uint32_t *averages = (uint32_t*)calloc(m_sensor_ct, sizeof(uint32_t));

    for(uint16_t i = 0; i < iter; i++)
    {
        updateData();

        for(uint8_t j = 0; j < m_sensor_ct; j++)
        {
            averages[j] += m_sensor_data_external[j];
        }

        delay(delay_ms);
    }

    uint32_t total_average = 0;

    for(uint8_t i = 0; i < m_sensor_ct; i++)
    {
        averages[i] /= iter;
        total_average += averages[i];
    }

    total_average /= iter;

    for(uint8_t i = 0; i < m_sensor_ct; i++)
    {
        m_sensor_offsets[i] = (int16_t)(averages[i]) - (int16_t)(total_average);
    }

    free(averages);
}
