#include <xc.h>
#include "app.h"


void appInit()
{
    //Aca ponemos la inicializacion de la app, estados iniciales etc
    BUZZER_OUT = 0;
    LED_OUT = 1;
    SERIE_TX = 1;
    
    valueTimer0 = 0xFF - 0x80;      //128 cuentas de 4us para 1 kHz
    valueTimer1 = 0xFFFF - 0x8B;// - 0x01F4;  //500 cuentas de 2us para 1 ms con ps 1:2
    
    //Encender los Timers si fuese necesario activarlo desde el principio
}

void appTask()
{
    static char estado = REPOSO;
    static bool retencion = false;
    
    
    if(!PULSADOR_IN && estado == REPOSO && retencion == false)
    {
        __delay_ms(50);
        if(!PULSADOR_IN)
        {
            INTCONbits.GIE = 1;
            INTCONbits.T0IE = 1;        
            PIE1bits.TMR1IE = 1;
            T1CONbits.TMR1ON = 1;
            retencion = true;
            flag1s = true;
            estado = ACTIVO;
        }  
    }
    
    switch(estado)
    {
        case REPOSO:
        {
            INTCONbits.GIE = 0;
            INTCONbits.T0IE = 0;        
            PIE1bits.TMR1IE = 0;
            T1CONbits.TMR1ON = 0;   //Apagamos el timer
            BUZZER_OUT = 0;
            LED_OUT = 1;
            break;
        }
        case ACTIVO:
        {
            
            
            if(!PULSADOR_IN && retencion == true && flag1s)
            {
                
                blinkingLed();
                freqOut();          //seleccion de Frecuencia y envio de dato
                flag1s = false;
                
                
            }
            if(PULSADOR_IN && retencion == true)
            {
                retencion = false;
                estado = APAGADO;
            }
            break;
        }
        case APAGADO:
        {
            //T1CONbits.TMR1ON = 1;
            if(PULSADOR_IN && flag1M == true)
            {
                INTCONbits.GIE = 0;
                INTCONbits.T0IE = 0;        
                PIE1bits.TMR1IE = 0;
                T1CONbits.TMR1ON = 0;
                estado = REPOSO;
                //buscar como inicializar nuevamente el case 0: el freqOut()
            }
            if(!PULSADOR_IN && retencion == false && estado == APAGADO)
            {
                __delay_ms(100);
                INTCONbits.GIE = 0;
                INTCONbits.T0IE = 0;        
                PIE1bits.TMR1IE = 0;
                T1CONbits.TMR1ON = 0;
                BUZZER_OUT = 0;
                LED_OUT = 1;
                estado = REPOSO;
            }
            break;
        }
        default:
            break;
    }
}

void blinkingLed(void)
{
    LED_OUT = 0;
    __delay_ms(100);
    LED_OUT = 1;
}

void freqOut(void)
{
    static char paso = 0;//F1K;
    switch(paso)
    {
        case 0://F1K:
        {
            valueTimer0 = 0xFF - 0x80;    //128 cuentas para overflow
            freq = 0x03E8;  //1kHz
            uart_Tx(freq);
            paso++;
            break;
        }
        case 1://F2K:
        {
            valueTimer0 = 0xFF - 0x40;     //64 para overflow
            freq = 0x07D0;  //2kHz
            uart_Tx(freq);
            paso++;
            break;
        }
        case 2://F4K:
        {
            valueTimer0 = 0xFF - 0x20;     //32 para overflow
            freq = 0x0FA0;  //4kHz
            uart_Tx(freq);
            paso++;
            break;
        }
        case 3://F8K:
        {
            valueTimer0 = 0xFF - 0x10;     //16 para overflow
            freq = 0x0140;  //8kHz
            uart_Tx(freq);
            paso++;
            break;
        }
        case 4://F16K:
        {
            valueTimer0 = 0xFF - 0x08;     //8 para overflow
            freq = 0x3E80;  //16kHz
            uart_Tx(freq);
            paso = 0;
            break;
        }
        default:
            break;
    }  
    
}

void buzzer(void)
{
    BUZZER_OUT = !BUZZER_OUT;
}

void uart_Tx(unsigned int dato)
{
    unsigned char i = 0;
    unsigned char datoL;
    unsigned char datoH;

    INTCONbits.GIE = 0;
    INTCONbits.T0IE = 0;        
    PIE1bits.TMR1IE = 0;
    T1CONbits.TMR1ON = 0;   //Apagamos el timer
    
    datoH = dato / 0x100;
    datoL = dato - (datoH * 0x100);
    //Envio datoH
    SERIE_TX = 0;   //Start bit
    __delay_us(124);
    
    for(i=0 ; i<8 ; i++)
    {
        SERIE_TX = datoH >> i & 0x01;
        __delay_us(114);
    }
    SERIE_TX = 1;   //Stop bit
     //__delay_us(508);
    //Envio datoL
    SERIE_TX = 0;   //Start bit
    __delay_us(124);
    
    for(i=0 ; i<8 ; i++)
    {
        SERIE_TX = datoL >> i & 0x01;
        __delay_us(114);
    }
    
    INTCONbits.GIE = 1;
    INTCONbits.T0IE = 1;        
    PIE1bits.TMR1IE = 1;
    T1CONbits.TMR1ON = 1;   //Apagamos el timer
    
//    SERIE_TX = 1;   //Stop bit
//    __delay_us(204);
}

void uart_Rx(void)
{
    static unsigned char dato = 0;
    static char i = 0;
    INTCONbits.GIE = 0;
    INTCONbits.T0IE = 0;        
    PIE1bits.TMR1IE = 0;
    T1CONbits.TMR1ON = 0;   //Apagamos el timer   
    __delay_us(150);
    for(i = 0 ; i < 8 ; i++)
    {
        dato = dato + SERIE_RX * (0x01 << i);
        __delay_us(124);
    }
    if(!dato == 0x1B)
    {   
        INTCONbits.GIE = 1;
        INTCONbits.T0IE = 1;        
        PIE1bits.TMR1IE = 1;
        T1CONbits.TMR1ON = 1;   //Apagamos el timer
    }
    
}
