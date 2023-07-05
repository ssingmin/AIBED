/*todolist
1. 침대 다중제어
2. 220v 제어//조명, 펌프전원 , 모니터 , 산소발생기
3. 485통신해결
오프모드//공기가 없는상태
온모드 //모두 빵빵한 상태
교대부양모드// 135 24가 교차
알람모드// 파도타기 
////////*/
#include <MsTimer2.h>

#include <Adafruit_ADS1X15.h>

#include <math.h>
#include <DHT.h>
#include <Wire.h>

//#define SHIFTTIME 300// 5 MINUTES

#define RELAY_STARTPIN 37
#define RELAY_NUM 17
// the setup function runs once when you press reset or power the board

#define THERCONST_A 0.9766319919e-03
#define THERCONST_B 2.502638882e-04
#define THERCONST_C 0.9262423434e-07

#define DHTPIN 2        // 데이터 입력 핀의 설정
#define DHTTYPE DHT22   // DHT22 (AM2302) 센서종류 설정

#define LENGTH 45

#define PUMPON 37
#define UNIT 38//PSI VAR

  //msg[35]
// #define BODY_UP 0x80
// #define BODY_DN 0x40
// #define LEG_UP 0x20
// #define LEG_DN 0x10
// #define BED_UP 0x08
// #define BED_DN 0x04

#define BED_DN 0x80
#define LEG_DN 0x40
#define BODY_DN 0x20
#define BODY_UP 0x10
#define LEG_UP 0x08
#define BED_UP 0x04

#define LIGHT 0x02
/////////
//msg[36]
#define UVB   0x80
#define LEDR  0x40
#define LEDG  0x20
#define LEDB  0x10
#define SOL1  0x08
#define SOL2  0x04
#define SOL3  0x02
/////////
//msg[37]
#define SOL4  0x80
#define SOL5  0x40
#define TVUP  0x20
#define TVDN  0x10
/////////

Adafruit_ADS1115 ads1115;	// Construct an ads1115 
DHT dht(DHTPIN, DHTTYPE);

uint8_t LimitSW_flag = 0;
uint8_t pumponflag = 0;

uint8_t flag_holdsol = 1;
uint8_t flag_defaultsol = 0;

uint8_t cnt_solrst = 0;
uint8_t solflag = 0;

uint32_t cnt_cycle=0;
uint32_t cnt_sol=9;
//uint32_t sol_delay=0;
uint32_t pre_cnt_sol=0;
uint32_t post_cnt_sol=0;

uint32_t sol_func;
uint32_t shifttime;

const byte LimitSWPin = 3;

uint8_t TVflag;
uint8_t duration;

uint32_t counter = 0;
uint32_t onoffcounter = 0;

uint8_t tmp_vol[2];

double R1 = 7680;//value of R1 resistor. R2 is thermistor
uint8_t msg[LENGTH] = {255,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,31,32,33,34,0,0,0,38,39,40,41,42,43,44};

float dht_h;
float dht_t;


int8_t Co2Tx[4]={0x11,0x01,0x01,0xED};
int8_t Co2Rx[8]={0,};

int16_t adc[4];

double temp[10];
uint8_t press[10];
uint16_t tmp;

uint8_t toogle=0;

uint8_t rev_msg[9] = {0,};

uint8_t volume[5] = {0,};

void pushbtn(int i)
{

  i-=37;
  if(i==0)
    {
      digitalWrite(PUMPON, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(150);
      digitalWrite(PUMPON, LOW);  // turn the LED on (HIGH is the voltage level)
    }
    if(i==1)
    {
      digitalWrite(UNIT, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(150);
      digitalWrite(UNIT, LOW);  // turn the LED on (HIGH is the voltage level)
    }

}

void pushbtndelay(int i, uint32_t sol_delay)
{
  i-=37;
    
  if(cnt_sol>sol_delay){
    cnt_sol=0;
    if(i==0)
    {
      digitalWrite(PUMPON, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(130);
      digitalWrite(PUMPON, LOW);  // turn the LED on (HIGH is the voltage level)
    }
    if(i==1)
    {
      digitalWrite(UNIT, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(130);
      digitalWrite(UNIT, LOW);  // turn the LED on (HIGH is the voltage level)
    }
  }
}
void solfunc(int func)
{
//37=on 38=unit
//if(cnt_solrst == 0){cnt_solrst = 1;}
    
    switch (func) {
    case 1 : 
      
      if((counter%5)==0){
        solflag=1;
      if(cnt_solrst>0){
        pushbtn(PUMPON);
      }
      
      cnt_solrst = 1;
      digitalWrite(49, 1);  //sol1
      digitalWrite(50, 0);  //sol2
      digitalWrite(51, 0);  //sol3
      digitalWrite(52, 0);  //sol4
      digitalWrite(53, 0);  //sol5      
      }
      if((counter%5)==1){
      digitalWrite(49, 0);  //sol1
      digitalWrite(50, 1);  //sol2
      digitalWrite(51, 0);  //sol3
      digitalWrite(52, 0);  //sol4
      digitalWrite(53, 0);  //sol5
      }
      if((counter%5)==2){
      digitalWrite(49, 0);  //sol1
      digitalWrite(50, 0);  //sol2
      digitalWrite(51, 1);  //sol3
      digitalWrite(52, 0);  //sol4
      digitalWrite(53, 0);  //sol5
      }
      if((counter%5)==3){
      digitalWrite(49, 0);  //sol1
      digitalWrite(50, 0);  //sol2
      digitalWrite(51, 0);  //sol3
      digitalWrite(52, 1);  //sol4
      digitalWrite(53, 0);  //sol5
      }
      if((counter%5)==4){
      digitalWrite(49, 0);  //sol1
      digitalWrite(50, 0);  //sol2
      digitalWrite(51, 0);  //sol3
      digitalWrite(52, 0);  //sol4
      digitalWrite(53, 1);  //sol5
      solflag=0;
      }
    
    break;
    
    case 2 : 
    
      if((counter%5)==0){
        solflag=2;
        pushbtn(PUMPON);
        cnt_solrst = 1;
      digitalWrite(49, 0);  //sol1
      digitalWrite(50, 0);  //sol2
      digitalWrite(51, 0);  //sol3
      digitalWrite(52, 0);  //sol4
      digitalWrite(53, 1);  //sol5      
      }
      if((counter%5)==1){
      digitalWrite(49, 0);  //sol1
      digitalWrite(50, 0);  //sol2
      digitalWrite(51, 0);  //sol3
      digitalWrite(52, 1);  //sol4
      digitalWrite(53, 1);  //sol5
      }
      if((counter%5)==2){
      digitalWrite(49, 0);  //sol1
      digitalWrite(50, 0);  //sol2
      digitalWrite(51, 1);  //sol3
      digitalWrite(52, 1);  //sol4
      digitalWrite(53, 0);  //sol5
      }
      if((counter%5)==3){
      digitalWrite(49, 0);  //sol1
      digitalWrite(50, 1);  //sol2
      digitalWrite(51, 1);  //sol3
      digitalWrite(52, 0);  //sol4
      digitalWrite(53, 0);  //sol5
      }
      if((counter%5)==4){
      digitalWrite(49, 1);  //sol1
      digitalWrite(50, 1);  //sol2
      digitalWrite(51, 0);  //sol3
      digitalWrite(52, 0);  //sol4
      digitalWrite(53, 0);  //sol5
      solflag=0;
      }

    break;

    case 3 : 
    
    if(flag_holdsol == 1){pushbtn(PUMPON);flag_holdsol=0;}
      digitalWrite(49, 1);  //sol1
      digitalWrite(50, 1);  //sol2
      digitalWrite(51, 1);  //sol3
      digitalWrite(52, 1);  //sol4
      digitalWrite(53, 1);  //sol5
    break;
    
    case 4 : 
      digitalWrite(49, 0);  //sol1
      digitalWrite(50, 0);  //sol2
      digitalWrite(51, 0);  //sol3
      digitalWrite(52, 0);  //sol4
      digitalWrite(53, 0);  //sol5
    break;
    
    default : 
    flag_holdsol = 1;
    if(solflag == 0){

    
    //pushbtndelay(PUMPON, 20);
    if(((counter/300) % 2)  ==  1){//solenoid
          if(flag_defaultsol==1){
        pushbtn(PUMPON);
        pumponflag=1;
        cnt_solrst = 1;
        flag_defaultsol=0;   
      }
    digitalWrite(49, 0);  //3
    digitalWrite(50, 1);  //2
    digitalWrite(51, 0);  //1
    digitalWrite(52, 1);  //7
    digitalWrite(53, 0);  //6
    }
    else{
      if(flag_defaultsol==0){
        pushbtn(PUMPON);
        pumponflag=1;
        cnt_solrst = 1;
        flag_defaultsol=1;
      }
    digitalWrite(49, 1);  //3
    digitalWrite(50, 0);  //2
    digitalWrite(51, 1);  //1
    digitalWrite(52, 0);  //7
    digitalWrite(53, 1);  //6
    }      
    }

    break;
  }


}
double ThermisterScan(int RawADC){

  double Temp;
  double R2;

  R2 = R1 * (1023.0 / (double)RawADC - 1.0);
  Temp = log(R2);
  Temp = 1 / (THERCONST_A + (THERCONST_B * Temp) + (THERCONST_C*Temp*Temp*Temp));
  Temp = Temp - 273.15;
  return Temp;
}

int pressureScan(int RawADC){
    
  int mfsr_r18 = map(RawADC, 0, 1024, 0, 100);

  return mfsr_r18;
}



void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  for(int i=0;i<RELAY_NUM;i++) {pinMode(i+RELAY_STARTPIN, OUTPUT);}
  pinMode(2, OUTPUT);
 
  //pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  dht.begin();

  ads1115.begin();  // Initialize ads1115 at address 0x49
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);

  pinMode(LimitSWPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LimitSWPin), LimitSW, FALLING);
  MsTimer2::set(1000,timerISR);  
  MsTimer2::start();
  digitalWrite(PUMPON, LOW);  // turn the LED on (HIGH is the voltage level)

}


void loop() {

  toogle ^= 1; 
  digitalWrite(LED_BUILTIN, toogle);  // turn the LED on (HIGH is the voltage level)
#if 1
//scan temp of thermistor//
  for(int i=10;i<16;i++){temp[i-10] = ThermisterScan(analogRead(i));}

  for(int i=0;i<4;i++){
    adc[i] = ads1115.readADC_SingleEnded(i);
    adc[i] = map(adc[i], 0,26230,0,1023);
    temp[i+6] = ThermisterScan(adc[i]);
  }

  for(int i=0;i<10;i++)
  {
    tmp = (uint16_t)(temp[i]*10);
    msg[2*i + 1]=(uint8_t)(tmp>>8);
    msg[2*i + 2]=(uint8_t)tmp;
  }

///////////////////////////

//scan pressure of ra12a///
  for(int i=0;i<10;i++){
    press[i] = pressureScan(analogRead(i));
    msg[i+21] = press[i];
  }
///////////////////////////  

//////scan data of dht/////
  dht_h = dht.readHumidity();
  dht_t = dht.readTemperature();

  if (isnan(dht_t) || isnan(dht_h)) {
    //값 읽기 실패시 시리얼 모니터 출력
    Serial.println("Failed to read from DHT");
  }
  else {
    //온도, 습도 표시 시리얼 모니터 출력
    // Serial.print("Humidity: "); 
    // Serial.print(dht_h);
    // Serial.print(" %\t");
    // Serial.print("Temperature: "); 
    // Serial.print(dht_t);
    // Serial.println(" *C");

    tmp = (uint16_t)(dht_t*10);
    msg[31]=(uint8_t)(tmp>>8);
    msg[32]=(uint8_t)tmp;

    tmp = (uint16_t)(dht_h*10);
    msg[33]=(uint8_t)(tmp>>8);
    msg[34]=(uint8_t)tmp;
  }

/////////////////////////// 
  #endif
//scan CO2 data

  for (int i = 0; i < 4; i++) {Serial2.write(Co2Tx[i]);}
  delay(15);
    if(Serial2.available() > 0) { 
      for (int i=0;i<8;i++) { 
        while (Serial2.available() == 0) { } 
        Co2Rx[i] = Serial2.read(); 
      }
    }
  //  for (int i = 0; i < 8; i++) {Serial.print(Co2Rx[i]);Serial.print(" ");}

    msg[43]=Co2Rx[3];
    msg[44]=Co2Rx[4];
//16 05 01 0d 47 00 95 fb
#if 0
  //temp
  for(int i=0;i<10;i++){temp[i]=36.5;}
    for(int i=0;i<10;i++)
  {
    tmp = (uint16_t)(temp[i]*10);
    msg[2*i + 1]=(uint8_t)(tmp>>8);
    msg[2*i + 2]=(uint8_t)tmp;
  }
    for(int i=0;i<10;i++){
    msg[i+21] = 50;
  }

    tmp = (uint16_t)(dht_t*10);
    msg[31]=(uint8_t)(tmp>>8);
    msg[32]=(uint8_t)tmp;

    tmp = (uint16_t)(dht_h*10);
    msg[33]=(uint8_t)(tmp>>8);
    msg[34]=(uint8_t)tmp;


  // if(testcounter == 12){
  //   testcounter = 0;
    
  //   if(onoffcounter == 5){onoffcounter = 0;}
  //   if(onoffcounter==0){
  //     digitalWrite(49, 1);  //1
  //     digitalWrite(50, 0);  //2
  //     digitalWrite(51, 0);  //3
  //     digitalWrite(52, 0);  //4
  //     digitalWrite(53, 0);  //5
  //   }
  //   if(onoffcounter==1){
  //     digitalWrite(49, 0);  //1
  //     digitalWrite(50, 1);  //2
  //     digitalWrite(51, 0);  //3
  //     digitalWrite(52, 0);  //4
  //     digitalWrite(53, 0);  //5
  //   }
  //   if(onoffcounter==2){
  //     digitalWrite(49, 0);  //1
  //     digitalWrite(50, 0);  //2
  //     digitalWrite(51, 1);  //3
  //     digitalWrite(52, 0);  //4
  //     digitalWrite(53, 0);  //5      
  //   }
  //   if(onoffcounter==3){
  //     digitalWrite(49, 0);  //1
  //     digitalWrite(50, 0);  //2
  //     digitalWrite(51, 0);  //3
  //     digitalWrite(52, 1);  //4
  //     digitalWrite(53, 0);  //5     
  //   }
  //   if(onoffcounter==4){
  //     digitalWrite(49, 0);  //1
  //     digitalWrite(50, 0);  //2
  //     digitalWrite(51, 0);  //3
  //     digitalWrite(52, 0);  //4
  //     digitalWrite(53, 1);  //5     
  //   }
  //   onoffcounter++;
  // }
#endif
  //////


/////////////////////////// 


 //parsing//
if (Serial.available() > 0) { 
    if (Serial.read() == 255) {
      for (int i=0;i<9;i++) { 
        while (Serial.available() == 0) {} 
        rev_msg[i] = Serial.read(); 
      }
      msg[35] = rev_msg[0];
      msg[36] = rev_msg[1];
      msg[37] = rev_msg[2];

      msg[38] = rev_msg[3];
      msg[39] = rev_msg[4];
      msg[40] = rev_msg[5];
      msg[41] = rev_msg[6];
      msg[42] = rev_msg[7];

      duration = rev_msg[8];
      sol_func = msg[36]&0x0f;

      volume[0] = msg[38];//0~15
      volume[1] = msg[39];//0~15
      volume[2] = msg[40];//0~15
      volume[3] = msg[41];//0~15
      volume[4] = msg[42];//0~15


      Serial.print(255);
      Serial.print(255);
      for (int i=0;i<8;i++) {rev_msg[i]=0;}
    }
  }

//control shift solenoid valve

#if 0

if(((counter/SHIFTTIME) % 2)  ==  1){//solenoid

  digitalWrite(49, 0);  //3
  digitalWrite(50, 1);  //2
  digitalWrite(51, 0);  //1
  digitalWrite(52, 1);  //7
  digitalWrite(53, 0);  //6
}
else{
  digitalWrite(49, 1);  //3
  digitalWrite(50, 0);  //2
  digitalWrite(51, 1);  //1
  digitalWrite(52, 0);  //7
  digitalWrite(53, 1);  //6
}
#endif

if(solflag==1){solfunc(solflag);}
else if(solflag==2){solfunc(solflag);}
else solfunc(sol_func);


if((cnt_solrst>12)&&(pumponflag==1))
{
  cnt_solrst=0;
  pushbtn(PUMPON);
  //Serial.println("pushbtn\n!!");
  pumponflag=0;
}
//if(cnt_solrst>12){cnt_solrst=0;}

//Serial.println(cnt_solrst);


  //control DC relay 
#if 1
if(duration>0){
  //if(duration>10){duration=10;}
  duration--;
  digitalWrite(39, (msg[35]&BED_DN));  //7
  digitalWrite(40, (msg[35]&LEG_DN));  //6
  digitalWrite(41, (msg[35]&BODY_DN));  //5
  digitalWrite(42, (msg[35]&BODY_UP));  //4
  digitalWrite(43, (msg[35]&LEG_UP));  //3
  if(digitalRead(LimitSWPin))
  {
    //Serial.print("danger!!");
    // LimitSW_flag = msg[35]&BED_UP;
    digitalWrite(44, (msg[35]&BED_UP));  //2
  }

}

else
{
  digitalWrite(39, 0);  //7
  digitalWrite(40, 0);  //6
  digitalWrite(41, 0);  //5
  digitalWrite(42, 0);  //4
  digitalWrite(43, 0);  //3
  digitalWrite(44, 0);  //2
}

  digitalWrite(38, (msg[35]&LIGHT));  //1
  digitalWrite(45, (msg[36]&UVB));  //7
  digitalWrite(46, (msg[36]&LEDR));  //6
  digitalWrite(47, (msg[36]&LEDB));  //5
  digitalWrite(48, (msg[36]&LEDG));  //4
  // digitalWrite(49, (msg[36]&SOL1));  //3
  // digitalWrite(50, (msg[36]&SOL2));  //2
  // digitalWrite(51, (msg[36]&SOL3));  //1
  // digitalWrite(52, (msg[37]&SOL4));  //7
  // digitalWrite(53, (msg[37]&SOL5));  //6

  if(((msg[37]&TVUP)==0) && ((msg[37]&TVDN)==0)){

    digitalWrite(4, 1);
    delay(100);
    digitalWrite(6, 0);//tv up stop
    digitalWrite(4, 0);
    digitalWrite(5, 0);//tv down stop
      // Serial.print("msgif[37]:"); 
      // Serial.println(msg[37]);
      TVflag = 1; 
    }

    if((((msg[37]&TVUP)!=0) || ((msg[37]&TVDN)!=0))  &&  (TVflag==1)){
      digitalWrite(6, (msg[37]&TVUP));  
      digitalWrite(5, (msg[37]&TVDN)); 
      delay(100);
      digitalWrite(6, 0);  
      digitalWrite(5, 0); 
     // Serial.print("msgelse[37]:"); 
      //Serial.println(msg[37]);
      TVflag = 0;
    }

#endif

  for(int j=0;j<5;j++)
  {
    for(int i=0;i<3;i++){  
      Serial1.print('s');
      Serial1.print(j);
      Serial1.print(volume[j]/10);
      Serial1.print(volume[j]%10);
      delay(5);
    }
  }
  

  //for(int i=0;i<LENGTH;i++){Serial.write(msg[i]);}//send msg to pc
  //send msg to pc
  for (int i = 0; i < LENGTH; i++) {
    
    Serial.print(msg[i]);
    Serial.print(";");    
  }
  Serial.println();
  
  while(1){
    delay(1);
    if(cnt_cycle>0){
      cnt_cycle=0;
      break;
      }
  }
  

}

  void LimitSW() {
  //Serial.println("irq danger!!");
//  digitalWrite(43, 0);  //3
  digitalWrite(44, 0);  //3
  if(msg[35]&BED_UP == 1){
    duration = 0;
    // LimitSW_flag=0;
  }

  }
  
  void timerISR(){
  cnt_cycle++;
  cnt_sol++;
    counter++;
    
    if(cnt_solrst>0){cnt_solrst++;}
  }