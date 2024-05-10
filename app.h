/* 
 * File:   app.h
 * Author: marcelo
 *
 * Created on April 19, 2024, 7:22 PM
 */

#ifndef APP_H
#define	APP_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* APP_H */
#include <xc.h>
#include <stdbool.h>

#define _XTAL_FREQ 4000000

//defines TRIS

#define LED_TRIS        TRISIObits.TRISIO4
#define BUZZER_TRIS     TRISIObits.TRISIO5
#define SERIE_TX_TRIS   TRISIObits.TRISIO0

#define SERIE_RX_TRIS   TRISIObits.TRISIO2
#define PULSADOR_TRIS   TRISIObits.TRISIO1

//defines GPIO

#define LED_OUT     GPIObits.GP4
#define BUZZER_OUT  GPIObits.GP5
#define SERIE_TX    GPIObits.GP0

#define SERIE_RX    GPIObits.GP2
#define PULSADOR_IN GPIObits.GP1



//defines estados
#define REPOSO  0
#define ACTIVO  1
#define APAGADO 2

//defines PASOS DE FRECUENCIA
#define F1K     0
#define F2K     1
#define F4K     2
#define F8K     3
#define F16K    4

volatile bool flag1s = false;
volatile bool flag1M = false;
volatile unsigned int tick = 0x0000;
volatile unsigned char valueTimer0 = 0x00;
volatile unsigned int valueTimer1 = 0x0000;
volatile unsigned int freq = 0;

void appInit(void);
void appTask(void);


void blinkingLed(void);
void freqOut(void);
void buzzer(void);

void uart_Tx(unsigned int);
void uart_Rx(void);
