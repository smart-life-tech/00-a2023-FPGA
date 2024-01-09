//=============================================================================
//===2023-1217==========> THE SOURCE IS THE DOCU <=============================
//=============================================================================
// Some remarks:
// -1- This text must be compiled using the arduino-ide.
//     This is because of the USB-Serial-I/O of the Arduino-Board.
// -2- If you want to avoid the arduino-ide: patch the TTY @PORTs D0,D1
//     and use an external RS232-Level-Converter!
//     Run GCC and (e.g.) avrdude.
// -3- Use gtkterm, minicom,...
//=============================================================================
// TODO: ELbug: activate Morse-Keyer after power-up
//       ELbug: add dot/dash-storage
//=============================================================================
// BELOW: FOOTPRINT of ARDUINO-BOARD
//==================================
//             +--xxxxx--+
//       txd/D1!01 x x 30!VIN,via ext.5V          Ext. 5V VCC
//       rxd/D0!02     29!GND                     FUSES:
//     RESET/C6!03     28!C6/RESET                lfuse=0xE0 : ext.Xtal
//          GND!04     27!+5Vlocal                lfuse=0xE4 : int.8MHz
//           D2!05     26!A7 /PINA7               hfuse=0xD9 : disables JTAG
//           D3!06     25!A6 /PINA6 FPGA MORSE
//           D4!07     24!C5 /PINC5 FPGA QUIT     Terminal-Mode:
//  LEDonAIR D5!08     23!C4 /PINC4 FPGA 12WPM    lfuse: 0x00 0xE4 (int.8MHz)
// LEDbeacon D6!09     22!C3 /PINC3 TTYi QRQ/QRS  hfuse: 0x00 0xD9 (dis.JTAG)
//   mySOUND D7!10     21!C2 /PINC2 S2   dash     wr memtype addr byte
//           B0!11     20!C1 /PINC1 S1   dots
//           B1!12     19!C0 /PINC0 rb   manKEY   CPU:ATmega168P,ATmega328P
//           B2!13     18!AREF                    XTAL20MHz
//      MOSI/B3!14     17!3V3 local
//  LED/MISO/B4!15     16!B5/SCK/LED     Arduino-Nano-LED,Heartbeat
//             +--plug2--+
//                 usb
//==================================
// ABOVE: FOOTPRINT of ARDUINO-BOARD
//=============================================================================
//=============================================================================
// BELOW: FOOTPRINT of ATMEGA328P, DIL28-PACKAGE
//==============================================
//              +--xxxxx--+
//     RESET C6!01 x x 28!C5  QUIT               Ext. 5V VCC
//       RXD D0!02     27!C4  12WPM              FUSES:
//       TXD D1!03     26!C3  QRQ/QRS  <==txd_o  lfuse=0xE0 : ext.Xtal
//           D2!04     25!C2  dash     S2=====>  lfuse=0xE4 : int.8MHz
//           D3!05     24!C1  dots     S1=====>  hfuse=0xD9 : disables JTAG
//           D4!06     23!C0  manKEY   RESET==>
//          VCC!07     22!GND                     Terminal-Mode:
//          GND!08     21!AREF                    lfuse: 0x00 0xE4 (int.8MHz)
//     XTAL1 B6!09     20!AVCC                    hfuse: 0x00 0xD9 (dis.JTAG)
//     XTAL2 B7!10     19!B5  SCK                 wr memtype addr byte
//  LEDonAIR D5!11     18!B4  MISO
// LEDbeacon D6!12     17!B3  MOSI               CPU:ATmega168P,ATmega328P
//   mySOUND D7!13     16!B2                     XTAL20MHz
//           B0!14     15!B1
//             +--plug2--+
//                 usb
//==============================================
// ABOVE: FOOTPRINT of ATMEGA328P, DIL28-PACKAGE
//=============================================================================
//=============================================================================
#define HAV_Arduino  // ONLY FOR ARDUINO-STUFF, ELSE USE GCC
#define HAV_16MHz    // CHECK XTAL-FREQUENCY
#define HAV_EXT_FPGA // FPGA...
// #define HAV_EKEY                     // Morse-Keyer, not activated
// #define HAV_EEPROM                   // not activated

#include <avr/io.h>       // ATMEL:   memory map
#include <avr/pgmspace.h> // ATMEL:   access FLASH-storage
#ifdef HAV_Arduino        // Arduino: needed for serial in/out!
#include "Arduino.h"      // Arduino: connects to USB.
#endif                    // gtkterm: no ttyS0, use ttyUSB!

const char msgH0[] PROGMEM = "=> FILE:  00-a2023-FPGA.ino @2023-1217 20:00";
const char msgH1[] PROGMEM = "=> Setup: 328, Uno: /dev/ACM0               ";
const char msgH2[] PROGMEM = "=>        328, Nano, ATm328P(Old Bootloader)";
const char msgH3[] PROGMEM = "=>        168, Nano: ATm168                 ";
const char msgH4[] PROGMEM = "=> Tools: Serial Monitor: no-line-ending!   ";
const char msgHH[] PROGMEM = "=> ?Cmds: <?,H> to HELP                     ";

unsigned char rxd = 0x30;      // Byte received
unsigned char rxd0 = 0x30;     // Byte received, for external stuff
unsigned char rxd1 = 0x30;     // Byte received, for external stuff
unsigned char txd = 0x30;      // Byte to be TXed
unsigned char dav = 0x00;      // New data available
unsigned char mode = 0x30;     // Normal Mode, when '0'
unsigned short int icp = 0x00; // Heartbeat ticks
unsigned int I = 0;            // Loop index
unsigned int Idot = 100;       // CW-dots, used for sound
unsigned int icur = 0;         // Cursor, when having a LCD

//=============================================================================
#include "./a00init.h" // a00init.h, GCC, inits Ports
#include "./a06misc.h" // a06misc.h, GCC, misc. Routines
//=============================================================================
#ifdef HAV_EXT_FPGA
#include "./a10fpga.h" // a10fpga.h, GCC, FPGA
#endif
//=============================================================================
#ifdef HAV_EKEY
#include "./a11ekey.h" // a11ekey.h, GCC, Morse-Keyer
#endif
//=============================================================================
const char m_ax[] PROGMEM = "#--------------------------------------";
const char m_a0[] PROGMEM = "# x   :x=<H?0>,   ?H:Help";
const char m_a1[] PROGMEM = "# x   :x=<F>       FPGA";
#ifdef HAV_EKEY
const char m_a2[] PROGMEM = "# x   :x=<M>       MORSE-EKeyer";
#endif
const char m_a3[] PROGMEM = "# Sxy :x=<BCDZ>;  y=00:IN,y=FF:OUT";
const char m_a4[] PROGMEM = "# Rx  :x=<BCD>";
const char m_a5[] PROGMEM = "# RPx :x=<BCD>";
const char m_a6[] PROGMEM = "# Wxy :x=<BCDT><nn>; T=TTYo";
const char m_notavail[] PROGMEM = "# not available";
const char m_WHAT[] PROGMEM = "# WHAT?";
const char m_nega[] PROGMEM = "# 168,328, check datasheet";

// Enumeration for the states of data collection
enum DataCollectionState
{
   WAIT_START,
   COLLECT_DATA,
   WAIT_LF,
   WAIT_CR
};

// Function to collect six serial ASCII chars followed by CR and LF
// Returns 1 if successful, 0 otherwise
uint8_t collectDataFromFPGA(char *receivedData)
{
   static enum DataCollectionState state = WAIT_START;
   static uint8_t dataIndex = 0;
   uint8_t rxd;

   switch (state)
   {
   case WAIT_START:
      if (TTYs() == 0xFF)
      {
         state = COLLECT_DATA;
      }
      break;

   case COLLECT_DATA:
      if (TTYs() == 0xFF)
      {
         rxd = TTYi(); // Read UART

         // Convert lowercase to uppercase
         if ((rxd >= 0x61) && (rxd <= 0x7A))
         {
            rxd &= 0xDF; // Convert to uppercase
         }

         receivedData[dataIndex] = rxd;
         dataIndex++;

         TTYo(rxd); // Echo to TTY

         // Check if collected 6 characters
         if (dataIndex >= 6)
         {
            state = WAIT_LF;
         }
      }
      break;

   case WAIT_LF:
      if (TTYs() == 0xFF)
      {
         rxd = TTYi();
         if (rxd == 0x0A)
         {
            receivedData[dataIndex] = rxd;
            dataIndex++;
            state = WAIT_CR;
         }
      }
      break;

   case WAIT_CR:
      if (TTYs() == 0xFF)
      {
         rxd = TTYi();
         if (rxd == 0x0D)
         {
            receivedData[dataIndex] = rxd;
            dataIndex++;
            receivedData[dataIndex] = '\0'; // Null-terminate the string
            state = WAIT_START;
            return 1; // Successfully collected data
         }
         else
         {
            // Unexpected character, reset state
            state = WAIT_START;
         }
      }
      break;
   }

   return 0; // Data collection not complete yet
}

void HELPmsg(void)
{
   CRLF();
   Fmsg(msgH0);
   CRLF();
   Fmsg(msgH1);
   CRLF();
   Fmsg(m_ax);
   CRLF();
   Fmsg(m_a0);
   CRLF();
   Fmsg(m_a1);
#ifdef HAV_EKEY
   CRLF();
   Fmsg(m_a2);
#endif
   CRLF();
   Fmsg(m_a3);
   CRLF();
   Fmsg(m_a4);
   CRLF();
   Fmsg(m_a5);
   CRLF();
   Fmsg(m_a6);
   CRLF();
   Fmsg(m_ax);
   CRLF();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(void)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
{
   init_port(); // init ports     <------HARDWARE!!!--
//-----------------------------------------------------------------------------
#ifdef HAV_Arduino
   Serial.begin(9600); // FOR ARDUINO, USB-Interface
#else
   TTY0baud;
   TTY0mode;
   TTY0ena; // expects MOSI,MISO,SCP and TTY
#endif
   //-----------------------------------------------------------------------------
   rxd = UDR0reg; // dump transient garbage after boot
   snd0();
   CRLF();
   TTYo('!'); // make a noise, writeUART ok?
//-----------------------------------------------------------------------------
#ifdef HAV_EEPROM // EEPROM? ---------------------------
   TTYo(' ');
   TTYo(':');        // Space and Colon to TTY
   EEwr(0x00, 0x40); // write to EEPROM, adr=0x00
   EEwr(0x01, 0x41); // write to EEPROM, adr=0x01
   EEwr(0x02, 0xFF); // write to EEadr=0x02, done
   TTYo(EErd(0x00)); // read EEPROM, adr=0x00
   TTYo(EErd(0x01)); // read EEPROM, adr=0x01
   TTYo(':');
   TTYo(' '); // EEprom-String should be: @A
   Emsg();    // Emsg diplays @A om the screen
#endif        // -----------------------------------
              //-----------------------------------------------------------------------------
   CRLF();
   Fmsg(msgH0);
   CRLF(); // message after reset
   Fmsg(msgH1);
   CRLF(); // message after reset
   Fmsg(msgH2);
   CRLF(); // message after reset
   Fmsg(msgH3);
   CRLF(); // message after reset
   Fmsg(msgH4);
   CRLF(); // message after reset
   Fmsg(msgHH);
   CRLF();  // message after reset
            //-----------------------------------------------------------------------------
            //-----------------------------------------------------------------------------
   for (;;) // loop forever ......
   {
      //-----------------------------------------------------------------------------
      icp = doqtr(icp); // heartbeat
                        //-----------------------------------------------------------------------------
                        // declared a char array to store the received data
      char receivedData[8];
      // Call the function to collect data from FPGA
      // Now, 'receivedData' contains the 6 ASCII characters followed by LF and CR
      // You can use or process the data as needed.
      // Non-blocking data collection
      if (collectDataFromFPGA(receivedData))
      {
         // Successfully collected data
         // 'receivedData' contains the 6 ASCII characters followed by LF and CR
      }

      // Continue with other non-blocking tasks

      dav = TTYs(); // any UART-Input?

      if (dav == 0xFF) // yes, when 0xFF
      {
         rxd = TTYi(); // read UART
         if ((rxd >= 0x61) & (rxd <= 0x7A))
         {
            rxd = rxd & 0xDF;
         }          // do upper case
         TTYo(rxd); // echo to TTY
         if (rxd == 0x0D)
         {
            TTYo(0x0A);
         } // add CRLF
      }
      //-----------------------------------------------------------------------------
      if (dav == 0xFF) // dav was 0xFF,then
      {                // we have UART data
                       //-----------------------------------------------------------------------------
         switch (rxd)  // process choices
         {
            //-----------------------------------------------------------------------------
         case 'S': // Set IO-Mode for Ports A6,A7,B,C,D?
         {
            rxd = TTYw();
            TTYo(rxd);
            if (rxd == 'A')
            {
               Fmsg(m_nega);
               CRLF();
               rxd = 0x00;
            } // m168: A6
            if (rxd == 'B')
            {
               rxd = hexcom();
               DDRB = rxd;
               rxd = 0x00;
            } // Set PORTB mode
            if (rxd == 'C')
            {
               rxd = hexcom();
               DDRC = rxd;
               rxd = 0x00;
            } // Set PORTC mode
            if (rxd == 'D')
            {
               rxd = hexcom();
               DDRD = rxd;
               rxd = 0x00;
            } // Set PORTD mode
            if (rxd == 'Z')
            {               // ALL to INPUT!
               DDRB = 0x00; // DDRA=0x00 IS
               DDRC = 0x00; // NOT available
               DDRD = 0x00;
            } // on m168
            CRLF();
            rxd = 0x00;
            break;
         }
            //-----------------------------------------------------------------------------
         case 'W': // Wx writes PORTs ?=  A6,A7,BCD
         {
            rxd = TTYw();
            TTYo(rxd);
            if (rxd == 'A')
            {
               Fmsg(m_nega);
               CRLF();
               rxd = 0x00;
            } // m168
            if (rxd == 'B')
            {
               TTYo('?');
               rxd = hexcom();
               PORTB = rxd;
               rxd = 0x00;
            } // PORTB
            if (rxd == 'C')
            {
               TTYo('?');
               rxd = hexcom();
               PORTC = rxd;
               rxd = 0x00;
            } // PORTC
            if (rxd == 'D')
            {
               TTYo('?');
               rxd = hexcom();
               PORTD = rxd;
               rxd = 0x00;
            } // PORTD
            if (rxd == 'T')
            {
               TTYo('?');
               rxd = hexcom();
               TTYo(rxd);
               rxd = 0x00;
            } // TTYout
            if (rxd != 0x00)
            {
               Fmsg(m_WHAT);
            }
            TTYo('.');
            TTYo('.');
            rxd = 0x00;
            break;
         }
            //-----------------------------------------------------------------------------
            //-----------------------------------------------------------------------------
         case 'R': // Rx reads PORTX
         {
            rxd = TTYw();
            TTYo(rxd);      // wait for input
            if (rxd == 'P') // RPx reads PINx
            {
               rxd = TTYw();
               TTYo(rxd);
               TTYo(':'); // which PIN?
               if (rxd == 'A')
               {
                  Fmsg(m_nega);
                  CRLF();
                  rxd = 0x00;
               } // m168
               if (rxd == 'B')
               {
                  rxd = PINB;
                  tbcd2(rxd);
                  rxd = 0x00;
               } // PIN B?
               if (rxd == 'C')
               {
                  rxd = PINC;
                  tbcd2(rxd);
                  rxd = 0x00;
               } // PIN C?
               if (rxd == 'D')
               {
                  rxd = PIND;
                  tbcd2(rxd);
                  rxd = 0x00;
               } // PIN D?
            }
            if (rxd == 'A')
            {
               Fmsg(m_nega);
               CRLF();
               rxd = 0x00;
            }
            if (rxd == 'B')
            {
               TTYo(':');
               rxd = PORTB;
               tbcd2(rxd);
               rxd = 0x00;
            } // rd PORTB
            if (rxd == 'C')
            {
               TTYo(':');
               rxd = PORTC;
               tbcd2(rxd);
               rxd = 0x00;
            } // rd PORTC
            if (rxd == 'D')
            {
               TTYo(':');
               rxd = PORTD;
               tbcd2(rxd);
               rxd = 0x00;
            } // rs PORTD
            TTYo('.');
            TTYo('.');
            CRLF();
            rxd = 0x00;
            break;
         }
            //-----------------------------------------------------------------------------
            // USED FOR FPGA-Hardware. Add more activities, if needed.
            //-----------------------------------------------------------------------------
         case 'F':
         {
            CRLF();
#ifdef HAV_EXT_FPGA // Any external hardware?
            FPGA_COMMAND(mode = '0');
#else
            Fmsg(m_notavail);
#endif
            CRLF();
            rxd = 0x00;
            break;
         }
            //-----------------------------------------------------------------------------
         case 'M':
         {
            CRLF();
#ifdef HAV_EKEY
            Ekey(0x00);
#else
            Fmsg(m_notavail);
#endif
            CRLF();
            rxd = 0x00;
            break;
         }
            //-----------------------------------------------------------------------------
         case '0':
         {
            CRLF();
            Fmsg(m_notavail);
            CRLF();
            rxd = 0x00;
            break;
         }
         case 'H':
         {
            rxd = '?';
         }
         case '?':
         {
            CRLF();
            HELPmsg();
            CRLF();
            rxd = 0x00;
            break;
         }
            //-----------------------------------------------------------------------------
            //-----------------------------------------------------------------------------
         default:
         {
            if (rxd != 0x0D)
            {
               TTYo('>');
               TTYo('E');
               TTYo('R');
               TTYo('R');
               TTYo(' ');
               CRLF();
            }
            break;
         }
            //-----------------------------------------------------------------------------
         }           // end of switch-case, cases done
         dav = 0x00; // clear dav-flag and
      }              // all activities caused by dav-flag
                     //-----------------------------------------------------------------------------
   }                 // Ende: for-loop
} // Ende: main
//-----------------------------------------------------------------------------
