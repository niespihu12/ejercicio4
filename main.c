#include <xc.h>
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config LVP = OFF
#pragma config DEBUG = OFF

#define _XTAL_FREQ 20000000

const unsigned char DIGITOS[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
unsigned char digito_actual = 0;
unsigned char valor_mostrar = 0;

void mostrar_numeros(int valor) {
    unsigned char dec = valor / 10;
    unsigned char uni = valor % 10;
    PORTD = 0x00;
    
    if (digito_actual == 0) {
        // Mostrar unidades
        PORTC = DIGITOS[uni];
        PORTD = 0x02;
        digito_actual = 1;
    } else {
        PORTC = DIGITOS[dec];
        PORTD = 0x01;
        digito_actual = 0;
    }
}

unsigned char button_pressed(unsigned char current_state, unsigned char *previous_state) {
    if (current_state == 0) {
        __delay_ms(20);
        if (PORTBbits.RB0 == current_state || PORTBbits.RB1 == current_state || PORTBbits.RB2 == current_state) {
            if (*previous_state == 1) {
                *previous_state = 0;
                return 1;
            }
        }
    } else {
        *previous_state = 1;
    }
    return 0;
}

int main(void) {
    unsigned int cont_display = 0;
    unsigned int cont_segundo = 0;
    int valor = 0;
    int valor_pausado = 0;
    unsigned int estado = 0;
    unsigned char prev_btn0 = 1;
    unsigned char prev_btn1 = 1;
    unsigned char prev_btn2 = 1;
    PORTC = 0x00;
    TRISC = 0x00;
    PORTD = 0x00;
    TRISD = 0x00;
    ANSELH = 0x00;
    PORTB = 0xFF;
    TRISB = 0x07;
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0x07;
    
    while(1) {
        mostrar_numeros(valor_mostrar);
        __delay_ms(2);
        
        cont_display++;
        cont_segundo++;
        
        if (cont_display >= 50) {
            cont_display = 0;
            
            unsigned char start_btn = button_pressed(PORTBbits.RB0, &prev_btn0);
            unsigned char pause_btn = button_pressed(PORTBbits.RB1, &prev_btn1);
            unsigned char resume_btn = button_pressed(PORTBbits.RB2, &prev_btn2);
            
            
            switch(estado) {
                case 0: 
                    valor_mostrar = valor;
                    if (start_btn) {
                        estado = 1;
                        valor = 0;
                        cont_segundo = 0;
                    }
                    break;
                    
                case 1: 
                    if (cont_segundo >= 500) {
                        cont_segundo = 0;
                        valor++;
                        if (valor >= 100) {
                            valor = 0;
                        }
                    }
                    valor_mostrar = valor;
                    
                    if (pause_btn) {
                        estado = 2;
                        valor_pausado = valor;
                    }
                    break;
                    
                case 2: 
                    valor_mostrar = valor_pausado;
                    cont_segundo = 0; 
                    if (resume_btn) {
                        estado = 1;
                        valor = valor_pausado;
                    }
                    break;
                    
                default:
                    estado = 0;
                    break;
            }
        }
    }
    return 0;
}