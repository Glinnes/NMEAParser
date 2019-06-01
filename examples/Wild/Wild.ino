/*
 * NMEAParser library.
 * 
 * A NMEA example to switch ON or OFF the built in LED but with the use
 * of wild char in the type
 * 
 * Sentences to send are :
 * 
 * $ARLE1*2B
 * 
 * to switch the LED on and
 * 
 * $ARLE0*2A
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
  Serial.print("Got ARLEx with ");
  Serial.print(parser.argCount());
  Serial.println(" arguments");
  
  char wantedLedState;
  if (parser.getType(4, wantedLedState)) { // get the 4th character ot the type
    if (wantedLedState == '0' || wantedLedState == '1') {
      digitalWrite(LED_BUILTIN, wantedLedState == '0' ? LOW : HIGH);
    }
    else {
      Serial.println("x should be 0 or 1");
    }
  }
}

void setup() {
  Serial.begin(115200);
  parser.setErrorHandler(errorHandler);
  parser.addHandler("ARLE-", ledHandler);
  parser.setDefaultHandler(unknownCommand);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    parser << Serial.read();
  }
}
