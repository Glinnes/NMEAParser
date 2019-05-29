#include "NMEAParser.h"
#include <string.h>

NMEAParser<4> commandNMEA;

void error()
{
  printf("=================================================\n");
  printf("*** ERROR %d\n",commandNMEA.error());
  commandNMEA.printBuffer();
  printf("=================================================\n");
}

void arv1cCommand()
{
  printf("got ARV1C\n");
  int arg0, arg1;
  char arg2;
  char buf[10];
  if (commandNMEA.getArg(0,arg0)) printf("%d\n", arg0);
  if (commandNMEA.getArg(1,arg1)) printf("%d\n", arg1);
  if (commandNMEA.getArg(2,arg2)) printf("%c\n", arg2);
  if (commandNMEA.getArg(2,buf)) printf("%s\n", buf);
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
//  char sentence[] = "$ARV1C,2,3,AB*19\r\n";

  commandNMEA.setErrorHandler(error);
  commandNMEA.setDefaultHandler(defaultHandler);
  commandNMEA.addHandler("ARV1C", arv1cCommand);
  commandNMEA.addHandler("ARV1C", arv1cCommand);

  int v;
  while ((v = getchar()) != EOF) {
    commandNMEA << v;
  }
}
