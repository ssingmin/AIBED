#include <math.h>
#include "DHT.h"
#include <Wire.h>
#include <ADS1115-Driver.h>

#define RELAY_STARTPIN 39
#define RELAY_NUM 15
// the setup function runs once when you press reset or power the board

#define THERCONST_A 0.9766319919e-03
#define THERCONST_B 2.502638882e-04
#define THERCONST_C 0.9262423434e-07

#define DHTPIN 2        // 데이터 입력 핀의 설정
#define DHTTYPE DHT22   // DHT22 (AM2302) 센서종류 설정
 
DHT dht(DHTPIN, DHTTYPE);
ADS1115 ads1115 = ADS1115(ADS1115_I2C_ADDR_SDA);


double R1 = 7680;//value of R1 resistor. R2 is thermistor

uint16_t readValue(uint8_t input) {
	ads1115.setMultiplexer(input);
	ads1115.startSingleConvertion();

	delayMicroseconds(25); // The ADS1115 needs to wake up from sleep mode and usually it takes 25 uS to do that

	while (ads1115.getOperationalStatus() == 0);

	return ads1115.readConvertedValue();
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
  
  dht.begin();
  
  ads1115.reset();
	ads1115.setDeviceMode(ADS1115_MODE_SINGLE);
	ads1115.setDataRate(ADS1115_DR_250_SPS);
	ads1115.setPga(ADS1115_PGA_4_096);

  Serial.begin(115200);
}

// the loop function runs over and over again forever
void loop() {


	uint16_t value0 = readValue(ADS1115_MUX_AIN0_GND);
	uint16_t value1 = readValue(ADS1115_MUX_AIN1_GND);
	uint16_t value2 = readValue(ADS1115_MUX_AIN2_GND);
	uint16_t value3 = readValue(ADS1115_MUX_AIN3_GND);

	Serial.println("Values: ");
	Serial.print("IN 0: ");
	Serial.print(value0);
	Serial.print(" IN 1: ");
	Serial.print(value1);
	Serial.print(" IN 2: ");
	Serial.print(value2);
	Serial.print(" IN 3: ");
	Serial.print(value3);
	Serial.print("");

  
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(t) || isnan(h)) {
    //값 읽기 실패시 시리얼 모니터 출력
    Serial.println("Failed to read from DHT");
  }
  else {
    //온도, 습도 표시 시리얼 모니터 출력
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
  }
  
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  for(int i=0;i<RELAY_NUM;i++){
    digitalWrite(i+RELAY_STARTPIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(100);       // wait for a second    
  }
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW 
  for(int i=0;i<RELAY_NUM;i++){
    digitalWrite(i+RELAY_STARTPIN, LOW);  // turn the LED on (HIGH is the voltage level)
    delay(100);       // wait for a second   
  }
  Serial.println(" hihi  ");
  double temp = ThermisterScan(analogRead(10));
  int press = pressureScan(analogRead(0));

  // Serial.print("Temperature is : ");
  // Serial.print(temp);
  // Serial.println(" C  ");

  // Serial.print("pressure is : ");
  // Serial.print(press);
  // Serial.println(" % ");

}
