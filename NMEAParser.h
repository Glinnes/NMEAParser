/*
 * NMEA parser library for Arduino
 *
 * Simple and compact NMEA parser designed for Arduino
 *
 * This work is distributed under license CC0.
 * Check https://creativecommons.org/publicdomain/zero/1.0/deed.en
 *
 * No Copyright
 *
 * The person who associated a work with this deed has dedicated the work
 * to the public domain by waiving all of his or her rights to the work
 * worldwide under copyright law, including all related and neighboring rights,
 * to the extent allowed by law.
 *
 * You can copy, modify, distribute and perform the work, even for commercial
 * purposes, all without asking permission. See Other Information below.
 *
 * Other Information
 *
 * In no way are the patent or trademark rights of any person affected by CC0,
 * nor are the rights that other persons may have in the work or in how the
 * work is used, such as publicity or privacy rights.
 * Unless expressly stated otherwise, the person who associated a work with
 * this deed makes no warranties about the work, and disclaims liability for
 * all uses of the work, to the fullest extent permitted by applicable law.
 * When using or citing the work, you should not imply endorsement by the
 * author or the affirmer.
 */

#ifndef __NMEAParser_h__
#define __NMEAParser_h__

#ifdef __amd64__
/* To use on my development platform */
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#else
#include <Arduino.h>
#endif

/*
 * The library consists of a single template: NMEAParser.
 */
template <size_t S> class NMEAParser {

private:
  typedef void (*NMEAErrorHandler)(void);
  typedef void (*NMEAHandler)(void);
  typedef struct { char mToken[6]; NMEAHandler mHandler; } NMEAHandlerEntry;
  typedef enum { INIT, SENT, ARG, CRCH, CRCL, CRLFCR, CRLFLF } State;

public:
  /*
   * maximum sentence size is 82 including the starting '$' and the <cr><lf>
   * at the end. Since '$' and the <cr><lf> are not bufferized, 79 chars +1 are
   * enough
   */
  static const uint8_t kSentenceMaxSize = 80;

  /*
   * Error codes
   */
  typedef enum {
      NO_ERROR,
      UNEXPECTED_CHAR,
      BUFFER_FULL,
      CRC_ERROR,
      INTERNAL_ERROR
  } ErrorCode;

private:
  /*
   * buffer to store the NMEA sentence excluding starting '$', the ','
   * separator, the '*', the CRC and the <cr><lf>. The tail of the buffer
   * is used to store the index of the arguments.
   */
  char mBuffer[kSentenceMaxSize];

  /*
   * Current index to store a char of the sentence
   */
  uint8_t mIndex;

  /*
   * Current index to store the index of an argument
   */
  uint8_t mArgIndex;

  /*
   * A handler to notify a malformed sentence
   */
  NMEAErrorHandler mErrorHandler;

  /*
   * A handler for well formed but unrecognized sentences
   */
  NMEAHandler mDefaultHandler;

  /*
   * An array of NMEA handler : pointers to functions to call when a sentence
   * is recognized
   */
  NMEAHandlerEntry mHandlers[S];

  /*
   * The current number of mHandlers
   */
  uint8_t mHandlerCount;

  /*
   * Parsing automaton variable
   */
  State mState;

  /*
   * mError
   */
  ErrorCode mError;

  /*
   * char that caused an mError
   */
  char mChar;

  /*
   * Call the error handler if defined
   */
  void callErrorHandler(void)
  {
    if (mErrorHandler != NULL) {
      mErrorHandler();
    }
  }

  /*
   * Reset the parser
   */
  void reset() {
    mState = INIT;
    mIndex = 0;
    mArgIndex = kSentenceMaxSize;
  }

  /*
   * Called when the parser encounter a char that should not be there
   */
  void unexpectedChar(char inChar)
  {
    mError = UNEXPECTED_CHAR;
    mChar = inChar;
    callErrorHandler();
    reset();
  }

  /*
   * Called when the buffer is full because of a malformed sentence
   */
  void bufferFull(char inChar)
  {
    mError = BUFFER_FULL;
    mChar = inChar;
    callErrorHandler();
    reset();
  }

  /*
   * Called when the CRC is wrong
   */
  void crcError()
  {
    mError = CRC_ERROR;
    callErrorHandler();
    reset();
  }

  /*
   * Called when the state of the parser is not ok
   */
  void internalError()
  {
    mError = INTERNAL_ERROR;
    callErrorHandler();
    reset();
  }

  /*
   * retuns true is there is at least one byte left in the buffer
   */
  bool spaceAvail()
  {
    return (mIndex < mArgIndex);
  }

  /*
   * convert a one hex digit char into an int. Used for the CRC
   */
  int8_t hexToNum(const char inChar)
  {
    if (isdigit(inChar)) return inChar - '0';
    else if (isupper(inChar) && inChar <= 'F') return inChar - 'A' + 10;
    else if (islower(inChar) && inChar <= 'f') return inChar - 'a' + 10;
    else return -1;
  }

  /*
   * return the slot number fo a handler. -1 if not found
   */
  int8_t getHandler(const char *inToken)
  {
    /* Look for the token */
    for (uint8_t i = 0; i < mHandlerCount; i++) {
      if (strncmp(mHandlers[i].mToken, inToken, 5) == 0) {
        return i;
      }
    }
    return -1;
  }

  /*
   * When all the sentence has been parsed, process it by calling the handler
   */
  void processSentence()
  {
    /* Look for the token */
    uint8_t endPos = endArgPos(0);
    char tmp = mBuffer[endPos];
    mBuffer[endPos] = '\0';
    int8_t slot = getHandler(mBuffer);
    mBuffer[endPos] = tmp;
    if (slot != -1) {
      mHandlers[slot].mHandler();
    }
    else {
      mDefaultHandler();
    }
  }

  /*
   * Return true if inArgNum corresponds to an actual argument
   */
  bool validArgNum(uint8_t inArgNum)
  {
    return inArgNum < (kSentenceMaxSize - mArgIndex);
  }

  /*
   * Return the start index of the inArgNum th argument
   */
  uint8_t startArgPos(uint8_t inArgNum)
  {
    return mBuffer[kSentenceMaxSize - 1 - inArgNum];
  }

  /*
   * Return the end index of the inArgNum th argument
   */
  uint8_t endArgPos(uint8_t inArgNum)
  {
    return mBuffer[kSentenceMaxSize - 2 - inArgNum];
  }

  class NMEAParserStringify {
    uint8_t       mPos;
    char          mTmp;
    NMEAParser<S> *mParent;
  public:
    NMEAParserStringify(NMEAParser<S> *inParent, uint8_t inPos) :
      mPos(inPos),
      mParent(inParent)
    {
      mTmp = mParent->mBuffer[mPos];
      mParent->mBuffer[mPos] = '\0';
    }
    ~NMEAParserStringify()
    {
      mParent->mBuffer[mPos] = mTmp;
    }
  };

public:
  NMEAParser() :
    mErrorHandler(NULL),
    mDefaultHandler(NULL),
    mHandlerCount(0),
    mError(NO_ERROR)
  {
    reset();
  }

  void addHandler(const char *inToken, NMEAHandler inHandler)
  {
    if (mHandlerCount < S) {
      if (getHandler(inToken) == -1) {
        strncpy(mHandlers[mHandlerCount].mToken, inToken, 5);
        mHandlers[mHandlerCount].mToken[5] = '\0';
        mHandlers[mHandlerCount].mHandler = inHandler;
        mHandlerCount++;
      }
    }
  }

  void setErrorHandler(NMEAErrorHandler inHandler)
  {
    mErrorHandler = inHandler;
  }

  void setDefaultHandler(NMEAHandler inHandler)
  {
    mDefaultHandler = inHandler;
  }

  void operator<<(char inChar)
  {
    static uint8_t computedCRC = 0;
    static uint8_t gotCRC = 0;
    int8_t tmp;

    switch (mState) {

      case INIT:
        mError = NO_ERROR;
        if (inChar == '$') {
          computedCRC = 0;
          mState = SENT;
        }
        else unexpectedChar(inChar);
        break;

      case SENT:
        if (isalnum(inChar)) {
          if (spaceAvail()) {
            mBuffer[mIndex++] = inChar;
            computedCRC ^= inChar;
          }
          else bufferFull(inChar);
        }
        else {
          switch(inChar) {
            case ',' :
              computedCRC ^= inChar;
              mBuffer[--mArgIndex] = mIndex;
              mState = ARG;
              break;
            case '*' :
              gotCRC = 0;
              mBuffer[--mArgIndex] = mIndex;
              mState = CRCH;
              break;
            default :
              unexpectedChar(inChar);
              break;
          }
        }
        break;

      case ARG:
        if (spaceAvail()) {
          switch(inChar) {
            case ',' :
              computedCRC ^= inChar;
              mBuffer[--mArgIndex] = mIndex;
              break;
            case '*' :
              gotCRC = 0;
              mBuffer[--mArgIndex] = mIndex;
              mState = CRCH;
              break;
            default :
              computedCRC ^= inChar;
              mBuffer[mIndex++] = inChar;
              break;
          }
        }
        else bufferFull(inChar);
        break;

      case CRCH:
        tmp = hexToNum(inChar);
        if (tmp != -1) {
          gotCRC |= (uint8_t)tmp << 4;
          mState = CRCL;
        }
        else unexpectedChar(inChar);
        break;

      case CRCL:
        tmp = hexToNum(inChar);
        if (tmp != -1) {
          gotCRC |= (uint8_t)tmp;
          mState = CRLFCR;
        }
        else unexpectedChar(inChar);
        break;

      case CRLFCR:
        if (inChar == '\r') {
          mState = CRLFLF;
        }
        else unexpectedChar(inChar);
        break;

      case CRLFLF:
        if (inChar == '\n') {
          if (gotCRC != computedCRC) {
            crcError();
          }
          else {
            processSentence();
          }
          reset();
        }
        else unexpectedChar(inChar);
        break;

      default:
        internalError();
        break;
    }
  }

  uint8_t argCount()
  {
    return kSentenceMaxSize - mArgIndex - 1;
  }

  bool getArg(uint8_t num, char &arg)
  {
    if (validArgNum(num)) {
      uint8_t startPos = startArgPos(num);
      uint8_t endPos = endArgPos(num);
      arg = mBuffer[startPos];
      return (endPos - startPos) == 1;
    }
    else return false;
  }

  bool getArg(uint8_t num, char *arg)
  {
    if (validArgNum(num)) {
      uint8_t startPos = startArgPos(num);
      uint8_t endPos = endArgPos(num);
      {
        NMEAParserStringify stfy(this, endPos);
        strcpy(arg, &mBuffer[startPos]);
      }
      return true;
    }
    else return false;
  }

  bool getArg(uint8_t num, int &arg)
  {
    if (validArgNum(num)) {
      uint8_t startPos = startArgPos(num);
      uint8_t endPos = endArgPos(num);
      {
        NMEAParserStringify stfy(this, endPos);
        arg = atoi(&mBuffer[startPos]);
      }
      return true;
    }
    else return false;
  }

  bool getType(char *arg)
  {
    if (mIndex > 0) {
      uint8_t endPos = endArgPos(0);
      {
        NMEAParserStringify stfy(this, endPos);
        strncpy(arg, mBuffer, 5);
      }
      return true;
    }
    else return false;
  }

  ErrorCode error() {
    return mError;
  }

#ifdef __amd64__
  void printBuffer()
  {
    printf("%s\n", mBuffer);
    for (uint8_t i = 0; i < argCount(); i++) {
      uint8_t startPos = startArgPos(i);
      uint8_t endPos = endArgPos(i);
      {
        NMEAParserStringify stfy(this, endPos);
        printf("%s\n", &mBuffer[startPos]);
      }
    }
  }
#endif
};

#endif