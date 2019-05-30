#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

char intToHex(uint8_t v)
{
  if (v < 10) return '0' + v;
  else if (v < 16) return 'A' + v - 10;
  else return '-';
}

void generateSentence(bool goodCRC)
{
  uint8_t crc = 0;
  putchar('$');
  /* generate the id */
  for (int i = 0; i < 5; i++) {
    char letter = 'A' + random() % 26;
    putchar(letter);
    crc ^= letter;
  }
  int numberOfArgs = random() % 10;
  for (int i = 0; i < numberOfArgs; i++) {
    putchar(',');
    crc ^= ',';
    int kind = random() % 5;
    switch (kind) {
      case 0: /* integer */
        for (int j = 0; j < random() % 5 + 1; j++) {
          char digit = '0' + random() % 10;
          putchar(digit);
          crc ^= digit;
        }
        break;
      case 1: /* char */
        if (1) {
          char letter = 'A' + random() % 26;
          putchar(letter);
          crc ^= letter;
        }
        break;
      case 2: /* string */
        for (int i = 0; i < 5; i++) {
          char letter = 'A' + random() % 26;
          putchar(letter);
          crc ^= letter;
        }
        break;
      case 3: /* float */
        for (int j = 0; j < random() % 5 + 1; j++) {
          char digit = '0' + random() % 10;
          putchar(digit);
          crc ^= digit;
        }
        putchar('.');
        crc ^= '.';
        for (int j = 0; j < random() % 5 + 1; j++) {
          char digit = '0' + random() % 10;
          putchar(digit);
          crc ^= digit;
        }
      case 4: /* nothing */
        break;
    }
  }
  if (! goodCRC) crc = ~crc;
  putchar('*');
  putchar(intToHex(crc >> 4));
  putchar(intToHex(crc & 0x0F));
  putchar('\r');
  putchar('\n');
}

int main(int argc, char *argv[])
{
  if (argc < 2 || argc > 3) {
    printf("Usage: nmeagen <n> [<c>] where <n> is the number of sentences\n"
           "       and <c> the number of sentences with a wrong CRC\n");
    exit(1);
  }
  else {
    int numberOfSentences = strtol(argv[1], NULL, 0);
    int numberOfWrongCRCSentences = 0;
    if (argc == 3) {
      numberOfWrongCRCSentences = strtol(argv[2], NULL, 0);
    }

    /* Init a the random seed with the epoch */
    srandom(time(NULL));

    for (int i = 0; i < numberOfSentences; i++) {
      generateSentence(true);
    }
    for (int i = 0; i < numberOfWrongCRCSentences; i++) {
      generateSentence(false);
    }
  }
}
