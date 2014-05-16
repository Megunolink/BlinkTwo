/* ****************************************************************************
*  Blink 2.0
*  Adjustable blink settings using MegunoLink's Interface Panel
*  ****************************************************************************/

// Include Libraries
#include "MegunoLink.h"
#include "CommandHandler.h"
#include "Filter.h"

// LED Settings
const int LEDPin = 13;
uint32_t LastBlinkTime = 0;   // Time that we last turn on the LED [milliseconds]
uint32_t BlinkInterval = 100; // Time between blinking LED [milliseconds]
uint32_t BlinkDuration = 50;  // Time LED remains on [milliseconds]

// Plotting Settings
const int LightSensorPin = 0;
uint32_t LastPlotTime = 0;   // Time that we last turn on the LED [milliseconds]
TimePlot LightSensorPlot;
ExponentialFilter<float> LightLevel(0.01, 0);

// Serial Command Handling
CommandHandler<> CmdHandler;


/* 
 Command handlers
 The blink parameters can be changed by sending commands over the serial port. The
 Parser receives and dispatches commands to these functions. 
 */
void Cmd_SetInterval(CommandParameter &Parameter)
{
  BlinkInterval = Parameter.NextParameterAsInteger();
}

void Cmd_SetDuration(CommandParameter &Parameter)
{
  BlinkDuration = Parameter.NextParameterAsInteger();
}

void Cmd_ListSettings(CommandParameter &Parameter)
{
  Serial.println("---------");
  Serial.print("Interval = ");
  Serial.println(BlinkInterval);

  Serial.print("Duration = ");
  Serial.println(BlinkDuration);
}

void Cmd_Unknown()
{
  Serial.println("I don't understand!");
}


// Main Setup
void setup()
{
  Serial.begin(9600);
  Serial.println("Blink 2.0");
  Serial.println("---------");
  
  pinMode(LEDPin, OUTPUT);
  pinMode(LightSensorPin,INPUT_PULLUP);
  
  CmdHandler.AddCommand(F("SetInterval"), Cmd_SetInterval);
  CmdHandler.AddCommand(F("SetDuration"), Cmd_SetDuration);
  CmdHandler.AddCommand(F("ListSettings"), Cmd_ListSettings);
}

//Main Loop
void loop()
{
  // Process Serial Commands
  CmdHandler.Process();

  // Blink the LED. 
  uint32_t Now = millis();
  if ((Now - LastBlinkTime) > BlinkInterval)
  {
    LastBlinkTime = Now;
  }
  digitalWrite(LEDPin, Now - LastBlinkTime < BlinkDuration);
  
  // Plot light intensity data every 1000mS
  LightLevel.Filter(analogRead(LightSensorPin));
  
  if ((Now - LastPlotTime) > 1000)
  {
    LastPlotTime = Now;
    LightSensorPlot.SendData("Light Signal", LightLevel.Current());
  }
}


