
void TFT_SendCMD(unsigned char cmd) {
// 8080 MCU 8-bit bus interface I (page 27)
  TFT_CTRL_PORT = ((TFT_CTRL_PORT & 0b11111100) | (cmd & 0b00000011));// DATA[0..1]
  TFT_DATA_PORT = ((TFT_DATA_PORT & 0b00000011) | (cmd & 0b11111100));// DATA[2..7]
  TFT_CSX_LO;  // enable ILI9341 chip
  TFT_DCX_LO;  // commands
  TFT_RDX_HIGH;
  TFT_WRX_LO;
  #asm("nop");
  TFT_WRX_HIGH;// parallel data write strobe
}

void TFT_WriteData(unsigned char data) {
// 8080 MCU 8-bit bus interface I
  TFT_CTRL_PORT = ((TFT_CTRL_PORT & 0b11111100) | (data & 0b00000011));// DATA[0..1]
  TFT_DATA_PORT = ((TFT_DATA_PORT & 0b00000011) | (data & 0b11111100));// DATA[2..7]
  TFT_DCX_HIGH;// display RAM data or command’s parameters
  TFT_WRX_LO;
  #asm("nop");
  TFT_WRX_HIGH;// parallel data write strobe
}

//void TFT_SetOrientation(unsigned char orient){
//unsigned char t;
//  TFT_SendCMD(ILI9341_MADCTL);
//  switch (orient){
//     case 2: t = ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR; break;
//     case 3: t = ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR; break;
//     case 0: t = ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR; break;
//     case 1: t = ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR; break;
//    }
//  TFT_WriteData(t); TFT_CSX_HIGH; // disable ILI9341 chip
//  if(orient == 1 || orient == 3){max_X=TFTWIDTH-1, max_Y=TFTHEIGHT-1;} else {max_Y=TFTWIDTH-1, max_X=TFTHEIGHT-1;}
//  TFT_SetCol(0,max_X); TFT_SetPage(0,max_Y);
//}

void TFT_Init(void) {
//unsigned char orient
  TFT_RST_HIGH;
  delay_ms(1);
  TFT_SendCMD(0x01);
  TFT_WriteData(0x00);
  delay_ms(50);
//DISPLAYOFF
  TFT_SendCMD(0x28);
  TFT_WriteData(0x00);
//Power Control 1
  TFT_SendCMD(0xC0);
  TFT_WriteData(0x23);
//Power Control 2
  TFT_SendCMD(0xC1);
  TFT_WriteData(0x10);
//VCOM Control 1
  TFT_SendCMD(0xC5);
  TFT_WriteData(0x2B);
  TFT_WriteData(0x2B);
//VCOM Control 2
  TFT_SendCMD(0xC7);
  TFT_WriteData(0xC0);
//-MEMCONTROL-----------------
  TFT_SendCMD(ILI9341_MADCTL);
  TFT_WriteData(ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
  max_X=TFTWIDTH-1; max_Y=TFTHEIGHT-1;
  TFT_SetCol(0,max_X); TFT_SetPage(0,max_Y);
//----------------------------
//COLMOD: Pixel Format Set
  TFT_SendCMD(0x3A);
  TFT_WriteData(0x55);
//Frame Rate Control (In Normal Mode/Full Colors)
  TFT_SendCMD(0xB1);
  TFT_WriteData(0x00);
  TFT_WriteData(0x1B);
//ENTRYMODE
  TFT_SendCMD(0xB7);
  TFT_WriteData(0x07);
  TFT_SendCMD(ILI9341_SLEEPOUT);
  TFT_WriteData(0x00);
  delay_ms(150);
//Display On
  TFT_SendCMD(ILI9341_DISPLAYON);
  TFT_WriteData(0x00);
  delay_ms(500);
}

void TFT_SetCol(unsigned int startCol,unsigned int endCol) {
 TFT_SendCMD(0x2A);// Column Command address
 TFT_WriteData(startCol>>8); TFT_WriteData(startCol&0xFF);
 TFT_WriteData(endCol>>8); TFT_WriteData(endCol&0xFF);
 TFT_CSX_HIGH;// disable ILI9341 chip
}

void TFT_SetPage(unsigned int startPage,unsigned int endPage) {
 TFT_SendCMD(0x2B);// Page Command address
 TFT_WriteData(startPage>>8); TFT_WriteData(startPage&0xFF);
 TFT_WriteData(endPage>>8); TFT_WriteData(endPage&0xFF);
 TFT_CSX_HIGH;// disable ILI9341 chip
}

void TFT_SetPixel(unsigned int poX, unsigned int poY,unsigned int color) {
 TFT_SetCol(poX, poX);
 TFT_SetPage(poY, poY);
 TFT_SendCMD(0x2c);
 TFT_WriteData(color>>8); TFT_WriteData(color&0xFF);
 TFT_CSX_HIGH; // disable ILI9341 chip
}

void  TFT_DrawHorizontalLine( unsigned int poX, unsigned int poY, unsigned int length,unsigned int color) {
unsigned int i;
 TFT_SetCol(poX,poX + length);
 TFT_SetPage(poY,poY);
 TFT_SendCMD(0x2c);
 for(i=0; i<length; i++){TFT_WriteData(color>>8); TFT_WriteData(color&0xFF);}
 TFT_CSX_HIGH; // disable ILI9341 chip
}

void TFT_DrawVerticalLine( unsigned int poX, unsigned int poY, unsigned int length, unsigned int color) {
unsigned int i;
 TFT_SetCol(poX,poX);
 TFT_SetPage(poY,poY+length);
 TFT_SendCMD(0x2c);
 for(i=0; i<length; i++){TFT_WriteData(color>>8); TFT_WriteData(color&0xFF);}
 TFT_CSX_HIGH; // disable ILI9341 chip
}
/*
void TFT_ClearScreen(void)
{
unsigned int i;
	TFT_SetCol(0, MAX_X);
	TFT_SetPage(0, max_Y);
	TFT_SendCMD(0x2C);                                                  // start to write to display ram
	for(i=0; i<38400; i++){TFT_WriteData(~0);TFT_WriteData(~0);TFT_WriteData(~0);TFT_WriteData(~0);}
	TFT_CSX_HIGH;// disable ILI9341 chip
}
*/