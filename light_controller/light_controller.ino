/* SparkFun TSL2561 library example sketch

This sketch shows how to use the SparkFunTSL2561
library to read the AMS/TAOS TSL2561
light sensor.

Product page: https://www.sparkfun.com/products/11824
Hook-up guide: https://learn.sparkfun.com/tutorials/getting-started-with-the-tsl2561-luminosity-sensor

Hardware connections:

3V3 to 3.3V
GND to GND

(WARNING: do not connect 3V3 to 5V
or the sensor will be damaged!)

You will also need to connect the I2C pins (SCL and SDA) to your Arduino.
The pins are different on different Arduinos:

                    SDA    SCL
Any Arduino         "SDA"  "SCL"
Uno, Redboard, Pro  A4     A5
Mega2560, Due       20     21
Leonardo            2      3

You do not need to connect the INT (interrupt) pin
for basic operation.

Operation:

Upload this sketch to your Arduino, and open the
Serial Monitor window to 9600 baud.

Have fun! -Your friends at SparkFun.

Our example code uses the "beerware" license.
You can do anything you like with this code.
No really, anything. If you find it useful,
buy me a beer someday.

V10 Mike Grusin, SparkFun Electronics 12/26/2013
Updated to Arduino 1.6.4 5/2015
*/

// Your sketch must #include this library, and the Wire library
// (Wire is a standard library included with Arduino):

#include <SparkFunTSL2561.h>
#include <Wire.h>

// Create an SFE_TSL2561 object, here called "light":

SFE_TSL2561 light;
int         ledlevel=255;
int         redpin=9;
int         bluepin=10;
int         colorsw=3;
int         redlux;
// Global variables:

boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds

void setup()
{
  // Initialize the Serial port:
  
  Serial.begin(9600);
  Serial.println("Kodachrome color printing block.");
  Serial.println("Copyright 2017 Kelly-Shane Fuller.");
  Serial.println("photo@piratelogy.com");
  Serial.println("");
  
  pinMode(colorsw,INPUT);
  
  light.begin();

  // Get factory ID from sensor:
  // (Just for fun, you don't need to do this to operate the sensor)

  unsigned char ID;
  Serial.println("Initializing light sensor...");
  if (light.getID(ID))
  {
    Serial.print("Got factory ID: 0X");
    Serial.print(ID,HEX);
    Serial.println(", should be 0X5X");
  }
  // Most library commands will return true if communications was successful,
  // and false if there was a problem. You can ignore this returned value,
  // or check whether a command worked correctly and retrieve an error code:
  else
  {
    byte error = light.getError();
    printError(error);
  }

  // The light sensor has a default integration time of 402ms,
  // and a default gain of low (1X).
  
  // If you would like to change either of these, you can
  // do so using the setTiming() command.
  
  // If gain = false (0), device is set to low gain (1X)
  // If gain = high (1), device is set to high gain (16X)

  gain = 0;

  // If time = 0, integration will be 13.7ms
  // If time = 1, integration will be 101ms
  // If time = 2, integration will be 402ms
  // If time = 3, use manual start / stop to perform your own integration

  unsigned char time = 0;

  // setTiming() will set the third parameter (ms) to the
  // requested integration time in ms (this will be useful later):
  
  Serial.println("Set timing...");
  light.setTiming(gain,time,ms);

  // To start taking measurements, power up the sensor:
  
  Serial.println("Powerup...");
  light.setPowerUp();
  // The sensor will now gather light during the integration time.
  // After the specified time, you can retrieve the result from the sensor.
  // Once a measurement occurs, another integration period will start.
}

void loop()
{
  // Wait between measurements before retrieving the result
  // (You can also configure the sensor to issue an interrupt
  // when measurements are complete)
  
  // This sketch uses the TSL2561's built-in integration timer.
  // You can also perform your own manual integration timing
  // by setting "time" to 3 (manual) in setTiming(),
  // then performing a manualStart() and a manualStop() as in the below
  // commented statements:
  
  // ms = 1000;
  // light.manualStart();
  delay(30);
  // light.manualStop();
   
  // There are two light sensors on the device, one for visible lightre
  // and one for infrared. Both sensors are needed for lux calculations.
  
  // Retrieve the data from the device:

  unsigned int data0, data1;
  
  if (light.getData(data0,data1))
  {
    // getData() returned true, communication was successful
    
    Serial.print("data0: ");
    Serial.print(data0);
    Serial.print(" data1: ");
    Serial.println(data1);
  
    // To calculate lux, pass all your settings and readings
    // to the getLux() function.
    
    // The getLux() function will return 1 if the calculation
    // was successful, or 0 if one or both of the sensors was
    // saturated (too much light). If this happens, you can
    // reduce the integration time and/or gain.
    // For more information see the hookup guide at: https://learn.sparkfun.com/tutorials/getting-started-with-the-tsl2561-luminosity-sensor
  
    double lux;    // Resulting lux value
    boolean good;  // True if neither sensor is saturated
    
    // Perform lux calculation:

    good = light.getLux(gain,ms,data0,data1,lux);
    
    //Calculate LED intensity based on light sensor readings. 
    if(digitalRead(colorsw)==HIGH)
    {
    if(lux > 3500) { ledlevel++;} //6830
    if(lux < 3000)  { ledlevel--;}
    if(ledlevel >= 255) { ledlevel =255;}
    if(ledlevel <=0) { ledlevel = 0; }
      analogWrite(redpin, ledlevel);
      analogWrite(bluepin, 255);
      Serial.print("RED ");
    }
    else
    {
      if(lux > 2732) { ledlevel++;} //4088
      if(lux < 2700)  { ledlevel--;}
      if(ledlevel >= 255) { ledlevel =255;}
      if(ledlevel <=0) { ledlevel = 0; }
      analogWrite(bluepin, ledlevel);      
      analogWrite(redpin,255);
      Serial.print("BLUE ");
    }
    Serial.print("LEDLevel:");
    Serial.println(ledlevel);
    Serial.print(" lux: ");
    Serial.print(lux);
    if (good) Serial.println(" (good)"); else Serial.println(" (BAD)");
  }
  else
  {
    // getData() returned false because of an I2C error, inform the user.

    byte error = light.getError();
    printError(error);
  }
}

void printError(byte error)
  // If there's an I2C error, this function will
  // print out an explanation.
{
  Serial.print("I2C error: ");
  Serial.print(error,DEC);
  Serial.print(", ");
  
  switch(error)
  {
    case 0:
      Serial.println("success");
      break;
    case 1:
      Serial.println("data too long for transmit buffer");
      break;
    case 2:
      Serial.println("received NACK on address (disconnected?)");
      break;
    case 3:
      Serial.println("received NACK on data");
      break;
    case 4:
      Serial.println("other error");
      break;
    default:
      Serial.println("unknown error");
  }
}

