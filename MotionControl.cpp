#include "Arduino.h"
#include "MotionControl.h"
#include "Encoder.h"
#include "HBridge.h"

float lmotor_velocity;		// rot/s
float rmotor_velocity;

float gyro_velocity;

float xSpeed, wSpeed, xError, wError, pxError, pwError;
float targetxSpeed, targetwSpeed, xAccel, wAccel;

float pd_Dx, pd_Px, pd_Dw, pd_Pw, pd_x, pd_w;

void control_initialize()
{
    xSpeed = wSpeed = xError = wError = pxError = pwError = 0;
	targetxSpeed = targetwSpeed = xAccel = wAccel = 0;
}

void motor_update(float dt)
{
    // Translational
	if(xSpeed < targetxSpeed)
	{
		xSpeed += xAccel/dt;
		if(xSpeed > targetxSpeed)
		{
			xSpeed = targetxSpeed;
		}
	}
	else
	{
		xSpeed -= xAccel/dt;
		if(xSpeed < targetxSpeed)
		{
			xSpeed = targetxSpeed;
		}
	}
	// Rotational
	if(wSpeed < targetwSpeed)
	{
		wSpeed += wAccel/dt;
		if(wSpeed > targetwSpeed)
		{
			wSpeed = targetwSpeed;
		}
	}
	else
	{
		wSpeed -= wAccel/dt;
		if(wSpeed < targetwSpeed)
		{
			wSpeed = targetwSpeed;
		}
	}
}

void control_update(float dt)
{
    lmotor_velocity = getLWheelVelocity();
    rmotor_velocity = getRWheelVelocity();

    motor_update(dt);

    // Previous model:
	//xError += xSpeed - ((lmotor_velocity + rmotor_velocity)/2);
	//wError += wSpeed - (((rmotor_velocity - lmotor_velocity)/2)/(WHEEL_BASE*PI));

	xError += (xSpeed - ((lmotor_velocity + rmotor_velocity)/2));
	wError += (wSpeed - (rmotor_velocity - lmotor_velocity));

    Serial.print(xError);
    Serial.print(" ");
    Serial.print(wError);
    Serial.print(" ");

	pd_Dx = (((xError - pxError)/dt) * Kdx);
	pd_Px = (xError * Kpx);

	pd_Dw = (((wError - pwError)/dt) * Kdw);
	pd_Pw = (wError * Kpw);

	pd_x = pd_Dx + pd_Px;
	pd_w = pd_Dw + pd_Pw;

	Serial.print(pd_x);
    Serial.print(" ");
    Serial.print(pd_w);
    Serial.print(" ");

	setMotor(MOTOR_A, (pd_x - pd_w));
	setMotor(MOTOR_B, (pd_x + pd_w));



	pxError = xError;
	pwError = wError;
}

//void move_forward(uint8_t cells)
//{
//
//}
//
//void turn(float degrees)
//{
//
//}
