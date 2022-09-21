#define YP_DIR          DDRA.TFT_WRX   // TFT_WRX=6
#define YP_PORT         PORTA.TFT_WRX  // TFT_WRX=6
#define YP_PIN          TFT_WRX        // = 6

#define YM_DIR          DDRC.7
#define YM_PORT         PORTC.7
  
#define XP_DIR          DDRC.6
#define XP_PORT         PORTC.6

#define XM_DIR          DDRA.TFT_DCX   // TFT_DCX=5
#define XM_PORT         PORTA.TFT_DCX  // TFT_DCX=5
#define XM_PIN          TFT_DCX        // = 5

#define MINPRESSURE     10
#define MAXPRESSURE     1000
#define INPUT           0
#define OUTPUT          1
#define LOW             0
#define HIGH            1

#define TS_MINX        125
#define TS_MAXX        907
#define TS_MINY        130
#define TS_MAXY        935

unsigned char buttonCount;
unsigned int point_X, point_Y, point_Z, bottom_Y;
struct ram_structure {int x,y; char w,h;} buttons[12];// Global structure located in RAM

// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input)
{
adc_input |= (ADC_VREF_TYPE & 0xff);
ADMUX=adc_input;
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=0x40;
// Wait for the AD conversion to complete
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCW;
}

void initializeButtons(char col, char row, char h)// высота кнопки
{
  char i,j,indx, w;
  unsigned int x,y;
  switch (col)                  // ширина кнопки зависит от кол-ва кнопок в строке
   {
    case 4: w = max_X/4-6; break;  // 72
    case 3: w = max_X/3-6; break;  // 100
    case 2: w = max_X/2-9; break;  // 150 
    default: w = max_X-6;
   };
  if(h<20) h=20;
  y = max_Y - h - 4;      // начало контура кнопки
  indx = 0;
  for (j=0; j<row; j++)
   {
    x = 4;// начало 1 кнопки
    #pragma warn-
    for (i=0; i<col; i++)
      {
        buttons[indx].x = x+i*(w+5);// интервал между кнопками
        buttons[indx].w = w;
        buttons[indx].h = h;
        buttons[indx].y = y;
        indx++;
      }
    y -= (h+5);// интервал между кнопками
    #pragma warn+ 
   }
  bottom_Y = y;// верхн€€ граница меню до которой можно закрашивать экран
  buttonCount = col * row;// обшее количество кнопок
}
//---------------- цвет фона ------- цвет рамки -------- цвет текста --- номер --- текст ---------
void drawButton(unsigned int fill, unsigned int bord, unsigned int text, char b, char flash *str)
{
   int x, y, w, h;//, r;
   w = buttons[b].w;      // ширина кнопки
   h = buttons[b].h;      // высота кнопки
   x = buttons[b].x;      // начало контура кнопки
   y = buttons[b].y;      // начало контура кнопки
//   r = min(w,h)/4;
   //TFT_FillRoundRect(x, y, w, h, r, fill);
   TFT_FillRectangle(x, y, w, h, fill);
   //TFT_DrawRoundRect(x, y, w, h, r, bord);
   TFT_DrawRectangle(x, y, w, h, bord);
#pragma warn-
   x = x + w/2 - strlenf(str)*5;
   y = buttons[b].y+h/2-8;
#pragma warn+  
   sprintf(buff,str); TFT_DrawString(buff, x, y, 1,1, text, fill);
}
/*
void drawButtonBord(unsigned int bord, char b)
{
   int x, y, w, h, r;
   w = buttons[b].w;      // ширина кнопки
   h = buttons[b].h;      // высота кнопки
   x = buttons[b].x;      // начало контура кнопки
   y = buttons[b].y;      // начало контура кнопки
   r = min(w,h)/4;
   TFT_DrawRoundRect(x, y, w, h, r, bord);
}
*/
//x:    переменна€, значение которой необходимо преобразовать
//in_min:  нижний предел текущего диапазона переменной value
//in_max: верхний предел текущего диапазона переменной value
//out_min:    нижний предел нового диапазона переменной value
//out_max:   верхний предел нового диапазона переменной value
unsigned int map(unsigned int x, char in_min, unsigned int in_max, char out_min, unsigned int out_max)
{
long val;
 val = (x - in_min); val *= (out_max - out_min); val /= (in_max - in_min); val += out_min;
 return val;
}

char contains(int touch_X, int touch_Y, char b)// проверка попадани€ пересчитаной координаты в область кнопки.
 {
 int beg, end;
   beg = buttons[b].x;
   end = beg + buttons[b].w-3;
   if ((touch_X < beg)||(touch_X > end)) return 0;
   beg = buttons[b].y;
   end = beg + buttons[b].h-3;
   if ((touch_Y < beg)||(touch_Y > end)) return 0;
   return 1;
 }

char checkTouch(void)// вычисление кординаты касани€ резистивной матрицы (point_Y, point_X)
{
char i;
int val, samples[2];
//------- Set X+ to ground --------------------
  XP_DIR = OUTPUT; // DDRC.6 ??????????????????
  XP_PORT = LOW;   // DDRC.6
//------- Set Y- to VCC -----------------------
  YM_DIR = OUTPUT; // DDRC.7 ??????????????????
  YM_PORT = HIGH;  // DDRC.7 ??????????????????
//------- Hi-Z X- and Y+ ----------------------
  YP_DIR = INPUT;  // TFT_WRX
  YP_PORT = LOW;   // TFT_WRX
  XM_DIR = INPUT;  // TFT_DCX
  XM_PORT = LOW;   // TFT_DCX
  val = read_adc(YP_PIN);  // y
  val-= read_adc(XM_PIN);  //(y-x)
  val = 1023-val;          // z
  point_Z = val; 

  if(val < 200){InitInterface(); return 0;}// если слабое нажатие то выход
  else                // иначе определ€ем координаты
   {
     YP_DIR = INPUT;   // TFT_WRX ?????????????
     YP_PORT = LOW;    // TFT_WRX ?????????????
     YM_DIR = INPUT;   // DDRC.7
     YM_PORT = LOW;    // DDRC.7
     XP_DIR = OUTPUT;  // DDRC.6  ?????????????
     XP_PORT = HIGH;   // DDRC.6
     XM_DIR = OUTPUT;  // TFT_DCX
     XM_PORT = LOW;    // TFT_DCX ?????????????
     do{
        for (i=0; i<2; i++){samples[i] = read_adc(YP_PIN);}
        val = abs(samples[0]-samples[1]);
       } while (val>4);
    // point_Y = (1023-(samples[0]+samples[1])/2);  // Orientation=0
     point_X = (samples[0]+samples[1])/2;           // Orientation=3
     
     XP_DIR = INPUT;  // DDRC.6
     XP_PORT = LOW;   // DDRC.6
     XM_DIR = INPUT;  // TFT_DCX
     XM_PORT = LOW;   // TFT_DCX ?????????????
     YP_DIR = OUTPUT; // TFT_WRX
     YP_PORT = HIGH;  // TFT_WRX
     YM_DIR = OUTPUT; // DDRC.7
     YM_PORT = LOW;   // DDRC.7  ?????????????
     do{
        for (i=0; i<2; i++){samples[i] = read_adc(XM_PIN);}
        val = abs(samples[0]-samples[1]);
       } while (val>4);
     point_Y = (1023-(samples[0]+samples[1])/2);     // Orientation=3
     //point_X = (1023-(samples[0]+samples[1])/2);   // Orientation=0
     InitInterface();
     return 1;
   }
}

char checkButton(char count)
{
char i;
 point_X = map(point_X, TS_MINX, TS_MAXX, 0, max_X);// пересчет координаты резистивной матрицы на пиксельную метрицу
 point_Y = map(point_Y, TS_MINY, TS_MAXY, 0, max_Y);// пересчет координаты резистивной матрицы на пиксельную метрицу
 //TFT_FillRectangle(point_X, point_Y, 5, 5, RED);
 for (i=0; i<count; i++){if(contains(point_X, point_Y, i)) break;}// проверка попадани€ новой координаты в область кнопки
 return i;
}