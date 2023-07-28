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
#include "UART.h"

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
#define     _XTAL_FREQ 8000000  //Constante para delay
#define     _TMR0n 50   //TMR0 load value
uint8_t     pot_val;    //ADC read value
uint16_t    pot_volt;  //Pot val mapped to voltage
uint8_t     counter;
char        UART_read; 
char        count[] = {0,0,0};
char        volt[] = {0,0,0};


        
/*-------------------------------- PROTOTYPES --------------------------------*/
void ioc_portB(void);
void setup(void);
void output_LCD(void);
void separar_digitos8(uint8_t num, char dig8[]);
void separar_digitos16(uint16_t num, char dig16[]);
uint16_t map(uint8_t val, uint8_t min1, uint8_t max1, uint8_t min2, long max2);
void UART(void);
/*------------------------------- RESET VECTOR -------------------------------*/

/*----------------------------- INTERRUPT VECTOR -----------------------------*/
void __interrupt() isr(void){
    if(RCIF){        
        UART_read  = UART_read_char();
        RCIF = 0;
    }
            
}

/*--------------------------- INTERRUPT SUBROUTINES --------------------------*/

/*---------------------------------- TABLES ----------------------------------*/

/*----------------------------------- SETUP ----------------------------------*/

/*----------------------------------- MAIN -----------------------------------*/
int main(void) {
    setup();
    Lcd_Init(); //Initialize LCD
    UART_RX_config(9600);
    UART_TX_config(9600);
    while(1){
        //Loop
        //Pot value
        pot_val = (adc_read()>>8) & 0x00FF;            //Read pot value
        //Display LCD
        output_LCD();
        //UART
        UART();
    }
}
/*-------------------------------- SUBROUTINES -------------------------------*/
void setup(void){
   //I/O CONFIG
    ANSEL = 0;  //PORTA as Digital
    ANSELH= 0b00100000; //AN13 enabled
    TRISB = 32;  //button input
    PORTB = 0;  //Clear Port
    TRISD = 0;  //LCD Output
    PORTD = 0;
    TRISE = 0;
    PORTE = 0;
    TRISC = 0xFF;
    PORTC = 0;
    TRISA = 0;
    PORTA = 0;
    
    //OSCILLATOR CONFIG
    OSCCONbits.IRCF = 0b111;  //Internal clock frequency 8MHz
    SCS = 1;
    
    adc_init(0, 0, 8, 0b1101); //Initialize ADC. Left, Vdd/Vss, 8MHz, AN13.
    
}

void output_LCD(void){
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Counter: ");
    //Conversion a decimal  
    separar_digitos8(counter, count);
    //Mostrar valor
    Lcd_Write_String(count);
    
    pot_volt = map(pot_val,0,255,0,500);
    separar_digitos16(pot_volt, volt);
    Lcd_Set_Cursor(2,1);
    Lcd_Write_String("Voltage: ");    
    //Mostrar valor  
    Lcd_Write_Char(volt[0]);
    Lcd_Write_Char('.');
    Lcd_Write_Char(volt[1]);
    Lcd_Write_Char(volt[2]);
    Lcd_Write_Char('V');
}

//Esta función separa los dígitos de un valor decimal en registros separados/array
void separar_digitos8(uint8_t num, char dig8[]){
    uint8_t div1,div2,div3,centenas,decenas,unidades;
    div1 = num / 10;
    unidades = num % 10;
    div2 = div1 / 10;
    decenas = div1 % 10;    
    //div3 = div2 / 10;
    centenas = div2 % 10;
    
    dig8[2] = unidades + 0x30;
    dig8[1] = decenas  + 0x30;
    dig8[0] = centenas + 0x30;
}

void separar_digitos16(uint16_t num, char dig16[]){
    uint16_t div1,div2,div3,centenas,decenas,unidades;
    div1 = num / 10;
    unidades = num % 10;
    div2 = div1 / 10;
    decenas = div1 % 10;    
    //div3 = div2 / 10;
    centenas = div2 % 10;
    
    dig16[2] = unidades + 0x30;
    dig16[1] = decenas + 0x30;
    dig16[0] = centenas + 0x30;
}

uint16_t map(uint8_t val, uint8_t min1, uint8_t max1, uint8_t min2, long max2){
    return ((val-min1)*(max2-min2)/(max1-min1))+min2;
}

void UART(void){
        //Send pot voltage
        UART_write_string("Voltaje del potenciometro: ");
        UART_write_char(volt[0]);
        UART_write_char('.');
        UART_write_char(volt[1]);
        UART_write_char(volt[2]);
        UART_write_string("V\n\r");
        //Read counter +/-        
        if(UART_read == '+')
            counter++;
        else if (UART_read == '-')
            counter--;
}