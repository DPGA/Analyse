#ifndef TOOLS_H
#define TOOLS_H
/*
 *********************************************************************
*	Projet		:	Atlas
*	Identification	:	Color.h
*	Contents		:	Header file
 *	Author		:	Remy MARCHIONINI
 *	Created on		:	18-dec-2006
 *	Last Update 	:
 *********************************************************************
 */
#include "GenericTypeDefs.h"
#include <string>
#include <cstdio>

template <typename T>
std::string CFormat(const char *format,const T value)
{
  char buffer[128];

  std::sprintf(buffer,format,value);

  return std::string(buffer);
}


class CtColor
{
  public:
    static const char * none;
    static const char * bold;
    static const char * underscore;
    static const char * blink;
    static const char * reverse;
    static const char * cancel;
};

class FgColor
{
  public:
    static const char * green;
    static const char * white;
    static const char * normal;
    static const char * cyan;
    static const char * yellow;
    static const char * red;
    static const char * black;
    static const char * blue;
    static const char * magenta;
    static const char * test1;
    static const char * test2;
};

class BgColor
{
  public:
    static const char * green;
    static const char * white;
    static const char * normal;
    static const char * cyan;
    static const char * yellow;
    static const char * red;
    static const char * black;
    static const char * blue;
    static const char * magenta;
    static const char * test1;
    static const char * test2;
};

class cTimeMs {
private:
  u64 begin;
public:
  cTimeMs(int Ms = 0);
      ///< Creates a timer with ms resolution and an initial timeout of Ms.
      ///< If Ms is negative the timer is not initialized with the current
      ///< time.
  static u64 Now(void);
  void Set(int Ms = 0);
  bool TimedOut(void);
  u64 Elapsed(void);
  };

class cTimeUs {
private:
  u64 begin;
public:
  cTimeUs(int Ms = 0);
      ///< Creates a timer with ms resolution and an initial timeout of Ms.
      ///< If Ms is negative the timer is not initialized with the current
      ///< time.
  static u64 Now(void);
  void Set(int Us = 0);
  bool TimedOut(void);
  u64 Elapsed(void);
  };

void Date(const char *prefix,const char *suffix,char *s);
#endif

