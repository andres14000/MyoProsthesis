#include <p33FJ128MC802.h>
#define FOSC    (80000000ULL)
#define FCY     (FOSC/2)
#include <libpic30.h>
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>




// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)
#pragma config RBS = NO_RAM             // Boot Segment RAM Protection (No Boot RAM)

// FSS
#pragma config SWRP = WRPROTECT_OFF     // Secure Segment Program Write Protect (Secure segment may be written)
#pragma config SSS = NO_FLASH           // Secure Segment Program Flash Code Protection (No Secure Segment)
#pragma config RSS = NO_RAM             // Secure Segment Data RAM Protection (No Secure RAM)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Mode (Primary Oscillator (XT, HS, EC) w/ PLL)
#pragma config IESO = ON                // Internal External Switch Over Mode (Start-up device with FRC, then automatically switch to user-selected oscillator source when ready)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Source (HS Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow Multiple Re-configurations)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler (1:32,768)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR1             // POR Timer Value (Disabled)
#pragma config ALTI2C = OFF             // Alternate I2C  pins (I2C mapped to SDA1/SCL1 pins)
#pragma config LPOL = OFF               // Motor Control PWM Low Side Polarity bit (PWM module low side output pins have active-low output polarity)
#pragma config HPOL = OFF               // Motor Control PWM High Side Polarity bit (PWM module high side output pins have active-low output polarity)
#pragma config PWMPIN = OFF             // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PWM module at device Reset)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)
////////////////////////////////////////////////7
///////////////////////////////////////////////
int VADC = 0;
#define BAUDRATE 9600
#define BRGVAL ((FCY/BAUDRATE)/16)-1
#define DELAY_105uS asm volatile ("REPEAT, #4201"); Nop(); // 105uS delay


int pos1 = 0;
int vuelta = 0;
int CU = 0, i = 0, PRNT = 0;
int recib, CONTA = 0;


int Pon = 0;
int t = 0;
int valor = 0;
int inicio = 1;
int S1 = 0;
int R1 = 0;
int R2 = 0;
int E1 = 0;

int pos = 7, posi = 0;
int lec = 0;
int mod = 0;
int inter = 0;
int ext = 0;

int arr[8000];

///////////////////////////////////////////////////////////////////////////////7

int error1() {
    while (1) {
         LATBbits.LATB14=1;
    }
    return 0;
}

int inicio2() {

    if (valor <= 330) {
        if (ext == 1) {
            inicio = 1;
            ext = 0;
            S1 = 0;
            return;
        }
        if (S1 == 0) {
            S1 = 1;
            t = 0;
            Pon = Pon + 1;
        }

        R2 = R2 + 1;

        if (R2 >= 100) {
            t = 0;
            R1 = 0;
            R2 = 0;
            E1 = 0;
            inicio = 1;
            return;
        } else {
            return;
        }


    } else {
        if (t == 500) {
            ext = 1;
            t = 500;
            if (mod == 1) {
                pos = 1;
                return;
            } else {
                pos = 4;
                return;
            }


        } else {
            inicio = 2;
            return;
        }

    }
}

int inicio1() {
    if (valor >= 4094) {
        E1 = E1 + 1;
        if (E1 >= 10) {///// errores

            // error1();
            return;
        } else {
            inicio = 1;
            return;
        }
    } else {
        if (valor >= 396) {
            if (Pon >= 1) {
                S1 = 0;
            }
            t = 0;
            R1 = R1 + 1;
            if (R1 >= 20) { //positivo minimo
                inicio2();
                return;
            } else {
                inicio = 1;
                return;
            }
        } else {
            if (t >= 600) {
                t = 0;
                R1 = 0;
                if (Pon == 1) {
                    if (mod == 0) {
                        mod = 1;
                        inter = 1;
                        S1 = 0;
                    }
                    if (mod == 1 && inter == 0) {
                        mod = 0;
                        S1 = 0;
                    }
                    inter = 0;
                }
                if (Pon > 1 && Pon < 7) {
                    pos = Pon;
                }
                S1 = 0;
                Pon = 0;
                inicio = 1;
                return;
            } else {
                inicio = 1;
                return;
            }


        }

    }
}

void main(void) {
    CLKDIVbits.PLLPRE = 0XE; //fin/16(n1) 
    CLKDIVbits.PLLPOST = 0; // x/2(n2))
    PLLFBDbits.PLLDIV = 0X9E; //*160 .. FOSC=80 PARA CRISTAL DE 16 
    PWM1CON1 = 0;
    PWM2CON1 = 0;
    I2C1CON = 0;

    T1CON = 0;
    T2CON = 0;
    T3CON = 0;
    T4CON = 0;
    IC1CON = 0;
    IC2CON = 0;

    TRISB = 0;
    TRISA = '0xFF';

    //CONFIGURACION DE PUERTOS 
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;

    LATBbits.LATB15 = 0;
    LATBbits.LATB14 = 0;
    LATBbits.LATB13 = 0;
    LATBbits.LATB12 = 0;
    /////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////    // Configure Remappables Pins
    __builtin_write_OSCCONL(OSCCON & ~(1 << 6));
    //RPINR14 = 0x605; //////////////////////////////////qea1 al rp5 y qeb1 al rp6
    RPINR19bits.U2RXR = 0x06; ///u2rx-- input al rp6 ---- 
    RPOR3bits.RP7R = 0x05; ///a rp7 se le asigna u2tx
    //RPINR7bits.IC1R=5; // imput capture a rp9

    __builtin_write_OSCCONL(OSCCON | (1 << 6));
    ///////////////////////////////////////////////////////////////////////////////////   
    //////////////////////////////////////////////////////////////////////     
    //configuracion conversor
    //////////////////////////////////////////////////////////////////////////// 
    AD1CON1bits.ADON = 0; // apagar ADC

    AD1CON1bits.AD12B = 1; // 12 BITS 

    AD1PCFGL = 1; //A0 COMO ANALOGICA

    AD1CON2bits.VCFG = 0; // REFERENCIA AVDD Y AVSS
    AD1CON3bits.ADCS = 100; // PERIODO DE CONVERSION TAD= TCY*5
    AD1PCFGL = 0xFFFC; //AN0 Y AN1 COMO ANALOGAS
    AD1CHS0bits.CH0NA = 0; // channel 0  entrada negativa  es vref-
    AD1CHS0bits.CH0SA = 000; // channel 0 entrada positiva es AN0
    //ADxCON1<3> SIMSAM READ AS 0 
    //AD1CSSL=1; // SCAN
    AD1CON1bits.ASAM = 0; // CONVERSION CON SAMP manual
    AD1CON1bits.SSRC = 7; // CONVERSION auto USNDO SAMP
    AD1CON1bits.FORM = 0; //SALIDA EN ENTEROS 
    AD1CON2bits.BUFM = 0; //EL BUFFER COMIENZA A LLENAR EN 0X0
    AD1CON2bits.BUFS = 0; // LLENADO DEL BUFFER EN 0X0-0X7 Y ACCESO EN 0X8-0XF
    AD1CON4bits.DMABL = 0; //  UN BUFFER DE UNA PALABRA  A CADA ENTRADA ANALOGA

    AD1CON1bits.ADSIDL = 0; //CONTINUE MODE LDLE
    AD1CON1bits.ADDMABM = 1; //DMA ESCRITO EN ORDEN DE CONVERSION

    //AD1CON2bits.CHPS=0;

    AD1CON2bits.CSCNA = 0; //NO ESCANEAR ENTRADAS

    AD1CON2bits.SMPI = 0; //LA DIRECCION DEL BUFFER AUMENTA DESUES DE COMPLETAR CADA MUESTREO  Y CONVERSION

    AD1CON2bits.ALTS = 0; //SIEMPRE USA EL MISMO CANAL SELECCIONADO



    AD1CON3bits.ADRC = 0; // RELOJ DE CONVERSION EN BASE AL RELOJ DEL SISTEMA

    AD1CON3bits.SAMC = 1110;

    AD1CON4bits.DMABL = 0; //  UN BUFFER DE UNA PALABRA  A CADA ENTRADA ANALOGA

    //AD1CHS0bits.CH0NB=0;
    //AD1CHS0bits.CH0SB=0;

    AD1CON1bits.ADON = 1; // ENCENDER ADC

    ////////////////////////////////////// Configure Timer 1:
    T1CONbits.TON = 0; // Turn on Timer 1
    T1CON = 0; // Timer reset
    IFS0bits.T1IF = 0; // Reset Timer1 interrupt flag
    IPC0bits.T1IP = 5; // Timer1 Interrupt priority level=4
    IEC0bits.T1IE = 1; // Enable Timer1 interrupt
    TMR1 = 0; // Reset Timer 1 counter
    PR1 = 5000; // Set the Timer 1 period (max 65535)
    T1CONbits.TCKPS = 1; // Prescaler (0=1:1, 1=1:8, 2=1:64, 3=1:256)

    T1CONbits.TON = 1;
    //////////////////////////////////////////////////////////////////////////7
    //////////////////////////////////////////////////////////////////////////7
    //uart////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    U2MODEbits.BRGH = 0; // Standard-Speed mode

    U2MODEbits.STSEL = 0; // 1-Stop bit
    U2MODEbits.PDSEL = 0; // No Parity, 8-Data bits
    U2MODEbits.ABAUD = 0; // Auto-Baud disabled
    U2MODEbits.RTSMD = 0; //FLOW CONTROL
    U2BRG = 259;
    U2STAbits.UTXISEL0 = 1; // Interrupt after one TX character is transmitted
    U2STAbits.UTXISEL1 = 0;
    U2STAbits.UTXISEL1 = 0; // Interrupt after one RX character iS RECIEVE

    U2STAbits.TRMT = 0;
    IEC1bits.U2TXIE = 1; // Enable UART TX interrupt
    IPC7bits.U2TXIP = 5; // PRIORIDAD DE LA INTERRUPCION
    IFS1bits.U2TXIF = 0; // Clear TX Interrupt flag
    IEC1bits.U2RXIE = 1; // Enable UART RX interrupt
    IPC7bits.U2RXIP = 4; // PRIORIDAD DE LA INTERRUPCION
    IFS1bits.U2RXIF = 0; // Clear TX Interrupt flag
    __C30_UART = 2;
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
    // Wait at least 105 microseconds (1/9600) before sending first char 
    LATBbits.LATB15 = 1;
    LATBbits.LATB14 = 1;
    LATBbits.LATB13 = 1;
    LATBbits.LATB12 = 1;
    __delay_ms(6000);
    LATBbits.LATB15 = 0;
    LATBbits.LATB14 = 0;
    LATBbits.LATB13 = 0;
    LATBbits.LATB12 = 0;

    /////////////////////////////////////////////////////////////////////////
    U2TXREG = ('3');
    __delay_ms(1000);
    U2TXREG = ('z');
    __delay_ms(1000);
    while (1) {
        PRNT++;


        if (pos == 2) {

            LATBbits.LATB15 = 0;
            LATBbits.LATB14 = 0;
            LATBbits.LATB13 = 0;
            LATBbits.LATB12 = 1;
            //printf("%d\n",valor);
            if (posi != pos && PRNT >= 10000) {

                U2TXREG = ('1');

                __delay_ms(1000);
                U2TXREG = ('k');

                CU = 0;
                PRNT = 0;
                posi = pos;
            }

            //printf("posi2");


        }
        if (pos == 3) {

            LATBbits.LATB15 = 0;
            LATBbits.LATB14 = 1;
            LATBbits.LATB13 = 0;
            LATBbits.LATB12 = 0;
            if (posi != pos && PRNT >= 10000) {


                U2TXREG = ('2');
                __delay_ms(1000);
                U2TXREG = ('O');
                __delay_ms(12);


                CU = 0;
                PRNT = 0;
                posi = pos;
            }

        }

        if (pos == 4) {

            LATBbits.LATB15 = 1;
            LATBbits.LATB14 = 0;
            LATBbits.LATB13 = 0;
            LATBbits.LATB12 = 0;

            if (posi != pos && PRNT >= 10000) {


                U2TXREG = ('3');
                __delay_ms(1000);
                U2TXREG = ('d');
                __delay_ms(12);

                CU = 0;
                PRNT = 0;
                posi = pos;
            }
        }
        if (pos == 5) {

            LATBbits.LATB15 = 0;
            LATBbits.LATB14 = 0;
            LATBbits.LATB13 = 1;
            LATBbits.LATB12 = 0;
            if (posi != pos && PRNT >= 10000) {
                U2TXREG = ('1');
                __delay_ms(1000);
                U2TXREG = ('2');
                __delay_ms(1000);

                U2TXREG = ('2');
                __delay_ms(1000);
                U2TXREG = ('2');
                __delay_ms(1000);

                U2TXREG = ('3');
                __delay_ms(1000);
                U2TXREG = ('z');
                __delay_ms(1000);
                CU = 0;
                posi = pos;
            }
        }

        if (lec == 1) {
            if (inicio == 2) {
                inicio2();
            } else {
                inicio1();
            }
            lec = 0;
        }
        //return 0;
    }
}

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void) {
    LATBbits.LATB9 = 1;
    t = t + 1; //ms
    //LATBbits.LATB15 = 1;

    AD1CON1bits.SAMP = 1;
    while (!AD1CON1bits.DONE);

    valor = ADC1BUF0;
    // LATBbits.LATB15 = 0;
    if (i <= 7999) {
        arr[i] = valor;
        i++;


    }
    if (i > 7999) {
        i = 0;
    }
    lec = 1;
    CU = 1;

    LATBbits.LATB9 = 0;

    IFS0bits.T1IF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _U2TXInterrupt(void) {
    IFS1bits.U2TXIF = 0; // Clear TX Interrupt flag

}

void __attribute__((__interrupt__, no_auto_psv)) _U2RXInterrupt(void) {
    _U2RXIF = 0;
    recib = U2RXREG;

}