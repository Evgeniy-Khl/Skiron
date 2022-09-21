void fraction(signed int t)
{
  if (t<0) {t = -t; signchar = '-';} else signchar = ' ';
  frcval = t%10; intval = t/10;
}

char buttonCheck(void){
 char res=0;
   if(checkTouch()){
     res = (checkButton(buttonCount));
     if (res < buttonCount) {newButton = res; res=1;} // была нажата кнопка
     else {newButton = 100; res=0;}                   // нажатие не обнаружено
   }
   return res;
}

signed char plusCheck(char *str, char *Ystr){
 signed char bt,res=0;
   if(checkTouch()){
       bt = (checkPlus(plusCount));
//       sprintf(buff,"but=%2u; amt=%u", bt, plusCount);
//       TFT_DrawString(buff,150,TFTBUTTON-25,1,1,BLACK,GREEN1);
       if (bt < plusCount){
            *str = bt/2;    // присваиваем номер строки в которой была нажата кнопка
            if(plusCount<11) *Ystr = 37+*str*36; else *Ystr = 27+*str*20; // координаты строки для широких или узких кнопок
            if(bt%2) res=-1; else res=1;   // кнопка "+" или кнопка "-"
       }
   }
//   sprintf(buff,"res=%2i",res);
//   TFT_DrawString(buff,5,TFTBUTTON-25,1,1,BLACK,GREEN1);
   return res;
}

void drawButtSkip(unsigned int fW){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fW);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(BLUE, WHITE, WHITE, 0, "Выход");
    drawButton(GREEN, BLACK, BLACK, 1, "<-");
    drawButton(GREEN, BLACK, BLACK, 2, "->");
    drawButton(MAGENTA, BLACK, BLACK, 3, "Кор.");
}

void drawButtEdit(unsigned int fW){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fW);
    initializeButtons(2,1,25);// 2 колонки; одна строка; высота 25
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(BLUE, WHITE, WHITE, 0, "Отмена");
    drawButton(MAGENTA, BLACK, BLACK, 1, "Сохран.");
}

void displ_0(void){
 unsigned char i, /*x, inttemp, frctemp*/;
 unsigned int fillWindow = BLUE1, bordWindow = BLACK, temp, X_left;
//-------------------------------- Основные показатели микроклимата. ----------------------------------------------------------------------------
  if (newSetButt){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fillWindow);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(fillWindow, fillWindow, bordWindow, 0, "Общие");
    drawButton(WHITE, WHITE, BLACK, 1, "Аналог");
    drawButton(WHITE, WHITE, BLACK, 2, "Реле");
    drawButton(WHITE, WHITE, BLACK, 3, "Уст.");
  }
 //-- Индикация часов -----------------------------------------------------
  X_left=5; pointY=5;
  if(Clock_Ok){
    read_TWI(DS3231,0,clock_buffer,7);// чтение данных часовой микросхемы
    sprintf(buff,"%02x:%02x   %02x.%02x.20%02x",clock_buffer[2],clock_buffer[1],clock_buffer[4],clock_buffer[5],clock_buffer[6]);//час:мин дата.мес.год
    TFT_DrawString(buff,70,pointY,1,1,BLACK,WHITE); TFT_DrawString(buff,70,pointY,1,1,WHITE,BLACK); // "БЕГУЩАЯ СТРОКА"
  }
  else {sprintf(buff,"Ошибка часов!"); TFT_DrawString(buff,95,pointY,1,1,YELLOW,RED); TFT_DrawString(buff,108,pointY,1,1,RED,YELLOW);}// "БЕГУЩАЯ СТРОКА"
  pointY = pointY+25;
  if (buttonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
  //--- Индикация t ВОЗДУХА ---
  if(Dht){
      temp = set[0][0]; fraction(temp);     // проверка знака температуры
      sprintf(buff,"[%2u.%u]",intval,frcval); // ЗАДАНИЕ T показываем с десятичным знаком
      TFT_DrawString(buff,5,pointY-4,1,1,bordWindow,fillWindow);
      TFT_DrawString("возду0",5,pointY+12,1,1,bordWindow,fillWindow);
      if (pvT>1250) sprintf(buff,"t = **.*");
      else {
          temp = pvT; fraction(temp);     // проверка знака температуры
          sprintf(buff,"t = %2u.%u",intval,frcval); // T датчиков показываем с десятичным знаком 
      }
      TFT_DrawString(buff,80,pointY,2,0,bordWindow,fillWindow);
      // индикация тревоги alarm[0]
      switch (alarm[0]) {
        case 0: temp=GREEN; break;
        case 1: temp=RED;   break;
        default: temp=fillWindow;
      }; 
      TFT_FillRectangle(240,pointY,45,30,temp);
      pointY = pointY+40;
      if (buttonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
      //--- Индикация RH ---
      sprintf(buff,"[%3u%%]",set[1][0]); // ЗАДАНИЕ RH
      TFT_DrawString(buff,5,pointY-4,1,1,bordWindow,fillWindow);
      TFT_DrawString("возду0",5,pointY+12,1,1,bordWindow,fillWindow);
      if (pvRH>100) sprintf(buff,"Bл= ***%%",pvRH);
      else sprintf(buff,"Bл= %2u%% ",pvRH);
      TFT_DrawString(buff,80,pointY,2,0,bordWindow,fillWindow); 
      // индикация тревоги alarm[1]
      switch (alarm[1]) {
        case 0: temp=GREEN; break;
        case 1: temp=RED;   break;
        default: temp=fillWindow;
      };
      TFT_FillRectangle(240,pointY,45,30,temp);
      pointY = pointY+35;
      if (buttonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
      //--- Индикация t ГРУНТА ---
      for (i=0;i<devices;i++){
        sprintf(buff,"зона %u",i+1);
        TFT_DrawString(buff,5,pointY+12,1,1,bordWindow,fillWindow);
        temp = t.point[i];
        if (temp>1250) sprintf(buff,"t = **.*");
        else {
            fraction(temp);     // проверка знака температуры
            sprintf(buff,"t = %2u.%u",intval,frcval); // T датчиков показываем с десятичным знаком 
        }
        TFT_DrawString(buff,80,pointY,2,0,bordWindow,fillWindow);
        // нет индикации тревоги
        pointY = pointY+33;
      };
      if (buttonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
  }
  else {
      //--- Индикация t только DS18B20 ---
      for (i=0;i<devices;i++){
        if (i==0) {
            temp = set[0][0]; fraction(temp);     // проверка знака температуры
            sprintf(buff,"[%2u.%u]",intval,frcval); // ЗАДАНИЕ T показываем с десятичным знаком
            TFT_DrawString(buff,5,pointY-4,1,1,bordWindow,fillWindow);
            TFT_DrawString("воздух",5,pointY+12,1,1,bordWindow,fillWindow);
            temp = t.point[0];
            if (temp>1250) sprintf(buff,"t = **.*");
            else {
                fraction(temp);     // проверка знака температуры
                sprintf(buff,"t = %2u.%u",intval,frcval); // T датчиков показываем с десятичным знаком 
            }
            TFT_DrawString(buff,80,pointY,2,0,bordWindow,fillWindow);
            // индикация тревоги alarm[0]
            switch (alarm[0]) {
              case 0: temp=GREEN; break;
              case 1: temp=RED;   break;
              default: temp=fillWindow;
            };
            TFT_FillRectangle(240,pointY,45,30,temp);
            pointY = pointY+40;
            //--- Индикация RH ---
            TFT_DrawString("воздух",5,pointY+6,1,1,bordWindow,fillWindow);
            if (pvRH>100) sprintf(buff,"Bл= ***%%",pvRH);
            else sprintf(buff,"Bл= %2u%% ",pvRH); 
            TFT_DrawString(buff,80,pointY,2,0,bordWindow,fillWindow);
            // нет индикации тревоги
            pointY = pointY+35;
            if (buttonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
        }
        else {
            temp = set[2][0]; fraction(temp);     // проверка знака температуры
            sprintf(buff,"[%2u.%u]",intval,frcval); // ЗАДАНИЕ T показываем с десятичным знаком
            TFT_DrawString(buff,5,pointY-4,1,1,bordWindow,fillWindow);
            sprintf(buff,"зона %u",i); 
            TFT_DrawString(buff,5,pointY+12,1,1,bordWindow,fillWindow);
            temp = t.point[i];
            if (temp>1250) sprintf(buff,"t = **.*");
            else {
                fraction(temp);     // проверка знака температуры
                sprintf(buff,"t = %2u.%u",intval,frcval); // T датчиков показываем с десятичным знаком 
            }
            TFT_DrawString(buff,80,pointY,2,0,bordWindow,fillWindow);
            // индикация тревоги alarm[1] и далее
            switch (alarm[i]) {
              case 0: temp=GREEN; break;
              case 1: temp=RED;   break;
              default: temp=fillWindow;
            };
            TFT_FillRectangle(240,pointY,45,30,temp);
            pointY = pointY+33;
            if (buttonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
        }
     }     
  }
  //--- Индикация CO2 ---
  if (CO2module){
    if (error&0x08){
      TFT_DrawString("CO2 ошибка модуля!",40,pointY,2,0,bordWindow,fillWindow);
    }
    else {
      sprintf(buff,"CO2 = %4u",pvCO2);
      TFT_DrawString(buff,40,pointY,2,0,bordWindow,fillWindow);
      TFT_DrawString("ppm",240,pointY+12,1,1,bordWindow,fillWindow);
    }
  }
}

//- АНАЛОГОВЫЕ ВЫХОДЫ -
void displ_1(void){
 char i, num=1;
 unsigned int fillWindow = YELLOW2, bordWindow = BLACK; 
  pointY=7;
  if (newSetButt){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fillWindow);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    TFT_DrawRectangle(2, 2, max_X-5, TFTBUTTON+6, bordWindow);
    TFT_FillRectangle(buttons[num].x-3, TFTBUTTON, buttons[num].w+3, buttons[num].h, fillWindow);
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(WHITE, WHITE, BLACK, 0, "Общие");
    drawButton(fillWindow, fillWindow, bordWindow, 1, "Аналог");
    drawButton(WHITE, WHITE, BLACK, 2, "Реле");
    drawButton(WHITE, WHITE, BLACK, 3, "Уст.");
    TFT_DrawVerticalLine(buttons[num].x-2, TFTBUTTON+8, buttons[num].h+5, bordWindow);
    TFT_DrawVerticalLine(buttons[num].x+buttons[num].w, TFTBUTTON+8, buttons[num].h+5, bordWindow);
    TFT_DrawHorizontalLine(buttons[num].x-1, max_Y-2, buttons[num].w, bordWindow);
    TFT_DrawString("АНАЛОГОВЫЕ ВЫХОДЫ",50,pointY,1,1,bordWindow,fillWindow);
  }
  //+++++++++++++++++
  pointY = pointY+30;
  for (i=0;i<4;i++){
    sprintf(buff,"ВЫХ.N%u:%3u%% ",i+1, analogOut[i]);
    if (analog[i]==-1) strcat(buff,"АВТ");
    else strcat(buff,"РУЧ");
    TFT_DrawString(buff,5,pointY,2,0,bordWindow,fillWindow);
    pointY = pointY+35;
    if (buttonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
  };
}

//- РЕЛЕЙНЫЕ ВЫХОДЫ -
void displ_2(void){
 char i, num=2;
 unsigned int fillWindow = GRAY1, bordWindow = BLACK, color_box; 
  pointY=7;
  if (newSetButt){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fillWindow);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    TFT_DrawRectangle(2, 2, max_X-5, TFTBUTTON+6, bordWindow);
    TFT_FillRectangle(buttons[num].x-3, TFTBUTTON, buttons[num].w+3, buttons[num].h, fillWindow);
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(WHITE, WHITE, BLACK, 0, "Общие");
    drawButton(WHITE, WHITE, BLACK, 1, "Аналог");
    drawButton(fillWindow, fillWindow, bordWindow, 2, "Реле");
    drawButton(WHITE, WHITE, BLACK, 3, "Уст.");
    TFT_DrawVerticalLine(buttons[num].x-2, TFTBUTTON+8, buttons[num].h+5, bordWindow);
    TFT_DrawVerticalLine(buttons[num].x+buttons[num].w, TFTBUTTON+8, buttons[num].h+5, bordWindow);
    TFT_DrawHorizontalLine(buttons[num].x-1, max_Y-2, buttons[num].w, bordWindow);
    TFT_DrawString("РЕЛЕЙНЫЕ ВЫХОДЫ",50,pointY,1,1,bordWindow,fillWindow); 
  }
  //+++++++++++++++++
  pointY = pointY+30;
  for (i=0;i<4;i++){
    sprintf(buff,"РЕЛЕ N%u: ",i+1);
    switch (relay[i]){
      case 2: strcat(buff,"АВТ"); break;
      case 1: strcat(buff,"ON "); break;
      case 0: strcat(buff,"OFF"); break;
    };
    TFT_DrawString(buff,5,pointY,2,0,bordWindow,fillWindow);
    if (relOut[i]) color_box=YELLOW;
    else color_box=BLACK;
    TFT_FillRectangle(250,pointY,45,32,color_box);
    pointY = pointY+35;
    if (buttonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
  };
}

//- Установки Общий список -
void displ_3(void){
 char item;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, color_txt = BLACK, color_fon = GREEN1; 
  pointY=7;
  if (newSetButt){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fillWindow);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    TFT_DrawRectangle(2, 2, max_X-5, TFTBUTTON+6, bordWindow);
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(BLUE, WHITE, WHITE, 0, "Выход");
    drawButton(GREEN, BLACK, BLACK, 1, "<-");
    drawButton(GREEN, BLACK, BLACK, 2, "->");
    drawButton(MAGENTA, BLACK, BLACK, 3, "Кор.");
    TFT_DrawString("УСТАНОВКИ",100,pointY,1,1,bordWindow,fillWindow); //---------------------------------------- УСТАНОВКИ -
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  pointY = pointY+25;
  for (item = 0; item < MAX_MENU; item++){
    sprintf(buff,"%s", setMenu[item]);   //  %10s
    if (item == numMenu){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
    TFT_DrawString(buff,20,pointY,2,0,color_txt,color_fon);
    pointY = pointY+33;
  }
}

//- Установки пунктов -
void displ_4(void){
 char item, tmpv0, tmpv1;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, color_txt = BLACK, color_fon = GREEN1, temp;
  pointY=7;
  if (newSetButt){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fillWindow);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    TFT_DrawRectangle(2, 2, max_X-5, TFTBUTTON+6, bordWindow);
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(BLUE, WHITE, WHITE, 0, "Выход");
    drawButton(GREEN, BLACK, BLACK, 1, "<-");
    drawButton(GREEN, BLACK, BLACK, 2, "->");
    drawButton(MAGENTA, BLACK, BLACK, 3, "Кор.");
    sprintf(buff,"%s", setMenu[numMenu]);
    TFT_DrawString(buff,80,pointY,1,1,bordWindow,fillWindow); 
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  pointY = pointY+30;
  switch (numMenu){
    case 0:
        for (item = 0; item < MAX_SET;item++){
          if (item<3){
            temp = set[0][item]; tmpv0 = temp%10; tmpv1 = temp/10;
            sprintf(buff,"%7s= %2u.%u", setName0[item],tmpv1,tmpv0); // T с десятичным знаком
          }
          else {
            sprintf(buff,"%7s= ", setName0[item]);
            if(set[0][item]==0) strcat(buff,"НАГР.");
            else  strcat(buff,"ОХЛ."); 
          }
          if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
          TFT_DrawString(buff,5,pointY,2,0,color_txt,color_fon);
          pointY = pointY+35;
        }
    break;
    case 1:
        for (item = 0; item < MAX_SET;item++){
          if(item<3){
            if (Dht) sprintf(buff,"%7s= %i", setName0[item],set[1][item]);
            else {
              temp = set[2][item]; tmpv0 = temp%10; tmpv1 = temp/10;
              sprintf(buff,"%7s= %2u.%u", setName0[item],tmpv1,tmpv0); // T с десятичным знаком
            } 
          }
          else {
            sprintf(buff,"%7s= ", setName0[item]);
            if(set[1][item]==0) strcat(buff,"УВЛ.");
            else  strcat(buff,"ОСУ."); 
          }
          if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
          TFT_DrawString(buff,5,pointY,2,0,color_txt,color_fon);
          pointY = pointY+35;
        }
    break;
    case 2:
        for (item = 0; item < MAX_SET;item++){
          if (item==2){
            sprintf(buff,"%8s= ", setName1[item]);
            if (set[3][item]==0) strcat(buff,"СЕК.");
            else strcat(buff,"МИН.");
          }
          else sprintf(buff,"%8s= %i", setName1[item],set[3][item]);
          if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
          TFT_DrawString(buff,5,pointY,2,0,color_txt,color_fon);
          pointY = pointY+35;
        }
    break;
    case 3:
        for (item = 0; item < MAX_SET;item++){
          sprintf(buff,"%11s= %i", setName2[item],set[4][item]);
          if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
          TFT_DrawString(buff,5,pointY,2,0,color_txt,color_fon);
          pointY = pointY+35;
        }
    break;
    case 4:
        for (item = 0; item < MAX_DATE;item++){
          if (item<2) tmpv0 = clock_buffer[item+1];
          else tmpv0 = clock_buffer[item+2]; 
          sprintf(buff,"%8s= %x", setName3[item],tmpv0);
          if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
          TFT_DrawString(buff,50,pointY,1,1,color_txt,color_fon);
          pointY = pointY+15;
        }
    break;
    };   
}

//- РЕДАКТИРОВАНИЕ -
void displ_5(void){
 char tmpv0, tmpv1;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, temp;
  pointY=7;
  if (newSetButt){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fillWindow);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    TFT_DrawRectangle(2, 2, max_X-5, TFTBUTTON+6, bordWindow);
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(BLUE, WHITE, WHITE, 0, "Отм.");
    drawButton(GREEN, BLACK, BLACK, 1, "+");
    drawButton(GREEN, BLACK, BLACK, 2, "-");
    drawButton(MAGENTA, BLACK, BLACK, 3, "Зап.");
    TFT_DrawString("РЕДАКТИРОВАНИЕ",90,pointY,1,1,bordWindow,fillWindow); 
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  pointY = pointY+50;
   switch (numMenu){
    case 0:
      if (numSet<3){
        temp = newval[numSet]; tmpv0 = temp%10; tmpv1 = temp/10;
        sprintf(buff,"%5s= %2u.%u", setName0[numSet],tmpv1,tmpv0); // T с десятичным знаком
      }
      else {
        sprintf(buff,"%7s= ", setName0[numSet]);
        if(newval[numSet]==0) strcat(buff,"НАГР.");
        else strcat(buff,"ОХЛ."); 
      }
      TFT_DrawString(buff,5,pointY,2,0,bordWindow,fillWindow);
    break;
    case 1:
      if(numSet<3){
        if (Dht) sprintf(buff,"%7s= %i ", setName0[numSet],newval[numSet]);
        else {
          temp = newval[numSet]; tmpv0 = temp%10; tmpv1 = temp/10;
          sprintf(buff,"%5s= %2u.%u ", setName0[numSet],tmpv1,tmpv0); // T с десятичным знаком
        } 
      }
      else {
        sprintf(buff,"%7s= ", setName0[numSet]);
        if(newval[numSet]==0) strcat(buff,"УВЛ.");
        else  strcat(buff,"ОСУ."); 
      }
      TFT_DrawString(buff,5,pointY,2,0,bordWindow,fillWindow);
    break;
    case 2:
      sprintf(buff,"%5s: %i ", setName1[numSet], newval[numSet]);
      TFT_DrawString(buff,5,pointY,2,0,bordWindow,fillWindow);
    break;
    case 3:
      sprintf(buff,"%5s: %i ", setName2[numSet], newval[numSet]);
      TFT_DrawString(buff,5,pointY,2,0,bordWindow,fillWindow);
    break;
    case 4:
      sprintf(buff,"%7s: %u ", setName3[numSet], newval[numSet]);
      TFT_DrawString(buff,5,pointY,2,0,bordWindow,fillWindow);
    break;
    }
    
}

//- УСТАНОВКИ АНАЛОГОВЫЕ ВЫХОДЫ -
void displ_6(void){
 char i, num=1;
 unsigned int fillWindow = YELLOW2, bordWindow = BLACK, color_txt, color_fon;
  pointY=7;
  if (newSetButt){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fillWindow);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    TFT_DrawRectangle(2, 2, max_X-5, TFTBUTTON+6, bordWindow);
    TFT_FillRectangle(buttons[num].x-3, TFTBUTTON, buttons[num].w+3, buttons[num].h, fillWindow);
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(BLUE, WHITE, WHITE, 0, "Выход");
    drawButton(GREEN, BLACK, BLACK, 1, "<-");
    drawButton(GREEN, BLACK, BLACK, 2, "->");
    drawButton(MAGENTA, BLACK, BLACK, 3, "Кор.");
    TFT_DrawString("УСТАНОВКИ АНАЛОГОВЫЕ ВЫХОДЫ",5,pointY,1,1,bordWindow,fillWindow);
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  pointY = pointY+30;
  for (i=0;i<4;i++){
    if (analog[i]==-1) sprintf(buff,"ВЫХ.N%u: АВТ    ",i+1);
    else sprintf(buff,"ВЫХ.N%u:%3u%% РУЧ",i+1, analog[i]);
    if (i == numSet){color_txt = WHITE; color_fon = bordWindow;} else {color_txt = bordWindow; color_fon = fillWindow;}
    TFT_DrawString(buff,5,pointY,2,0, color_txt, color_fon);
    pointY = pointY+35;
  };
}

//- УСТАНОВКИ РЕДЛЕЙНЫЕ ВЫХОДЫ -
void displ_7(void){
 char i, num=1;
 unsigned int fillWindow = GRAY1, bordWindow = BLACK, color_txt, color_fon;
  pointY=7;
  if (newSetButt){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fillWindow);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    TFT_DrawRectangle(2, 2, max_X-5, TFTBUTTON+6, bordWindow);
    TFT_FillRectangle(buttons[num].x-3, TFTBUTTON, buttons[num].w+3, buttons[num].h, fillWindow);
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(BLUE, WHITE, WHITE, 0, "Выход");
    drawButton(GREEN, BLACK, BLACK, 1, "<-");
    drawButton(GREEN, BLACK, BLACK, 2, "->");
    drawButton(MAGENTA, BLACK, BLACK, 3, "Кор.");
    TFT_DrawString("УСТАНОВКИ РЕЛЕЙНЫЕ ВЫХОДЫ",5,pointY,1,1,bordWindow,fillWindow);
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  pointY = pointY+30;
  for (i=0;i<4;i++){
    sprintf(buff,"РЕЛЕ N%u: ",i+1);
    switch (relay[i]){
      case 2: strcat(buff,"АВТ"); break;
      case 1: strcat(buff,"ON "); break;
      case 0: strcat(buff,"OFF"); break;
    };
    if (i == numSet){color_txt = WHITE; color_fon = bordWindow;} else {color_txt = bordWindow; color_fon = fillWindow;}
    TFT_DrawString(buff,5,pointY,2,0, color_txt, color_fon);
    pointY = pointY+35;
  };
}

//- РЕДАКТИР. АНАЛОГОВЫЕ ВЫХОДЫ -
void displ_8(void){
 unsigned int fillWindow = BLUE1, bordWindow = BLACK; 
  pointY=7;
  if (newSetButt){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fillWindow);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    TFT_DrawRectangle(2, 2, max_X-5, TFTBUTTON+6, bordWindow);
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(BLUE, WHITE, WHITE, 0, "Отм.");
    drawButton(GREEN, BLACK, BLACK, 1, "+");
    drawButton(GREEN, BLACK, BLACK, 2, "-");
    drawButton(MAGENTA, BLACK, BLACK, 3, "Зап.");
    TFT_DrawString("РЕДАКТИР. АНАЛОГОВЫЕ ВЫХОДЫ",15,pointY,1,1,bordWindow,fillWindow); 
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  pointY = pointY+50;  
  if (newval[numSet]==-1) sprintf(buff,"ВЫХ.N%u: АВТ     ",numSet+1);
  else sprintf(buff,"ВЫХ.N%u:%3u%% РУЧ",numSet+1, newval[numSet]);
  TFT_DrawString(buff,5,pointY,2,0,bordWindow,fillWindow);
}

//- РЕДАКТИР. РЕЛЕЙНЫЕ ВЫХОДЫ -
void displ_9(void){
 unsigned int fillWindow = GRAY1, bordWindow = BLACK; 
  pointY=7;
  if (newSetButt){
    newSetButt = 0;
    TFT_FillScreen(0, max_X, 0, max_Y, fillWindow);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    TFT_DrawRectangle(2, 2, max_X-5, TFTBUTTON+6, bordWindow);
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(BLUE, WHITE, WHITE, 0, "Отм.");
    drawButton(GREEN, BLACK, BLACK, 1, "+");
    drawButton(GREEN, BLACK, BLACK, 2, "-");
    drawButton(MAGENTA, BLACK, BLACK, 3, "Зап.");
    TFT_DrawString("РЕДАКТИР. РЕЛЕЙНЫЕ ВЫХОДЫ",15,pointY,1,1,bordWindow,fillWindow); 
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  pointY = pointY+50;
  sprintf(buff,"РЕЛЕ N%u: ",numSet+1);  
  switch (newval[numSet]){
    case 2: strcat(buff,"АВТ"); break;
    case 1: strcat(buff,"ON "); break;
    case 0: strcat(buff,"OFF"); break;
  };
  TFT_DrawString(buff,5,pointY,2,0,bordWindow,fillWindow);
}

void display(void){
 switch (displ_num){
  	case 0: displ_0(); break;
  	case 1: displ_1(); break;
    case 2: displ_2(); break;
    case 3: displ_3(); break;
    case 4: displ_4(); break;
    case 5: displ_5(); break;
    case 6: displ_6(); break;
    case 7: displ_7(); break;
    case 8: displ_8(); break;
    case 9: displ_9(); break;
//  	default: displ_0();	break;
  }
}

//char buttonCheck(void){
// char res=0;
//   if(checkTouch()){
//     res = (checkButton(buttonCount));
//     if (res < buttonCount) {newButton = res; res=1;}
//     else {newButton = 100; res=0;}
//   }
//   return res;
//}