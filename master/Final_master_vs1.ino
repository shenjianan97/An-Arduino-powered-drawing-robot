#include <SoftwareSerial.h>
// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(19, 18, 17, 16, 7, 2);

char val;  //存储接收的变量
SoftwareSerial BT(14, 15);  //新建对象，接收脚为8，发送脚为9

String inString = "";

// setup servo
int PEN_DOWN = 10; // angle of servo when pen is down
int PEN_UP = 51;   // angle of servo when pen is up
int servopin = 13; //设置舵机驱动脚到数字口9
int myangle;//定义角度变量
int pulsewidth;//定义脉宽变量

//定义小车属性
float wheel_dia = 66; //      # mm (increase = spiral out) 车轮直径
float wheel_base = 92.5; //,    # mm (increase = spiral in) 轮距
int steps_rev = 512; //,     # 512 for 64x gearbox, 128 for 16x gearbox
int delay_time = 6; //            # time between steps in ms
float stepperSpeed = 2;//电机转速,1ms一步
//脉冲总数，或者说步的总数
int stepsum = 0;

//定义引脚
int _step = 0;

int L_stepper_pins[] = {8, 9, 10, 11};
int R_stepper_pins[] = {3, 4, 5, 6};
int forwardSpin[][4] = {{0, 0, 0, 1},
  {0, 0, 1, 1},
  {0, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 0},
  {1, 1, 0, 0},
  {1, 0, 0, 0},
  {1, 0, 0, 1}
};
int reverseSpin[][4] = {{1, 0, 0, 1},
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1}
};

int times = 0; //主循环执行次数
void setup()
{
  pinMode(servopin, OUTPUT); //设定舵机接口为输出接口
  BT.begin(9600);  //设置波特率
  Serial.begin(57600);//设置波特率为9600

  for (int pin = 0; pin < 4; pin++) {
    pinMode(L_stepper_pins[pin], OUTPUT);
    digitalWrite(L_stepper_pins[pin], LOW);
    pinMode(R_stepper_pins[pin], OUTPUT);
    digitalWrite(R_stepper_pins[pin], LOW);
  }
  penup();
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  getBTInput();
  getBTInput();
}
//舵机模块辅助函数
void servopulse(int servopin, int myangle) /*定义一个脉冲函数，用来模拟方式产生PWM值*/
{
  pulsewidth = (myangle * 11) + 500; //将角度转化为500-2480 的脉宽值
  digitalWrite(servopin, HIGH); //将舵机接口电平置高
  delayMicroseconds(pulsewidth);//延时脉宽值的微秒数
  digitalWrite(servopin, LOW); //将舵机接口电平置低
  delay(20 - pulsewidth / 1000); //延时周期内剩余时间
}
void changeServoDegree(int degree)
{
  for (int i = 0; i <= 50; i++) //产生PWM个数，等效延时以保证能转到响应角度
  {
    servopulse(servopin, degree); //模拟产生PWM
  }
}
void penup() {
  delay(100);
  //Serial.println("PEN_UP()");
  changeServoDegree(PEN_UP);
  delay(250);
}
void pendown() {
  delay(250);
  //Serial.println("PEN_DOWN()");
  changeServoDegree(PEN_DOWN);
  delay(250);
}

//步进电机模块辅助函数
void leftWheelSpin(int steps)
{
  //判断是正转还是反转
  // boolean dir = Serial.parseInt();
  // if(dir)
  // {
  // _step++;
  //
  // }else{
  //
  // _step--;
  //
  if (steps > 0) {
    for (int i = 0; i < steps; i++) {
      for (int mask = 0; mask < 8; mask++) {
        for (int pin = 0; pin < 4; pin++) {
          digitalWrite(L_stepper_pins[pin], forwardSpin[mask][pin]);
        }
        delay(stepperSpeed);
      }
      stepsum++;
    }
  }
  if (steps < 0) {
    steps = (-1) * steps;
    for (int i = 0; i < steps; i++) {
      for (int mask = 0; mask < 8; mask++) {
        for (int pin = 0; pin < 4; pin++) {
          digitalWrite(L_stepper_pins[pin], reverseSpin[mask][pin]);
        }
        delay(stepperSpeed);
      }
      stepsum--;
    }
  }
  Serial.println(stepsum);
}
void rightWheelSpin(int steps)
{
  //判断是正转还是反转
  // boolean dir = Serial.parseInt();
  // if(dir)
  // {
  // _step++;
  //
  // }else{
  //
  // _step--;
  //
  if (steps > 0) {
    for (int i = 0; i < steps; i++) {
      for (int mask = 0; mask < 8; mask++) {
        for (int pin = 0; pin < 4; pin++) {
          digitalWrite(R_stepper_pins[pin], forwardSpin[mask][pin]);
        }
        delay(stepperSpeed);
      }
      stepsum++;
    }
  }
  if (steps < 0) {
    steps = (-1) * steps;
    for (int i = 0; i < steps; i++) {
      for (int mask = 0; mask < 8; mask++) {
        for (int pin = 0; pin < 4; pin++) {
          digitalWrite(R_stepper_pins[pin], reverseSpin[mask][pin]);
        }
        delay(stepperSpeed);
      }
      stepsum--;
    }
  }
  Serial.println(stepsum);
}
int step(float distance) { //计算走指定距离需要多少步
  int steps = distance * steps_rev / (wheel_dia * 3.1412); //24.61
  /*
    Serial.print(distance);
    Serial.print(" ");
    Serial.print(steps_rev);
    Serial.print(" ");
    Serial.print(wheel_dia);
    Serial.print(" ");
    Serial.println(steps);
    delay(1000);*/
  return steps; //1.074
}
void slaveForwardSteps(int steps) {
  String temp = "a";
  temp += steps;
  Serial.println(temp);
}
void forwardSteps(int steps)
{
  slaveForwardSteps(steps);
  for (int step = 0; step < steps; step++) {
    for (int mask = 0; mask < 8; mask++) {
      for (int pin = 0; pin < 4; pin++) {
        //digitalWrite(L_stepper_pins[pin], reverseSpin[mask][pin]);
        digitalWrite(R_stepper_pins[pin], forwardSpin[mask][pin]);
      }
      delay(stepperSpeed);
    }
  }
}
void forwardDistance(float distance) {
  int steps = step(distance);
  forwardSteps(steps);
}
void bluerun()
{
  char a;
  lcd.clear();
  lcd.print("Running");
  Serial.print("1");
  for (int step = 0; ; step++) {
    for (int mask = 0; mask < 8; mask++) {
      for (int pin = 0; pin < 4; pin++) {
        //digitalWrite(L_stepper_pins[pin], reverseSpin[mask][pin]);
        digitalWrite(R_stepper_pins[pin], forwardSpin[mask][pin]);
      }
      delay(stepperSpeed);
    }
    if (BT.available()) {
      a = BT.read();
    }
    if (a == '0')
    {
      times = 0;
      Serial.print("0");
      return;
    }
  }
}
void bluerun1()
{
  char a;
  //lcd.clear();
  //lcd.print("Running");
  Serial.print("1");
  for (int step = 0; ; step++) {
    for (int mask = 0; mask < 8; mask++) {
      for (int pin = 0; pin < 4; pin++) {
        //digitalWrite(L_stepper_pins[pin], reverseSpin[mask][pin]);
        digitalWrite(R_stepper_pins[pin], forwardSpin[mask][pin]);
      }
      delay(stepperSpeed);
    }
    if (BT.available()) {
      a = BT.read();
    }
    if (a == '0')
    {
      times = 0;
      Serial.print("0");
      return;
    }
  }
}
void slaveBackwardSteps(int steps) {
  String temp = "b";
  temp += steps;
  Serial.println(temp);
}
void backwardSteps(int steps)
{
  slaveBackwardSteps(steps);
  for (int step = 0; step < steps; step++) {
    for (int mask = 0; mask < 8; mask++) {
      for (int pin = 0; pin < 4; pin++) {
        //digitalWrite(L_stepper_pins[pin], forwardSpin[mask][pin]);
        digitalWrite(R_stepper_pins[pin], reverseSpin[mask][pin]);
      }
      delay(stepperSpeed);
    }
  }
}
void backwardDistance(float distance) {
  int steps = step(distance);
  backwardSteps(steps);
}
void blueback() {
  char a;
  lcd.clear();
  lcd.print("Backing");
  Serial.print("2");
  while (1) {
    for (int step = 0; ; step++) {
      for (int mask = 0; mask < 8; mask++) {
        for (int pin = 0; pin < 4; pin++) {
          //digitalWrite(L_stepper_pins[pin], forwardSpin[mask][pin]);
          digitalWrite(R_stepper_pins[pin], reverseSpin[mask][pin]);
        }
        delay(stepperSpeed);
      }
      if (BT.available()) {
        a = BT.read();
      }
      if (a == '0')
      {
        Serial.print("0");
        times = 0;
        return;
      }
    }
  }
}
void right(float degrees) { //右旋指定度数
  float rotation = degrees / 360.0;
  float distance = wheel_base * 3.1412 * rotation;
  int steps = step(distance);
  slaveForwardSteps(steps);
  for (int step = 0; step < steps; step++) {
    for (int mask = 0; mask < 8; mask++) {
      for (int pin = 0; pin < 4; pin++) {
        digitalWrite(R_stepper_pins[pin], reverseSpin[mask][pin]);
        //digitalWrite(L_stepper_pins[pin], reverseSpin[mask][pin]);
      }
      delay(stepperSpeed);
    }
  }
}
void blueRight() {
  char a;
  Serial.print("4");
  lcd.clear();
  lcd.print("Turning right");
  while (1) {
    //Serial.println("going right");
    for (int step = 0; ; step++) {
      for (int mask = 0; mask < 8; mask++) {
        for (int pin = 0; pin < 4; pin++) {
          //digitalWrite(L_stepper_pins[pin], reverseSpin[mask][pin]);
          digitalWrite(R_stepper_pins[pin], reverseSpin[mask][pin]);
        }
        delay(stepperSpeed);

      }
      if (BT.available()) {
        a = BT.read();
      }
      if (a == '0')
      {
        times = 0;
        Serial.print("0");
        return;
      }
    }
  }
}
void left(float degrees) { //左旋指定度数
  lcd.clear();
  lcd.print("Please give comm");
  float rotation = degrees / 360.0;
  float distance = wheel_base * 3.1412 * rotation;
  int steps = step(distance);
  slaveBackwardSteps(steps);
  for (int step = 0; step < steps; step++) {
    for (int mask = 0; mask < 8; mask++) {
      for (int pin = 0; pin < 4; pin++) {
        digitalWrite(R_stepper_pins[pin], forwardSpin[mask][pin]);
        //digitalWrite(L_stepper_pins[pin], forwardSpin[mask][pin]);
      }
      delay(stepperSpeed);
    }
  }
}
void blueLeft()
{
  char a;
  Serial.print("3");
  lcd.clear();
  lcd.print("Turning left");
  while (1) {
    for (int step = 0; ; step++) {
      for (int mask = 0; mask < 8; mask++) {
        for (int pin = 0; pin < 4; pin++) {
          //digitalWrite(L_stepper_pins[pin], forwardSpin[mask][pin]);
          digitalWrite(R_stepper_pins[pin], forwardSpin[mask][pin]);
        }
        delay(stepperSpeed);
      }
      if (BT.available()) {
        a = BT.read();
      }
      if (a == '0')
      {
        times = 0;
        Serial.print("0");
        return;
      }
    }
  }
}
void done() { // unlock stepper to save battery
  for (int mask = 0; mask < 8; mask++) {
    for (int pin = 0; pin < 4; pin++) {
      digitalWrite(R_stepper_pins[pin], LOW);
      digitalWrite(L_stepper_pins[pin], LOW);
    }
    delay(delay_time);
  }
}

//画图代码
String getBTInput()
{
  String inString = "";
  while (1) {
    while (BT.available() > 0) {
      char inChar = BT.read();
      //only receive numbers
      //if (isDigit(inChar)) {
      // convert the incoming byte to a char
      // and add it to the string:
      inString += inChar;
      //}
      // if you get a newline, print the string,
      // then the string's value:
      if (inChar == '\n') {
        //Serial.print("Value:");
        //Serial.println(inString.toInt());
        //Serial.print("String: ");
        //Serial.print(inString);
        BT.print("SideLength is: ");
        BT.println(inString);
        return inString;
        // clear the string for new input:
        inString = "";
      }
      delay(5);
    }
  }
}
void rectangle() //从起点向右画
{
  lcd.clear();
  lcd.print("Drawing rectangl");
  lcd.setCursor(0, 1);
  lcd.print("e");
  String input;
  delay(10);
  float sideLength = 100;
  input = getBTInput();
  //Serial.println("input is" + input);
  sideLength = input.toInt();
  //Serial.print("sideLength is: ");
  //Serial.println(sideLength);
  for (int i = 0; i < 3; i++)
  {
    pendown();
    delay(200);
    forwardDistance(sideLength);
    delay(200);
    penup();
    delay(200);
    backwardDistance(13);
    delay(200);
    right(87);
    delay(200);
    //backwardDistance(5); //7
    //delay(200);
  }
  pendown();
  delay(200);
  forwardDistance(sideLength);
  delay(200);
  penup();
  times = 0;
}

void triangle()
{
  lcd.clear();
  lcd.print("Drawing triangle");
  String input;
  delay(10);
  float sideLength = 100;
  input = getBTInput();
  sideLength = input.toInt();
  for (int i = 0; i < 2; i++)
  {
    pendown();
    delay(200);
    forwardDistance(sideLength);
    delay(200);
    penup();
    delay(200);
    backwardDistance(18);
    delay(200);
    right(117.5); //120
    delay(200);
    backwardDistance(5);
    delay(200);
  }
  pendown();
  delay(200);
  forwardDistance(sideLength);
  delay(200);
  penup();
  times = 0;
}
void circleComplex()
{
  delay(10);
  float diameter = getBTInput().toFloat();
  if ( diameter <= wheel_base) {
    BT.println("illegal diameter");
    lcd.clear();
    lcd.print("Illegal diameter");
    return;
  }else{
    lcd.clear();
    lcd.print("Drawing circle");
  }
  float k = (1 + (diameter / 2) / wheel_base) / ((diameter / 2) / wheel_base - 1) * 0.5;
  float leftstepperSpeed = 2;
  float rightstepperSpeed = leftstepperSpeed * k;
  setMasterSpeed(rightstepperSpeed);
  delay(50);
  setSlaveSpeed(leftstepperSpeed);
  delay(300);
  pendown();
  delay(300);
  bluerun1();
  delay(200);
  penup();
  delay(200);
  setMasterSpeed(2);
  delay(50);
  setSlaveSpeed(2);
  times = 0;
}
void circle()
{
  //  lcd.clear();
  //  lcd.print("Painting circle");
  //  float distance = wheel_base * PI;
  //  int steps = step(distance);
  //rightWheelSpin(steps);
  pendown();
  rightWheelSpin(2000);
  penup();
}
void star()
{
  lcd.clear();
  lcd.print("Drawing star");
  String input;
  delay(10);
  float sideLength = 100;
  input = getBTInput();
  sideLength = input.toInt();
  for (int i = 0; i < 4; i++)
  {
    pendown();
    delay(200);
    forwardDistance(sideLength);
    delay(200);
    penup();
    delay(200);
    backwardDistance(21.5);
    delay(200);
    right(140);
    delay(200);
    backwardDistance(7.5);
    //delay(200);
  }
  pendown();
  delay(200);
  forwardDistance(sideLength);
  delay(200);
  penup();
  times = 0;
}
void setMasterSpeed(float Speed) {
  stepperSpeed = Speed;
}
void setSlaveSpeed(float Speed) {
  String temp = "g";
  temp += Speed;
  BT.print(temp);
  Serial.println(temp);
}
void loop()
{
  if (times == 0)
  {
    lcd.clear();
    lcd.print("Please give comm");
    //lcd.scrollDisplayLeft();
    lcd.setCursor(0, 1);
    lcd.print("ands");
    times++;
  }
  if (BT.available()) {
    val = BT.read();
    Serial.print(val);
  }
  switch (val)
  {
    case '1' : bluerun(); break;
    case '2' : blueback(); break;
    case '3' : blueLeft(); break;
    case '4' : blueRight(); break;
    case '7' : changeServoDegree(PEN_DOWN); break; //落笔
    case '8' : changeServoDegree(PEN_UP); break; //抬笔
    case 'r' : rectangle(); break;
    //case 'c' : circle(); break;
    case 's' : star(); break;
    case 'c' : circleComplex(); break;
    case 't' : triangle(); break;
    case '-' : test(); break;
  }
  val = '~';
  //forwardSteps(512);
}
void test() {
  setSlaveSpeed(3);
}

