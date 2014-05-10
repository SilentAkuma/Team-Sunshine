#include "HBridge.h"
#include "Sensors.h"
#include "Wire.h"
#include "ITG3200.h"
#include "Encoder.h"
#include "MotionControl.h"
#include "IntervalTimer.h"
#include "Solver.h"

float t;
int speed;

uint16_t sensor_data[4];

uint8_t rx_pins[] =
{
    20,21,22,23
};

uint8_t tx_pins[] =
{
    7,8,9,10
};

ITG3200 gyro = ITG3200();

float x,y,z;

int curtime, prevtime;

uint32_t sensor_f_average, sensor_s_average;

uint8_t wallmask = 0;

float drive_speed = 0.8, turn_speed = 1;

#define WALL_LEFT 0x01
#define WALL_FRONT 0x02
#define WALL_RIGHT 0x04

#define FRONT_THRESHOLD 450
#define LR_THRESHOLD 330

#define SUGGESTED_LOOP_DELAY_TIME 10000

IntervalTimer move_timer;

const static float CenteringKp = 0.01;

bool done_move = false;
bool driving_straight = false;

bool compoundMove = false;

int32_t deviation = 0;

float dt;

int command = 0;

float prevAngle = 0;

void setup()
{
    // Initialize variables
    x = 0;
    y = 0;
    z = 0;
    t = 0;
    speed = 0;

    // Initialize Serial and I2C bus
    Serial.begin(115200);
    Wire.begin();

    // Wait 5000 ms to let the user get his/her hand away from the mouse
    delay(5000);

    // Initialize stuff
    Serial.print("Initializing: ");

    Serial.print("IR sensors... ");
    sensors.init(sensor_data, tx_pins, rx_pins);

    Serial.print(" center calibrating IR sensors... ");
    sensors.calibrateSensors(500, 2);

    sensor_s_average = (sensors.averages[0] + sensors.averages[3])/2;
    sensor_f_average = (sensors.averages[1] + sensors.averages[2])/2;

    sensors.m_sensor_offsets[0] = sensor_s_average - sensors.averages[0];
    sensors.m_sensor_offsets[1] = sensor_f_average - sensors.averages[1];
    sensors.m_sensor_offsets[2] = sensor_f_average - sensors.averages[2];
    sensors.m_sensor_offsets[3] = sensor_s_average - sensors.averages[3];

    Serial.println(" gyro...");
    gyro.init(ITG3200_ADDR_AD0_HIGH);

    Serial.print(" zero calibrating gyro...");
    gyro.zeroCalibrate(500, 2);

    Serial.print(" starting motion controller...");
    control_initialize(gyro);

    Serial.print(" zero calibrating drift...");
    calibrateZeroDrift(500, 2);

    Serial.print(" motors...");
    initMotors(3,4,6,5,12,11);
    enableMotor(MOTOR_A|MOTOR_B);

    Serial.print(" encoders...");
    initEncoders(14,13,16,15);

    Serial.println(" done.");

    //Record the start time
    prevtime = micros()-100;

    turn(180, 1, 10);

    prevAngle = gyroAngle;
}

bool completedWallApproach = true;

void completed_move(void)
{
    stop_moving();
    if(!completedWallApproach)
    {
        if(wallmask & WALL_FRONT)
        {
            Serial.println("APPROACHIN' THE WALL!");
            approachWall();
        }else
        {
            move_timer.begin(setMoveFlagISR, 10000);
        }
        completedWallApproach = true;
    }
    else
    {
        move_timer.begin(setMoveFlagISR, 10000);
    }
}

void setMoveFlagISR(void)
{
    done_move = true;
    move_timer.end();
}

void loop()
{
    //-----Compute dt-----
    curtime = micros();
    dt = (curtime - prevtime) * 0.000001f;
    prevtime = curtime;
    //--------------------

    // Update the IR sensor data
    sensors.updateData();

    // Recalculate the wallmask
    update_wallmask();

    // Calculate the deviation from center according to the IR sensors
    deviation = constrain(update_deviation(), -30, 30);

    // Determine what to do next
    if(done_move)
    {
        if(compoundMove)
        {
            move_forward(DIST_CELLS(1), drive_speed, 10);
            driving_straight = true;
            compoundMove = false;
            completedWallApproach = false;
        }
        else
        {
            if(firstTurn)
                handleFirstTurn(wallmask & WALL_LEFT, wallmask & WALL_RIGHT, wallmask & WALL_FRONT);

            updateWalls(wallmask & WALL_LEFT, wallmask & WALL_RIGHT, wallmask & WALL_FRONT, curRow, curCol, curHd);

            floodfill();

            Point nextSpace = findNextSpace();

            Vector nextMove = moveNextSpace(nextSpace);
            /*Vector nextMove;
             if((wallmask & WALL_LEFT) && (wallmask & WALL_RIGHT) && (wallmask & WALL_FRONT))
             {
             nextMove.dir = Reverse;
             }else if((wallmask & WALL_LEFT) && (wallmask & WALL_RIGHT))
             {
             nextMove.dir = Forward;
             }else if((wallmask & WALL_RIGHT) && (wallmask & WALL_FRONT))
             {
             nextMove.dir = Left;
             }else if((wallmask & WALL_LEFT) && (wallmask & WALL_FRONT))
             {
             nextMove.dir = Right;
             }else if((wallmask & WALL_FRONT))
             {
             nextMove.dir = Right;
             }else
             {
             nextMove.dir = Forward;
             }

             nextMove.mag = 1;
             */
            Serial.print("Direction: ");
            Serial.println(nextMove.dir);

            switch(nextMove.dir)
            {
            case Left:
                Serial.println("TURNING LEFT!");
                turn(-90, turn_speed, 10);
                driving_straight = false;
                compoundMove = true;
                break;
            case Right:
                Serial.println("TURNING RIGHT!");
                turn(90, turn_speed, 10);
                driving_straight = false;
                compoundMove = true;
                break;
            case Forward:
                Serial.println("GOIN' STEADY!");
                move_forward(DIST_CELLS(1), drive_speed, 10);
                driving_straight = true;
                compoundMove = false;
                completedWallApproach = false;
                break;
            case Reverse:
                Serial.println("U-TURN BEEYOTCH!");
                turn(180, turn_speed, 10);
                driving_straight = false;
                compoundMove = true;
                break;
            }
        }
        done_move = false;

        print_debug_data();
    }



    if(driving_straight)
    {
        targetwSpeed = deviation * CenteringKp + (gyroAngle - prevAngle)*dt*-0.05;
        wAccel = 1000;
    }

    prevAngle = gyroAngle;

    // Update the motion controller
    control_update(dt);

    // Loop delay
    //    int32_t delay_time = SUGGESTED_LOOP_DELAY_TIME - (dt*1000000);
    //    if(delay_time > 0)
    //        delayMicroseconds(delay_time);
    delay(10);
}

void update_wallmask(void)
{
    bool leftwall = sensor_data[0] < (sensor_s_average+LR_THRESHOLD);
    bool rightwall = sensor_data[3] < (sensor_s_average+LR_THRESHOLD);
    bool frontwall = sensor_data[1] < (sensor_f_average+FRONT_THRESHOLD) || sensor_data[2] < (sensor_f_average+FRONT_THRESHOLD);

    wallmask = (leftwall?WALL_LEFT:0)|(frontwall?WALL_FRONT:0)|(rightwall?WALL_RIGHT:0);
}

int16_t centering_factor = 0;

int32_t update_deviation(void)
{
    int16_t a, b, c;
    a = sensor_data[0];
    b = sensor_data[3];
    c = sensor_s_average;
    if((wallmask & WALL_LEFT) && (wallmask & WALL_RIGHT))
    {
        return a - b + centering_factor;
    }
    else if(wallmask & WALL_LEFT)
    {
        return (a - c);
    }
    else if(wallmask & WALL_RIGHT)
    {
        return (c - b);
    }
    return 0;
}

void print_debug_data(void)
{

    Serial.print("Left vel: ");
    Serial.print(getLWheelVelocity());
    Serial.print(" Right vel: ");
    Serial.print(getRWheelVelocity());
    Serial.print(" Left ct: ");
    Serial.print(Lcounter);
    Serial.print(" Right ct: ");
    Serial.print(Rcounter);
    Serial.print(" Sensors: ");
    Serial.print(sensor_data[0]);
    Serial.print(", ");
    Serial.print(sensor_data[1]);
    Serial.print(", ");
    Serial.print(sensor_data[2]);
    Serial.print(", ");
    Serial.print(sensor_data[3]);

    Serial.print(wallmask & WALL_LEFT ? " L" : "  ");
    Serial.print(wallmask & WALL_FRONT ? "F" : " ");
    Serial.print(wallmask & WALL_RIGHT ? "R " : "  ");

    Serial.print(deviation);
    Serial.print(", ");
    Serial.print(curHd);

    //    Serial.print(gyroAngle);

    Serial.print(" Micros: ");
    Serial.println(micros());
}


