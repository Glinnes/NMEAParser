/*
 * NMEAParser library.
 * 
 * A NMEA example to switch ON or OFF the built in LED
 * 
 * Sentences to send are :
 * 
 * $ARLED,1*43
 * 
 * to switch the LED on and
 * 
 * $ARLED,0*42
 * 
 * to switch the LED off
 * 
 * Set the serial monitor end of line to <cr><lf>
 */
#include <NMEAParser.h>

/* A parser with only one handler */
NMEAParser<1> parser;

void errorHandler()
{
  Serial.print("*** Error : ");
  Serial.println(parser.error()); 
}

void unknownCommand()
{
  Serial.print("*** Unkown command : ");
  char buf[6];
  parser.getType(buf);
  Serial.println(buf);
}

void ledHandler()
{
  Serial.print("Got ARLED with ");
  Serial.print(parser.argCount());
  Serial.println(" arguments");
  int ledState;
  if (parser.getArg(0,ledState)) {
    digitalWrite(LED_BUILTIN, ledState == 0 ? LOW : HIGH);
  }
}

void setup() {
  Serial.begin(115200);
  parser.setErrorHandler(errorHandler);
  parser.addHandler("ARLED", ledHandler);
  parser.setDefaultHandler(unknownCommand);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    parser << Serial.read();
  }
}
