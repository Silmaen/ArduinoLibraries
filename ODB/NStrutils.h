/**
 *
 */
 
#ifndef NSTRUTILS_H_
#define NSTRUTILS_H_

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#ifdef __AVR__
  #include <avr/pgmspace.h>
#endif

namespace nstd {

void mswap(char& a,char& b);

void mreverse(char* str, unsigned int length);

char* mitoa(long int num, char* str, int base);

char* mftoa(char* str,uint8_t maxlen,float val,uint8_t prec);

} /* namespace nstd */


#endif

