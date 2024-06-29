#include <Wire.h>
#include <String.h>
#include <Servo.h>

const int LEFT_INF = 38;
const int RIGHT_INF = 40;
const int fdr = 1;
const int sdr = 2;
const int k = 200;

Servo MG995;
 
int hexadecimalToDecimal(String hexVal)
{
    int len = hexVal.length();
    int base = 1;
    int dec_val = 0;
    for (int i = len - 1; i >= 0; i--) {
        if (hexVal[i] >= '0' && hexVal[i] <= '9') {
            dec_val += (int(hexVal[i]) - 48) * base;
            base = base * 16;
        }
        else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') {
            dec_val += (int(hexVal[i]) - 55) * base;
            base = base * 16;
        }
    }
    return dec_val;
}

void acommand(int x, String y, String z){
  Wire.begin();
  Wire.beginTransmission(1);
  Wire.write(hexadecimalToDecimal(y));
  if (z != "00"){
    Wire.write(hexadecimalToDecimal(z));
  }
  Serial.println(analogRead(A7));
  Wire.endTransmission();
  delay(10);
}
void bcommand(int x, String y, String z, String d){
  Wire.begin();
  Wire.beginTransmission(1);
  Wire.write(hexadecimalToDecimal(y));
  if (z != "00"){
    Wire.write(hexadecimalToDecimal(z));
  }
  if (d != "00"){
    Wire.write(hexadecimalToDecimal(d));
  }
  Wire.endTransmission();
  delay(10);
}

void awspeed(int c_id, String y, uint16_t speedMotor){
//выделяем первый байт скорости, накладываем маску, обнуляем последние 8 бит
  uint16_t speedFirstByte = speedMotor & 0b1111111100000000;
  speedFirstByte = speedFirstByte >> 8;
  //выделяем второй байт скорости, обнуляем первые 8 бит
  uint16_t speedSecondByte = speedMotor & 0b0000000011111111; 

  Wire.beginTransmission(c_id);
  Wire.write(hexadecimalToDecimal(y)); //Sets the speed parameter for Motor 1 in degrees per second
  Wire.write(speedFirstByte);//два байта на один мотор, это против часовой
  Wire.write(speedSecondByte);    
  Wire.endTransmission();
  delay(10);
}

void stopMotors(uint8_t c_id){
  Wire.beginTransmission(c_id);
  Wire.write(0x42);
  Wire.endTransmission();
}
void setup() {
 
  //MG995.attach(A5);
  Wire.begin();
  Wire.beginTransmission(fdr);
  Wire.write(0x25);
  Wire.endTransmission();
  delay(10);
  Wire.beginTransmission(sdr);
  Wire.write(0x25);
  Wire.endTransmission();
  delay(10);
  Serial.begin(9600);
  acommand(fdr, String(42), String(79));
  acommand(sdr, String(42), String(79));
}
int angle = 1;

void loop() {

  if(Serial.available() > 0){
    int x = Serial.parseInt();
    if (x != angle && x!= 0){
      angle = x;
    }
    //map(analogRead(A7), 0, 1022, 0, 360);
  }
  Serial.println(angle);
  
  
  awspeed(fdr, "43", cos(2*3.14*(angle/360.0))*(-k));
  awspeed(sdr, "44", sin(2*3.14*(angle/360.0))*k);
  awspeed(sdr, "43", cos(2*3.14*(angle/360.0))*k);
  awspeed(fdr, "44", sin(2*3.14*(angle/360.0))*(-k));  
  //awspeed(fdr, "43", cos(2*3.14*(angle/360.0))*(-k));
  //awspeed(fdr, "43", map(analogRead(A7), 0, 1022, -600, 600));
  //awspeed(fdr, "44", map(analogRead(A7), 0, 1022, -600, 600));
  //awspeed(sdr, "43", map(analogRead(A7), 0, 1022, -600, 600));
  //awspeed(sdr, "44", map(analogRead(A7), 0, 1022, -600, 600));
  
  //MG995.write(map(analogRea
  //bcommand(1, String(42), String(79), String(79));
  //bcommand(1, String(43), String(1, String(2));
  //bcommand(1, String(44), String(1), String(2));
  

}
