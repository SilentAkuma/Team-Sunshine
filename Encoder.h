#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#define ENC_L_ENCODER 1
#define ENC_R_ENCODER 2
#define ENC_FULL_ROT_TICKS 48

#define ENC_L_WHEEL_INVERTED
//#define ENC_R_WHEEL_INVERTED

extern int32_t Lcounter, Rcounter;

void initEncoders(uint8_t outA1, uint8_t outB1, uint8_t outA2, uint8_t outB2, uint8_t bufferSize = 10);
void resetCounter(uint8_t encoder);

float getLWheelVelocity();
float getRWheelVelocity();

void cleanupEncoders(void);

#endif // ENCODER_H
