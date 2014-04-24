#ifndef HBRIDGE_H
#define HBRIDGE_H

#include <stdint.h>

#define MOTOR_FULL_REVERSE -1023
#define MOTOR_FULL_FORWARD 1023
#define MOTOR_STOP 0

#define MOTOR_A 1
#define MOTOR_B 2

void initMotors(uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4, uint8_t ENA, uint8_t ENB);
void setMotor(uint8_t motor, int16_t speed);
void stopMotor(uint8_t motor);
void enableMotor(uint8_t motor);
void disableMotor(uint8_t motor);

#endif // HBRIDGE_H
