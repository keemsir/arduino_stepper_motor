
//// include library ////
// scale: millisecond(ms)
// ex) z500 500 \n x400 367 -> time(x): 2641, time(y): 2641

// z750 1000 \n x750 1000 \n c500 800 \n v500 800 (acc: 2000, 1200)
// 1.5cm, 1.5cm, 1cm, 1.5cm (xyzZ)

#include <AccelStepper.h>
//#include <MultiStepper.h>

// -
#define CW_x    22
#define CCW_x   24
// +
#define CW_y    30
#define CCW_y   32
// -
#define CW_z    38
#define CCW_z   40
// +
#define CW_Z    46
#define CCW_Z   48

// encoder setting (NC: GND, NO: 5V, COM: pwm)
#define Encoder_x1      11


/// input PWM setting
///
//AccelStepper 
//AccelStepper stepper_x(AccelStepper::DRIVER,2,3);
//AccelStepper stepper_y(AccelStepper::DRIVER,4,5);
//AccelStepper stepper_z(AccelStepper::DRIVER,10,11);
//AccelStepper stepper_Z(AccelStepper::DRIVER,12,13);

//AccelStepper FULL4WIRE mode
AccelStepper stepper_x(AccelStepper::FULL4WIRE,2,4,3,5);
AccelStepper stepper_y(AccelStepper::FULL4WIRE,6,8,7,9);
AccelStepper stepper_z(AccelStepper::FULL4WIRE,6,8,7,9);
AccelStepper stepper_Z(AccelStepper::FULL4WIRE,6,8,7,9);

//AccelStepper DRIVER mode (CLK 1)
//AccelStepper stepper_x(AccelStepper::DRIVER, CW_x, CCW_x); // CW+: PLS(pulse), CCW+: DIR
//AccelStepper stepper_y(AccelStepper::DRIVER, CW_y, CCW_y); // CW+: PLS(pulse), CCW+: DIR
//AccelStepper stepper_z(AccelStepper::DRIVER, CW_z, CCW_z); // CW+: PLS(pulse), CCW+: DIR
//AccelStepper stepper_Z(AccelStepper::DRIVER, CW_Z, CCW_Z); // CW+: PLS(pulse), CCW+: DIR


//MultiStepper steppers;


// parameter setting
long receivedDistance_x = 0;
long receivedDistance_y = 0;
long receivedDistance_z = 0;
long receivedDistance_Z = 0;

long receivedDelay_x = 0;
long receivedDelay_y = 0;
long receivedDelay_z = 0;
long receivedDelay_Z = 0;

//long receivedAcceleration_x = 0;
//long receivedAcceleration_y = 0;
//long receivedAcceleration_z = 0;
//long receivedAcceleration_Z = 0;

char receivedCommand;
char movedAxis = '0';

unsigned long time_x, time_x1, time_y, time_y1, time_z, time_z1, time_Z, time_Z1;

//temp
volatile int stepPosition = 0;
bool repeat_1 = false;


//bool newData, runallowed = false; // booleans for new data from serial, and runallowed flag
bool newData, runallowed_x, runallowed_y, runallowed_z, runallowed_Z = false; // booleans for new data from serial, and runallowed flag
bool RepeatOperation_x, RepeatOperation_y, RepeatOperation_z, RepeatOperation_Z = false;
bool Homing_x, Homing_y, Homing_z, Homing_Z = false; // 


// Define x,y,z,Z axis motor objects


void setup()
{
  Serial.begin(115200); // 115200
  Serial.println("-----Starting Accelstepper Motor-----");
  Serial.println("input axis: setMaxSpeed, Move");

  pinMode(Encoder_x1,INPUT);


  // 1 revolution Motor x CW
//  stepper_x.setMaxSpeed(2000.0); // SPEED = Steps / second
  stepper_x.setAcceleration(800.0); // ACCELERATION = Steps / (second)^2
//  stepper_x.setSpeed(1000);
//  stepper_x.moveTo(128);
  stepper_x.disableOutputs();

//  repeat_1 = false;

  // 1 revolution Motor y CW
//  stepper_y.setMaxSpeed(2000.0); // SPEED = Steps / second
  stepper_y.setAcceleration(800.0); // ACCELERATION = Steps / (second)^2
//  stepper_y.setSpeed(1000);
  stepper_y.moveTo(128);
  stepper_y.disableOutputs();

  // 1 revolution Motor z CW
//  stepper_z.setMaxSpeed(2000.0); // SPEED = Steps / second
  stepper_z.setAcceleration(800.0); // ACCELERATION = Steps / (second)^2
//  stepper_z.setSpeed(1000);
  stepper_z.moveTo(128);
  stepper_z.disableOutputs();

  // 1 revolution Motor Z CW
//  stepper_Z.setMaxSpeed(2000.0); // SPEED = Steps / second
  stepper_Z.setAcceleration(800.0); // ACCELERATION = Steps / (second)^2
//  stepper_Z.setSpeed(1000);
  stepper_Z.moveTo(128);
  stepper_Z.disableOutputs();

}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


void loop()
{
// Checking the serial
  checkSerial();

// Runing the stepper motor (x,y,z,Z axis)
  continuousRun_x();
  continuousRun_y();
  continuousRun_z();
  continuousRun_Z();

// Rotary encoder
//  if (digitalRead(Encoder_x1) == HIGH)
//  {
//    Serial.println("NC");
//  }
//
//  if (digitalRead(NO))
//  {
//    Serial.println(NO);
//  }
//  delay(1000);

  GoHome();
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


void checkSerial()
{
  //switch-case would also work, and maybe more elegant

  if (Serial.available() > 0)
  {
    receivedCommand = Serial.read();
    newData = true;
  }

  if (newData == true)
  {
    // motor 1 moving
    if (receivedCommand == 'z') //this is the measure part
    {
      // example a 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed
      runallowed_x = true;
      RepeatOperation_x = true;

      receivedDistance_x = Serial.parseFloat();
      receivedDelay_x = Serial.parseFloat();
      
      Serial.print("Distance: ");
      Serial.print(receivedDistance_x);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_x);
      stepper_x.setCurrentPosition(0);
      stepper_x.move(-1 * receivedDistance_x);
      stepper_x.setMaxSpeed(receivedDelay_x);
    }
    
    // START - verse
    if (receivedCommand == 'a')
    {
      // example c 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed; will rotate in the other direction
      runallowed_x = true;
      RepeatOperation_x = false;

      receivedDistance_x = Serial.parseFloat();
      receivedDelay_x = 500;

      Serial.print("Distance: ");
      Serial.print(receivedDistance_x);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_x);
      stepper_x.setCurrentPosition(0);
      stepper_x.move(-1 * receivedDistance_x);
      stepper_x.setMaxSpeed(receivedDelay_x);
    }

    // START - reverse
    if (receivedCommand == 'g')
    {
      // example c 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed; will rotate in the other direction
      runallowed_x = true;
      RepeatOperation_x = false;

      receivedDistance_x = Serial.parseFloat();
      receivedDelay_x = 500;

      Serial.print("Distance: ");
      Serial.print(receivedDistance_x);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_x);
      stepper_x.setCurrentPosition(0);
      stepper_x.move(receivedDistance_x);
      stepper_x.setMaxSpeed(receivedDelay_x);
    }

    // motor 2 moving
    if (receivedCommand == 'x') //this is the measure part
    {
      // example a 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed
      runallowed_y = true;
      RepeatOperation_y = true;

      receivedDistance_y = Serial.parseFloat();
      receivedDelay_y = Serial.parseFloat();

      Serial.print("Distance: ");
      Serial.print(receivedDistance_y);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_y);
      stepper_y.setCurrentPosition(0);
      stepper_y.move(receivedDistance_y);
      stepper_y.setMaxSpeed(receivedDelay_y);
    }

    // START - reverse
    if (receivedCommand == 's')
    {
      // example c 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed; will rotate in the other direction
      runallowed_y = true;
      RepeatOperation_y = false;

      receivedDistance_y = Serial.parseFloat();
      receivedDelay_y = 500;

      Serial.print("Distance: ");
      Serial.print(receivedDistance_y);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_y);
      stepper_y.setCurrentPosition(0);
      stepper_y.move(receivedDistance_y);
      stepper_y.setMaxSpeed(receivedDelay_y);
    }

    // START - reverse
    if (receivedCommand == 'h')
    {
      // example c 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed; will rotate in the other direction
      runallowed_y = true;
      RepeatOperation_y = false;

      receivedDistance_y = Serial.parseFloat();
      receivedDelay_y = 500;

      Serial.print("Distance: ");
      Serial.print(receivedDistance_y);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_y);
      stepper_y.setCurrentPosition(0);
      stepper_y.move(-1 * receivedDistance_y);
      stepper_y.setMaxSpeed(receivedDelay_y);
    }
    

    // motor 3 moving
    if (receivedCommand == 'c') //this is the measure part
    {
      // example a 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed
      runallowed_z = true;
      RepeatOperation_z = true;

      receivedDistance_z = Serial.parseFloat();
      receivedDelay_z = Serial.parseFloat();

      Serial.print("Distance: ");
      Serial.print(receivedDistance_z);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_z);
      stepper_z.setCurrentPosition(0);
      stepper_z.move(-1 * receivedDistance_z);
      stepper_z.setMaxSpeed(receivedDelay_z);
    }

    // START - reverse
    if (receivedCommand == 'd')
    {
      // example c 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed; will rotate in the other direction
      runallowed_z = true;
      RepeatOperation_z = false;

      receivedDistance_z = Serial.parseFloat();
      receivedDelay_z = 500;

      Serial.print("Distance: ");
      Serial.print(receivedDistance_z);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_z);
      stepper_z.setCurrentPosition(0);
      stepper_z.move(-1 * receivedDistance_z);
      stepper_z.setMaxSpeed(receivedDelay_z);
    }

    // START - reverse
    if (receivedCommand == 'j')
    {
      // example c 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed; will rotate in the other direction
      runallowed_z = true;
      RepeatOperation_z = false;

      receivedDistance_z = Serial.parseFloat();
      receivedDelay_z = 500;

      Serial.print("Distance: ");
      Serial.print(receivedDistance_z);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_z);
      stepper_z.setCurrentPosition(0);
      stepper_z.move(receivedDistance_z);
      stepper_z.setMaxSpeed(receivedDelay_z);
    }


    // Z-axis motor moving
    if (receivedCommand == 'v') //this is the measure part
    {
      // example a 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed
      runallowed_Z = true;
      RepeatOperation_Z = true;


      receivedDistance_Z = Serial.parseFloat();
      receivedDelay_Z = Serial.parseFloat();

      Serial.print("Distance: ");
      Serial.print(receivedDistance_Z);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_Z);
      stepper_Z.setCurrentPosition(0);
      stepper_Z.move(receivedDistance_Z);
      stepper_Z.setMaxSpeed(receivedDelay_Z);
    }

    // START - reverse
    if (receivedCommand == 'f')
    {
      // example c 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed; will rotate in the other direction
      runallowed_Z = true;
      RepeatOperation_Z = false;

      receivedDistance_Z = Serial.parseFloat();
      receivedDelay_Z = 500;

      Serial.print("Distance: ");
      Serial.print(receivedDistance_Z);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_Z);
      stepper_Z.setCurrentPosition(0);
      stepper_Z.move(receivedDistance_Z);
      stepper_Z.setMaxSpeed(receivedDelay_Z);
    }

    // START - reverse
    if (receivedCommand == 'k')
    {
      // example c 2000 500 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed; will rotate in the other direction
      runallowed_Z = true;
      RepeatOperation_Z = false;

      receivedDistance_Z = Serial.parseFloat();
      receivedDelay_Z = 500;

      Serial.print("Distance: ");
      Serial.print(receivedDistance_Z);
      Serial.print(", Delay: ");
      Serial.println(receivedDelay_Z);
      stepper_Z.setCurrentPosition(0);
      stepper_Z.move(-1 * receivedDistance_Z);
      stepper_Z.setMaxSpeed(receivedDelay_Z);
    }


    // STOP the stepper motor
    if (receivedCommand == 'q') //"stop-x"
    {
      runallowed_x = false;
      RepeatOperation_x = false;

//      stepper_x.setCurrentPosition(0);
      Serial.println("STOP ");
      stepper_x.stop();
      stepper_x.disableOutputs();
    }

    // STOP the stepper motor
    if (receivedCommand == 'w') //"stop-y"
    {
      runallowed_y = false;
      RepeatOperation_y = false;

//      stepper_y.setCurrentPosition(0);
      Serial.println("STOP ");
      stepper_y.stop();
      stepper_y.disableOutputs();
    }

    // STOP the stepper motor
    if (receivedCommand == 'e') //"stop-z"
    {
      runallowed_z = false;
      RepeatOperation_z = false;

//      stepper_z.setCurrentPosition(0);
      Serial.println("STOP ");
      stepper_z.stop();
      stepper_z.disableOutputs();
    }

    // STOP the stepper motor
    if (receivedCommand == 'r') //"stop-Z"
    {
      runallowed_Z = false;
      RepeatOperation_Z = false;

//      stepper_Z.setCurrentPosition(0);
      Serial.println("STOP ");
      stepper_Z.stop();
      stepper_Z.disableOutputs();
    }


    // Homing the stepper motor x-axis
    if (receivedCommand == 'u')
    {
      Homing_x = true;
      Serial.println("Homing the x-axis motor");
    }

    // Homing the stepper motor y-axis
    if (receivedCommand == 'i')
    {
      Homing_y = true;
      Serial.println("Homing the y-axis motor");
    }

    // Homing the stepper motor z-axis
    if (receivedCommand == 'o')
    {
      Homing_z = true;
      Serial.println("Homing the z-axis motor");
    }

    // Homing the stepper motor Z-axis
    if (receivedCommand == 'p')
    {
      Homing_Z = true;
      Serial.println("Homing the Z-axis motor");
    }

  }
  // after we went through the above tasks, newData becomes false again, so we are ready to receive new commands again.
  newData = false;
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


void continuousRun_x()
{
  if (runallowed_x == true && RepeatOperation_x == true)
  {
    if (stepper_x.distanceToGo() == 0)
    {
      Serial.print("x-axis motor position: ");
      Serial.println(stepper_x.currentPosition());
      stepper_x.moveTo(-stepper_x.currentPosition());

      if (stepper_x.currentPosition() == receivedDistance_x)
      {
        time_x = millis();
        Serial.print("time(x): ");
        Serial.println(time_x-time_x1);
      }
      else if (stepper_x.currentPosition() == -receivedDistance_x)
      {
        time_x1 = millis();
        Serial.print("time(x): ");
        Serial.println(time_x1-time_x);
      }
    }
    stepper_x.enableOutputs();
    stepper_x.run();
  }
  
  else if (runallowed_x == true && RepeatOperation_x == false)
  {
    if (stepper_x.distanceToGo() != 0)
    {
      stepper_x.enableOutputs();
      stepper_x.run();
    }
    else
    {
      stepper_x.stop();
      stepper_x.disableOutputs();
    }
  }
  
  else // program enters this part if the runallowed is FALSE, we do not do anything
  {
    return;
  }
}



void continuousRun_y()
{

  if (runallowed_y == true && RepeatOperation_y == true)
  {
    if (stepper_y.distanceToGo() == 0)
    {
      Serial.print("y-axis motor position: ");
      Serial.println(stepper_y.currentPosition());
      stepper_y.moveTo(-stepper_y.currentPosition());

      if (stepper_y.currentPosition() == receivedDistance_y)
      {
        time_y = millis();
        Serial.print("time(y): ");
        Serial.println(time_y-time_y1);
      }
      else if (stepper_y.currentPosition() == -receivedDistance_y)
      {
        time_y1 = millis();
        Serial.print("time(y): ");
        Serial.println(time_y1-time_y);
      }
    }
    stepper_y.enableOutputs();
    stepper_y.run();
  }

  else if (runallowed_y == true && RepeatOperation_y == false)
  {
    if (stepper_y.distanceToGo() != 0)
    {
      stepper_y.enableOutputs();
      stepper_y.run();
    }
    else
    {
      stepper_y.stop();
      stepper_y.disableOutputs();
    }
  }
  
  else // program enters this part if the runallowed is FALSE, we do not do anything
  {
    return;
  }
}



void continuousRun_z()
{

  if (runallowed_z == true && RepeatOperation_z == true)
  {
    if (stepper_z.distanceToGo() == 0)
    {
      Serial.print("z-axis motor position: ");
      Serial.println(stepper_z.currentPosition());
      stepper_z.moveTo(-stepper_z.currentPosition());

      if (stepper_z.currentPosition() == receivedDistance_z)
      {
        time_z = millis();
        Serial.print("time(z): ");
        Serial.println(time_z-time_z1);
      }
      else if (stepper_z.currentPosition() == -receivedDistance_z)
      {
        time_z1 = millis();
        Serial.print("time(z): ");
        Serial.println(time_z1-time_z);
      }
    }
    stepper_z.enableOutputs();
    stepper_z.run();
  }

  else if (runallowed_z == true && RepeatOperation_z == false)
  {
    if (stepper_z.distanceToGo() != 0)
    {
      stepper_z.enableOutputs();
      stepper_z.run();
    }
    else
    {
      stepper_z.stop();
      stepper_z.disableOutputs();
    }
  }
  
  else // program enters this part if the runallowed is FALSE, we do not do anything
  {
    return;
  }
}


void continuousRun_Z()
{

  if (runallowed_Z == true && RepeatOperation_Z == true)
  {
    if (stepper_Z.distanceToGo() == 0)
    {
      Serial.print("Z-axis motor position: ");
      Serial.println(stepper_Z.currentPosition());
      stepper_Z.moveTo(-stepper_Z.currentPosition());

      if (stepper_Z.currentPosition() == receivedDistance_Z)
      {
        time_Z = millis();
        Serial.print("time(Z): ");
        Serial.println(time_Z-time_Z1);
      }
      else if (stepper_Z.currentPosition() == -receivedDistance_Z)
      {
        time_Z1 = millis();
        Serial.print("time(Z): ");
        Serial.println(time_Z1-time_Z);
      }
    }
    stepper_Z.enableOutputs();
    stepper_Z.run();
  }

  else if (runallowed_Z == true && RepeatOperation_Z == false)
  {
    if (stepper_Z.distanceToGo() != 0)
    {
      stepper_Z.enableOutputs();
      stepper_Z.run();
    }
    else
    {
      stepper_Z.stop();
      stepper_Z.disableOutputs();
    }
  }
  
  else // program enters this part if the runallowed is FALSE, we do not do anything
  {
    return;
  }
}


void printVIEW()
{
  Serial.println();
}


void updateSelection()
{
  Serial.println();
}


void GoHome() // homing
{
  if (Homing_x == true)
  {
    if (stepper_x.currentPosition() == 0 && Homing_x == true)
    {
      Serial.println("Going at the x-axis home position.");
      stepper_x.disableOutputs();
      Homing_x = false;
    }
    else
    {
      stepper_x.enableOutputs();
      stepper_x.setMaxSpeed(400);
      stepper_x.moveTo(0);
      stepper_x.run();
    }
  }

  if (Homing_y == true)
  {
    if (stepper_y.currentPosition() == 0 && Homing_y == true)
    {
      Serial.println("Going at the y-axis home position.");
      stepper_y.disableOutputs();
      Homing_y = false;
    }
    else
    {
      stepper_y.enableOutputs();
      stepper_y.setMaxSpeed(400);
      stepper_y.moveTo(0);
      stepper_y.run();
    }
  }

  if (Homing_z == true)
  {
    if (stepper_z.currentPosition() == 0 && Homing_z == true)
    {
      Serial.println("Going at the z-axis home position.");
      stepper_z.disableOutputs();
      Homing_z = false;
    }
    else
    {
      stepper_z.enableOutputs();
      stepper_z.setMaxSpeed(400);
      stepper_z.moveTo(0);
      stepper_z.run();
    }
  }

  if (Homing_Z == true)
  {
    if (stepper_Z.currentPosition() == 0 && Homing_Z == true)
    {
      Serial.println("Going at the Z-axis home position.");
      stepper_Z.disableOutputs();
      Homing_Z = false;
    }
    else
    {
      stepper_Z.enableOutputs();
      stepper_Z.setMaxSpeed(400);
      stepper_Z.moveTo(0);
      stepper_Z.run();
    }
  }
}
