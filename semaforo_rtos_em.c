#include "SanUSB48.h"
#include <osa.h>

#define DIGIT1 PORTCbits.RC0
#define DIGIT2 PORTCbits.RC1

unsigned char count = 0, botao = 0, wait = 0;
unsigned char SEGMENT[ ] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
unsigned int i = 0, d, u;

#pragma interrupt interrupcao 

void interrupcao() {

    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0;
        TMR1H = 0xD8;
        TMR1L = 0xF0;
        OS_Timer();
    }
}

void PIC_Init(void) {
    LATB = 0x00;
    TRISB = 0x00;

    T1CON = 0x80;
    TMR1H = 0xD8;
    TMR1L = 0xF0;

    INTCON = 0;
    INTCONbits.PEIE = 1;
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1;
    T1CONbits.TMR1ON = 1;
}

void Task_1(void) {
    while (1) {
        if (botao == 0) {
            nivel_alto(pin_a0);
            nivel_baixo(pin_a1);
            nivel_baixo(pin_a2);
            nivel_baixo(pin_a3);
            nivel_alto(pin_a4);
        } else {
            nivel_baixo(pin_a0);
            nivel_alto(pin_a1);
            nivel_alto(pin_a4);

            OS_Delay(3000);

            nivel_baixo(pin_a1);
            nivel_baixo(pin_a4);
            nivel_alto(pin_a2);
            nivel_alto(pin_a3);

            count = 30;
            OS_Delay(15000);

            botao = 0;
            wait = 1;
        }

        OS_Delay(1);
    }
}

void Task_2(void) {
    while (1) {
        if (wait == 1) {
            OS_Delay(5000);
            wait = 0;
        }

        if (!entrada_pin_e3) {
            botao = 1;
        }

        OS_Delay(1);
    }
}

void Task_3(void) {
    while (1) {
        while (count > 0) {
            for (i = 0; i < 50; i++) {
                DIGIT2 = 0;
                d = count / 10;
                PORTB = SEGMENT[d];
                DIGIT1 = 1;
                OS_Delay(5);

                DIGIT1 = 0;
                u = count % 10;
                PORTB = SEGMENT[u];
                DIGIT2 = 1;
                OS_Delay(5);
            }

            count--;
        }

        if (count == 0) {
            OS_Delay(1);
            PORTB = 0x7F;
        }
    }
}

void main(void) {
    clock_int_48MHz();

    PIC_Init();
    OS_Init();

    TRISA = 0x00;
    TRISC = 0x00;

    OS_Task_Create(1, Task_1);
    OS_Task_Create(1, Task_2);
    OS_Task_Create(1, Task_3);

    OS_EI();
    OS_Run();
}