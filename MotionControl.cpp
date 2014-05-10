#include "Arduino.h"
#include "MotionControl.h"
#include "Encoder.h"
#include "HBridge.h"
#include "Wire.h"
#include "ITG3200.h"

float lmotor_velocity;		// rot/s
float rmotor_velocity;

float gyroAngle;

float xSpeed, wSpeed, xError, wError, pxError, pwError;
float targetxSpeed, targetwSpeed, xAccel, wAccel;

float pd_Dx, pd_Px, pd_Dw, pd_Pw, pd_x, pd_w;

float zero_drift_rate;

uint32_t deltaLcounter, deltaRcounter;

float targetWheelPos;
float targetAngle;
bool rotatingRight;

bool targetxPointSet, targetwPointSet, targetApproachPointSet;

ITG3200 *m_gyro;

void control_initialize(ITG3200& gyro_dev)
{
    targetxPointSet = targetwPointSet = false;
    targetWheelPos = 0;
    m_gyro = &gyro_dev;
    lmotor_velocity = rmotor_velocity = 0;
    xSpeed = wSpeed = xError = wError = pxError = pwError = 0;
    targetxSpeed = targetwSpeed = xAccel = wAccel = 0;
    gyroAngle = 0;
    zero_drift_rate = 0;
    deltaLcounter = deltaRcounter = 0;
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

void updateGyroAngle(float dt)
{
    static float gx = 0, gy = 0, gz = 0;
    if(m_gyro->isRawDataReady())
    {
        m_gyro->readGyro(&gx, &gy, &gz);
    }
    gyroAngle += dt*gz - dt*zero_drift_rate;
}

void calibrateZeroDrift(uint16_t iter, uint16_t delay_ms)
{
    zero_drift_rate = 0;
    gyroAngle = 0;
    uint32_t start_micros, prev_micros, cur_micros;
    start_micros = cur_micros = prev_micros = micros();
    float dt;
    while(iter)
    {
        dt = (cur_micros - prev_micros)*0.000001f;
        prev_micros = cur_micros;
        cur_micros = micros();

        updateGyroAngle(dt);

        iter--;
        delay(delay_ms);
    }

    zero_drift_rate = (gyroAngle/((cur_micros - start_micros)*0.000001f));
    gyroAngle = 0;
}

void control_update(float dt)
{
    lmotor_velocity = getLWheelVelocity();
    rmotor_velocity = getRWheelVelocity();

    updateGyroAngle(dt);

    if(targetApproachPointSet)
    {
        float lSensorError, rSensorError, tempa, tempb;
        tempa = sensors.averages[1];
        lSensorError = sensors.m_sensor_data_external[1];
        lSensorError -= tempa;
        tempb = sensors.averages[2];
        rSensorError = sensors.m_sensor_data_external[2];
        rSensorError -= tempb;

        if(abs(lSensorError) < 50 && abs(rSensorError) < 50)
        {
            targetxSpeed = 0;
            targetwSpeed = 0;

            targetApproachPointSet = false;
            completed_move();
        }

        targetxSpeed =
        constrain((lSensorError + rSensorError)*-0.005, 0.75, -0.75);

        targetwSpeed =
        (lSensorError - rSensorError)*-0.005;

        Serial.print(" ");
        Serial.print(lSensorError);
        Serial.print(" ");
        Serial.print(rSensorError);
        Serial.print(" ");

    }

    // Linear setpoint handling
    if(targetxPointSet)
    {
        if(((float)(abs(Lcounter - deltaLcounter)+abs(Rcounter - deltaRcounter)))/(2*ENC_FULL_ROT_TICKS) >= targetWheelPos)
        {
            targetxPointSet = false;
            completed_move();
        }
    }

    // Angular setpoint handling
    if(targetwPointSet)
    {
        if(abs(gyroAngle - targetAngle) < 10)
            {
                targetwSpeed= (targetAngle - gyroAngle) * -0.2;
            }
        if(rotatingRight)
        {
            if(gyroAngle >= targetAngle)
            {
                targetwPointSet = false;
                completed_move();
            }
        }
        else
        {
            if(gyroAngle <= targetAngle)
            {
                targetwPointSet = false;
                completed_move();
            }
        }
    }

    motor_update(dt);

    xError += (xSpeed - ((lmotor_velocity + rmotor_velocity)/2));
    wError += (wSpeed - (rmotor_velocity - lmotor_velocity));

//    Serial.print(xError);
//    Serial.print(" ");
//    Serial.print(wError);
//    Serial.print(" ");

    pd_Dx = (((xError - pxError)/dt) * Kdx);
    pd_Px = (xError * Kpx);

    pd_Dw = (((wError - pwError)/dt) * Kdw);
    pd_Pw = (wError * Kpw);

    pd_x = pd_Dx + pd_Px;
    pd_w = pd_Dw + pd_Pw;

//    Serial.print(pd_x);
//    Serial.print(" ");
//    Serial.print(pd_w);
//    Serial.print(" ");

    setMotor(MOTOR_A, (pd_x - pd_w));
    setMotor(MOTOR_B, (pd_x + pd_w));

    pxError = xError;
    pwError = wError;
}

void stop_moving(void)
{
    xSpeed = 0;
    wSpeed = 0;
    targetxSpeed = 0;
    targetwSpeed = 0;
    wAccel = 0;
    xAccel = 0;
    targetwPointSet = false;
    targetxPointSet = false;
}

void move_forward(float rotations, float speed, float accel)
{
    targetwSpeed = 0;
    wAccel = 0;
    wSpeed = 0;
    targetxSpeed = abs(speed);
    xAccel = abs(accel);

    deltaLcounter = Lcounter;
    deltaRcounter = Rcounter;
    targetWheelPos = rotations;
    targetxPointSet = true;
}

void turn(float degrees, float angspeed, float angaccel)
{
    targetxSpeed = 0;
    xAccel = 0;
    xSpeed = 0;
    rotatingRight = degrees > 0;
    targetwSpeed = abs(angspeed)*((rotatingRight)?(-1):(1));
    wAccel = abs(angaccel);

    targetAngle = gyroAngle + degrees;
    targetwPointSet = true;
}

void approachWall(void)
{
    targetApproachPointSet = true;
    xAccel = 1000;
    xSpeed = 1000;
    targetxSpeed = 0;
    targetwSpeed = 0;
}
