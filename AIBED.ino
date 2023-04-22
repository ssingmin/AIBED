 #include <math.h>

#define RELAY_STARTPIN 39
#define RELAY_NUM 15
// the setup function runs once when you press reset or power the board

#define THERCONST_A 0.9766319919e-03
#define THERCONST_B 2.502638882e-04
#define THERCONST_C 0.9262423434e-07

double R1 = 7680;

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
  for(int i=0;i<RELAY_NUM;i++){
    pinMode(i+RELAY_STARTPIN, OUTPUT);
    }
  Serial.begin(115200);

}

// the loop function runs over and over again forever
void loop() {

  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  for(int i=0;i<RELAY_NUM;i++){
    digitalWrite(i+RELAY_STARTPIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(10);       // wait for a second    
  }
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW 
  for(int i=0;i<RELAY_NUM;i++){
    digitalWrite(i+RELAY_STARTPIN, LOW);  // turn the LED on (HIGH is the voltage level)
    delay(10);       // wait for a second   
  }
  Serial.println(" hihi  ");
  double temp = ThermisterScan(analogRead(10));
  int press = pressureScan(analogRead(0));

  Serial.print("Temperature is : ");
  Serial.print(temp);
  Serial.println(" C  ");

  Serial.print("pressure is : ");
  Serial.print(press);
  Serial.println(" % ");

}
