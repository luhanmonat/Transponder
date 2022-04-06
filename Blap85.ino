/*
 * Personal Code Transponder for ATtiny85
 * 4/22 Luhan Monat
 * 
 * Once activated by vibration sensor, send out single byte coded
 * signal every second for about 30 seconds.  Use lowered clock speed between 
 * data bursts to save power. Then go back to sleep mode.
 * 
 */

#define VIBIN     4     // Vibration sensor (normally open contacts)
#define XMIT      2     // 433 Mhz transmitter draws power ONLY when active
#define SPEED     1000  // data speed factor


// We are using 'main' instead of 'setup & loop'

int	main() {
int i,j;

DDRB  = 0b101111;     // PB4 is input
bitSet(PORTB,4);      // PB4 enable pullup

again:

  Sleeper();

  bitClear(PORTB,XMIT);
  bitClear(ADCSRA,7);
  
  for(j=0;j<25;j++) {
    RFAttn();           // long pulse needed for most RF reciever units
    RFXmit(0x51);       // pick a number - any number
    ReClock(8);         // clock/256  slow down clock
    Wait(5);            // 5ms*256 = 1280ms = 1.28 sec
    ReClock(0);         // set clock to normal
  }

  goto again;
}


//  You can replace these 2 routines with whatever coding
//  scheme you want to use.

void  RFAttn() {
  bitSet(PORTB,XMIT);
  delayMicroseconds(9000);
  bitClear(PORTB,XMIT);
  delayMicroseconds(4500);
}

void  RFXmit(byte dat) {
byte  i,mask;
  
  mask=0x80;
  for(i=0;i<8;i++) {
    bitSet(PORTB,XMIT);
    if(dat&mask) delayMicroseconds(SPEED*2);
    else         delayMicroseconds(SPEED);
    bitClear(PORTB,XMIT);
    delayMicroseconds(SPEED);
    mask>>=1;
  }
  delayMicroseconds(2000);
}


// Perform clock re-setting sequence required by ATtiny85

void  ReClock(byte div) {

  CLKPR=0x80;
  CLKPR=div;
  
}


//  Do lots of stuff to perform sleep mode

void  Sleeper() {

  bitSet(GIMSK,PCIE);       // pin change int enable
  bitSet(PCMSK,4);          // enable pin change interrupt
  MCUCR = 0b00110000;       
  bitSet(SREG,7);           // global interrupt enable bit
  PRR   = 0b00001111;       // shut down peripherals
  asm("sleep \n");          // need to use assembler sleep instruction
}

//  Vibration activated pin change interrupt vectors here

ISR (PCINT0_vect) {

  bitClear(SREG,7);           // disable global interrupts for now

}

//  In place of delay instruction (needed when using 'main')

void  Wait(word ms) {
word  i;

  for(i=0;i<ms;i++)
    delayMicroseconds(1000);
}
