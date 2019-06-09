# NMEAParser

An Arduino library to parse NMEA sentences.

NMEA is a communication standard in the marine equipment industry: GPS, anemometers,... The NMEAParser library allows you to analyze NMEA sentences and associate handlers to each of those that need to be recognized. The library provides the means to associate a handler to each identifier and also provides functions to retrieve the arguments of the sentence in different data formats: character, integer, float, character string.

## Changelog

* ```1.1``` : Added joker in type. Remove binaries from ```extra```
* ```1.0``` : Initial release

## Memory footprint

On an Arduino Uno, an instance of a NMEAParser requires 97 bytes with only one handler. 8 bytes per additional handler are required.

## Using NMEAParser

As usual, the library should be included at the beginning of the sketch.

```C++
#include <NMEAParser.h>
```

Then, you must instantiate a parser as follows:

```C++
NMEAParser<4> parser;
```

The ```4``` is the maximum number of handlers desired, ```parser``` is the name of the object that will allow the analysis of NMEA sentences. you can of course use the name you want.

In ```setup``` you configure your parser as you wish using the following functions.

### ```void addHandler(<type>, <handler>)```

where ```<type>``` is a character string and the type of sentence to recongnize, and ```<handler>``` the function to call when a sentence is recognize. ```<type>``` can be a string stored un RAM or a string stored in flash : ```F("ASTRI")```. If ```<type>``` has more than 5 characters, it is trucated.

For example, suppose you want to recognize the following sounder sentences which give the water depth below keel (DBK) and below surface (DBS):

```
$SDDBK,...
```

and

```
$SDDBS,...
```

You will install two handlers as follows (assuming your parsing object is named ```parser```):

```C++
parser.addHandler("SDDBK", handleDepthBelowKeel);
parser.addHanlder("SDDBS", handleDepthBelowSurface);
```

```handleDepthBelowKeel``` and ```handleDepthBelowSurface``` are functions that will be called when sentences are recognized.

With version 1.1, ```<type>``` may include hyphens. An hyphens matches any character. For instance if you want the handler to match all sentences coming from the sounder, you would write:

```C++
parser.addHandler("SD---", handleSounder);
```

```handleSounder``` function would be called for any sentence with the type beginning with SD.

### ```void setDefaultHandler(<handler>)```

When a sentence is succefully parsed but none of the handler correspond to it, ```<handler>``` is called. It is a function corresponding to a ```void f(void)``` prototype.  By default, no function is called.

### ```void setErrorHandler(<handler>)```

When a sentence is malformed : too long, wrong characters, etc, ```<handler>``` is called. It is a function corresponding to a ```void f(void)``` prototype.  By default, no function is called.

### ```void setHandleCRC(<doCRC>)```

Specifies whether the CRC is checked or not. By default, the CRC is checked. If you do not want CRC verification, pass ```false``` to ```setHandleCRC```.

---
In the handlers, you will get the arguments of the sentence, the sentence type or the error if any by using the following functions:

### ```bool getArg(<argnum>, <argval>)```

is used to get the arguments of the sentence. ```<argnum>``` is the number of the argument, starting at 0 for the argument that follows the identifier. ```<argval``` is a variable where the argument value will be stored if successful. ```getArg``` returns a boolean which is true if successfull, false if not.

Continuing with the example, [both sentences have the same 6 arguments](https://gpsd.gitlab.io/gpsd/NMEA.html#_dbk_depth_below_keel)  

* Argument 0 is a float number giving the depth in feet.
* Argument 1 is a ```f``` for feet.
* Argument 2 is a float number giving the depth in meters.
* Argument 3 is a ```M``` for Meters.
* Argument 4 is a float number giving the depth in fathoms.
* At last Argument  5 is a ```F``` for Fathoms.

Suppose you are interested by the depths in meters and you have two float variables to store theses data:

```C++
float depthBelowKeel;
float depthBelowSurface;
```

You would implement ```handleDepthBelowKeel``` and ```handleDepthBelowSurface``` as follow:

```C++
void handleDepthBelowKeel(void)
{
  float depth;
  if (parser.getArg(2, depth))
  {
    depthBelowKeel = depth;
  }
}

void handleDepthBelowSurface(void)
{
  float depth;
  if (parser.getArg(2, depth))
  {
    depthBelowSurface = depth;
  }
}
```

### ```bool getType(<type>) / bool getType(<num>, <charType>)```

3 versions of ```getType``` exist. The first one puts the type of the sentence in ```<type>``` which is a ```char *```. The second one does the same but ```<type>``` is a ```String```. Return ```true``` if a type has been parsed, ```false``` otherwise. The third one puts a character of the type at position ```<num>```

### ```uint8_t argCount()```

Return the number of arguments.

### ```NMEA::ErrorCode error()```

Return the error. The returned code can be:

* ```NMEA::NO_ERROR```: no error;
* ```NMEA::UNEXPECTED_CHAR```: a char which is not expected in the sentence has been encountered;
* ```NMEA::BUFFER_FULL```: the sentence is too long to fit in the buffer;
* ```NMEA::TYPE_TOO_LONG```: the sentence type has more than 5 characters;
* ```NMEA::CRC_ERROR```: the CRC is wrong;
* ```NMEA::INTERNAL_ERROR```: the internal state of the parser is wrong, should not happen by the way.

### Feeding characters to the parser

Characters are fed to the parser in ```loop```, assuming we get the characters from ```Serial```, the following way:

```C++
while (Serial.available()) {
  parser << Serial.read();
}
```

This can also be done in ```serialEvent```. ```while``` could be replaced by ```if```.

## A complete example

Let's say you want to turn the Arduino's LED on and off. We define a NMEA sentence taking a single argument: 1 to turn on the LED and 0 to turn it off. The sentence can therefore be:

```
$ARLED,1*43
```

to turn the LED on and

```
$ARLED,0*42
```

to turn the LED off. We define a single handler to retrieve the argument and control the LED accordingly:

```C++
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
```

We define 2 other handlers for anything else than ```ARLED``` and for errors

```C++
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
```

In ```setup```, the handlers are installed:

```C++
void setup() {
  Serial.begin(115200);
  parser.setErrorHandler(errorHandler);
  parser.addHandler("ARLED", ledHandler);
  parser.setDefaultHandler(unknownCommand);
  pinMode(LED_BUILTIN, OUTPUT);
}
```

At last in loop, we feed the parser with the chars coming from ```Serial```.

```C++
void loop() {
  if (Serial.available()) {
    parser << Serial.read();
  }
}
```

---
# Extra software

Additional software can be found in the ```extra``` directory.

## NMEA sentences generator

The ```gen``` subdirectory contains ```nmeagen```, a NMEA sentence generator program. This program generates well formed sentences with good or bad CRC. It can be used to test the parser. To build ```nmeagen```, run the ```build.sh``` script. ```nmeagen``` takes 1 or 2 arguments. The first argument is the number of sentences to generate. The second optional one is the number of sentences with bad CRC.

## Test program

The ```test``` subdirectory contains a test program that compile on Linux or Mac OS X. It takes sentences from the standard input, parse them and print out the type, the arguments and if an error occured.

---
# Additional links

* [NMEA CRC calculator](https://nmeachecksum.eqth.net)
* [NMEA 0183 on Wikipedia](https://fr.wikipedia.org/wiki/NMEA_0183)
* [NMEA revealed](https://gpsd.gitlab.io/gpsd/NMEA.html)

That's all folks !
