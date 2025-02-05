#include <math.h>
#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2); 
int sensor = 10;
int IN3 = 6;
int IN4 = 7;
int enB = 3;

// Keypad setup
//const int ROW_NUM = 4;
//const int COLUMN_NUM = 4;
//
//char keys[ROW_NUM][COLUMN_NUM] = {
//  {'1','2','3', 'A'},
//  {'4','5','6', 'B'},
//  {'7','8','9', 'C'},
//  {'*','0','#', 'D'}
//};
// 
//byte pin_rows[ROW_NUM] = {5, 4, 3, 2}; 
//byte pin_column[COLUMN_NUM] = {A0, A1, A2, A3}; 
//Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// Sensor Variable
int sensorValue;
int countPulse = 0; // count pulse from encoder gear
int changeStatus = 0; // create a delay between HIGH and LOW to avoid receiving continuous signal

// Keypad data handle
String inputSpeed = "0";
int outputSpeed;
int tempo = 0;

// Timer Variable
double lastTime = 0;
double currentTime;
int Revolution = 0; // number of Revolution motor rotary


// tester
double RPM;
int expectRPM = 877;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(enB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(sensor, INPUT);
//  lcd.begin();                    
//  lcd.backlight();
//  lcd.setCursor(0,0);
//  lcd.print("MyLab Centrifugal");
//  lcd.setCursor(0,1);
//  lcd.print("Enter the keyboard desired speed");

  
}

void loop() {
  // put your main code here, to run repeatedly:
  // runtime program
  double runTime = (double) millis() / 1000; // convert milis to sec
//  keypad_handle();

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(enB, 120);
  
//  if(runTime < 5)
//  {
//  digitalWrite(IN1, HIGH);
//  digitalWrite(IN2, LOW);
//  analogWrite(enA, 255);
//  }
//  else 
//  {
//    digitalWrite(IN1, HIGH);
//  digitalWrite(IN2, LOW);
//  analogWrite(enA, 100);
//  }
  motor_speed(runTime);
}

void motor_speed(double runTime)
{
  // motor speed calculate by encoder

  // Calculate RPM of Motor
  sensorValue = digitalRead(sensor);
  if(sensorValue == 0 && changeStatus == 0)
  { 
    countPulse++;
    //Serial.println(countPulse);
    if(countPulse == 1) currentTime = runTime;
    if(countPulse % 8 == 0 && countPulse > 0)
    {
      Revolution++;
      if(Revolution >= 1)
      {
        lastTime = currentTime;
        currentTime = runTime;
        
        RPM = (60/ (currentTime - lastTime)) * 9;
        Serial.print("RPM: ");
        //Input = RPM;
        Serial.println(RPM ); // check RPM
        //Serial.println(outputSpeed);
        //Serial.println(Output);
      }
    }
    changeStatus = 1;
  }
  
  if(sensorValue == 1 && changeStatus == 1)
  {
    changeStatus = 0;
  }
}

void predictPulse(int expectRPM) 
{
  int prePwm;
  if(expectRPM >= 303 && expectRPM < 451)
  {
    prePwm = (expectRPM + 141) / 14.8;
  }

  if(expectRPM >= 451 && expectRPM < 505)
  {
    prePwm = (expectRPM - 55) / 9.9;
  }

  if(expectRPM >= 505 && expectRPM < 606)
  {
    prePwm = (expectRPM - 270) / 5.6;
  }

  if(expectRPM >= 606 && expectRPM < 645)
  {
    prePwm = (expectRPM - 372) / 3.9;
  }

  if(expectRPM >= 645 && expectRPM < 681)
  {
    prePwm = (expectRPM - 393) / 3.6;
  }
 
  if(expectRPM >= 681 && expectRPM < 722)
  {
    prePwm = (expectRPM - 353) / 4.1;
  }
  if(expectRPM >= 722 && expectRPM < 740)
  {
    prePwm = (expectRPM - 560) / 1.8;
  }

  if(expectRPM >= 740 && expectRPM < 750)
  {
    prePwm = (expectRPM - 640);
  }

  if(expectRPM >= 750 && expectRPM < 759)
  {
    prePwm = (expectRPM - 651) / 0.9;
  }

  if(expectRPM >= 759 && expectRPM < 769)
  {
    prePwm = (expectRPM - 639);
  }

  if(expectRPM >= 769 && expectRPM < 800)
  {
    prePwm = (expectRPM - 366) / 3.1;
  }

  if(expectRPM >= 800 && expectRPM < 810)
  {
    prePwm = (expectRPM - 660);
  }

  if(expectRPM >= 810 && expectRPM < 821)
  {
    prePwm = (expectRPM - 645) / 1.1;
  }

  if(expectRPM >= 821 && expectRPM < 857)
  {
    prePwm = (expectRPM - 725) / 0.6;
  }

  if(expectRPM >= 857 && expectRPM < 863)
  {
    prePwm = (expectRPM - 725) / 0.6;
  }

  if(expectRPM >= 863 && expectRPM < 882)
  {
    prePwm = (expectRPM - 559) / (19/15);
  }

  prePwm = (int) prePwm;
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
//  Serial.print("prePulse: ");
//  Serial.println(prePwm);
  analogWrite(enB, prePwm);
}

//void keypad_handle()
//{
//  char key = keypad.getKey();
//  String tempKey;
//
//  // Keypad input handle
//  if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == '0')
//  {
//    tempKey = String(key);
//    inputSpeed = inputSpeed + tempKey;
//    Serial.println("Mời nhập tiếp tốc độ động cơ - Ấn các phím không phải số nguyên để xác nhận: " + inputSpeed);
//    lcd.setCursor(2,0);
//    lcd.print("");
//  }
//  else if(key == '*' || key == '#')
//  {
//    if(inputSpeed != "0")
//    {
//      inputSpeed = inputSpeed.substring(0, inputSpeed.length() - 1);
//      Serial.println("Mời nhập tiếp tốc độ động cơ - Ấn các phím không phải số nguyên để xác nhận: " + inputSpeed);
//    }
//    
//  }
//  else if(key != NULL)
//  {
//    tempo = 0;
//    Serial.println("Xác nhận tốc động cơ yêu cầu: " + inputSpeed + " vòng/phút");
//    outputSpeed = myAtoi(inputSpeed);
//    if(outputSpeed > 5000)
//    {
//      Serial.print("Tốc độ này vượt quá mức động cơ cho phép, vui lòng nhập lại tốc độ của động cơ trong khoảng 150-800 (RPM): ");
//      lcd.setCursor(2,0);
//      lcd.print("Over Limit");
//      lcd.setCursor(0,1);
//      lcd.print("Please re-enter");
//      outputSpeed = 0;
//      keypad_handle();
//    }
//    else if(outputSpeed < 500 && outputSpeed != 0)
//    {
//      Serial.print("Tốc độ này nhỏ hơn mức thấp nhất động cơ có thể chạy được, vui lòng nhập lại tốc độ của động cơ trong khoảng 150-800 (RPM): ");
//      lcd.setCursor(2,0);
//      lcd.print("Over Limit");
//      lcd.setCursor(0,1);
//      lcd.print("Please re-enter");
//      outputSpeed = 0;
//      keypad_handle();
//    }
//    else if(outputSpeed < 800 && outputSpeed > 100)
//    {
//      lcd.setCursor(0,1);
//      lcd.print("");
//    }
////    isPWM(outputSpeed);
//    
////    int reprePWM;
//
//  
//    inputSpeed = "0";
//   
//  }
//}
// -----------------------------------------

int myAtoi(String str) {
    int result = 0;
    int sign = 1;
    int i = 0;
    
    if (str[0] == '-') {
        sign = -1;
        i++;
    }
    
    for (; str[i] != '\0'; ++i) {
        result = result * 10 + str[i] - '0';
    }
    
    return sign * result;
}
