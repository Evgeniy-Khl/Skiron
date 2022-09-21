unsigned char buttonCount;
unsigned int bottom_Y;
struct ram_structure {int x,y; char w,h;} buttons[12];// Global structure located in RAM
flash char *buttonmenu0[4]={"ОТЧЕТ"," ","ПЕЧТ","УСТВ"};
flash char *buttonmenu1[4]={"ВОЗВР","СЛЕД","ПРЕД","ПЕЧТ"};
flash char *buttonmenu3[4]={"ВОЗВР","ПЕЧТ","+","-"};
flash char *buttonmenu4[4]={"ЗАПС.","СЛЕД","+","-"};
void initializeButtons(char col, char row, char h)// высота кнопки
{
  char i,j,indx, w;
  unsigned int x,y;
  switch (col)                  // ширина кнопки зависит от кол-ва кнопок в строке
   {
    case 4: w = 54; break;
    case 3: w = 74; break;
    case 2: w = 114; break;   
    default: w = TFTWIDTH-6;
   };
  if(h<20) h=20;
  y = TFTHEIGHT - h - 4;      // начало контура кнопки
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
  bottom_Y = y;// верхняя граница меню до которой можно закрашивать экран
  buttonCount = col * row;// обшее количество кнопок
}

void drawButton(unsigned int fill, unsigned int bord, unsigned int text, char b, char flash *str)
{
   int x, y, w, h, r;
   w = buttons[b].w;      // ширина кнопки
   h = buttons[b].h;      // высота кнопки
   x = buttons[b].x;      // начало контура кнопки
   y = buttons[b].y;      // начало контура кнопки
   r = min(w,h)/4;
   TFT_FillRoundRect(x, y, w, h, r, fill);
   TFT_DrawRoundRect(x, y, w, h, r, bord);
#pragma warn-
   x = x + w/2 - strlenf(str)*5;
   y = buttons[b].y+h/2-8;
#pragma warn+  
   sprintf(buff,str); TFT_DrawString(buff, x, y, 1,0, text, fill);
}

char check_key(void)
{
 char key;
  key =~(PIND>>4); key &= 0x0F;
  if(key==0)
   {
    if(PINB.0==0) key=0x10;
    else if(PINB.1==0) key=0x20;
   }
  return key;
}