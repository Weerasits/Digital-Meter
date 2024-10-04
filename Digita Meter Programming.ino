#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int Volt_Low_Range_Pin = A0;
const int Volt_High_Range_Pin = A1;
const int Current_Pin = A2;
const int AC_Volt_Pin = A3;
const int Dc_Voltage_Selector = 2;
const int Current_Selector = 3;
const int Ac_Voltage_Selector = 4;
int i;

const float R1_LowRange = 100000.0; // R1-LowRange == 100k Ohm
const float R2_LowRange = 1000.0;   // R2-LowRange == 1k Ohm
const float R1_HighRange = 10000.0; // R1-HighRange == 10k Ohm
const float R2_HighRange = 1000.0;  // R2-HighRange == 1k Ohm

const float r3 = 1680.0;
const float r4 = 110000.0;
const float R_Shunt = 0.5;
float volt = 0.0;
float amp = 0.0;
float temp = 0;

void setup() {
  //Setup
  i = 2;
  while (i<=4){
    pinMode(i,INPUT_PULLUP); // Set Switch to INPUT_PULLUP
    i++;
  }

  Serial.begin(9600);
  Serial.println("Hello World!");

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("DigitalMeter");
  lcd.setCursor(4, 1);
  lcd.print("By Ins46");
  delay(2000);
  lcd.clear();
}

void loop() {
  int Dc_Volt_State = digitalRead(Dc_Voltage_Selector);
  int Current_State = digitalRead(Current_Selector);
  int Ac_Volt_State = digitalRead(Ac_Voltage_Selector);
  if(Dc_Volt_State == LOW){
    DC_Volt();
    // Serial.println("DC");
  }
  if(Current_State == LOW){
    DC_Current();
    // Serial.println("Amp");

  }
  if(Ac_Volt_State == LOW){
    AC_Volt();
    // Serial.println("AC");
  }
  // For Test
  // DC_Volt();
  // DC_Current();
  // AC_Volt();
}

void DC_Volt(){

  // OLD CODE
  int sensorLowValue = analogRead(Volt_Low_Range_Pin); // Read Value from AnalogPin from Low Range Pin
  float Low_voltage = (sensorLowValue * 5.0) / 1023.0; // Calculate Voltage
  float actual_Low_Voltage = Low_voltage * ((R1_LowRange + R2_LowRange) / R2_LowRange); // Actual Voltage

  int sensorHighValue = analogRead(Volt_High_Range_Pin); // Read Value from AnalogPin from Low Range Pin
  float High_voltage = (sensorHighValue * 5.0) / 1023.0; // Calculate Voltage
  float actual_High_Voltage = High_voltage * ((R1_HighRange + R2_HighRange) / R2_HighRange); // Actual Voltage


  // Range Calibation
  if(actual_High_Voltage >= 1.3 && actual_High_Voltage <= 4.94){
    actual_High_Voltage = actual_High_Voltage + 0.1;
  }
  if(actual_High_Voltage >= 4.95 && actual_High_Voltage <= 8.91){
    actual_High_Voltage = actual_High_Voltage + 0.05;
  }
  if(actual_High_Voltage >= 8.92 && actual_High_Voltage <= 11.93){
    actual_High_Voltage = actual_High_Voltage + 0.08;
  }
  if(actual_High_Voltage >= 11.94 && actual_High_Voltage <= 23.97){
    actual_High_Voltage = actual_High_Voltage + 0.06;
  }
  if(actual_High_Voltage >= 23.98 && actual_High_Voltage <= 31.50){
    actual_High_Voltage = actual_High_Voltage + 0.08;
  }
  if(actual_High_Voltage >= 31.51){
    actual_High_Voltage = actual_High_Voltage - 0.03;
  }
  // Voltage Monitor (For Debug)
  // Serial.print("Dc Voltage ");
  // Serial.print("L: ");
  Serial.print(actual_Low_Voltage);
  Serial.print(" H: ");
  Serial.println(actual_High_Voltage);

  // LCD Display
  lcd.setCursor(2, 0);
  lcd.print(" DC Voltage ");
  lcd.setCursor(1, 1);
  lcd.print("Voltage:");
  lcd.setCursor(9, 1);
  lcd.print(actual_High_Voltage);
  lcd.setCursor(14,1);
  lcd.print("V");
  if(actual_High_Voltage < 10.00){
    lcd.setCursor(13, 1);
    lcd.print(" ");
  }
  delay(400);
}

void DC_Current(){
  volt = (analogRead(Current_Pin) * (5.0 / 1023.0)); // Convert ADC To Voltage
  amp = (volt/R_Shunt); // Caculate Current
  if(amp > 0.38){
    amp = amp + 0.02;
  }

  // Current Monitor (For Debug)
  Serial.print("Current : ");
  Serial.println(amp);

  // LCD Display
  lcd.setCursor(2, 0); // กำหนดให้ เคอร์เซอร์ อยู่ตัวอักษรตำแหน่งที่0 แถวที่ 1 เตรียมพิมพ์ข้อความ
  lcd.print("Ampere Meter");
  lcd.setCursor(1, 1);
  lcd.print("Current:");
  lcd.setCursor(9, 1);
  lcd.print(amp);
  lcd.setCursor(14,1);
  lcd.print("A");
  lcd.setCursor(13,1);
  lcd.print(" ");
  delay(400);
}

void AC_Volt(){
  float analogvalue = analogRead(AC_Volt_Pin);
  temp = (analogvalue * 5.0) / 1024.0;
  float ac_volt = temp * (120000 + 1680) / 1680;
  float ac_cal = ac_volt;
  ac_cal = (ac_cal * 0.707) + 1;
  float ac_actual = ac_cal;
  if(ac_cal == 1.0){
    ac_actual = 0.0;
  }
  
  // Error Correcting
  if(ac_cal >= 7 && ac_cal < 11){
    ac_actual = ac_cal - 0.25;
  }
  if(ac_cal >= 11 && ac_cal < 15){
    ac_actual = ac_cal - 0.5;
  }
  if(ac_cal >= 15 && ac_cal < 20){
    ac_actual = ac_cal - 0.75;
  }
  if(ac_cal >= 20 && ac_cal < 30){
    ac_actual = ac_cal - 1;
  }
  if(ac_cal >= 30 && ac_cal < 35){
    ac_actual = ac_cal - 1.5;
  }
  if(ac_cal >= 35 && ac_cal < 37){
    ac_actual = ac_cal - 1.5 - 0.75;
  }
  if(ac_cal >= 37 && ac_cal < 40){
    ac_actual = ac_cal - 1.5;
  }
  if(ac_cal >= 40 && ac_cal < 49){
    ac_actual = ac_cal - 1.25;
  }
  if(ac_cal >= 50 && ac_cal < 75){
    ac_actual = ac_cal - 3;
  }
  if(ac_cal >= 75 && ac_cal < 100){
    ac_actual = ac_cal - 4.75;
  }
  if(ac_cal >= 100 && ac_cal < 110){
    ac_actual = ac_cal - 6.5;
  }
  if(ac_cal >= 110 && ac_cal < 120){
    ac_actual = ac_cal - 7.5;
  }
  if(ac_cal >= 120 && ac_cal < 130){
    ac_actual = ac_cal - 8.25;
  }


  lcd.setCursor(2, 0);
  lcd.print(" AC Voltage ");
  lcd.setCursor(1, 1);
  lcd.print("Voltage:");
  lcd.setCursor(9, 1);
  lcd.print(ac_actual);
  lcd.setCursor(14,1);
  lcd.print("V");
  if(ac_actual < 10.00){
    lcd.setCursor(13, 1);
    lcd.print(" ");
  }
  delay(400);
  // Serial.println(ac_volt);

}