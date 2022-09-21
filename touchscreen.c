// Touch screen library with X Y and Z (pressure) readings as well as oversampling to avoid 'bouncing'
// Библиотека сенсорного экрана с показаниями X Y и Z (давление), а также избыточная выборка, чтобы избежать «подпрыгивания»

int point_X, point_Y, point_Z;

// increase or decrease the touchscreen oversampling. This is a little different than you make think:
// Увеличивать или уменьшать избыточную выборку сенсорного экрана. Это немного отличается от того, что вы думаете:
// 1 is no oversampling, whatever data we get is immediately returned
// 1 не является избыточной выборкой, все данные, которые мы получаем, немедленно возвращаются
// 2 is double-sampling and we only return valid data if both points are the same
// 2 является двойной выборкой, и мы возвращаем только действительные данные, если обе точки одинаковы
// 3+ uses insert sort to get the median value.
// 3+ использует сортировку вставки для получения медианного значения.
// We found 2 is precise yet not too slow so we suggest sticking with it!
// Мы нашли, что 2 точно, но не слишком медленно, поэтому мы рекомендуем придерживаться его!

#if (NUMSAMPLES > 2)
void insert_sort(int array[], unsigned char size)
{
unsigned char j; int save;
  for (int i = 1; i < size; i++)
   {
    save = array[i];
    for (j = i; j >= 1 && save < array[j - 1]; j--) {array[j] = array[j - 1];}// самое малое в начало
    array[j] = save; 
   }
}
#endif

// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=0x40;
// Wait for the AD conversion to complete
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCW;
}

void getPoint(void)
{
  unsigned char i, valid = 1;
  int x, y, z, z1, z2;
  int samples[NUMSAMPLES];
  float rtouch;

  YP_DIR = INPUT; //pinMode(_yp, INPUT);
  YM_DIR = INPUT; //pinMode(_ym, INPUT);
  YP_PORT = LOW;  //*portOutputRegister(yp_port) &= ~yp_pin; //digitalWrite(_yp, LOW);
  YM_PORT = LOW;  //*portOutputRegister(ym_port) &= ~ym_pin; //digitalWrite(_ym, LOW);
  
  XP_DIR = OUTPUT;//pinMode(_xp, OUTPUT);
  XM_DIR = OUTPUT;//pinMode(_xm, OUTPUT);
  XP_PORT = HIGH; //*portOutputRegister(xp_port) |= xp_pin;  //digitalWrite(_xp, HIGH);
  XM_PORT = LOW;  //*portOutputRegister(xm_port) &= ~xm_pin; //digitalWrite(_xm, LOW);

  for (i=0; i<NUMSAMPLES; i++){samples[i] = read_adc(YP_PIN);}//_yp
#if NUMSAMPLES > 2
   insert_sort(samples, NUMSAMPLES);
#endif
#if NUMSAMPLES == 2
   if(samples[0] != samples[1]) valid = 0;
#endif
   x = (1023-samples[NUMSAMPLES/2]);

   XP_DIR = INPUT;//pinMode(_xp, INPUT);
   XM_DIR = INPUT;//pinMode(_xm, INPUT);
   XP_PORT = LOW; //*portOutputRegister(xp_port) &= ~xp_pin;  //digitalWrite(_xp, LOW);
   
   YP_DIR = OUTPUT; //pinMode(_yp, OUTPUT);
   YM_DIR = OUTPUT; //pinMode(_ym, OUTPUT);
   YP_PORT = HIGH;  //*portOutputRegister(yp_port) |= yp_pin; //digitalWrite(_yp, HIGH);
  
   for (i=0; i<NUMSAMPLES; i++){samples[i] = read_adc(XM_PIN);}//_xm
#if NUMSAMPLES > 2
   insert_sort(samples, NUMSAMPLES);
#endif
#if NUMSAMPLES == 2
   if (samples[0] != samples[1]) valid = 0;
#endif
   y = (1023-samples[NUMSAMPLES/2]);

//------- Set X+ to ground --------------------
   XP_DIR = OUTPUT;//pinMode(_xp, OUTPUT);
   XP_PORT = LOW;  //*portOutputRegister(xp_port) &= ~xp_pin; //digitalWrite(_xp, LOW);
  
//------- Set Y- to VCC -----------------------
   YM_PORT = HIGH;  //*portOutputRegister(ym_port) |= ym_pin; //digitalWrite(_ym, HIGH); 
  
//------- Hi-Z X- and Y+ ----------------------
   YP_PORT = LOW;  //*portOutputRegister(yp_port) &= ~yp_pin; //digitalWrite(_yp, LOW);
   YP_DIR = INPUT; //pinMode(_yp, INPUT);
  
   z1 = read_adc(XM_PIN);//_xm 
   z2 = read_adc(YP_PIN);//_yp
   
   if(_rxplate != 0){rtouch = z2; rtouch /= z1; rtouch -= 1; rtouch *= x; rtouch *= _rxplate; rtouch /= 1024; z = rtouch;}
   else {z = (1023-(z2-z1));}
   if(!valid) z = 0;

   point_X = x; point_Y = y; point_Z = z; // return TSPoint(x, y, z); //return TSPoint(x, 1023 - y, z);
}

void waitOneTouch(void)
{
  do {
    getPoint();
    XM_DIR = OUTPUT;//pinMode(XM, OUTPUT); //Pins configures again for TFT control
    YP_DIR = OUTPUT;//pinMode(YP, OUTPUT);
  } while((point_Z < MINPRESSURE )|| (point_Z > MAXPRESSURE));
}

//++++++++++++++++++ Show the coordinates +++++++++++++++++++++++++++++++++++++++++++++++

void showTouched(void)
{
  char w = 50; // Width
  char h = 10; // Heigth
  char x = (TFTWIDTH - (w*2)); // X
  char y = 11; // Y
  TFT_FillRectangle(x, y, w*2, h, WHITE);  // tft.fillRect(x, y, w*2, h, WHITE);   // For cleanup
  TFT_DrawRectangle(x, y, w, h, RED);      // tft.drawRect(x, y, w, h, RED);       // For X
  TFT_DrawRectangle(x+w+2, y, w*2, h, RED);// tft.drawRect(x+w+2, y, w*2, h, RED); // For Y
  sprintf(buff,"X=%03u",point_X); TFT_DrawString(buff,x+2, y+1, 1, BLACK);
  sprintf(buff,"Y=%03u",point_Y); TFT_DrawString(buff,x+2+w+2, y+1, 1, BLACK);
}

/*
TouchScreen::TouchScreen(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym) {
  _yp = yp;
  _xm = xm;
  _ym = ym;
  _xp = xp;
  _rxplate = 0;
  pressureThreshhold = 10;
}


TouchScreen::TouchScreen(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym, uint16_t rxplate) {
  _yp = yp;
  _xm = xm;
  _ym = ym;
  _xp = xp;
  _rxplate = rxplate;

  pressureThreshhold = 10;
}

int readTouchX(void)
{
   YP_DIR = INPUT;  //pinMode(_yp, INPUT);
   YM_DIR = INPUT;  //pinMode(_ym, INPUT);
   YP_PORT = LOW;   //digitalWrite(_yp, LOW);
   YM_PORT = LOW;   //digitalWrite(_ym, LOW);
   
   XP_DIR = OUTPUT; //pinMode(_xp, OUTPUT);
   XM_DIR = OUTPUT; //pinMode(_xm, OUTPUT);
   XP_PORT = HIGH;  //digitalWrite(_xp, HIGH);
   XM_PORT = LOW;   //digitalWrite(_xm, LOW);
   
   return (1023-analogRead(YP_PIN));//_yp
}


int readTouchY(void)
{
   XP_DIR = INPUT;  //pinMode(_xp, INPUT);
   XM_DIR = INPUT;  //pinMode(_xm, INPUT);
   XP_PORT = LOW;   //digitalWrite(_xp, LOW);
   XM_PORT = LOW;   //digitalWrite(_xm, LOW);
   
   YP_DIR = OUTPUT; //pinMode(_yp, OUTPUT);
   YM_DIR = OUTPUT; //pinMode(_ym, OUTPUT);
   YP_PORT = HIGH;  //digitalWrite(_yp, HIGH);
   YM_PORT = LOW;   //digitalWrite(_ym, LOW);
   
   return (1023-analogRead(XM_PIN));//_xm
}


unsigned int pressure(void)
{
int z1,z2;
float rtouch;
  // Set X+ to ground
  XP_DIR = OUTPUT; //pinMode(_xp, OUTPUT);
  XP_PORT = LOW;   //digitalWrite(_xp, LOW);
  
  // Set Y- to VCC
  YM_DIR = OUTPUT; //pinMode(_ym, OUTPUT);
  YM_PORT = HIGH;  //digitalWrite(_ym, HIGH); 
  
  // Hi-Z X- and Y+
  YM_PORT = LOW;   //digitalWrite(_xm, LOW);
  XM_DIR = INPUT;  //pinMode(_xm, INPUT);
  YP_PORT = LOW;   //digitalWrite(_yp, LOW);
  YP_DIR = INPUT;  //pinMode(_yp, INPUT);
  
  z1 = analogRead(XM_PIN);//_xm 
  z2 = analogRead(YP_PIN);//_yp

  if(_rxplate != 0){rtouch = z2; rtouch /= z1; rtouch -= 1; rtouch *= readTouchX(); rtouch *= _rxplate; rtouch /= 1024; return rtouch;}
  else {return (1023-(z2-z1));}
}
*/