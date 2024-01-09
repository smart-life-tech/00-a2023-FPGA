//-----------------------------------------------------------------------------
#warning ====== 2023-1217 =============> INCL: a20misc.h =======>
//-----------------------------------------------------------------------------
// n2b: ASCII 0..F is converted to 0x00 .. 0x0F
//
  char n2b(unsigned short int txdh)
  {char txd=0x30;
   if (txdh=='0'){txd=0x00;} if (txdh=='1'){txd=0x01;}
   if (txdh=='2'){txd=0x02;} if (txdh=='3'){txd=0x03;}
   if (txdh=='4'){txd=0x04;} if (txdh=='5'){txd=0x05;}
   if (txdh=='6'){txd=0x06;} if (txdh=='7'){txd=0x07;}
   if (txdh=='8'){txd=0x08;} if (txdh=='9'){txd=0x09;}
   if (txdh=='A'){txd=0x0A;} if (txdh=='B'){txd=0x0B;}
   if (txdh=='C'){txd=0x0C;} if (txdh=='D'){txd=0x0D;}
   if (txdh=='E'){txd=0x0E;} if (txdh=='F'){txd=0x0F;}
   return txd;
  }
//-----------------------------------------------------------------------------
// hex2byte: converts two ASCII chars to one BYTE
//
  unsigned short int hex2byte(unsigned short int txdh,unsigned short int txdl)
  {unsigned short int txd; unsigned short int txd1; unsigned short int txd2;
   txd1 = n2b(txdh); txd1 = txd1 & 0x0F; txd1 = txd1 << 4; txd1 = txd1 & 0xF0;
   txd2 = n2b(txdl); txd2 = txd2 & 0x0F;
   txd  = txd2 | txd1;
   return txd;
  }
//-----------------------------------------------------------------------------
// nib2asc: converts a nibble (xxxx0000..xxxx1111) to ASCII-'0'..'F'
//
  unsigned short int nib2asc(unsigned short int txd)
  {unsigned short int tx=0x00;
   tx=txd & 0x0F;
   if ((tx>=0x00) & (tx<=0x09)) {tx=tx | 0x30;}
   else {tx=tx-0x09; tx=tx | 0x40;}
   return tx;
  }
//-----------------------------------------------------------------------------
// tbcd2: tabulate byte
//
  void tbcd2(unsigned short int txd)
  {unsigned short int txdl; unsigned short int txdh;
   txdl = txd; txdh = txd >> 4;
   txdh = txdh & 0x0F; txdh = nib2asc(txdh); TTYo(txdh);
   txdl = txdl & 0x0F; txdl = nib2asc(txdl); TTYo(txdl); TTYo(' ');
  }
//-----------------------------------------------------------------------------
// hexcom: hex input. read two chars, converts to hex
//
  unsigned short int hexcom(void)
  {unsigned short int rxd; unsigned short int rxdh; unsigned short int rxdl;
   TTYo('.');
   rxdh=TTYw();TTYo(rxdh);if ( (rxdh>=0x61) & (rxdh<=0x7A) ){rxdh=rxdh & 0xDF;}
   rxdl=TTYw();TTYo(rxdl);if ( (rxdl>=0x61) & (rxdl<=0x7A) ){rxdl=rxdl & 0xDF;}
   rxd=hex2byte(rxdh,rxdl);
   TTYo('.');
   return(rxd);
  }
//-----------------------------------------------------------------------------
// END OF THE MISCELLANEOUS CODE
//-----------------------------------------------------------------------------

