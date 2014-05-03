#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#define ENC_L_ENCODER 1
#define ENC_R_ENCODER 2
#define ENC_FULL_ROT_TICKS 48

extern int32_t Lcounter, Rcounter;

void initEncoders(uint8_t outA1, uint8_t outB1, uint8_t outA2, uint8_t outB2);
void iterate1();
void iterate2();
void iterate3();
void iterate4();
void resetCounter(uint8_t encoder);

float getLWheelVelocity(float dt);
float getRWheelVelocity(float dt);

#endif // ENCODER_H
