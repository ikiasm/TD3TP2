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

#define PULSADOR_TRIS   TRISIObits.TRISIO1
#define SERIE_TRIS      TRISIObits.TRISIO2
#define LED_TRIS        TRISIObits.TRISIO4
#define BUZZER_TRIS     TRISIObits.TRISIO5

#define PULSADOR_IN GPIObits.GP1
#define SERIE_IN    GPIObits.GP2
#define LED_OUT     GPIObits.GP4
#define BUZZER_OUT  GPIObits.GP5

void appInit(void);
void appTask(void);
