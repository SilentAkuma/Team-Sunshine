#include "Encoder.h"
#include "Arduino.h"

#define ENC_BUFFER_SIZE 10

void iterate1();
void iterate2();
void iterate3();
void iterate4();

typedef struct
{
    uint8_t outA1;
    uint8_t outB1;
    uint8_t outA2;
    uint8_t outB2;
    uint8_t state1;
    uint8_t state2;
    uint8_t state3;
    uint8_t state4;
} encoder_data;

static encoder_data m_encoder;

int32_t Lcounter = 0;
int32_t Rcounter = 0;

uint8_t LHeadIndex, LTailIndex, RHeadIndex, RTailIndex;

typedef struct
{
    int32_t m_micros;
    int32_t m_count;
} micros_and_enc_ct_t;

//micros_and_enc_ct_t *LMicrosBuffer, *RMicrosBuffer;
micros_and_enc_ct_t LMicrosBuffer[ENC_BUFFER_SIZE];
micros_and_enc_ct_t RMicrosBuffer[ENC_BUFFER_SIZE];

uint8_t microsBufferSize = ENC_BUFFER_SIZE;

void initEncoders(uint8_t outA1, uint8_t outB1, uint8_t outA2, uint8_t outB2, uint8_t bufferSize)
{
//    LMicrosBuffer = (micros_and_enc_ct_t*)calloc(bufferSize, sizeof(micros_and_enc_ct_t));
//    RMicrosBuffer = (micros_and_enc_ct_t*)calloc(bufferSize, sizeof(micros_and_enc_ct_t));

    for(uint8_t i = 0; i < ENC_BUFFER_SIZE; i++)
    {
        LMicrosBuffer[i].m_count = 0;
        LMicrosBuffer[i].m_micros = 10*i;
        RMicrosBuffer[i].m_count = 0;
        RMicrosBuffer[i].m_micros = 10*i;
    }

    LHeadIndex = RHeadIndex = bufferSize-1;
    LTailIndex = RTailIndex = 0;
    microsBufferSize = bufferSize;

    m_encoder.outA1 = outA1;
    m_encoder.outB1 = outB1;
    m_encoder.outA2 = outA2;
    m_encoder.outB2 = outB2;

    pinMode(outA1, INPUT);
    pinMode(outB1, INPUT);
    pinMode(outA2, INPUT);
    pinMode(outB2, INPUT);

    m_encoder.state1 = digitalRead(outA1);
    m_encoder.state2 = digitalRead(outB1);
    m_encoder.state3 = digitalRead(outA2);
    m_encoder.state4 = digitalRead(outB2);

    attachInterrupt(outA1, iterate1, CHANGE);
    attachInterrupt(outB1, iterate2, CHANGE);
    attachInterrupt(outA2, iterate3, CHANGE);
    attachInterrupt(outB2, iterate4, CHANGE);
}

void cleanupEncoders(void)
{
//    free(LMicrosBuffer);
//    free(RMicrosBuffer);
}

void IncrementLCounter(void)
{
    Lcounter++;

    LHeadIndex++;
    if(LHeadIndex == microsBufferSize)
    {
        LHeadIndex = 0;
    }
    LTailIndex++;
    if(LTailIndex == microsBufferSize)
    {
        LTailIndex = 0;
    }

    LMicrosBuffer[LHeadIndex].m_micros = micros();
    LMicrosBuffer[LHeadIndex].m_count = Lcounter;
}

void IncrementRCounter(void)
{
    Rcounter++;

    RHeadIndex++;
    if(RHeadIndex == microsBufferSize)
    {
        RHeadIndex = 0;
    }
    RTailIndex++;
    if(RTailIndex == microsBufferSize)
    {
        RTailIndex = 0;
    }

    RMicrosBuffer[RHeadIndex].m_micros = micros();
    RMicrosBuffer[RHeadIndex].m_count = Rcounter;
}

void DecrementLCounter(void)
{
    Lcounter--;

    LHeadIndex++;
    if(LHeadIndex == microsBufferSize)
    {
        LHeadIndex = 0;
    }
    LTailIndex++;
    if(LTailIndex == microsBufferSize)
    {
        LTailIndex = 0;
    }

    LMicrosBuffer[LHeadIndex].m_micros = micros();
    LMicrosBuffer[LHeadIndex].m_count = Lcounter;
}

void DecrementRCounter(void)
{
    Rcounter--;

    RHeadIndex++;
    if(RHeadIndex == microsBufferSize)
    {
        RHeadIndex = 0;
    }
    RTailIndex++;
    if(RTailIndex == microsBufferSize)
    {
        RTailIndex = 0;
    }

    RMicrosBuffer[RHeadIndex].m_micros = micros();
    RMicrosBuffer[RHeadIndex].m_count = Rcounter;
}

void iterate1()
{
    m_encoder.state1= !m_encoder.state1;
    if(!m_encoder.state1)
    {
        if(!m_encoder.state2)
        {
            //Lcounter++;
            IncrementLCounter();
        }
        else
        {
            //Lcounter--;
            DecrementLCounter();
        }
    }
    if(m_encoder.state1)
    {
        if(m_encoder.state2)
        {
            //Lcounter++;
            IncrementLCounter();
        }
        else
        {
            //Lcounter--;
            DecrementLCounter();
        }
    }
}

void iterate2()
{
    m_encoder.state2= !m_encoder.state2;
    if(!m_encoder.state2)
    {
        if(m_encoder.state1)
        {
            //Lcounter++;
            IncrementLCounter();
        }
        else
        {
            //Lcounter--;
            DecrementLCounter();
        }
    }
    if(m_encoder.state2)
    {
        if(!m_encoder.state1)
        {
            //Lcounter++;
            IncrementLCounter();
        }
        else
        {
            //Lcounter--;
            DecrementLCounter();
        }
    }
}

void iterate3()
{
    m_encoder.state3= !m_encoder.state3;
    if(!m_encoder.state3)
    {
        if(!m_encoder.state4)
        {
            //Rcounter++;
            IncrementRCounter();
        }
        else
        {
            //Rcounter--;
            DecrementRCounter();
        }
    }
    if(m_encoder.state3)
    {
        if(m_encoder.state4)
        {
            //Rcounter++;
            IncrementRCounter();
        }
        else
        {
            //Rcounter--;
            DecrementRCounter();
        }
    }
}

void iterate4()
{
    m_encoder.state4= !m_encoder.state4;
    if(!m_encoder.state4)
    {
        if(m_encoder.state3)
        {
            //Rcounter++;
            IncrementRCounter();
        }
        else
        {
            //Rcounter--;
            DecrementRCounter();
        }
    }
    if(m_encoder.state4)
    {
        if(!m_encoder.state3)
        {
            //Rcounter++;
            IncrementRCounter();
        }
        else
        {
            //Rcounter--;
            DecrementRCounter();
        }
    }
}


void resetCounter(uint8_t encoder)
{
    if(encoder&ENC_L_ENCODER)
    {
        Lcounter=0;
    }
    if(encoder&ENC_R_ENCODER)
    {
        Rcounter=0;
    }
}

float getLWheelVelocity()
{
    if((micros() - LMicrosBuffer[LHeadIndex].m_micros) > (2*(LMicrosBuffer[LHeadIndex].m_micros - LMicrosBuffer[LTailIndex].m_micros)))
    {
        return 0;
    }

    float ret = ((float)(LMicrosBuffer[LHeadIndex].m_count - LMicrosBuffer[LTailIndex].m_count))/
                ((LMicrosBuffer[LHeadIndex].m_micros - LMicrosBuffer[LTailIndex].m_micros)*0.000001f)/ENC_FULL_ROT_TICKS;
#ifdef ENC_L_WHEEL_INVERTED
    return -ret;
#else
    return ret;
#endif // ENC_L_WHEEL_INVERTED
}

float getRWheelVelocity()
{
    if((micros() - RMicrosBuffer[RHeadIndex].m_micros) > (2*(RMicrosBuffer[RHeadIndex].m_micros - RMicrosBuffer[RTailIndex].m_micros)))
    {
        return 0;
    }

    float ret = ((float)(RMicrosBuffer[RHeadIndex].m_count - RMicrosBuffer[RTailIndex].m_count))/
                ((RMicrosBuffer[RHeadIndex].m_micros - RMicrosBuffer[RTailIndex].m_micros)*0.000001f)/ENC_FULL_ROT_TICKS;
#ifdef ENC_R_WHEEL_INVERTED
    return -ret;
#else
    return ret;
#endif // ENC_R_WHEEL_INVERTED
}
