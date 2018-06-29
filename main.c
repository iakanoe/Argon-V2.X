#include "pragma.h"
#include "timer.h"
#include "ui.h"
#include "motors.h"
#include "CNY.h"
#include <xc.h>

enum {
    MENU,
    CAL,
    READY,
    RUN
};

void loop(){
    static char status = MENU;
    static unsigned long next = 0;
    static char cal = 0;
    static int maxSpeed;
    int pid = calcPID();
    switch(status){
        case MENU:
            setMotors(0, 0);
            setLEDR(1);
            setLEDY(0);
            if(cal){
                setLEDG(pid == 0);
                if(getBTN2()){
                    status = READY;
                    maxSpeed = ARGON_SPEED;
                    break;
                }
                if(getBTN3()){
                    status = READY;
                    maxSpeed = LENTO_SPEED;
                    break;
                }
            }
            if(getBTN1()) status = CAL;
            break;
            
        case CAL:
            setLEDs(0, 1, 0);
            cal = 1;
            calibrar();
            if(!getBTN1()) status = READY;
            break;
            
        case READY:
            break;
            
        case RUN:
            if(pid < 0) setMotors((maxSpeed - pid), maxSpeed);
            else setMotors(maxSpeed, (maxSpeed - pid));
            break;
    }
}

int values[9];
int max[9];
int min[9];

const float Kp = 0.0;
const float Ki = 0.0;
const float Kd = 0.0;
const int setpoint = 500;

int calcPID(){
    static int prevErr, err, integral; 
    unsigned long long s1 = 0, s2 = 0;
    for(int i = 0; i < 9; i++){
        s1 += values[i] * 100 * (i + 1);
        s2 += values[i];
    }
    int pos = (s1 / s2); // promedio ponderado | posición respecto a la linea
    prevErr = err; // guarda el error antiguo para derivada
    err = setpoint - pos; // calcula el error
    integral += err; // suma el error a la integral
    return ((int) (Kp * (float) err) + (Ki * (float) integral) + (Kd * (float) (err - prevErr)));
    //calcula el PID y lo devuelve
}


void initInterrupts(void){
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

void main(){
    initInterrupts();
    initTimer();
    initUI();
    initMotors();
    initCNY();
    while(1) loop();
}
void interrupt ISR(void){
    timerInterrupt();
    cnyInterrupt();
}
