#define TUNING    170

void temperature_check(void)
{
 int val;
 unsigned char item, byte, crc, try=0;
  for (item=0; item < devices;)
   {
     w1_init();                 // 1 Wire Bus initialization
     w1_write(0x55);            // Load MATCH ROM [55H] comand
     ptr_char = &familycode[item][0];
     for (byte=0; byte < 8; byte++) w1_write(*ptr_char++);
     ptr_char = ds.buffer;
     w1_write(0xBE);            // Read Scratchpad command [BE]
     for (byte=0; byte < 8; byte++) *ptr_char++ = w1_read(); // Read cont. byte
     crc = w1_read();           // Read CRC byte
     ptr_char = ds.buffer;
     if (w1_dow_crc8(ptr_char, 8)==crc){
       try = 0; val = ds.pvT;
       if (val<0) {val=-val; val = val*10/16; val=-val;} // ������� � ���������� �������� !!
       else val = val*10/16; // ������� � ���������� �������� !!
       //----- ������������ ������� DS18B20 ----------
       if(ds.buffer[2]==TUNING) val +=(signed char)ds.buffer[3]; // �������� ��������� �������
     }
     else if (++try > 2) {val = 1990; try = 0;}// (199.0) ���� ������ ����� X ��� �� ������ �� ����������     
     t.point[item] = val; 
     if (try==0) item++;
   }
  w1_init();                    // 1 Wire Bus initialization
  w1_write(0xCC);               // Load Skip ROM [CCH] command
  w1_write(0x44);               // Load Convert T [44H] command
}

unsigned char tableRH(signed int maxT, signed int minT)
 {
  signed int dT;
   if (maxT>199 && maxT<410) // maxT> 19.9 � maxT< 41.0
    {
     dT = (maxT-minT)*16/10;
     if (dT<0) dT = 240;        // ������ ����� ��� ������� dT >>=3; ����������� -> dT>20
     maxT /=10;
     dT >>=3;
     if (dT>20) dT = 255;
     else if (dT==0) dT = 100;
     else {maxT -= 20; maxT *= 20; maxT += (dT-1); dT = tabRH[maxT];};
    }
   else dT = 255;
   return dT;
 }

unsigned char module_check(unsigned char fc)
{
 unsigned char *p, try, byte;
  p = out.buffer;
  out.buffer[3]=w1_dow_crc8(p,3);// ����������� �����
  for (try=0; try<3; try++)
   {
    w1_init();            // 1 Wire Bus initialization
    w1_write(fc);         // Family code
    p = out.buffer;
    for (byte=0; byte<4; byte++) w1_write(*p++);
    delay_us(250);      // ������� ���� ������ ���������� ����������
    p = in.buffer;
    for (byte=0; byte<4; byte++) *p++ = w1_read();// Read 4 byte
    byte = w1_read();  // Read CRC byte #5
    p = in.buffer;
    if(byte==w1_dow_crc8(p,4)) break;
    delay_ms(10);
   };
  if(try>2) byte=0; else byte=1;
  return byte;
}

#define ID_CO2          0xF5    // ������������� �����
#define DATAREAD        0xA1    // Read Scratchpad
unsigned char readCO2(void) // ������ ������ ��2
{
 unsigned char byte;
 static unsigned char try;
  out.buffer[0]=DATAREAD;       // Function Command
  out.buffer[1]=0x00;           // Data 1
  out.buffer[2]=displCO2;       // Data 2 1->���������� ��������; 2->���������� � ������; 3->��������� �����;
  byte = module_check(ID_CO2); // ������������� �����
  if(byte){                     // ���� ���� ������� ...
     pvCO2 = in.val[1];// CO2
     try = 0;
  }
  else if(++try>5){pvCO2=0; error|=0x08;}// ����� ������ CO2
  return byte;
}

//- Converts a 2 digit decimal to BCD format. ---------
char ByteToBcd2(char Value){
 char bcdhigh = 0;
  while (Value >= 10){
    bcdhigh++;
    Value -= 10;
  }
  return ((bcdhigh << 4U) | Value);
}

// - Converts from 2 digit BCD to Binary. -----------
char Bcd2ToByte(char Value)
{
  char tmp = 0;
  tmp = ((Value & 0xF0) >> 0x4) * 10;
  return (tmp + (Value & 0x0F));
}

signed int LowPassF2(signed int t,unsigned char i)
{
float val;
  val = A1*Told1[i]-A2*Told2[i]+A3*t;
  Told2[i] = Told1[i];
  Told1[i] = val;
  return val;
}
