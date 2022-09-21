void RelayControl(signed int val, unsigned char chanal){
 char x=UNCHANGED, byte, chRelay=chanal;
 signed int error;
 if(Dht==0 && chanal==1) chanal = 2;            // ����������
  error = set[chanal][0]-val;                   //set[chanal][0] - SP
  if (abs(error)>set[chanal][1]){               //set[chanal][1] - maxError
    alarm[chRelay] = 1;
  }
  else alarm[chRelay] = 0;
  if (set[chanal][2]){                          //set[chanal][2] - mode=0 -> ������ / mode=1 -> ����������;
   if (error > 0) x = ON;                       // ��������
   if (error + set[chanal][3] < 0) x = OFF;     //set[chanal][3] - hysteresis ���������
  }
  else {
   if (error - set[chanal][3] > 0) x = ON;      //set[chanal][3] - hysteresis ��������
   if (error < 0) x = OFF;                      // ���������
  }
  if (x<UNCHANGED) {
    byte = 1 << chRelay;
    if (x) {portOut |= byte; relOut[chRelay] = 1;}
    else {portOut &= ~byte;  relOut[chRelay] = 0;}
  }
}

void relayHadl(void){
 char i; 
  for (i=0;i<MAX_SET;i++) {
    if (relay[i]<2) relOut[i] = relay[i];
  };
}

void analogHadl(void){
 char i; 
  for (i=0;i<MAX_SET;i++) {
    if (analog[i]!=-1) analogOut[i] = analog[i];
    else analogOut[i] = analogPV[i];
  };
}

//-------------------------------- ���������� ��2. ----------------------------------------------------------------------------------------------
void controlCompressorCO2(unsigned char chanal){
 char x=UNCHANGED, byte, chRelay=chanal; 
  if(CO2module){          // ���� ��������� ������ ��2
    if(set[4][3]){       // ���� ������� ��2 ������ 0 �� �������� ����������
       if(timerCO2<set[3][0]){
         x = ON; displCO2=2;// �������� ����������
         if(timerCO2>1 && timerCO2<set[3][0]){CheckCO2=1; displCO2=3;}// ����������� ��������� ��2
         else {CheckCO2=0;}
       }
       else {x = OFF; displCO2=1; CheckCO2=0;}// ���������� ��������
    }
    else {x = OFF; displCO2=0; CheckCO2=0;}
  }
  else {x = OFF; displCO2=0; CheckCO2=0;}
  if (x<UNCHANGED) {
    byte = 1 << chRelay;
    if (x) {portOut |= byte; relOut[chRelay] = 1;}
    else {portOut &= ~byte;  relOut[chRelay] = 0;}
  }
};
