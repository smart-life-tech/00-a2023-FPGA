//-----------------------------------------------------------------------------
#warning ====== 2023-1217 =============> INCL: a00init.h =======>
//-----------------------------------------------------------------------------
// ATmega168/ATmega328-specific settings
//-----------------------------------------------------------------------------
#define myTIME     200                                     // time-delay,@20MHz
#define myPULSE    0x20                                    // is 0x08 @ 4MHz
#define mySND1     PORTD |=  _BV(PORTD7);                  // sound is log1
#define mySND0     PORTD &= ~_BV(PORTD7);                  // sound is log0

#define LED_REG    PORTB                                   // ARDUINO-BOARD
#define LED_BIT    5                                       // ARDU-LED
#define LED_log0   PORTB &= ~_BV(PORTB5);                  // ARDU-LED is dark
#define LED_log1   PORTB |=  _BV(PORTB5);                  // ARDU-LED active

#define myCK1024   TCCR0B= 0x05;                           // CK/1024
#define myLOADgo   TCNT0 = 0x03;                           // load starts TCNT0
#define myTIFR     TIFR0                                   // TIFR0: 0x15,0x35
#define myTOV0     TOV0                                    // TOV0/TIFR0, Bit0
#define myQTR_agn  TIFR0 |= _BV(TOV0);TIFR0 &= ~_BV(TOV0); // Restart Timer
//-----------------------------------------------------------------------------
// BAUD:9k6: UBRR= 0x19/4MHz, 0x32/08MHz, 0x40/10MHz, 0x4E/12MHz, 0x80/20MHz
//
#define TTY0baud   UBRR0H=0x00; UBRR0L=0x80;           // Baudrate, see above
#define TTY0mode   asm volatile ("nop");               // ATmega168p ???
#define TTY0ena    UCSR0B = (1<<TXEN0) | (1<<RXEN0);   // **tx,rx enable
#define UDR0reg    UDR0                                // * UART RD/WR REGISTER
#define STA0reg    UCSR0A                              // **UART STATUS
#define DAV0bit    RXC0                                // * NEW DATA AVAILABLE
#define TMB0bit    UDRE0                               // TX BUFFER EMPTY 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Different with FPGA. This is ok for Morse-Keyer. 
//
  void init_port(void)
  {
// DDRA &= ~_BV(DDA1); PORTA |=  _BV(PORTA1);  // inp, not avail. on ardu-m168
// DDRA &= ~_BV(DDA0); PORTA |=  _BV(PORTA0);  // inp, not avail. on ardu-m168 
//-----------------------------------------------------------------------------
// DDRB &= ~_BV(DDB7); PORTB |=  _BV(PORTB7);  // n.a. XTAL2
// DDRB |=  _BV(DDB6); PORTB |=  _BV(PORTB6);  // n.a. XTAL1
   DDRB |=  _BV(DDB5); PORTB |=  _BV(PORTB5);  // SCK, LED:Ardu-Heartbeat
   DDRB &= ~_BV(DDB4); PORTB |=  _BV(PORTB4);  // MISO,LED:PCBs-Heartbeat
   DDRB &= ~_BV(DDB3); PORTB |=  _BV(PORTB3);  // MOSI
   DDRB &= ~_BV(DDB2); PORTB |=  _BV(PORTB2);  // inp, PIN
   DDRB &= ~_BV(DDB1); PORTB |=  _BV(PORTB1);  // inp, PIN
   DDRB &= ~_BV(DDB0); PORTB |=  _BV(PORTB0);  // inp, PIN 
//-----------------------------------------------------------------------------
// DDRC &= ~_BV(DDC7); PORTC |=  _BV(PORTC7);  // n.a.
// DDRC |=  _BV(DDC6); PORTC |=  _BV(PORTC6);  // n.a. RESET, m168
   DDRC &= ~_BV(DDC5); PORTC |=  _BV(PORTC5);  // inp, ADC5, quit ELbug  FPGA?
   DDRC &= ~_BV(DDC4); PORTC |=  _BV(PORTC4);  // inp, ADC4, QTC: PARIS  FPGA?
   DDRC &= ~_BV(DDC3); PORTC |=  _BV(PORTC3);  // inp, ADC3, QRS,QRQ     FPGA?
   DDRC &= ~_BV(DDC2); PORTC |=  _BV(PORTC2);  // inp, ADC2, dash        FPGA?
   DDRC &= ~_BV(DDC1); PORTC |=  _BV(PORTC1);  // inp, ADC1, dots        FPGA?
   DDRC &= ~_BV(DDC0); PORTC |=  _BV(PORTC0);  // inp, ADC0, man key     FPGA?
//-----------------------------------------------------------------------------
   DDRD |=  _BV(DDD7); PORTD |=  _BV(PORTD7);  // out, log1, mySND
   DDRD |=  _BV(DDD6); PORTD &= ~_BV(PORTD6);  // out, log0, Beacon
   DDRD |=  _BV(DDD5); PORTD |=  _BV(PORTD5);  // out, log1, onAIR (key active)
   DDRD |=  _BV(DDD4); PORTD |=  _BV(PORTD4);  // out, log1
   DDRD |=  _BV(DDD3); PORTD |=  _BV(PORTD3);  // out, log1
   DDRD |=  _BV(DDD2); PORTD |=  _BV(PORTD2);  // out, log1
   DDRD |=  _BV(DDD1); PORTD |=  _BV(PORTD1);  // out, log1, TxD
   DDRD &= ~_BV(DDD0); PORTD |=  _BV(PORTD0);  // inp,  pup, RxD
//-----------------------------------------------------------------------------
   myCK1024                                    // ** CK/1024
   myLOADgo                                    // load2start TCNT0
//   TCCR0B = 0x05;                            // ** CK/1024
//   TCNT0  = 0x03;                            // load2start TCNT0
  }
//-----------------------------------------------------------------------------
  unsigned short int doqtr(unsigned short int icp)       //
  {                                                      // run heartbeat LED
   if (bit_is_set(myTIFR,myTOV0))                        // Timer=0?
    {if (icp==myPULSE)                                   // is a second gone?
      {icp=0;
       if (bit_is_set(LED_REG,LED_BIT))
        {LED_log0;}                                      // heartbeat LED off
       else  
        {LED_log1;}                                      // heartbeat LED on
      }
     myQTR_agn;                                          // sbi,cbi(TIFR,TOV0);
     icp++;                                              // 
    }
   return(icp);
  }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  void dly0()                                               // WASTE SOME TIME
  {unsigned int xtime;
   xtime=myTIME; while (xtime) {asm volatile ("nop"); xtime--;}
  }
//-----------------------------------------------------------------------------
  void dly1()                                               // WASTE MORE TIME
  {unsigned int xtime;
   xtime= 10; while (xtime) {asm volatile ("nop"); dly0(); xtime--;}
  }
//-----------------------------------------------------------------------------
  void dly9()                                               // WASTE MUCH TIME
  {unsigned int xtime;
   xtime=100; while (xtime) {asm volatile ("nop"); dly1(); xtime--;}
  }
//-----------------------------------------------------------------------------
  void snd0()                                               // GENERATE A BEEP
  {I=Idot; while (I>0){mySND1; dly0();
                       mySND0; dly0(); I--;}
   I=Idot; while (I>0){mySND0; dly0(); I--;}    
  }
//-----------------------------------------------------------------------------
  void dlyE(unsigned int xtime)                             // Variable delay
  {while (xtime) {asm volatile ("nop"); xtime--;}
  }
//-----------------------------------------------------------------------------
  void sndxT(unsigned int xper)                             // GENERATE AUDIO
  {unsigned int xtime;                                      // SIDETONE CW
   I=Idot;
   while (I>0)
   {mySND1; xtime=xper; while (xtime){asm volatile ("nop"); xtime--;}
    mySND0; xtime=xper; while (xtime){asm volatile ("nop"); xtime--;}
    I--; 
   }
   I=Idot;
   while (I>0)
   {mySND0; xtime=xper; while (xtime){asm volatile ("nop"); xtime--;}
   I--;
   }
  }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  unsigned char TTYs(void)                                   // Get UART status
  {unsigned char statflag = 0x00;
   if (bit_is_set(STA0reg,DAV0bit)) {statflag=0xFF;}
   else                             {statflag=0x00;}
   return(statflag);
  }
//-----------------------------------------------------------------------------
  unsigned char TTYi(void)                                   // Get UART data
  {unsigned char rxd=0x00;
   rxd=UDR0reg; return(rxd);
  }
//-----------------------------------------------------------------------------
  void TTYo(unsigned short int txd)                          // Data to txd
  {UDR0reg=txd;
   loop_until_bit_is_set(STA0reg,TMB0bit);
  }
//-----------------------------------------------------------------------------
  void CRLF(void)                                            // CR and LINEFEED
  {TTYo(0x0D); TTYo(0x0A);
  }
//-----------------------------------------------------------------------------
  unsigned char TTYw(void)                                   // TTY is waiting
  {while(rxd!=0xFF)                                          // Wait for input
   {rxd=TTYs();                                              // read status
    if (bit_is_set(myTIFR,myTOV0))                           // second elapsed?
     {if (icp==myPULSE)                                      // update for LED
       {icp=0;
        if (bit_is_set(LED_REG,LED_BIT)) {LED_log0}          // ARDUINO-NANO
        else                             {LED_log1}          // LED PORT?
       }
      myQTR_agn; icp=icp+1;                                  // inc heartbeat
     }
   }
   rxd=TTYi();
   if ((rxd>=0x61)&(rxd<=0x7A)){rxd=rxd & 0xDF;}             // byte to upper
   return(rxd);                                              // return data
  }
//-----------------------------------------------------------------------------
// Function to read data from FPGA
// Function to read data from FPGA with proper timing
char TTYr() {
    char receivedData = 0;
    // Assuming PORTC3 is used for communication with the FPGA

    // Wait for start bit (assuming low start bit)
    while (PINC & (1 << PC3));

    // Move to the middle of the bit-cell before sampling the data
    delayMicroseconds(52 * 16 / 16 / 2);  // Half of the original delay value

    for (int i = 0; i < 8; ++i) {
        // Move to the middle of each data bit before sampling
        delayMicroseconds(104 * 16 / 16 / 2);  // Half of the original delay value

        if (PINC & (1 << PC3)) {
            receivedData |= (1 << i);
        }
    }

    // Wait for stop bit
    while (!(PINC & (1 << PC3)));
    return receivedData;
}

//------------------------------------------------------------------------------
  void Fmsg(const char text[])                               // text is stored
  {char c;                                                   // in prog-mem to
   const char* addr;                                         // save ram
   addr = text;
   while((c = pgm_read_byte(addr++))!= '\0') {TTYo(c);}
  }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef HAV_EEPROM           // use EEPROM to store config-data or text-msgs
//-----------------------------------------------------------------------------
//#define EEPE  EEWE        // check datasheet, values depend on chip
//#define EEMPE EEMWE       // check datasheet, values depend on chip

  unsigned char EErd(unsigned int uiAddress)  
  {while(EECR & (1<<EEPE))  // ** EEPROM still writing? Allow time.
   {asm volatile ("nop");}  // nop will not be removed by compiler
   EEAR = uiAddress;        // Set up address register
   EEARH = 0x00;            // Set up address register, use 256 Bytes
   EECR |= (1<<EERE);       // Start EEPROM READ by writing EERE
   return EEDR;             // Return data from Data Register
  }
//-----------------------------------------------------------------------------
  void EEwr(unsigned int uiAddress, unsigned char ucData)
  {while(EECR & (1<<EEPE))  // ** EEPROM busy writing?
   {asm volatile ("nop");}  // at least one SEMICOLON is needed!
   EEAR = uiAddress;        // setup address register
   EEARH= 0x00;             // setup address register, use 256 Bytes
   EEDR = ucData;           // setup data register
   EECR |= (1<<EEMPE);      // ** write logical one to EEMWE
   EECR |= (1<<EEPE);       // ** start eeprom write by setting EEWE=1
  }
//-----------------------------------------------------------------------------
  void Emsg(void)           // EEProm-Text-Sting to TTYo. Similar to FLmsg(..)
  {unsigned int adr=0x00;
   adr=0x00;
   rxd=EErd(adr);
   while (rxd!=0xFF){TTYo(rxd); adr++; rxd=EErd(adr);}
  }
#endif
//-----------------------------------------------------------------------------
// END OF THE BASIC INIT-FILE
//-----------------------------------------------------------------------------
