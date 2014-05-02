#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#define Lencoder 1
#define Rencoder 2
#define Rotation 24

extern int32_t Lcounter, Rcounter;

void initEncoders(uint8_t outA1, uint8_t outB1, uint8_t outA2, uint8_t outB2);
void iterate1();
void iterate2();
void iterate3();
void iterate4();
void resetCounter(uint8_t encoder);



#endif // ENCODER_H
