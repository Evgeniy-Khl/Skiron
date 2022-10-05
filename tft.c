

unsigned int constrain(unsigned int a, unsigned int b, unsigned int c)
{
  if (a < b) return b;
  if (c < a) return c;
  else return a;
}

void InitInterface(void){
  TFT_CTRL_DIR |= (1 << TFT_RDX) | (1 << TFT_WRX) | (1 << TFT_DCX) | (1 << TFT_CSX) | (1 << TFT_RST);
  TFT_CTRL_DIR |= 0x03;// DATA[0..1]
  TFT_CTRL_PORT |= (1 << TFT_RDX) | (1 << TFT_WRX) | (1 << TFT_DCX) | (1 << TFT_CSX) & (~(1 << TFT_RST));
  TFT_DATA_DIR |= 0xFC;// DATA[2..7]
  TFT_DATA_PORT &= 0x03;// DATA[2..7]
}

void TFT_FillScreen(unsigned int XL, unsigned int XR, unsigned int YU, unsigned int YD, unsigned int color)
{
unsigned long XY=0;
unsigned long i=0;
 if(XL > XR){XL = XL^XR; XR = XL^XR; XL = XL^XR;}
 if(YU > YD){YU = YU^YD; YD = YU^YD; YU = YU^YD;}
 XL = constrain(XL, 0, max_X);
 XR = constrain(XR, 0, max_X);
 YU = constrain(YU, 0, max_Y);
 YD = constrain(YD, 0, max_Y);
#pragma warn-
 XY = (XR-XL+1);
 XY = XY*(YD-YU+1);
#pragma warn+
 TFT_SetCol(XL,XR);
 TFT_SetPage(YU, YD);
 TFT_SendCMD(0x2C); // start to write to display ram
 for(i=0; i < XY; i++){TFT_WriteData(color>>8); TFT_WriteData(color&0xFF);}
 TFT_CSX_HIGH; // disable ILI9341 chip
}

void TFT_FillRectangle(unsigned int poX, unsigned int poY, unsigned int length, unsigned int width, unsigned int color){
 TFT_FillScreen(poX, poX+length, poY, poY+width, color);
}

void TFT_DrawLine( unsigned int x0,unsigned int y0,unsigned int x1, unsigned int y1,unsigned int color){

	unsigned int x = x1-x0;
	unsigned int y = y1-y0;
	unsigned int dx = abs(x), sx = x0<x1 ? 1 : -1;
	unsigned int dy = -abs(y), sy = y0<y1 ? 1 : -1;
	unsigned int err = dx+dy, e2;                                                // error value e_xy
	for (;;){                                                           // loop
		TFT_SetPixel(x0,y0,color);
		e2 = 2*err;
		if (e2 >= dy) {                                                 // e_xy+e_x > 0
			if (x0 == x1) break;
			err += dy; x0 += sx;
		}
		if (e2 <= dx) {                                                 // e_xy+e_y < 0
			if (y0 == y1) break;
			err += dx; y0 += sy;
		}
	}

}

void TFT_DrawRectangle(unsigned int poX, unsigned int poY, unsigned int length, unsigned int width,unsigned int color){
 TFT_DrawHorizontalLine(poX, poY, length, color);
 TFT_DrawHorizontalLine(poX, poY+width, length, color);
 TFT_DrawVerticalLine(poX, poY, width,color);
 TFT_DrawVerticalLine(poX + length, poY, width,color);
}

//+++++++++++++ Draw a Circle ++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TFT_DrawCircle(int x0, int y0, int r, unsigned int color)
 {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  TFT_SetPixel(x0  , y0+r, color);
  TFT_SetPixel(x0  , y0-r, color);
  TFT_SetPixel(x0+r, y0  , color);
  TFT_SetPixel(x0-r, y0  , color);

  while (x<y)
   {
    if(f >= 0){y--; ddF_y += 2; f += ddF_y;}
    x++; ddF_x += 2; f += ddF_x;
    TFT_SetPixel(x0 + x, y0 + y, color);
    TFT_SetPixel(x0 - x, y0 + y, color);
    TFT_SetPixel(x0 + x, y0 - y, color);
    TFT_SetPixel(x0 - x, y0 - y, color);
    TFT_SetPixel(x0 + y, y0 + x, color);
    TFT_SetPixel(x0 - y, y0 + x, color);
    TFT_SetPixel(x0 + y, y0 - x, color);
    TFT_SetPixel(x0 - y, y0 - x, color);
  }
}

//+++++++++++++ Draw a Circle Helper ++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TFT_DrawCircleHelper(int x0, int y0, int r, char cornername, unsigned int color)
{
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  while (x<y)
   {
    if(f >= 0){y--; ddF_y += 2; f+= ddF_y;}
    x++; ddF_x += 2; f+= ddF_x;
    if(cornername & 0x4){TFT_SetPixel(x0 + x, y0 + y, color); TFT_SetPixel(x0 + y, y0 + x, color);} 
    if(cornername & 0x2){TFT_SetPixel(x0 + x, y0 - y, color); TFT_SetPixel(x0 + y, y0 - x, color);}
    if(cornername & 0x8){TFT_SetPixel(x0 - y, y0 + x, color); TFT_SetPixel(x0 - x, y0 + y, color);}
    if(cornername & 0x1){TFT_SetPixel(x0 - y, y0 - x, color); TFT_SetPixel(x0 - x, y0 - y, color);}
   }
}

//+++++++++++++ Draw a Fill Circle Helper ++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TFT_FillCircleHelper(int x0, int y0, int r, char cornername, int delta, unsigned int color)
{

  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  while (x<y) {
    if(f >= 0){y--; ddF_y += 2; f+= ddF_y;}
    x++; ddF_x += 2; f+= ddF_x;
    if(cornername & 0x1){TFT_DrawVerticalLine(x0+x, y0-y, 2*y+1+delta, color); TFT_DrawVerticalLine(x0+y, y0-x, 2*x+1+delta, color);}
    if(cornername & 0x2){TFT_DrawVerticalLine(x0-x, y0-y, 2*y+1+delta, color); TFT_DrawVerticalLine(x0-y, y0-x, 2*x+1+delta, color);}
  }
}

//+++++++++++++ Draw a rounded rectangle ++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TFT_DrawRoundRect(int x, int y, int w, int h, int r, unsigned int color)
{
  // smarter version
  TFT_DrawHorizontalLine(x+r, y    , w-2*r, color); // Top
  TFT_DrawHorizontalLine(x+r, y+h-1, w-2*r, color); // Bottom
  TFT_DrawVerticalLine(x    , y+r  , h-2*r, color); // Left
  TFT_DrawVerticalLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  TFT_DrawCircleHelper(x+r    , y+r    , r, 1, color);
  TFT_DrawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  TFT_DrawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  TFT_DrawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

//++++++++++++++ Fill a rounded rectangle ++++++++++++++++++++++++++++++++++++++++++++++++++++
void TFT_FillRoundRect(int x, int y, int w, int h, int r, unsigned int color)
{
  // smarter version
  TFT_FillRectangle(x+r, y, w-2*r, h, color);

  // draw four corners
  TFT_FillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  TFT_FillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

void TFT_FillCircle(unsigned int poX, unsigned int poY, unsigned int r,unsigned int color)
{
	unsigned int x = -r, y = 0, err = 2-2*r, e2;
	do {

		TFT_DrawVerticalLine(poX-x, poY-y, 2*y, color);
		TFT_DrawVerticalLine(poX+x, poY-y, 2*y, color);

		e2 = err;
		if (e2 <= y) {
			err += ++y*2+1;
			if (-x == y && e2 <= x) e2 = 0;
		}
		if (e2 > x) err += ++x*2+1;
	} while (x <= 0);

}

void TFT_DrawChar( unsigned char ascii, unsigned int poX, unsigned int poY, unsigned char size, unsigned char bold, unsigned int fgcolor, unsigned int gcolor){
unsigned char temp=0, k, f;
unsigned int i;
  if(size == 0) size=1;
  if((ascii>=32)&&(ascii<=255)){;} else {ascii = '?'-32;} // если вне диапазона то ascii = 0x1F
  //------------- полоска фона перед символом -------------------------------------------
  for(f=0; f<FONT_X; f++){
     #pragma warn-
     TFT_FillRectangle(poX, poY+f*size, size, size, gcolor);
     #pragma warn+
    }
  poX++;
  //-------------------------------------------------------------------------------------
  for (i = 0; i < FONT_X; i++ ){
     // для информации FONT_X = 16
     if((ascii >= 0x20) && (ascii <= 0x7F)){temp =Font16x16[ascii-0x20][i];}// латиница
     else if(ascii >= 0xC0){temp =Font16x16[ascii-0x65][i];}                // кирилица
     k=i / 8; // определитель верхнняя части символа или нижняя 
     for(f =0 ; f < FONT_Y; f++){
        if((temp>>f)&0x01){
          #pragma warn-
          TFT_FillRectangle(poX+i*size-(k*8)*size, poY+f*size+(k*8)*size, size, size, fgcolor);
          #pragma warn+
         }
        else{
          #pragma warn-
          TFT_FillRectangle(poX+bold+i*size-(k*8)*size, poY+bold+f*size+(k*8)*size, size, size, gcolor);
          #pragma warn+
         }
       }
    }
  //------------- полоска фона после символом -------------------------------------------
  if(size>1){
    poX += FONT_X;
    for(f=0; f<FONT_X; f++){
       #pragma warn-
       TFT_FillRectangle(poX, poY+f*size, size, size, gcolor);
       #pragma warn+
      }
   }
  //-------------------------------------------------------------------------------------
}

void TFT_DrawString(char *string, unsigned int poX, unsigned int poY, unsigned char size, unsigned char bold, unsigned int fgcolor, unsigned int gcolor){
  while(*string)
   {
    if(size == 0) size=1;
    bold &= 1;
#pragma warn-
    if((poX + FONT_SPACE) > max_X-1){poX = 1; poY = poY + FONT_X*size;}
    TFT_DrawChar(*string, poX, poY, size, bold, fgcolor, gcolor);
    poX += FONT_SPACE*size;
#pragma warn+
    *string++;
   }
}
/*
void showTouched(char x,char y,char w,char h)// окно координаты
{
  x = (TFTWIDTH - (w*2+2)); // X
#pragma warn-
  TFT_FillRectangle(x, y, w*2+2, h, WHITE);  // tft.fillRect(x, y, w*2, h, WHITE);   // For cleanup
  TFT_DrawRectangle(x, y, w, h, RED);      // tft.drawRect(x, y, w, h, RED);       // For X
  TFT_DrawRectangle(x+w+2, y, w, h, RED);// tft.drawRect(x+w+2, y, w*2, h, RED); // For Y
  sprintf(buff,"X=%05u",point_X); TFT_DrawString(buff,x+2, y+1, 1, BLACK);
  sprintf(buff,"Y=%05u",point_Y); TFT_DrawString(buff,x+2+w+2, y+1, 1, BLACK);
#pragma warn+
  TFT_FillRectangle(point_X, point_Y, 2, 2, WHITE);
}
*/
/*
unsigned int changeEndianness16(unsigned int val)// синхронная перестановка байт в 16-разрядном числе
{
union {unsigned char ch[2]; unsigned int value;} bytes;
    char x;
    bytes.value=val;

    x= bytes.ch[0];
    bytes.ch[0]=bytes.ch[1];
    bytes.ch[1]=x;
    return bytes.value;
}

    //hitet = *ptr++;
    //hitet = changeEndianness16(hitet);
*/