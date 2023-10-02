  #include <Keypad.h>
#include <string.h>
#include <Arduino.h> // su dung parseInt
#include <stdlib.h>
#include <PID_v1.h>

// L298 Variable
int IN1 = 10;
int IN2 = 12;
int enA = 11;


// Keypad Variable
const int ROW_NUM = 4;
const int COLUMN_NUM = 4;


// Take data from keypad variable
String inputSpeed = "0";
int outputSpeed = 0; 


// Sensor Variable
int sensor = 2;
int sensorValue;
int countPulse = 0; // count pulse from encoder gear
int changeStatus = 0; // create a delay between HIGH and LOW to avoid receiving continuous signal
// Timer Variable
double lastTime = 0;
double currentTime;
int Revolution = 0; // number of Revolution motor rotary


double RPM;


// Temporary Variable 
int tempo = 0; // 


// Setup keypad 4x4
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
 
byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; 
byte pin_column[COLUMN_NUM] = {5, 4, 3, A0};
 
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);



// Prototype declaration
int myAtoi(String str); // Fuction convert String to Integer
void keypad_handle(); // Function handle input data from keypad
void motor_speed(double runTime); // Function motor speed caculate by encoder (RPM)
void isPWM(int outputSpeed);  // representative pwm pulse

// ---------------------------------------------------
// Cấu hình PID
double Setpoint, Input, Output;
double Kp = 2.5, Ki = 1.5, Kd = 1.5;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


  
// ---------------------------------------------------




void setup(){
  // Baud rate
  Serial.begin(9600);
  // L298N
  pinMode(enA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(sensor, INPUT);
  //attachInterrupt(digitalPinToInterrupt(sensor), motor_speed, RISING);
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255);
  // Serial Welcome
  Serial.println("Welcome to MyLab Spin Centrifugate Rotator");
  Serial.println("Nhập vào tốc độ mong muốn của động cơ trong khoảng 150-800 (RPM): ");
  Serial.println("Nhấn phím * hoăc # để xoá kí tự, tất cả các phím chữ để xác nhận tốc độ động cơ: ");
}








void loop(){
  
  // runtime program
  double runTime = (double) millis() / 1000; // convert milis to sec


 

  // Call function
  keypad_handle();

   if(outputSpeed == 0)
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW); 
  }
  else
  {
  

  
  // Call function
   motor_speed(runTime);
   if((RPM * 11/10 > Setpoint || RPM * 90/100 < Setpoint) && tempo == 0)
   {
   myPID.Compute();
   }
   else 
   {
    Setpoint = RPM;
    tempo = 1;
   }
   
   controlMotor(Output);
 
  
}
}








// ---------------------------------------------------
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

// ---------------------------------------------------

void keypad_handle()
{
  char key = keypad.getKey();
  String tempKey;

  // Keypad input handle
  if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == '0')
  {
    tempKey = String(key);
    inputSpeed = inputSpeed + tempKey;
    Serial.println("Mời nhập tiếp tốc độ động cơ - Ấn các phím không phải số nguyên để xác nhận: " + inputSpeed);
  }
  else if(key == '*' || key == '#')
  {
    if(inputSpeed != "0")
    {
      inputSpeed = inputSpeed.substring(0, inputSpeed.length() - 1);
      Serial.println("Mời nhập tiếp tốc độ động cơ - Ấn các phím không phải số nguyên để xác nhận: " + inputSpeed);
    }
    
  }
  else if(key != NULL)
  {
    tempo = 0;
    Serial.println("Xác nhận tốc động cơ yêu cầu: " + inputSpeed + " vòng/phút");
    outputSpeed = myAtoi(inputSpeed);
    if(outputSpeed > 800)
    {
      Serial.print("Tốc độ này vượt quá mức động cơ cho phép, vui lòng nhập lại tốc độ của động cơ trong khoảng 150-800 (RPM): ");
      outputSpeed = 0;
      keypad_handle();
    }
    else if(outputSpeed < 100 && outputSpeed != 0)
    {
      Serial.print("Tốc độ này nhỏ hơn mức thấp nhất động cơ có thể chạy được, vui lòng nhập lại tốc độ của động cơ trong khoảng 150-800 (RPM): ");
      outputSpeed = 0;
      keypad_handle();
    }
    else if(outputSpeed < 800 && outputSpeed > 100)
    {
      Setpoint = outputSpeed;
    }
//    isPWM(outputSpeed);
    
//    int reprePWM;
//    Serial.println(outputSpeed);
//    reprePWM = map(outputSpeed, 100, 800, 30, 255);
//    Serial.println(reprePWM);
//    analogWrite(enA, reprePWM);
//    digitalWrite(IN1, HIGH);
//    digitalWrite(IN2, LOW); 
    //Serial.println(outputSpeed);
    Serial.println(Setpoint);
    inputSpeed = "0";
   
  }
}

// ---------------------------------------------------

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
    if(countPulse % 9 == 0 && countPulse > 0)
    {
      Revolution++;
      if(Revolution >= 1)
      {
        lastTime = currentTime;
        currentTime = runTime;
        
        RPM = 60/ (currentTime - lastTime);
        Serial.print("RPM: ");
        Input = RPM;
        Serial.println(RPM); // check RPM
        Serial.println(outputSpeed);
        Serial.println(Output);
      }
    }
    changeStatus = 1;
  }
  
  if(sensorValue == 1 && changeStatus == 1)
  {
    changeStatus = 0;
  }
}

// ---------------------------------------------------

void controlMotor(int pwm) {
  // Điều khiển động cơ theo chiều thuận
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(enA, pwm);
}
  
