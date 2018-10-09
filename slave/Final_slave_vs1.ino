char val;
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
//定义小车属性
float wheel_dia = 66; //      # mm (increase = spiral out) 车轮直径
float wheel_base = 92.5; //,    # mm (increase = spiral in) 轮距
int steps_rev = 512; //,     # 512 for 64x gearbox, 128 for 16x gearbox
int delay_time = 6; //            # time between steps in ms
float stepperSpeed = 2;//电机转速,1ms一步
//脉冲总数，或者说步的总数
int stepsum = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  for (int pin = 0; pin < 4; pin++) {
    pinMode(L_stepper_pins[pin], OUTPUT);
    digitalWrite(L_stepper_pins[pin], LOW);
    //pinMode(R_stepper_pins[pin], OUTPUT);
    //digitalWrite(R_stepper_pins[pin], LOW);
  }
  pinMode(LED_BUILTIN, OUTPUT);
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
void forwardSteps()
{
  String temp = getMasterInput();
  int steps = temp.toInt();
  //Serial.println(steps);
  for (int step = 0; step < steps; step++) {
    for (int mask = 0; mask < 8; mask++) {
      for (int pin = 0; pin < 4; pin++) {
        digitalWrite(L_stepper_pins[pin], reverseSpin[mask][pin]);
        //digitalWrite(R_stepper_pins[pin], forwardSpin[mask][pin]);
      }
      delay(stepperSpeed);
    }
  }
}

void backwardSteps()
{
  String temp = getMasterInput();
  int steps = temp.toInt();
  for (int step = 0; step < steps; step++) {
    for (int mask = 0; mask < 8; mask++) {
      for (int pin = 0; pin < 4; pin++) {
        digitalWrite(L_stepper_pins[pin], forwardSpin[mask][pin]);
        //digitalWrite(R_stepper_pins[pin], reverseSpin[mask][pin]);
      }
      delay(stepperSpeed);
    }
  }
}
//void backwardDistance(float distance) {
//  int steps = step(distance);
//  backwardSteps(steps);
//}
void bluerun() {
  char a;
  while (1) {
    for (int mask = 0; mask < 8; mask++) {
      for (int pin = 0; pin < 4; pin++) {
        digitalWrite(L_stepper_pins[pin], reverseSpin[mask][pin]);
        //digitalWrite(R_stepper_pins[pin], forwardSpin[mask][pin]);
      }
      delay(stepperSpeed);
    }
    if (Serial.available()) {
      a = Serial.read();
    }
    if (a == '0')
    {
      return;
    }
  }
}
void blueback() {
  char a;
  while (1) {
    Serial.println("backing");
    for (int step = 0; ; step++) {
      for (int mask = 0; mask < 8; mask++) {
        for (int pin = 0; pin < 4; pin++) {
          digitalWrite(L_stepper_pins[pin], forwardSpin[mask][pin]);
          //digitalWrite(R_stepper_pins[pin], reverseSpin[mask][pin]);
        }
        delay(stepperSpeed);
      }
      if (Serial.available()) {
        a = Serial.read();
      }
      if (a == '0')
      {
        return;
      }
    }
  }
}
void blueRight() {
  char a;
  while (1) {
    //Serial.println("going right");
    for (int step = 0; ; step++) {
      for (int mask = 0; mask < 8; mask++) {
        for (int pin = 0; pin < 4; pin++) {
          digitalWrite(L_stepper_pins[pin], reverseSpin[mask][pin]);
          //digitalWrite(R_stepper_pins[pin], reverseSpin[mask][pin]);
        }
        delay(stepperSpeed);

      }
      if (Serial.available()) {
        a = Serial.read();
      }
      if (a == '0')
      {
        return;
      }
    }
  }
}
void blueLeft()
{
  char a;
  while (1) {
    //Serial.println("going right");
    for (int step = 0; ; step++) {
      for (int mask = 0; mask < 8; mask++) {
        for (int pin = 0; pin < 4; pin++) {
          digitalWrite(L_stepper_pins[pin], forwardSpin[mask][pin]);
          //digitalWrite(R_stepper_pins[pin], forwardSpin[mask][pin]);
        }
        delay(stepperSpeed);
      }
      if (Serial.available()) {
        a = Serial.read();
      }
      if (a == '0')
      {
        return;
      }
    }
  }
}
void getSlaveSpeed(){
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  String input = getMasterInput();
  float Speed = input.toFloat();
  stepperSpeed = Speed;
}
String getMasterInput()
{
  String inString = "";
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
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
        return inString;
        // clear the string for new input:
        inString = "";
      }
      delay(5);
    }
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    val = Serial.read();
    //Serial.print(val);
  }
  switch (val)
  {
    case '1' : bluerun(); break;
    case '2' : blueback(); break;
    case '3' : blueLeft(); break;
    case '4' : blueRight(); break;
    case 'g' : getSlaveSpeed(); break;
    case 'a' : forwardSteps(); break;
    case 'b' : backwardSteps(); break;
  }
  val = '~';
  //forwardSteps(512);
}
