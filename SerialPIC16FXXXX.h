
/* 
 * File: SerialPIC16FXXX  
 * Author: Esaú Sanabria Sánchez
 * Comments: Librería de comunicación serial a través de USART en modo asíncrono 
 *           a 9600BPS para microcontroladores de gama media de 8 bits.
 */

// This is a guard condition so that contents of this file are not included
// more than once. 

#ifndef SERIAL_PIC16FXXX_H
#define	SERIAL_PIC16FXXX_H


unsigned char cadena[20];
unsigned char *buffer;

/* UartInitTx() Configura los terminales los bits de los registros TXSTA y RCST,  
 * para habilitar la USART en modo asíncrono, 8 bits, sin paridad ni control de 
 * flujo con una tasa de 9600BPS. Si se desea otra tasa de transferencia, o se  
 * está utilizando otra frecuencia de trabajo, se deberá cambiar el valor del 
 * registro SPBRG.
 */
void UartInitTx(void){
    TXSTAbits.BRGH = 1;        //Habilita la transferencia de alta velocidad
    SPBRG = 0x19;              //Configura tasa de transferencia en 9600BPS con 
                               //BRGH = 1 y F = 4MHz
    TXSTAbits.SYNC = 0;         //Modo USART: Asíncrono
    RCSTAbits.SPEN = 1;         //Habilita el puerto serial
    TXSTAbits.TXEN = 1;         //Habilita transmisión
    RCSTAbits.CREN = 1;         //Habilita recepción
    
}

/* arrayInit(): Para evitar la aparición de caracteres basura en la 
 * impresión de datos recibidos, al iniciar el programa se debe dejar vacio el
 * array que recibe los datos via serial.
*/
void arrayInit(void){
    for(int i = 0; i <= 20; i++)
        cadena[i] = ' ';
}

/* SPutchar(char ch) Esta subrutina se implementó para cuando solo se deseé
 * transmitir un caracter, haciendo pruebas Sprintf() falla enviando caracteres
 * individualmente y SPutchar(char ch) soluciona el problema.
 */
void SPutchar(char ch)
{
    while(TXSTAbits.TRMT==0);    // Espera mientras el registro de transmisión
                                 // se vacie.
    TXSTAbits.TRMT=0;            // Limpia la bandera de transmisión
    TXREG=ch;                    // Carga el caracter a ser transmitido
                                 // en el registro de transmisión
}

/* SPrintf(unsigned char* dato) Mediante esta función es posible imprimir desde
 * el PIC una cadena de caracteres através de un puntero. Se reciben uno a uno
 * los caracteres de este y se van cargando al registro de transmisión TXREG
 */
void SPrintf(unsigned char* dato){
    //UartInitTx();
    while(*dato != 0x00){       //Espera hasta que el buffer de transmisión está
                                //vacio
        TXREG = *dato;          //Se pasa al registro de transmisión el caracter
                                //actual
        while(TXSTAbits.TRMT == 0); //Monitorea bit bandera que confirma que se 
                                //transmitió un caracter
        dato++;                 //Se incrementa el valor de la posición del 
                                //buffer para transmitir el siguiente caracter       
    }
}//SPrintf

/* Sgetch(void) Se utiliza cuando se va a leer un caracter de forma individual
 * y servirá también para SgetString() cuando se requiera leer cadenas de 
 * caracteres.
 */
unsigned char Sgetch(void){
    if(RCSTAbits.OERR == 1){ //Revisa si se ha producido algún error en la 
        RCSTAbits.CREN = 0;  //de caracter y resetea el pin que habilita 
        RCSTAbits.CREN = 1;  //la lectura, para nuevamente habilitarlo.
    }
    
    while(PIR1bits.RCIF == 0);  //Espera hasta que se terminen de leer todos los     
        return RCREG;           // bits del caracter RCIF == 1 y se envía al
                                // registro RCREG.
}


/* SgetString() se implementó para leer cadenas de caracteres en el PIC, 
 * mediante un array. Se pueden recibir cadenas de caracteres de hasta
 * 20 posiciones. 
 */
void SgetString(unsigned char input[]){
    int i;    
        for(i = 0; i < 20 ; i++){          
            input[i] = Sgetch();     //Recibe hasta 20 caracteres 
            if((input[i] == 0x0A) ||(input[i] == 0x0D)){//Espera el enter o el
                                    //salto de línea
                input[i] = ' ';     //Si no se leen los 20 caracteres, se deja
                break;              //la última posición vacia para que no se
                                    //carguen caracteres basura y se trunca la
            }//if                   //lectura de datos
        }//for
    }//SgetString()


#endif	/* XC_HEADER_TEMPLATE_H */
