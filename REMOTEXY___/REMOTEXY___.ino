#define REMOTEXY_MODE__SOFTSERIAL
#include <SoftwareSerial.h>
#include <String.h>
#include <Servo.h>
#include <RemoteXY.h>
#include <Wire.h>

#define REMOTEXY_SERIAL_RX 10
#define REMOTEXY_SERIAL_TX 11
#define REMOTEXY_SERIAL_SPEED 9600

const int LEFT_INF = 38;
const int RIGHT_INF = 40;
const int fdr = 1;
const int sdr = 2;

float angle = 1;
int k = 0.5;
char cxjoystick [7];
char cyjoystick [7];
char cangle [7];
float sinus = 0.0;
float cosinus = 0.0;
int LR = 180;



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
void rotate(int R){
  if(R != LR){
  for(R; map(R, 0, 100, 0, 360) < LR; LR--){
     awspeed(fdr, "43", 200);//левое
     awspeed(sdr, "44", 200);//переднее
     awspeed(sdr, "43", 200);//правое
     awspeed(fdr, "44", 200);//ЗАД
  }
  for(R; map(R, 0, 100, 0, 360) > LR; LR++){
     awspeed(fdr, "43", -200);//левое
     awspeed(sdr, "44", -200);//переднее
     awspeed(sdr, "43", -200);//правое
     awspeed(fdr, "44", -200);//ЗАД      
  }
 // awspeed(fdr, "43", 0);//левое
  //awspeed(sdr, "44", 0);//переднее
 // awspeed(sdr, "43", 0);//правое
//  awspeed(fdr, "44", 0);
  }
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


//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,48,0,0,0,229,0,15,213,1,
  5,0,1,64,30,30,2,26,31,7,
  36,5,10,20,5,2,26,2,11,7,
  36,5,24,20,5,2,26,2,11,7,
  36,31,10,20,5,2,26,2,11,7,
  36,31,24,20,5,2,26,2,11,129,
  0,4,56,26,4,17,208,157,208,176,
  208,191,209,128,208,176,208,178,208,187,
  208,181,208,189,208,184,208,181,0,4,
  128,35,74,23,8,2,26,129,0,38,
  56,17,4,17,208,159,208,190,208,178,
  208,190,209,128,208,190,209,130,0,129,
  0,8,4,11,5,17,208,163,208,179,
  208,190,208,187,0,129,0,27,4,28,
  5,17,208,160,208,176,209,129,209,129,
  209,130,208,190,209,143,208,189,208,184,
  208,181,0,129,0,4,18,23,5,17,
  208,161,208,186,208,190,209,128,208,190,
  209,129,209,130,209,140,0,129,0,36,
  18,9,5,17,208,154,208,190,208,180,
  0,1,0,46,42,12,12,2,31,0,
  129,0,41,37,21,4,17,208,158,209,
  129,209,130,208,176,208,189,208,190,208,
  178,208,186,208,176,0 };
  
// структура определяет все переменные и события вашего интерфейса управления 
struct {

    // input variables
  int8_t joystick_1_x; // =-100..100 координата x положения джойстика 
  int8_t joystick_1_y; // =-100..100 координата y положения джойстика 
  char edit_1[11];  // =строка UTF8 оканчивающаяся нулем  
  char edit_2[11];  // =строка UTF8 оканчивающаяся нулем  
  char edit_3[11];  // =строка UTF8 оканчивающаяся нулем  
  char edit_4[11];  // =строка UTF8 оканчивающаяся нулем  
  int8_t slider_1; // =0..100 положение слайдера 
  uint8_t button_1; // =1 если кнопка нажата, иначе =0 
  char text_1[11];  // =строка UTF8 оканчивающаяся нулем 
    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

void stopMotors(){
 awspeed(fdr, "43", 0);//левое
  
 awspeed(sdr, "44", 0);//переднее
  
 awspeed(sdr, "43", 0);//правое
  
 awspeed(fdr, "44", 0);//ЗАД
 RemoteXY.joystick_1_x = 0;
 RemoteXY.joystick_1_y = 0;
}


void setup() 
{
  RemoteXY_Init (); 
  pinMode(18,INPUT);
  pinMode(19,OUTPUT);
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
  RemoteXY.slider_1 = 50;
 // acommand(fdr, String(42), String(79));
 // acommand(sdr, String(42), String(79));
  
}

void loop() 
{ 
  RemoteXY_Handler ();
  float xx = RemoteXY.joystick_1_x;
  float yy = RemoteXY.joystick_1_y;
  
  //dtostrf (RemoteXY.joystick_1_x, 0, 1, cxjoystick);
  //sprintf (RemoteXY.text_1, cxjoystick);
  //dtostrf (RemoteXY.joystick_1_y, 0, 1, cyjoystick);
  //sprintf (RemoteXY.text_2, cyjoystick);
  dtostrf (angle, 0, 1, cangle);
  sprintf (RemoteXY.text_1, cangle);

  rotate(RemoteXY.slider_1);
  if (RemoteXY.joystick_1_x >= 0 and RemoteXY.joystick_1_y >= 0){
    angle = atan(xx*1.0/yy)*180.0/3.14;
  }
  if (RemoteXY.joystick_1_x >= 0 and RemoteXY.joystick_1_y < 0){
    angle = 180 - (atan(xx/yy*(-1.0))*180.0/3.14);
  }
  if (RemoteXY.joystick_1_x < 0 and RemoteXY.joystick_1_y >= 0){
    angle = 360 - (atan((-1.0)*xx/yy)*180.0/3.14);
  }
  if (RemoteXY.joystick_1_x < 0 and RemoteXY.joystick_1_y < 0){
    angle = atan(xx/yy)*180.0/3.14 + 180;
  }
  if(RemoteXY.button_1 == 1){
    stopMotors();
  }

  //sinus = (RemoteXY.joystick_1_x/sqrt(RemoteXY.joystick_1_x^2 + RemoteXY.joystick_1_y^2));
  //cosinus = (RemoteXY.joystick_1_y/sqrt(RemoteXY.joystick_1_x^2 + RemoteXY.joystick_1_y^2));
  
  k = map(sqrt(pow(RemoteXY.joystick_1_x, 2) + pow(RemoteXY.joystick_1_y, 2)), 0, 141.42, 0, 400);
  Serial.print(RemoteXY.joystick_1_x);
  Serial.print("   ");
  Serial.print(angle);
  Serial.print("   ");
  Serial.println(RemoteXY.joystick_1_y);
 /*
  awspeed(fdr, "43", cos(2*3.14*(angle/360.0))*(-k));
  awspeed(sdr, "44", sin(2*3.14*(angle/360.0))*k);
  awspeed(sdr, "43", cos(2*3.14*(angle/360.0))*k);
  awspeed(fdr, "44", sin(2*3.14*(angle/360.0))*(-k)); 
*/
 awspeed(fdr, "43", cos(2*3.14*(angle/360.0))*(-k));//левое
  
 awspeed(sdr, "44", sin(2*3.14*(angle/360.0))*(-k));//переднее
  
 awspeed(sdr, "43", cos(2*3.14*(angle/360.0))*(k));//правое
  
 awspeed(fdr, "44", sin(2*3.14*(angle/360.0))*k);//ЗАД
}
