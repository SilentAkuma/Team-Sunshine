#ifndef MOTION_CONTROL_H
#define MOTION_CONTROL_H

#include "Arduino.h"
#include "ITG3200.h"
#include "Sensors.h"

static const float fudge_factor = 3.2;
static const float cell_size = 17.9 - fudge_factor; //cm
static const float wheel_diameter = 4.25; //cm
static const float cell_size_in_wheel_rots = cell_size/(wheel_diameter * PI);

#define DIST_CELLS(x) (cell_size_in_wheel_rots * (x))
#define wDECEL .9

static const float Kpx = 0.005;
static const float Kdx = 0.001;

static const float Kpw = 0.005;
static const float Kdw = 0.001;

static const float Ka = 0.5;

extern float targetxSpeed, targetwSpeed, xAccel, wAccel;
extern float gyroAngle;

void control_initialize(ITG3200& gyro_dev);

void control_update(float dt);

void calibrateZeroDrift(uint16_t iter, uint16_t delay_ms);

void move_forward(float rotations, float speed, float accel);
void turn(float degrees, float angspeed, float angaccel);
void stop_moving(void);

void completed_move(void);

void approachWall(void);

#endif // MOTION_CONTROL_H
