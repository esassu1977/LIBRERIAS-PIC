
/* 
 * File: SerialPIC16FXXX  
 * Author: Esa� Sanabria S�nchez
 * Comments: Librer�a de comunicaci�n serial a trav�s de USART en modo as�ncrono 
 *           a 9600BPS para microcontroladores de gama media de 8 bits.
 */

// This is a guard condition so that contents of this file are not included
// more than once. 

#ifndef SERIAL_PIC16FXXX_H
#define	SERIAL_PIC16FXXX_H


unsigned char cadena[20];
unsigned char *buffer;

/* UartInitTx() Configura los terminales los bits de los registros TXSTA y RCST,  
 * para habilitar la USART en modo as�ncrono, 8 bits, sin paridad ni control de 
 * flujo con una tasa de 9600BPS. Si se desea otra tasa de transferencia, o se  
 * est� utilizando otra frecuencia de trabajo, se deber� cambiar el valor del 
 * registro SPBRG.
 */
void UartInitTx(void){
    TXSTAbits.BRGH = 1;        //Habilita la transferencia de alta velocidad
    SPBRG = 0x19;              //Configura tasa de transferencia en 9600BPS con 
                               //BRGH = 1 y F = 4MHz
    TXSTAbits.SYNC = 0;         //Modo USART: As�ncrono
    RCSTAbits.SPEN = 1;         //Habilita el puerto serial
    TXSTAbits.TXEN = 1;         //Habilita transmisi�n
    RCSTAbits.CREN = 1;         //Habilita recepci�n
    
}

/* arrayInit(): Para evitar la aparici�n de caracteres basura en la 
 * impresi�n de datos recibidos, al iniciar el programa se debe dejar vacio el
 * array que recibe los datos via serial.
*/
void arrayInit(void){
    for(int i = 0; i <= 20; i++)
        cadena[i] = ' ';
}

/* SPutchar(char ch) Esta subrutina se implement� para cuando solo se dese�
 * transmitir un caracter, haciendo pruebas Sprintf() falla enviando caracteres
 * individualmente y SPutchar(char ch) soluciona el problema.
 */
void SPutchar(char ch)
{
    while(TXSTAbits.TRMT==0);    // Espera mientras el registro de transmisi�n
                                 // se vacie.
    TXSTAbits.TRMT=0;            // Limpia la bandera de transmisi�n
    TXREG=ch;                    // Carga el caracter a ser transmitido
                                 // en el registro de transmisi�n
}

/* SPrintf(unsigned char* dato) Mediante esta funci�n es posible imprimir desde
 * el PIC una cadena de caracteres atrav�s de un puntero. Se reciben uno a uno
 * los caracteres de este y se van cargando al registro de transmisi�n TXREG
 */
void SPrintf(unsigned char* dato){
    //UartInitTx();
    while(*dato != 0x00){       //Espera hasta que el buffer de transmisi�n est�
                                //vacio
        TXREG = *dato;          //Se pasa al registro de transmisi�n el caracter
                                //actual
        while(TXSTAbits.TRMT == 0); //Monitorea bit bandera que confirma que se 
                                //transmiti� un caracter
        dato++;                 //Se incrementa el valor de la posici�n del 
                                //buffer para transmitir el siguiente caracter       
    }
}//SPrintf

/* Sgetch(void) Se utiliza cuando se va a leer un caracter de forma individual
 * y servir� tambi�n para SgetString() cuando se requiera leer cadenas de 
 * caracteres.
 */
unsigned char Sgetch(void){
    if(RCSTAbits.OERR == 1){ //Revisa si se ha producido alg�n error en la 
        RCSTAbits.CREN = 0;  //de caracter y resetea el pin que habilita 
        RCSTAbits.CREN = 1;  //la lectura, para nuevamente habilitarlo.
    }
    
    while(PIR1bits.RCIF == 0);  //Espera hasta que se terminen de leer todos los     
        return RCREG;           // bits del caracter RCIF == 1 y se env�a al
                                // registro RCREG.
}


/* SgetString() se implement� para leer cadenas de caracteres en el PIC, 
 * mediante un array. Se pueden recibir cadenas de caracteres de hasta
 * 20 posiciones. 
 */
void SgetString(unsigned char input[]){
    int i;    
        for(i = 0; i < 20 ; i++){          
            input[i] = Sgetch();     //Recibe hasta 20 caracteres 
            if((input[i] == 0x0A) ||(input[i] == 0x0D)){//Espera el enter o el
                                    //salto de l�nea
                input[i] = ' ';     //Si no se leen los 20 caracteres, se deja
                break;              //la �ltima posici�n vacia para que no se
                                    //carguen caracteres basura y se trunca la
            }//if                   //lectura de datos
        }//for
    }//SgetString()


#endif	/* XC_HEADER_TEMPLATE_H */
