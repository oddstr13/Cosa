/**
 * @file CosaSPI.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * Demonstration and verification of SPI timing; clock, mode and
 * chip select handling.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/SPI.hh"

static const uint8_t BUF_MAX = 8;
uint8_t m_buf[BUF_MAX];

SPI::Slave slave(m_buf, BUF_MAX);

void loop()
{
  Event::service();
}
