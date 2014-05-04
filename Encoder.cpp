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
#ifndef ENC_L_WHEEL_INVERTED
    if(!digitalRead(m_encoder.outA1))
    {
        if(!digitalRead(m_encoder.outB1))
        {
            IncrementLCounter();
        }
        else
        {
            DecrementLCounter();
        }
    }
    else
    {
        if(digitalRead(m_encoder.outB1))
        {
            IncrementLCounter();
        }
        else
        {
            DecrementLCounter();
        }
    }
#else
    if(!digitalRead(m_encoder.outA1))
    {
        if(!digitalRead(m_encoder.outB1))
        {
            DecrementLCounter();
        }
        else
        {
            IncrementLCounter();
        }
    }
    else
    {
        if(digitalRead(m_encoder.outB1))
        {
            DecrementLCounter();
        }
        else
        {
            IncrementLCounter();
        }
    }
#endif
}

void iterate2()
{
#ifndef ENC_L_WHEEL_INVERTED
    if(!digitalRead(m_encoder.outB1))
    {
        if(digitalRead(m_encoder.outA1))
        {
            IncrementLCounter();
        }
        else
        {
            DecrementLCounter();
        }
    }
    else
    {
        if(!digitalRead(m_encoder.outA1))
        {
            IncrementLCounter();
        }
        else
        {
            DecrementLCounter();
        }
    }
#else
    if(!digitalRead(m_encoder.outB1))
    {
        if(digitalRead(m_encoder.outA1))
        {
            DecrementLCounter();
        }
        else
        {
            IncrementLCounter();
        }
    }
    else
    {
        if(!digitalRead(m_encoder.outA1))
        {
            DecrementLCounter();
        }
        else
        {
            IncrementLCounter();
        }
    }
#endif
}

void iterate3()
{
#ifndef ENC_R_WHEEL_INVERTED
    if(!digitalRead(m_encoder.outA2))
    {
        if(!digitalRead(m_encoder.outB2))
        {
            IncrementRCounter();
        }
        else
        {
            DecrementRCounter();
        }
    }
    else
    {
        if(digitalRead(m_encoder.outB2))
        {
            IncrementRCounter();
        }
        else
        {
            DecrementRCounter();
        }
    }
#else
    if(!digitalRead(m_encoder.outA2))
    {
        if(!digitalRead(m_encoder.outB2))
        {
            DecrementRCounter();
        }
        else
        {
            IncrementRCounter();
        }
    }
    else
    {
        if(digitalRead(m_encoder.outB2))
        {
            DecrementRCounter();
        }
        else
        {
            IncrementRCounter();
        }
    }
#endif
}

void iterate4()
{
#ifndef ENC_R_WHEEL_INVERTED
    if(!digitalRead(m_encoder.outB2))
    {
        if(digitalRead(m_encoder.outA2))
        {
            IncrementRCounter();
        }
        else
        {
            DecrementRCounter();
        }
    }
    else
    {
        if(!digitalRead(m_encoder.outA2))
        {
            IncrementRCounter();
        }
        else
        {
            DecrementRCounter();
        }
    }
#else
    if(!digitalRead(m_encoder.outB2))
    {
        if(digitalRead(m_encoder.outA2))
        {
            DecrementRCounter();
        }
        else
        {
            IncrementRCounter();
        }
    }
    else
    {
        if(!digitalRead(m_encoder.outA2))
        {
            DecrementRCounter();
        }
        else
        {
            IncrementRCounter();
        }
    }
#endif
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
    if((micros() - LMicrosBuffer[LHeadIndex].m_micros) > (2*(LMicrosBuffer[LHeadIndex].m_micros - LMicrosBuffer[LTailIndex].m_micros)/ENC_BUFFER_SIZE))
    {
        return 0;
    }

    float ret = ((float)(LMicrosBuffer[LHeadIndex].m_count - LMicrosBuffer[LTailIndex].m_count))/
                ((LMicrosBuffer[LHeadIndex].m_micros - LMicrosBuffer[LTailIndex].m_micros)*0.000001f)/ENC_FULL_ROT_TICKS;
    return ret;
}

float getRWheelVelocity()
{
    if((micros() - RMicrosBuffer[RHeadIndex].m_micros) > (2*(RMicrosBuffer[RHeadIndex].m_micros - RMicrosBuffer[RTailIndex].m_micros)/ENC_BUFFER_SIZE))
    {
        return 0;
    }

    float ret = ((float)(RMicrosBuffer[RHeadIndex].m_count - RMicrosBuffer[RTailIndex].m_count))/
                ((RMicrosBuffer[RHeadIndex].m_micros - RMicrosBuffer[RTailIndex].m_micros)*0.000001f)/ENC_FULL_ROT_TICKS;
    return ret;
}
