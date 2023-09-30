#define MAG_ADDRESS 0x1E
#define MAG_XOUT_H 0x03
#define MAG_XOUT_L 0x04
#define MAG_YOUT_H 0x05
#define MAG_YOUT_L 0x06
#define MAG_ZOUT_H 0x07
#define MAG_ZOUT_L 0x08


#include <HMC5883L.h>
#include <Wire.h>
#include <PWM.h>

HMC5883L compass;

int32_t frequency = 500;

float Kpx = 0.03168764619;
float Kdx = 118.8542069;

float Kpy = 0.03168764619;
float Kdy = 118.8542069;

float Kpz = 0.03168764619;
float Kdz = 118.8542069;

float B_x = 0.035*10000; //.15 millitesla......since hmc588l measures in milliGauss  
float B_y = 0.0098*10000;//millitesla
float B_z = 0.0145*10000;//millitesla

//For X Axis
float current_x = 0.0009094320503*B_x;// milliGauss aan
//float voltage = 0.00109131846*B_one;// *1.2ohms 
//int32_t fduty_x = 0.008412246465*B_x*8; 
float duty_x = 0.009094320503*B_x;//out of 100--*100/12
float pwm_x = duty_x*160;

//For Y Axis
//float current_y = 0.0008988552085*B_y;//amps vs milli gauss
//float voltage = 0.00107862625*B_one;
//float fduty_y = 0.008314410678*B_y*8;
float duty_y = 0.008988552085*B_y;//out of 100
float pwm_y = duty_y*160;

//For Z Axis
//float current_z = 0.001060626562*B_one;
//float voltage = 1.177295484*B_one;
float duty_z = 0.01060626562*B_z;//out of 100
float pwm_z = duty_z*160;


void setup() {
  InitTimersSafe(); 
  SetPinFrequencySafe(3,frequency);
  SetPinFrequencySafe(5,frequency);
  SetPinFrequencySafe(6,frequency);
  SetPinFrequencySafe(7,frequency);
  SetPinFrequencySafe(8,frequency);
  SetPinFrequencySafe(2,frequency);

  Wire.begin();
  
  // Initialize the magnetometer
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write(0x02); // Mode register
  Wire.write(0x00); // Continuous measurement mode
  Wire.endTransmission(false);
  Wire.requestFrom(MAG_ADDRESS, 6);

  pinMode(2,OUTPUT); 
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  
  }

float FindError_x(float x, float B_x){

    float errorduty_x = ((0.009094320503*B_x) - (0.009094320503*x))*160;//we must get this out of 100.Niw, this is out of 16000
    // now add the error to the current value of duty cycle
    return errorduty_x;

  }

float FindError_y(float y, float B_y){
  
    float errorduty_y = ((0.008988552085*B_y) - (0.008988552085*y))*160;
    return errorduty_y;

 }

float FindError_z(float z, float B_z){
  
    float errorduty_z = ((0.009810795699*B_z) - (0.009810795699*z))*160;//make it out of 16000...1
    return errorduty_z;

  }

void loop() {
  
  Serial.begin(9600);
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write(MAG_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MAG_ADDRESS, 6);

  float x = ((Wire.read() << 8) | Wire.read());
  float y = ((Wire.read() << 8) | Wire.read());
  float z = ((Wire.read() << 8) | Wire.read());

  //Forward
  analogWrite(2,pwm_x);
  analogWrite(3,pwm_y);
  analogWrite(5,pwm_z);//800
  //Backward
  analogWrite(6,15000);//800.....but the magnitude sucks
  analogWrite(7,15000);//800
  analogWrite(8,15000);//800
  
  float error_x = 0;
  float error_y = 0;
  float error_z = 0;
  
    Serial.print("X=");
    Serial.print(x);
    Serial.print(" Duty Ratio= ");
    Serial.println(duty_x);
//    Serial.print(" Error X = ");
//    Serial.println(error_x);
    Serial.print("Y=");
    Serial.print(y);
    Serial.print(" Duty Ratio= ");
    Serial.println(duty_y);
//    Serial.print(" Error Y = ");
//    Serial.println(error_y);
    Serial.print("Z=");
    Serial.print(z);
    Serial.print(" Duty Ratio= ");
    Serial.println(duty_z);
//    Serial.print(" Error Z = ");
//    Serial.println(error_z);
   
   //Safety Mechanism 
   if(duty_x<100){ 
      if(x!=B_x){
        error_x = FindError_x(x,B_x);
        //float errorpwm = poserror*2.1;
        duty_x = duty_x + error_x;
        pwm_x = duty_x*160; 
       }  
   }
    if(duty_y<100){
      if(y!=B_y){
        error_y = FindError_y(y,B_y);
        //float errorpwm = poserror*2.1;
        duty_y = duty_y + error_y;
        pwm_y = duty_y*160; 
      } 
    }
    if(duty_z<100){
      if(z!=B_z){
        error_z = FindError_z(z,B_z);
        //float errorpwm = poserror*2.1;
        duty_z = duty_z + error_z;
        pwm_z = duty_z*160; 
      }
    }  
//    Serial.println(current1);
//    Serial.println(FindError_one(x,B_x));
   delay(1000);
  }
