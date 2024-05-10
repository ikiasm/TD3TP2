/*
 * File:   main.c
 * Author: marcelo
 *
 * Created on April 12, 2024, 9:00 PM
 */


#include <xc.h>
#include "app.h"
#include <stdbool.h>

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // GP3/MCLR pin function select (GP3/MCLR pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)




// Variables globales
volatile  uint16_t varTicks = 0;

volatile  uint8_t ticks1s = 0;
volatile  uint8_t ticks1M = 0;


void systemInit(void);
void ticks(void);

void main(void) {
    __delay_ms(200);
    systemInit();   //INICIALIZACION DE MODULOS REQUERIDOS POR LA APP (Config de timers, interrupciones, GPIO entrda/salida)
    appInit();      //INICIALIZACION DE ESTADOS INICIALES DE SALIDAS, VALOR INICIAL TIMERS, FLAGS
    while(1)
    {
        appTask();  //APP CON LA MAQUINA DE ESTADO Y TRANSICIONES REQUERIDAS POR EL TP
        
    }
    
    return;
}


void systemInit(void)
{
    //APAGADO CMP
    CMCONbits.CM0 = 1;
    CMCONbits.CM1 = 1;
    CMCONbits.CM2 = 1;
    
    //CONFIG PUERTOS
    SERIE_RX_TRIS = 1;      //RX serie
    SERIE_TX_TRIS = 0;      //TX serie
    PULSADOR_TRIS = 1;      //Entrada pulsador
    BUZZER_TRIS = 0;        //Salida para buzzer
    LED_TRIS = 0;           //Salida para LED    
    
    //CLOCK CALIBRACION
    OSCCALbits.CAL = 0b11111100;
    
    
    //CONFIG TIMER0 para buzzer
    OPTION_REGbits.PS0 = 1;//1;     //PS = 0x000 => 1:4
    OPTION_REGbits.PS1 = 0;
    OPTION_REGbits.PS2 = 0;
    OPTION_REGbits.PSA = 0;//0;     //Prescaler asignado a TMR0
    OPTION_REGbits.T0SE = 1;    //No importa
    OPTION_REGbits.T0CS = 0;    //TMR0 Internal clock source
    
    //CONFIG TIMER1 para tiempos grales
    T1CONbits.T1CKPS0 = 1;      //Prescaler actual 1:8 anterior 1:2
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.TMR1CS = 0;       //TMR1 Internal Clock (Fosc/4))
    
    //CONFIG EXT INT
    OPTION_REGbits.INTEDG = 0;
    INTCONbits.INTE = 1;
    
    //CONFIG INTERRUPCIONES
    INTCONbits.PEIE = 1;        // Interrupciones de periféricos para el TMR1
    INTCONbits.T0IE = 1;        // Interrupcion TMR0
    PIE1bits.TMR1IE = 1;//1;        // Interrupción del TMR1
    INTCONbits.GIE = 1;         // Interrupciones globales
    T1CONbits.TMR1ON = 1;
    
}


void __interrupt() isr(void)
{
    
    if(INTCONbits.T0IF)       //INT para Geenerador de frecuencia
    {
        //LED_OUT = !LED_OUT;
        buzzer();
        TMR0 = 0xFF - valueTimer0;
        INTCONbits.T0IF = 0;
        
    }
    
    if(PIR1bits.T1IF)         // INT para cuentas generales
    {
        
        ticks();
        TMR1 = valueTimer1;
        PIR1bits.T1IF = 0;
        
        
    }
    
    if(INTCONbits.INTF)
    {
        uart_Rx();
        INTCONbits.INTF = 0;
    }
    
    
    
}

void ticks(void) {
    varTicks++;  
    tick++;
    // Puedes agregar más código aquí según tus necesidades siempre que no sean
    // bloqueantes
 
    if((varTicks%500) == 0)
    {
        
        flag1s = true;
        ticks1s++;
    }
    if((varTicks%32000) == 0)
    {
        flag1M = true;
        ticks1M++;
        varTicks = 0;
    }   
}

