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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// Variables globales
volatile unsigned int varTicks = 0;

volatile unsigned int ticks100us = 0;
volatile unsigned int ticks1ms = 0;
volatile unsigned int ticks10ms = 0;

void systemInit(void);
void ticks(void);

void main(void) {
    
    systemInit();   //INICIALIZACION DE MODULOS REQUERIDOS POR LA APP (Config de timers, interrupciones, GPIO entrda/salida)
    appInit();      //INICIALIZACION DE ESTADOS INICIALES DE SALIDAS, VALOR INICIAL TIMERS, FLAGS
    while(true)
    {
        appTask();  //APP CON LA MAQUINA DE ESTADO Y TRANSICIONES REQUERIDAS POR EL TP
    }
    
    return;
}


void systemInit(void)
{
    //CONFIG PUERTOS
    SERIE_TRIS = 1;     //Entrada dato serie
    PULSADOR_TRIS = 1;  //Entrada pulsador
    BUZZER_TRIS = 0;    //Salida para buzzer
    LED_TRIS = 0;       //Salida para LED
    
    
    //CONFIG TIMERS
    
    
    //CONFIG INTERRUPCIONES
    GIE = 1;      // Habilitar las interrupciones globales
    PEIE = 1;     // Habilitar las interrupciones de periféricos para el TMR1
    TMR1IE = 1;   // Habilitar la interrupción del Timer1 para NO ME ACUERDO
    TMR0IE = 1;     //Habilitacion interrupcion Timer0 para TAMPOCO ME ACUERDO
    
    
    //TMR1ON = 1;
    
}

//ACA VA EL MANEJO DE INTERRUPCIONES
//HAY QUE VER COMO LAS ESPECIFICA EL COMPILADOR XC8
//HAY DOCUMENTACION EN LA PAGINA DE MICROCHIP SOBRE EL XC8
void __interrupt() isr(void)
{
    if (TMR1IF)
    {
        TMR1IF = 0;  // Limpiar la bandera de interrupción del Timer1
        TMR1H = 0x00;
        TMR1L = 0x00;
        ticks();     // Llamar a la función ticks()
    }
    
    if(TMR0IF)
    {
        TMR0IF = 0;
        //REINICIO LOS VALORES DEl TIMER0
    }
}

void ticks(void) {
    varTicks++;  // Incrementar la variable varTicks
    // Puedes agregar más código aquí según tus necesidades
    LED_OUT = ~LED_OUT;
    ticks100us++;
    
    if((varTicks%10) == 0)
    {
        
        ticks1ms++;
    }
    if((varTicks%100) == 0)
    {
        ticks10ms++;
    }
}
