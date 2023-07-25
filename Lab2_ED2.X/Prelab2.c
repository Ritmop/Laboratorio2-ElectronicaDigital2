/* 
 * File:   Prelab2.c
 * Device: PIC16F887
 * Author: Judah Pérez - 21536
 *Compiler: XC8 (v2.41)
 * 
 * Program: 
 * Hardware: 
 * 
 * Created: July 24, 2023
 * Last updated:
 */

/*--------------------------------- LIBRARIES --------------------------------*/
#include <xc.h>
#include "iocb_init.h"
#include "ADC_lib.h"
#include "LCD.h"

/*---------------------------- CONFIGURATION BITS ----------------------------*/
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

/*----------------------- GLOBAL VARIABLES & CONSTANTS -----------------------*/
#define _XTAL_FREQ 8000000  //Constante para delay
#define _TMR0n 50   //TMR0 load value
uint8_t adc_val;    //ADC read value
        
        
/*-------------------------------- PROTOTYPES --------------------------------*/
void ioc_portB(void);
void setup(void);
void output_LCD(void);
void separar_digitos(uint8_t num, char *dig);
/*------------------------------- RESET VECTOR -------------------------------*/

/*----------------------------- INTERRUPT VECTOR -----------------------------*/
void __interrupt() isr(void){
    if(T0IF){
        //Reset timer0
        TMR0 = _TMR0n;
        T0IF = 0;
    }
    if(RBIF){
        ioc_portB();
        RBIF = 0;
    }    
}

/*--------------------------- INTERRUPT SUBROUTINES --------------------------*/
void ioc_portB(void){
    if(!RB0) PORTA++;
    if(!RB1) PORTA--;
}
/*---------------------------------- TABLES ----------------------------------*/

/*----------------------------------- SETUP ----------------------------------*/

/*----------------------------------- MAIN -----------------------------------*/
int main(void) {
    setup();
    while(1){
        //Loop
        //Pot value
        __delay_ms(50);
        adc_val = adc_read()>>8;            //Read pot value
        PORTA = adc_val;
        //Display LCD
        output_LCD();
    }
}
/*-------------------------------- SUBROUTINES -------------------------------*/
void setup(void){
   //I/O CONFIG
    ANSEL = 0;  //PORTA as Digital
    ANSELH= 0b00100000; //AN13 enabled
    TRISA = 0;  //ADC Output
    PORTA = 0;  //Clear Port
    TRISB = 0;  //button input
    PORTB = 0;  //Clear Port
    TRISD = 0;  //LCD Output
    PORTD = 0;
    TRISC = 0;
    PORTC = 0;
    
    //OSCILLATOR CONFIG
    OSCCONbits.IRCF = 0b111;  //Internal clock frequency 8MHz
    SCS = 1;
    
    iocb_init(0b00100011); //Initialize IOCB
    adc_init(0, 0, 8, 0b1101); //Initialize ADC. Left, Vdd/Vss, 8MHz, AN13.
    
    //TMR0 config
    GIE = 1;    //Enable global interrupts
    T0IE = 1;   //Enable TMR0 interrupt
    OPTION_REGbits.PS = 0b000;  //Prescaler = 2
    T0CS = 0;   //Use internal cycle clock
    TMR0 = _TMR0n;
    T0IF = 0;   //Clear flag
    
    //Initialize LCD
    Lcd_Init();
}

void output_LCD(void){
    Lcd_Clear();
    Lcd_Set_Cursor(1, 5);
    Lcd_Write_String("POT: ");
    //Conversion a decimal    
    char decimal[3];
    separar_digitos(adc_val, decimal);
    //Mostrar valor
    Lcd_Write_String(decimal);
}

//Esta función separa los dígitos de un valor decimal en registros separados/array
void separar_digitos(uint8_t num, char *dig){
    uint8_t unidades,decenas,centenas,residuo_cent,residuo_dec,residuo_unit;
    centenas = num / 10;
    residuo_cent = num % 10;
    decenas = centenas / 10;
    residuo_dec = centenas % 10;
    unidades = decenas / 10;
    residuo_unit = decenas % 10;
    
    dig[2] = residuo_cent + 0x30;
    dig[1] = residuo_dec + 0x30;
    dig[0] = residuo_unit + 0x30;
}