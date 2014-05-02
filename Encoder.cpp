#include "Encoder.h"
#include "Arduino.h"

typedef struct{
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

void initEncoders(uint8_t outA1, uint8_t outB1, uint8_t outA2, uint8_t outB2){
    m_encoder.outA1 = outA1;
    m_encoder.outB1 = outB1;
    m_encoder.outA2 = outA2;
    m_encoder.outB2 = outB2;

    m_encoder.state1 = digitalRead(outA1);
    m_encoder.state2 = digitalRead(outB1);
    m_encoder.state3 = digitalRead(outA2);
    m_encoder.state4 = digitalRead(outB2);

    attachInterrupt(outA1, iterate1, CHANGE);
    attachInterrupt(outB1, iterate2, CHANGE);
    attachInterrupt(outA2, iterate3, CHANGE);
    attachInterrupt(outB2, iterate4, CHANGE);
}


void iterate1(){
    m_encoder.state1= !m_encoder.state1;
    if(!m_encoder.state1){
        if(!m_encoder.state2){
            Lcounter++;
        }
        else{
            Lcounter--;
        }
    }
    if(m_encoder.state1){
        if(m_encoder.state2){
            Lcounter++;
        }
        else{
            Lcounter--;
        }
    }
}

void iterate2(){
    m_encoder.state2= !m_encoder.state2;
    if(!m_encoder.state2){
        if(m_encoder.state1){
            Lcounter++;
        }
        else{
            Lcounter--;
        }
    }
    if(m_encoder.state2){
       if(!m_encoder.state1){
            Lcounter++;
       }
       else{
            Lcounter--;
       }
    }
}

void iterate3(){
    m_encoder.state3= !m_encoder.state3;
    if(!m_encoder.state3){
        if(!m_encoder.state4){
            Rcounter++;
        }
        else{
            Rcounter--;
        }
    }
    if(m_encoder.state3){
        if(m_encoder.state4){
            Rcounter++;
        }
        else{
            Rcounter--;
        }
    }
}

void iterate4(){
    m_encoder.state4= !m_encoder.state4;
    if(!m_encoder.state4){
        if(m_encoder.state3){
            Rcounter++;
        }
        else{
            Rcounter--;
        }
    }
    if(m_encoder.state4){
       if(!m_encoder.state3){
            Rcounter++;
       }
       else{
            Rcounter--;
       }
    }
}


void resestCounter(uint8_t encoder){
    if(encoder&Lencoder){
        Lcounter=0;
    }
    else{
        Rcounter=0;
    }
}

