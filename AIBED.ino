#include <Adafruit_ADS1X15.h>

#include <math.h>
#include "DHT.h"
#include <Wire.h>

#define RELAY_STARTPIN 39
#define RELAY_NUM 15
// the setup function runs once when you press reset or power the board

#define THERCONST_A 0.9766319919e-03
#define THERCONST_B 2.502638882e-04
#define THERCONST_C 0.9262423434e-07

#define DHTPIN 2        // 데이터 입력 핀의 설정
#define DHTTYPE DHT22   // DHT22 (AM2302) 센서종류 설정

#define LENGTH 42
Adafruit_ADS1115 ads1115;	// Construct an ads1115 
DHT dht(DHTPIN, DHTTYPE);


double R1 = 7680;//value of R1 resistor. R2 is thermistor
uint8_t msg[LENGTH] = {255,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41};

float dht_h;
float dht_t;

int16_t adc0, adc1, adc2, adc3;

double temp[10];
uint8_t press[10];

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
  
  dht.begin();

  ads1115.begin();  // Initialize ads1115 at address 0x49
  Serial.begin(115200);
}

// the loop function runs over and over again forever
void loop() {
  dht_h = dht.readHumidity();
  dht_t = dht.readTemperature();

  // adc0 = map(adc0, 0,26230,0,1023);
  // double temp = ThermisterScan(adc0);
  // int press = pressureScan(analogRead(0));


// for(int i=0;i<10;i++){
//   press[i] = pressureScan(analogRead(i));
// }

for(int i=10;i<16;i++){
  temp[i-10] = ThermisterScan(analogRead(i));
}
uint16_t tmp = (uint16_t)(temp[0]*10);
  Serial.print("Temperature is : ");
  for(int i=0;i<6;i++){Serial.print(temp[i]);Serial.print(" ");}
  Serial.print(tmp);
  Serial.print(" ");
  msg[1]=(uint8_t)(tmp>>8);
  msg[2]=(uint8_t)tmp;
  Serial.print(msg[1]);
  Serial.print(" ");
  Serial.print(msg[2]);
  Serial.println(" C  ");

  // adc0 = ads1115.readADC_SingleEnded(0);
  // adc1 = ads1115.readADC_SingleEnded(1);
  // adc2 = ads1115.readADC_SingleEnded(2);
  // adc3 = ads1115.readADC_SingleEnded(3);
  // Serial.print("AIN0:"); Serial.print(adc0);
  // Serial.print(" AIN1:"); Serial.print(adc1);
  // Serial.print(" AIN2:"); Serial.print(adc2);
  // Serial.print(" AIN3:"); Serial.println(adc3);
  // Serial.println(" ");
  

  // if (isnan(dht_t) || isnan(dht_h)) {
  //   //값 읽기 실패시 시리얼 모니터 출력
  //   Serial.println("Failed to read from DHT");
  // }
  // else {
  //   //온도, 습도 표시 시리얼 모니터 출력
  //   Serial.print("Humidity: "); 
  //   Serial.print(dht_h);
  //   Serial.print(" %\t");
  //   Serial.print("Temperature: "); 
  //   Serial.print(dht_t);
  //   Serial.println(" *C");
    
  //   // msg[21] = dht_t;
  //   // msg[22] = dht_h;
  // }
  


//  double temp = ThermisterScan(analogRead(10));
  // adc0 = map(adc0, 0,26230,0,1023);
  // double temp = ThermisterScan(adc0);
  // int press = pressureScan(analogRead(0));

  // Serial.print("Temperature is : ");
  // Serial.print(temp);
  // Serial.println(" C  ");

  // Serial.print("pressure is : ");
  // Serial.print(press);
  // Serial.println(" % ");
  //send msg

  //contorl DC relay 
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  for(int i=0;i<RELAY_NUM;i++){
    digitalWrite(i+RELAY_STARTPIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(50);       // wait for a second    
  }
  
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW 
  for(int i=0;i<RELAY_NUM;i++){
    digitalWrite(i+RELAY_STARTPIN, LOW);  // turn the LED on (HIGH is the voltage level)
    delay(50);       // wait for a second   
  }


  //for(int i=0;i<LENGTH;i++){Serial.write(msg[i]);}//send msg to pc



}
