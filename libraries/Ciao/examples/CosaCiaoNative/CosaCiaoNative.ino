/**
 * @file CosaCiaoNative.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * @section Description
 * Demonstration of Cosa Ciao data streaming format; standard data
 * types. Open the serial monitor to see the print out of the
 * serialized data stream. No special host program is required, the
 * example sketch will print the stream as hexadecimal numbers and
 * printable characters when possible.
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Ciao.h>

#include "Cosa/IOStream.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

#include <ctype.h>
#include <math.h>

// Ciao output stream
Ciao cout;

// We need a trick to allow mapping the binary stream to textual trace
// This is basically an example of the Decorator Design Pattern
// Otherwise we would need to write a host program
class TraceDevice : public IOStream::Device {
public:
  virtual int putchar(char c)
  {
    trace.print((uint8_t) c, IOStream::hex);
    if (isgraph(c)) {
      trace.print(PSTR(" '"));
      trace.print(c);
      trace.print('\'');
    }
    trace.println();
    return (1);
  }
};

// The new IOStream device that will print in hexadecimal
TraceDevice traceDevice;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaCiaoNative: started"));

  // Check amount of free memory and size of instances
  TRACE(free_memory());
  TRACE(sizeof(Ciao));
  TRACE(sizeof(TraceDevice));

  // Setup and start the data output stream on the trace device
  cout.set(&traceDevice);

  INFO("Write the header to the trace device", 0);
  cout.begin();

  // Values to stream
  char s[] = "Tjena, tjabba";
  uint8_t x = 15;
  int32_t y = -2;
  int16_t z[] = { 1, 2, 3, 4 };
  int32_t g = 0x01020304L;
  uint64_t h = 0x0102030405060708LL;
  float r = M_PI;
  float c[] = { -1.0, 1.0 };

  INFO("Stream standard type values", 0);
  cout.write(s);
  cout.write(x);
  cout.write(y);
  cout.write(z, membersof(z));
  cout.write(g);
  cout.write(h);
  cout.write(r);
  cout.write(c, membersof(c));
  cout.write((float) NAN);
  cout.write((float) INFINITY);
}

void loop()
{
}

/**
 * @section Output
 *
 *  CosaCiaoNative: started
 *  free_memory() = 1658
 *  sizeof(Ciao) = 2
 *  sizeof(TraceDevice) = 3
 *  84:void setup():info:Write the header to the trace device
 *  0x61 'a'
 *  0x0
 *  0x43 'C'
 *  0x6f 'o'
 *  0x73 's'
 *  0x61 'a'
 *  0x3a ':'
 *  0x3a ':'
 *  0x43 'C'
 *  0x69 'i'
 *  0x61 'a'
 *  0x6f 'o'
 *  0x0
 *  0x1
 *  0x0
 *  0x0
 *  97:void setup():info:Stream standard type values
 *  0x0
 *  0x54 'T'
 *  0x6a 'j'
 *  0x65 'e'
 *  0x6e 'n'
 *  0x61 'a'
 *  0x2c ','
 *  0x20
 *  0x74 't'
 *  0x6a 'j'
 *  0x61 'a'
 *  0x62 'b'
 *  0x62 'b'
 *  0x61 'a'
 *  0x0
 *  0x1
 *  0xf
 *  0xa1
 *  0xfe
 *  0xff
 *  0xff
 *  0xff
 *  0x94
 *  0x1
 *  0x0
 *  0x2
 *  0x0
 *  0x3
 *  0x0
 *  0x4
 *  0x0
 *  0xa1
 *  0x4
 *  0x3
 *  0x2
 *  0x1
 *  0x31 '1'
 *  0x8
 *  0x7
 *  0x6
 *  0x5
 *  0x4
 *  0x3
 *  0x2
 *  0x1
 *  0xd1
 *  0xdb
 *  0xf
 *  0x49 'I'
 *  0x40 '@'
 *  0xd2
 *  0x0
 *  0x0
 *  0x80
 *  0xbf
 *  0x0
 *  0x0
 *  0x80
 *  0x3f '?'
 *  0xd1
 *  0x0
 *  0x0
 *  0xc0
 *  0x7f
 *  0xd1
 *  0x0
 *  0x0
 *  0x80
 *  0x7f
 */

