/*
 * NMEAParser library.
 * 
 * Parsing example.
 * 
 * 2 handlers are added and 2 sentences are parsed
 */
#include <NMEAParser.h>

/* A parser is declared with 2 handlers at most */
NMEAParser<2> parser;

const char firstSentence[] = "$DBAXK*54\r\n";
const char secondSentence[] = "$EJCHQ,03,O,UUEIE,S,953.11,S,4.172,ASBUX,J*54\r\n";

void errorHandler()
{
  Serial.print("*** Error : ");
  Serial.println(parser.error()); 
}

void firstHandler()
{
  Serial.print("Got DBAXK with ");
  Serial.print(parser.argCount());
  Serial.println(" arguments");
}

void secondHandler()
{
  Serial.print("Got $EJCHQ with ");
  Serial.print(parser.argCount());
  Serial.println(" arguments");
  int arg0;
  char arg1;
  char arg2[10];
  char arg3;
  float arg4;
  char arg5;
  float arg6;
  String arg7;
  char arg8;
  if (parser.getArg(0,arg0)) Serial.println(arg0);
  if (parser.getArg(1,arg1)) Serial.println(arg1);
  if (parser.getArg(2,arg2)) Serial.println(arg2);
  if (parser.getArg(3,arg3)) Serial.println(arg3);
  if (parser.getArg(4,arg4)) Serial.println(arg4);
  if (parser.getArg(5,arg5)) Serial.println(arg5);
  if (parser.getArg(6,arg6)) Serial.println(arg6);
  if (parser.getArg(7,arg7)) Serial.println(arg7);
  if (parser.getArg(8,arg8)) Serial.println(arg8);
}

void setup()
{
  Serial.begin(115200);

  parser.setErrorHandler(errorHandler);
  parser.addHandler("DBAXK", firstHandler);
  parser.addHandler("EJCHQ", secondHandler);

  Serial.print("Parsing : ");
  Serial.print(firstSentence);
  for (uint8_t i = 0; i < strlen(firstSentence); i++) {
    parser << firstSentence[i];
  }

  Serial.print("Parsing : ");
  Serial.print(secondSentence);
  for (uint8_t i = 0; i < strlen(secondSentence); i++) {
    parser << secondSentence[i];
  }
}

void loop()
{
}
