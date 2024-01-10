//-----------------------------------------------------------------------------
#warning ====== 2023-1217 =============> INCL: a10fpga.h =======>
//-----------------------------------------------------------------------------
const char m_arduC3[] PROGMEM = "-C3 <==== from FPGA_txd_o";
const char m_arduC2[] PROGMEM = "-C2 ==B=> to FPGA_s2_i, stop";
const char m_arduC1[] PROGMEM = "-C1 ==A=> to FPGA_s1_i, start";
const char m_arduC0[] PROGMEM = "-C0 ==R=> to FPGA_rb_i, aux. reset";
const char m_ardpins[] PROGMEM = "----> Arduino Pins <---- ";
const char m_ardFPGA[] PROGMEM = "----> entering FPGA-Mode ";
const char m_arddone[] PROGMEM = "----> leaving  FPGA-Mode ";

const char m_ardcmds[] PROGMEM = " FPGA-cmds: Rx,Ax,Bx,M,H?,Quit";
const char m_ardexc1[] PROGMEM = " R0: rb=log0, R1: rb=log1";
const char m_ardexc2[] PROGMEM = " A0: s1=log0, B1: s2=log1 ";
const char m_ardexc5[] PROGMEM = " M: measurement has been STARTED";
const char m_ardexc6[] PROGMEM = "    hit a key to STOP";
const char m_ardexc7[] PROGMEM = "    collect data";
const char m_ardexc8[] PROGMEM = " done";

//-----------------------------------------------------------------------------
void FPGA_Help(void)
{
     Fmsg(m_ardpins);
     CRLF();
     Fmsg(m_arduC3);
     CRLF();
     Fmsg(m_arduC2);
     CRLF();
     Fmsg(m_arduC1);
     CRLF();
     Fmsg(m_arduC0);
     CRLF();
     Fmsg(m_ardcmds);
     CRLF();
     Fmsg(m_ardexc1);
     CRLF();
     Fmsg(m_ardexc2);
     CRLF();
     Fmsg(m_ardexc5);
     CRLF();
     Fmsg(m_ardexc6);
     CRLF();
     Fmsg(m_ardexc7);
     CRLF();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define s2_i_log0           \
     PORTC &= ~_BV(PORTC2); \
     dly0(); // (0/1)   :  s2_i
#define s2_i_log1          \
     PORTC |= _BV(PORTC2); \
     dly0(); // (0/1)   :  s2_i
#define s1_i_log0           \
     PORTC &= ~_BV(PORTC1); \
     dly0(); // (0/1)   :  s1_i
#define s1_i_log1          \
     PORTC |= _BV(PORTC1); \
     dly0(); // (0/1)   :  s1_i
#define rb_i_log0           \
     PORTC &= ~_BV(PORTC0); \
     dly0(); // (0/1)   :  rb_i
#define rb_i_log1          \
     PORTC |= _BV(PORTC0); \
     dly0(); // (0/1)   :  rb_i
             //-----------------------------------------------------------------------------
             //-----------------------------------------------------------------------------
             // not finished, might be continued ....
             //-----------------------------------------------------------------------------

// Function to collect serial data from FPGA
void collectDataFromFPGA(char *receivedData)
{
     uint8_t dataIndex = 0;
     uint8_t rxd;

     // Wait for the start of a new transmission on PORTC3
     while (PINC & (1 << PC3))
          ;

     // Collect data until CRLF is received
     while (1)
     {
          // Check for transition to log0 (start bit)
          while (PINC & (1 << PC3))
               ;
          rxd = TTYr(); // Read UART from PORTC3

          // Validate stop bit (assuming stop bit is expected to be high)
          if (PINC & (1 << PC3))
          {
               // Invalid stop bit, handle the framing error as needed
               // For simplicity, we  discarded the current character and continue
               continue;
          }

          // Check for end of transmission (CRLF)
          if (rxd == 0x0D)
          {
               // Null-terminate the received data
               receivedData[dataIndex] = '\0';
               break;
          }

          // Store the received character
          receivedData[dataIndex] = rxd;
          dataIndex++;

          // Echo the received character to TTY
          TTYo(rxd);
     }
}

void FPGA_COMMAND(unsigned int mode)
//-----------------------------------------------------------------------------
// RE-init Data-Direction-Registers and the output of PORTC
{
     DDRC |= _BV(DDC0);
     PORTC |= _BV(PORTC0); // DDC0:out,PORTC0: FPGA_rb_i=1
     DDRC |= _BV(DDC1);
     PORTC |= _BV(PORTC1); // DDC1:out,PORTC1: FPGA_s1_i=1
     DDRC |= _BV(DDC2);
     PORTC |= _BV(PORTC2); // DDC2:out,PORTC2: FPGA_s2_i=1
     DDRC &= ~_BV(DDC3);
     PORTC |= _BV(PORTC3); // DDC3: in,PORTC3:  uC_txd_i=pup

     rb_i_log1; // to FPGA_rb, Reset, check FPGA-LED
     s1_i_log1; // to FPGA_s1, Messung Start, when 110...(reason: FPGA-pullup)
     s2_i_log1; // to FPGA_s2, Messung Stop, when 11110...(reason: FPGA-pullup)
                //-----------------------------------------------------------------------------
                //-----------------------------------------------------------------------------
     CRLF();
     Fmsg(m_ardFPGA);
     CRLF();
     FPGA_Help();
     //-----------------------------------------------------------------------------
     while (mode != 'Q')
     {
          CRLF();
          TTYo('<');
          TTYo('?');
          rxd0 = TTYw();
          TTYo(rxd0);
          TTYo('=');
          //-----------------------------------------------------------------------------
          switch (rxd0)
          {
               //-----------------------------------------------------------------------------
          case 'R': // Reset to FPGA
          {
               TTYo('0');
               TTYo('/');
               TTYo('1');
               TTYo('?');
               TTYo('>'); // choice is 0 or 1
               TTYo('R');
               TTYo('B');
               TTYo('=');
               rxd0 = TTYw();                     // enter selection
               if ((rxd0 == '0') | (rxd0 == '1')) // 0 or 1 is ok
               {
                    TTYo(rxd0);
                    if (rxd0 == '0')
                    {
                         rb_i_log0;
                    } //  rb: log0
                    if (rxd0 == '1')
                    {
                         rb_i_log1;
                    } //  rb: log1
               }
               else
               {
                    TTYo('?');
                    TTYo('?');
               } // invalid choice
               rxd0 = 0x00;
               break;
          }
               //-----------------------------------------------------------------------------
          case 'A': // WRITE BIT s1
          {
               TTYo('0');
               TTYo('/');
               TTYo('1');
               TTYo('?');
               TTYo('>'); // choice is 0 or 1
               TTYo('S');
               TTYo('1');
               TTYo('=');
               rxd0 = TTYw();                     // enter selection
               if ((rxd0 == '0') | (rxd0 == '1')) // 0 or 1 is ok
               {
                    TTYo(rxd0);
                    if (rxd0 == '0')
                    {
                         s1_i_log0;
                    } // s1: log0
                    if (rxd0 == '1')
                    {
                         s1_i_log1;
                    } // s1: log1
               }
               else
               {
                    TTYo('?');
                    TTYo('?');
               } // invalid choice
               rxd0 = 0x00;
               break;
          }
               //-----------------------------------------------------------------------------
          case 'B': // WRITE BIT s2
          {
               TTYo('0');
               TTYo('/');
               TTYo('1');
               TTYo('?');
               TTYo('>'); // choice is 0 or 1
               TTYo('S');
               TTYo('2');
               TTYo('=');
               rxd0 = TTYw();                     // enter selection
               if ((rxd0 == '0') | (rxd0 == '1')) // 0 or 1 is ok
               {
                    TTYo(rxd0);
                    if (rxd0 == '0')
                    {
                         s2_i_log0;
                    } // s2: log0
                    if (rxd0 == '1')
                    {
                         s2_i_log1;
                    } // s2: log1
               }
               else
               {
                    TTYo('?');
                    TTYo('?');
               } // bad choice

               asm volatile("nop"); // patch here
               asm volatile("nop"); // Receive data here
               asm volatile("nop"); // patch here

               CRLF();
               Fmsg(m_ardexc8);
               rxd0 = 0x00;
               break;
          }
               //-----------------------------------------------------------------------------
          case 'M': // Run a Measurement
          {
               s1_i_log1; // Set S1 = log0
               s2_i_log1; // Set S2 = log0
               CRLF();
               Fmsg(m_ardexc5); // Message to TTY
               CRLF();
               Fmsg(m_ardexc6); // Message to TTY
               CRLF();
               Fmsg(m_ardexc7); // Message to TTY
               s1_i_log0;       // S1=log1, Measurement starts
               rxd0 = TTYw();
               TTYo(rxd); // hit any key to stop
               s2_i_log0; // S2=log1, Measurement stops

               asm volatile("nop"); // patch here
               // asm volatile("nop"); // receive data here
               char receivedData[8];// data is stored here
               collectDataFromFPGA(receivedData);
               asm volatile("nop"); // patch here

               CRLF();
               Fmsg(m_ardexc8); // we are done
               s1_i_log1;       // Set S1 = log0
               s2_i_log1;       // Set S2 = log0
               rxd0 = 0x00;
               break;
          }
               //-----------------------------------------------------------------------------
               //-----------------------------------------------------------------------------
          case 'H':
          {
               rxd0 = '?';
          } // H: HELP-message
          case '?':
          {
               CRLF();
               FPGA_Help();
               rxd0 = 0x00;
               break;
          } // ?: HELP-message
            //-----------------------------------------------------------------------------
          case 'Q':
          {
               CRLF();
               mode = 'Q';
               rxd0 = 0x00;
               break;
          } // Q: QUIT
            //-----------------------------------------------------------------------------
          default:
          {
               if (rxd0 != 0x00)
               {
                    TTYo('>');
                    TTYo('?');
                    TTYo('?');
                    CRLF();
               }
               break;
          }
               //-----------------------------------------------------------------------------
          } // end of switch-case, all cases done
            //-----------------------------------------------------------------------------
     }      // end of choices: rb,s1,s2
            //-----------------------------------------------------------------------------
     Fmsg(m_arddone);
     CRLF(); // End of FPGA-command,
} // Arduino-nano driving external hardware
//-----------------------------------------------------------------------------
// END OF FPGA-SPECIFIC CODE ---------------------------------------------------
//-----------------------------------------------------------------------------
