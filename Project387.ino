#define BAUD 9600

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdio.h>

uint16_t readADC (void){
  // request start conversion
  ADCSRA |= (1<<ADSC);

  //when conversion is done ADSC = 0. Wait here, blocking function.
  // while(!((ADCSRA & (1<<ADSC)) == 0());
  loop_until_bit_is_clear(ADCSRA, ADSC);
  
  //read 16 bit ADC data from ADCH and ADCL registers.
  //Compiler handles the reading, ADC is a virtual register.
  return(ADC);
}
void initADC(uint8_t muxChannel){
  //AVcc = 5v as vref
  ADMUX |= (1<<REFS0);
  
  ADCSRA |= ((1<<ADPS2) | (1<<ADPS1)| (1<<ADPS0));
  
  ADMUX = muxChannel;
  
  ADCSRA |= (1<<ADEN);
}
void initUSART(void){
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
 #if USE_2X
   UCSR0A |= (1 << U2X0);
 #else
   UCSR0A &= ~(1 << U2X0);
 #endif
 
   UCSR0B = (1 << TXEN0) | (1<< RXEN0);
   UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void transmitByte(uint8_t data){
  while((UCSR0A & (1 << UDRE0)) == 0);
  UDR0 = data;
}

uint8_t receiveByte(void){
  while((UCSR0A & (1<<RXC0)) == 0);
  return UDR0;
}
void printString(const char myString[]) {
  uint8_t i = 0;
  while (myString[i]) {
    transmitByte(myString[i]);
    i++;
  }
}

void readString(char myString[], uint8_t maxLength) {
  char response;
  uint8_t i;
  i = 0;
  while (i < (maxLength - 1)) {                   /* prevent over-runs */
    response = receiveByte();
    transmitByte(response);                                    /* echo */
    if (response == '\r') {                     /* enter marks the end */
      break;
    }
    else {
      myString[i] = response;                       /* add in a letter */
      i++;
    }
  }
  myString[i] = 0;                          /* terminal NULL character */
}
int main(void){
  uint8_t serialCharacter;
  uint16_t adc_value;
  PORTC = (0<<PORTC5);
  char s [32];
  
  initUSART();
  initADC(0b0101); //sets MUX channel to Analog Pin 5
  
  
  while(1) {
    adc_value = readADC();
    adc_value*=.0049;
    sprintf(s, "ADC Val = %u\n\r", adc_value);
    printString(s);
    //printf(s);    
    //serialCharacter = receiveByte();
    
  }
  return (0);
}
