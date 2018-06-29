#include <xc.h>
#include "CNY.h"

const unsigned char channels[] = {0, 1, 2, 11, 3, 9, 8, 4, 10};
unsigned char actualChannel = 0;
unsigned int data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void initCNY(void){
    ADCON1 = 0x0A;
    ADCON2 = 0xBE;
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    ADCON0bits.CHS = channels[actualChannel];
    ADCON0bits.ADON = 1;
    ADCON0bits.GO = 1;
}

void cnyInterrupt(void){
    PIR1bits.ADIF = 0;
    data[actualChannel++] = ADRESH << 8 + ADRESL;
    actualChannel %= 9;
    ADCON0bits.CHS = channels[actualChannel];
    ADCON0bits.GO = 1;
}

unsigned int getCNY(unsigned char channel){
    if(channel > 9) return 0;
    return data[channel];
}