#ifndef MOTION_CONTROL_H
#define MOTION_CONTROL_H

static const float Kpx = 2;
static const float Kdx = 10;

static const float Kpw = 1;
static const float Kdw = 1;

static const float Ka = 0.5;

extern float targetxSpeed, targetwSpeed, xAccel, wAccel;

void control_initialize();

void control_update(float dt);

//void move_forward(uint8_t cells);
//void turn(float degrees);

//void completed_move(void);

#endif // MOTION_CONTROL_H
