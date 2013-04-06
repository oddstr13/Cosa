/**
 * @file Cosa/EEPROM.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Driver for the ATmega/ATtiny internal EEPROM and abstraction of
 * EEPROM devices. See AT24CXX for an example of driver for external
 * EEPROM memory. The default device is the internal EEPROM.
 * The class EEPROM delegates to the device.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_EEPROM_HH__
#define __COSA_EEPROM_HH__

#include "Cosa/Types.h"
#include "Cosa/Power.hh"
#include <avr/sleep.h>

class EEPROM {
public:
  class Device {
  public:
    /**
     * Return true(1) if the device is ready, write cycle is completed,
     * otherwise false(0).
     * @return bool
     */
    virtual bool is_ready();

    /**
     * Read rom block with the given size into the buffer from the address.
     * Return number of bytes read or negative error code.
     * @param[in] dest buffer to read from rom.
     * @param[in] src address in rom to read from.
     * @param[in] size number of bytes to read.
     * @return number of bytes or negative error code.
     */
    virtual int read(void* dest, void* src, size_t size);

    /**
     * Write rom block at given address with the contents from the buffer.
     * Return number of bytes written or negative error code.
     * @param[in] dest address in rom to read write to.
     * @param[in] src buffer to write to rom.
     * @param[in] size number of bytes to write.
     * @return number of bytes or negative error code.
     */
    virtual int write(void* dest, void* src, size_t size);
    
    static Device eeprom;
  };

private:
  Device* m_dev;

public:
  /**
   * Construct access object for EEPROM given device. Default 
   * device is the internal EEPROM memory.
   * @param[in] dev device.
   */
  EEPROM(Device* dev = &Device::eeprom) : m_dev(dev) {}

  /**
   * Return true(1) if the device is ready, write cycle is completed,
   * otherwise false(0).
   * @return bool
   */
  bool is_ready() 
  {
    return (m_dev->is_ready());
  }

  /**
   * Wait for write to complete. 
   * @param[in] mode of sleep.
   */
  void write_await(uint8_t mode = SLEEP_MODE_IDLE)
  {
    while (!is_ready()) Power::sleep(mode);
  }

  /**
   * Read rom block with the given size into the buffer from the address.
   * Return number of bytes read or negative error code.
   * @param[in] dest buffer to read from rom.
   * @param[in] src address in rom to read from.
   * @param[in] size number of bytes to read.
   * @return number of bytes or negative error code.
   */
  int read(void* dest, void* src, size_t size) 
  { 
    return (m_dev->read(dest, src, size)); 
  }

  int read(unsigned char* dest, void* src) 
  { 
    return (read(dest, src, sizeof(unsigned char))); 
  }

  int read(unsigned short* dest, void* src) 
  { 
    return (read(dest, src, sizeof(unsigned short))); 
  }
  
  int read(unsigned int* dest, void* src) 
  { 
    return (read(dest, src, sizeof(unsigned int))); 
  }

  int read(unsigned long* dest, void* src)
  { 
    return (read(dest, src, sizeof(unsigned long))); 
  }
  
  int read(char* dest, void* src) 
  { 
    return (read(dest, src, sizeof(char))); 
  }

  int read(short* dest, void* src) 
  { 
    return (read(dest, src, sizeof(short))); 
  }

  int read(int* dest, void* src)
  { 
    return (read(dest, src, sizeof(int))); 
  }

  int read(long* dest, void* src)
  { 
    return (read(dest, src, sizeof(long))); 
  }

  int read(float* dest, void* src)
  { 
    return (read(dest, src, sizeof(float))); 
  }

  /**
   * Write rom block at given address with the contents from the buffer.
   * Return number of bytes written or negative error code.
   * @param[in] dest address in rom to read write to.
   * @param[in] src buffer to write to rom.
   * @param[in] size number of bytes to write.
   * @return number of bytes or negative error code.
   */
  int write(void* dest, void* src, size_t size) 
  { 
    return (m_dev->write(dest, src, size)); 
  }

  int write(void* dest, unsigned char src) 
  { 
    return (write(dest, &src, sizeof(unsigned char))); 
  }

  int write(void* dest, unsigned short src) 
  { 
    return (write(dest, &src, sizeof(unsigned short)));
  }

  int write(void* dest, unsigned int src) 
  { 
    return (write(dest, &src, sizeof(unsigned int))); 
  }

  int write(void* dest, unsigned long src) 
  { 
    return (write(dest, &src, sizeof(unsigned long))); 
  }

  int write(void* dest, char src) 
  { 
    return (write(dest, &src, sizeof(char))); 
  }

  int write(void* dest, short src) 
  { 
    return (write(dest, &src, sizeof(short)));
  }

  int write(void* dest, int src) 
  { 
    return (write(dest, &src, sizeof(int))); 
  }

  int write(void* dest, long src) 
  { 
    return (write(dest, &src, sizeof(long))); 
  }

  int write(void* dest, float src) 
  { 
    return (write(dest, &src, sizeof(float))); 
  }
};

#endif