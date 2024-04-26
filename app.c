ado
#include <xc.h>
#include "app.h"

void appInit()
{
    //Aca ponemos la inicializacion de la app, estados iniciales etc
    BUZZER_OUT = 0;
    LED_OUT = 0;
    TMR0 = 0x00;    //Aca va el valor inicial calculado
    TMR1 = 0x0000;  //aca va el valor inicial calculado
}

void appTask()
{
    static char estado = 0;

    switch(estado)
        {
            case 0:
                {
                    break;
                }
            case 1:
                {
                    break;
                }
            default:
                break;
        }
}
