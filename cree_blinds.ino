#include <Servo.h>
#include <avr/sleep.h>
/*
  Zigbee board from Cree Connected LED
   pin 1 ground 
   pin 2 3.3Vin
   pin 3 PWM Out
   pin 4 Digital Out

*/

/*
#define PIN_CREE_PWM   11 
#define PIN_CREE_ONOFF 21
*/
#define PIN_Servo 10
#define PIN_LED 13 
#define PIN_wakePWM 3
#define PIN_wakeOnOff 2 // pin used for waking up and connected to Cree_OnOff
                        // Arduino Pro Mini only allow Interrupt to wake up at pin 2 and 3

//******************************************************************************************
// Global Variables
//******************************************************************************************
byte lastCreeOnOffValue = LOW;
int pos = 0;
int state = 0;
int prevstate = 0;
int dirc = 0;
int count=0;
Servo myservo;

void setup() {
  //Serial.println("Set up");  
  Serial.begin(9600);
  pinMode(PIN_wakeOnOff, INPUT);  
  pinMode(PIN_wakePWM, INPUT);
  pinMode(PIN_Servo, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_wakeOnOff), wakeUpNow, CHANGE ); // use interrupt 0 (pin 2) to wake up and call wakeUpNow() PIN_wakeOnOff is connected to Cree Zigbee On/Off
}

void servo_move() {
  
  if ( dirc == 180) {
    myservo.attach(PIN_Servo);              // energize servo
    delay(50);
    for (pos = 0; pos <= 100; pos += 1) {   // goes from 0 degrees to 100 degrees in steps of 1 degree 
      myservo.write(pos);                  // tell servo to go to position in variable 'pos'
      delay(15);                           // waits 15ms between each degree to slow rotation speed
    }
    delay(50);
    myservo.detach();                      // movement finished so detach servo to conserve power
  }
  else if (dirc == 0) {
    myservo.attach(PIN_Servo);              // energize servo
    delay(50);
    for (pos = 100; pos >= 0; pos -= 1) {   // goes from 100 degrees to 0 degrees in steps of 1 degree
      myservo.write(pos);                  // tell servo to go to position in variable 'pos'
      delay(15);                           // waits 15ms between each degree to slow rotation speed
    }
    delay(50);
    myservo.detach();                      // movement finished so detach servo to conserve power
  }

  Serial.println("Returning to main loop");
  return;
}

void sleepNow()
{  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  Serial.println("Go Sleep");
  delay(100);  // Had to put some delay here to avoid some problem because it falls asleep to quick
  sleep_enable(); // enables the sleep bit in the mcucr register
  attachInterrupt(digitalPinToInterrupt(PIN_wakeOnOff),wakeUpNow, CHANGE); // use interrupt 0 (pin 2) and run function
  sleep_mode(); // here the device is actually put to sleep!!
  sleep_disable(); // first thing after waking from sleep:
  detachInterrupt(digitalPinToInterrupt(PIN_wakeOnOff)); // disables interrupt 0 on pin 2 so wakeUpNow code will not be executed during the normal Loop()
}

void wakeUpNow() // here the interrupt is handled after wakeup
{
  // execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  // we don't really need to execute any special functions here, since we
  // just want the thing to wake up
}

void loop() { 
  digitalWrite(PIN_LED, LOW);  // Turn the onboard LED Off
  
  Serial.println("Normal Loop"); 
  
  //Get the value of the Cree Zigbee board's On/Off digital ouput
  byte CreeOnOffValue = digitalRead(PIN_wakeOnOff);

  //Serial.println(lastCreeOnOffValue); 
  //Serial.println(CreeOnOffValue); 
  if(CreeOnOffValue != lastCreeOnOffValue)
  {
    Serial.print(F("Cree On/Off Value = "));
    Serial.println(CreeOnOffValue);       

    //Change the servo direction
    if (CreeOnOffValue == LOW)
    {
      dirc = 180;
    }
    else
    {
      dirc = 0;    
    }
    
    //Drive the servo    
    servo_move();
    lastCreeOnOffValue = CreeOnOffValue;          
  }

   delay(100);     // this delay is needed, the sleep function will provoke a Serial error otherwise!!       
   sleepNow();     // sleep function called here
}


