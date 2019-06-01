#include "../../src/NMEAParser.h"
#include <string.h>

NMEAParser<4> commandNMEA;
int errorCount = 0;

void error()
{
  printf("=================================================\n");
  int err = commandNMEA.error();
  printf("*** ERROR %d ",err);
  switch (err) {
    case NMEA::UNEXPECTED_CHAR:
      printf("(UNEXPECTED CHAR)\n");
      break;
    case NMEA::BUFFER_FULL:
      printf("(BUFFER FULL)\n");
      break;
    case NMEA::CRC_ERROR:
      printf("(CRC ERROR)\n");
      break;
    case NMEA::INTERNAL_ERROR:
      printf("(INTERNAL ERROR)\n");
      break;
    default:
      printf("(?)\n");
      break;
  }
  commandNMEA.printBuffer();
  printf("=================================================\n");
  errorCount++;
}

void defaultHandler()
{
  printf("------------\n");
  char buf[82];
  if (commandNMEA.getType(buf)) {
    printf("%s\n", buf);
    for (int i = 0; i < commandNMEA.argCount(); i++) {
      if (commandNMEA.getArg(i, buf)) {
        printf("    %s\n", buf);
      }
    }
  }
}

int main()
{
  printf("Debut du test\n");

  commandNMEA.setErrorHandler(error);
  commandNMEA.setDefaultHandler(defaultHandler);

  int count = 0;
  int v;
  while ((v = getchar()) != EOF) {
    commandNMEA << v;
    if (v == '\n') count++;
  }
  printf("*** Processed %d NMEA sentences\n", count);
  printf("*** Got %d error(s)\n", errorCount);
}
