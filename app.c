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
            freq = 0x03E8;  //1kHz valor en hexa para enviar por uart_Tx()
            uart_Tx(freq);
            paso++;
            break;
        }
        case 1://F2K:
        {
            valueTimer0 = 0xFF - 0x40;     //64 para overflow
            freq = 0x07D0;  //2kHz valor en hexa para enviar por uart_Tx()
            uart_Tx(freq);
            paso++;
            break;
        }
        case 2://F4K:
        {
            valueTimer0 = 0xFF - 0x20;     //32 para overflow
            freq = 0x0FA0;  //4kHz valor en hexa para enviar por uart_Tx()
            uart_Tx(freq);
            paso++;
            break;
        }
        case 3://F8K:
        {
            valueTimer0 = 0xFF - 0x10;     //16 para overflow
            freq = 0x0140;  //8kHz valor en hexa para enviar por uart_Tx()
            uart_Tx(freq);
            paso++;
            break;
        }
        case 4://F16K:
        {
            valueTimer0 = 0xFF - 0x08;     //8 para overflow
            freq = 0x3E80;  //16kHz valor en hexa para enviar por uart_Tx()
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
    
    //APAGO TODAS LAS INTERRUPCIONES PARA NO TENER PROBLEMAS
    INTCONbits.GIE = 0;
    INTCONbits.T0IE = 0;        
    PIE1bits.TMR1IE = 0;
    T1CONbits.TMR1ON = 0;   //Apagamos el timer
    
    //dato es de 16bits, lo separo en datoH y datoL para mandarlos en 8 bits. Ej: dato = 0x03E8
    datoH = dato / 0x100;              //Aca datoH = dato (que es de 16bits) /0x100 (Que lo desplaza 8 bits a la derecha) => datoH = 0x03
    datoL = dato - (datoH * 0x100);    //Aca datoL = 0x03E8 - 0x0300 => datoL = 0xE8
    
    //Envio datoH
    SERIE_TX = 0;   //Start bit
    __delay_us(124);        //Delay para el Start bit
    
    for(i=0 ; i<8 ; i++)
    {
        SERIE_TX = datoH >> i & 0x01;    //cada vuelta el valor de datoH lo desplazo 'i' lugares y le hago un AND 1 para que solo me tome el bit mas bajo
        __delay_us(114);                // la espera de rigor para 9600 baudios
    }
    SERIE_TX = 1;   //Stop bit    
     //__delay_us(508);    -> Delay para separar byte a byte. Este delay lo saque porque andaba mejor el envio, en principio deberia haber funcionado
    
    //Envio datoL
    SERIE_TX = 0;   //Start bit
    __delay_us(124);
    
    for(i=0 ; i<8 ; i++)
    {
        SERIE_TX = datoL >> i & 0x01;
        __delay_us(114);
    }
    SERIE_TX = 1;    //Stop bit 
    //Activo nuevamente todas las interrupciones
    INTCONbits.GIE = 1;
    INTCONbits.T0IE = 1;        
    PIE1bits.TMR1IE = 1;
    T1CONbits.TMR1ON = 1;
    
//    SERIE_TX = 1;   //Stop bit    => esto quedo asi, pero lo correcto es que quede en alto como lo puse en la linea 201
//    __delay_us(204);
}

void uart_Rx(void)
{
    static unsigned char dato = 0;
    static char i = 0;
    //De la misma forma que en tx, apago las interrupciones (ya entré aca cuando se activo la interrupcion externa puesta en __interrupt del main, no me hace falta mas hasta recibir el dato completo)
    INTCONbits.GIE = 0;
    INTCONbits.T0IE = 0;        
    PIE1bits.TMR1IE = 0;
    T1CONbits.TMR1ON = 0;   //Apagamos el timer   
    __delay_us(150);        //Esta es la espera de pulso y medio 104 + 52 aprox para capturar el primer bit recibido
    for(i = 0 ; i < 8 ; i++)
    {
        dato = dato + SERIE_RX * (0x01 << i);    //por cada vuelta, desplazo a la izq 'i' a 0x01 (0b00000001), lo multiplico por el dato recibido y se lo sumo a dato
        __delay_us(124);
    }
    if(!dato == 0x1B)        //Si dato no es igual a la tecla ESC, activo todo y todo sigue igual, caso contrario, se queda todo desactivado
    {   
        //Activamos todo nuevamente
        INTCONbits.GIE = 1;
        INTCONbits.T0IE = 1;        
        PIE1bits.TMR1IE = 1;
        T1CONbits.TMR1ON = 1;   
    }
    
}
