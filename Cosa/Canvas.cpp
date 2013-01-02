/**
 * @file Cosa/Canvas.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Virtual Canvas device/IOStream; abstraction of small screens,
 * LCD/TFT. See Cosa/SPI/ST7735R.hh for an example of usage.
 *
 * @section Limitations
 * Color model is 16-bit RBG<5,6,5>.
 *
 * @section Acknowledgement
 * Inspired by graphics library by ladyada/adafruit.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Canvas.hh"

void
Canvas::draw_bitmap(uint8_t x, uint8_t y, const uint8_t* bp, 
		    uint8_t width, uint8_t height)
{
  for (uint8_t i = 0; i < width; i++) {
    uint8_t line;
    for (uint8_t j = 0; j < height; j++) {
      if ((j & 0x7) == 0)
	line = pgm_read_byte(bp++);
      if (line & 0x1) {
	if (m_scale == 1)
	  draw_pixel(x + i, y + j);
	else {
	  fill_rect(x + i*m_scale, y + j*m_scale, m_scale, m_scale);
	} 
      }
      line >>= 1;
    }
  }
}

void
Canvas::draw_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
  draw_horizontal_line(x, y, width);
  draw_vertical_line(x + width, y, height);
  draw_vertical_line(x, y, height);
  draw_horizontal_line(x, y + height, width);
}

#define swap(a, b) { uint16_t t = a; a = b; b = t; }

void 
Canvas::draw_line(uint8_t _x0, uint8_t _y0, uint8_t _x1, uint8_t _y1)
{
  int16_t x0 = _x0;
  int16_t y0 = _y0;
  int16_t x1 = _x1;
  int16_t y1 = _y1; 

  uint16_t steep = (abs(y1 - y0) > abs(x1 - x0));
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  uint16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      draw_pixel(y0, x0);
    } else {
      draw_pixel(x0, y0);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void 
Canvas::draw_circle(uint8_t x, uint8_t y, uint8_t radius)
{
  int16_t f = 1 - radius;
  int16_t dx = 1;
  int16_t dy = -2 * radius;
  int16_t rx = 0;
  int16_t ry = radius;

  draw_pixel(x, y + radius);
  draw_pixel(x, y - radius);
  draw_pixel(x + radius, y);
  draw_pixel(x - radius, y);
  while (rx < ry) {
    if (f >= 0) {
      ry--;
      dy += 2;
      f += dy;
    }
    rx++;
    dx += 2;
    f += dx;
    draw_pixel(x + rx, y + ry);
    draw_pixel(x - rx, y + ry);
    draw_pixel(x + rx, y - ry);
    draw_pixel(x - rx, y - ry);
    draw_pixel(x + ry, y + rx);
    draw_pixel(x - ry, y + rx);
    draw_pixel(x + ry, y - rx);
    draw_pixel(x - ry, y - rx);
  }
}

void 
Canvas::fill_circle(uint8_t x, uint8_t y, uint8_t radius)
{
  int16_t f = 1 - radius;
  int16_t dx = 1;
  int16_t dy = -2 * radius;
  int16_t rx = 0;
  int16_t ry = radius;

  draw_vertical_line(x, y - radius, 2*radius + 1);
  while (rx < ry) {
    if (f >= 0) {
      ry--;
      dy += 2;
      f += dy;
    }
    rx++;
    dx += 2;
    f += dx;
    draw_vertical_line(x + rx, y - ry, 2*ry + 1);
    draw_vertical_line(x - rx, y - ry, 2*ry + 1);
    draw_vertical_line(x + ry, y - rx, 2*rx + 1);
    draw_vertical_line(x - ry, y - rx, 2*rx + 1);
  }
}

void 
Canvas::draw_char(char c)
{
  uint16_t color = m_pen_color;
  m_pen_color = m_text_color;
  if (c >= ' ')
    draw_bitmap(m_x, m_y, m_font + (c*FONT_WIDTH), FONT_WIDTH, FONT_HEIGHT);
  m_x += m_scale * (FONT_WIDTH + CHAR_SPACING);
  if (m_x > SCREEN_WIDTH || c == '\n') {
    m_x = 0;
    m_y += m_scale * (FONT_HEIGHT + LINE_SPACING);
    if (m_y > SCREEN_HEIGHT) m_y = 0;
  }
  m_pen_color = color;
}

uint8_t Canvas::font5x7[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00,   
  0x3E, 0x5B, 0x4F, 0x5B, 0x3E, 	
  0x3E, 0x6B, 0x4F, 0x6B, 0x3E, 	
  0x1C, 0x3E, 0x7C, 0x3E, 0x1C, 
  0x18, 0x3C, 0x7E, 0x3C, 0x18, 
  0x1C, 0x57, 0x7D, 0x57, 0x1C, 
  0x1C, 0x5E, 0x7F, 0x5E, 0x1C, 
  0x00, 0x18, 0x3C, 0x18, 0x00, 
  0xFF, 0xE7, 0xC3, 0xE7, 0xFF, 
  0x00, 0x18, 0x24, 0x18, 0x00, 
  0xFF, 0xE7, 0xDB, 0xE7, 0xFF, 
  0x30, 0x48, 0x3A, 0x06, 0x0E, 
  0x26, 0x29, 0x79, 0x29, 0x26, 
  0x40, 0x7F, 0x05, 0x05, 0x07, 
  0x40, 0x7F, 0x05, 0x25, 0x3F, 
  0x5A, 0x3C, 0xE7, 0x3C, 0x5A, 
  0x7F, 0x3E, 0x1C, 0x1C, 0x08, 
  0x08, 0x1C, 0x1C, 0x3E, 0x7F, 
  0x14, 0x22, 0x7F, 0x22, 0x14, 
  0x5F, 0x5F, 0x00, 0x5F, 0x5F, 
  0x06, 0x09, 0x7F, 0x01, 0x7F, 
  0x00, 0x66, 0x89, 0x95, 0x6A, 
  0x60, 0x60, 0x60, 0x60, 0x60, 
  0x94, 0xA2, 0xFF, 0xA2, 0x94, 
  0x08, 0x04, 0x7E, 0x04, 0x08, 
  0x10, 0x20, 0x7E, 0x20, 0x10, 
  0x08, 0x08, 0x2A, 0x1C, 0x08, 
  0x08, 0x1C, 0x2A, 0x08, 0x08, 
  0x1E, 0x10, 0x10, 0x10, 0x10, 
  0x0C, 0x1E, 0x0C, 0x1E, 0x0C, 
  0x30, 0x38, 0x3E, 0x38, 0x30, 
  0x06, 0x0E, 0x3E, 0x0E, 0x06, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x5F, 0x00, 0x00, 
  0x00, 0x07, 0x00, 0x07, 0x00, 
  0x14, 0x7F, 0x14, 0x7F, 0x14, 
  0x24, 0x2A, 0x7F, 0x2A, 0x12, 
  0x23, 0x13, 0x08, 0x64, 0x62, 
  0x36, 0x49, 0x56, 0x20, 0x50, 
  0x00, 0x08, 0x07, 0x03, 0x00, 
  0x00, 0x1C, 0x22, 0x41, 0x00, 
  0x00, 0x41, 0x22, 0x1C, 0x00, 
  0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 
  0x08, 0x08, 0x3E, 0x08, 0x08, 
  0x00, 0x80, 0x70, 0x30, 0x00, 
  0x08, 0x08, 0x08, 0x08, 0x08, 
  0x00, 0x00, 0x60, 0x60, 0x00, 
  0x20, 0x10, 0x08, 0x04, 0x02, 
  0x3E, 0x51, 0x49, 0x45, 0x3E, 
  0x00, 0x42, 0x7F, 0x40, 0x00, 
  0x72, 0x49, 0x49, 0x49, 0x46, 
  0x21, 0x41, 0x49, 0x4D, 0x33, 
  0x18, 0x14, 0x12, 0x7F, 0x10, 
  0x27, 0x45, 0x45, 0x45, 0x39, 
  0x3C, 0x4A, 0x49, 0x49, 0x31, 
  0x41, 0x21, 0x11, 0x09, 0x07, 
  0x36, 0x49, 0x49, 0x49, 0x36, 
  0x46, 0x49, 0x49, 0x29, 0x1E, 
  0x00, 0x00, 0x14, 0x00, 0x00, 
  0x00, 0x40, 0x34, 0x00, 0x00, 
  0x00, 0x08, 0x14, 0x22, 0x41, 
  0x14, 0x14, 0x14, 0x14, 0x14, 
  0x00, 0x41, 0x22, 0x14, 0x08, 
  0x02, 0x01, 0x59, 0x09, 0x06, 
  0x3E, 0x41, 0x5D, 0x59, 0x4E, 
  0x7C, 0x12, 0x11, 0x12, 0x7C, 
  0x7F, 0x49, 0x49, 0x49, 0x36, 
  0x3E, 0x41, 0x41, 0x41, 0x22, 
  0x7F, 0x41, 0x41, 0x41, 0x3E, 
  0x7F, 0x49, 0x49, 0x49, 0x41, 
  0x7F, 0x09, 0x09, 0x09, 0x01, 
  0x3E, 0x41, 0x41, 0x51, 0x73, 
  0x7F, 0x08, 0x08, 0x08, 0x7F, 
  0x00, 0x41, 0x7F, 0x41, 0x00, 
  0x20, 0x40, 0x41, 0x3F, 0x01, 
  0x7F, 0x08, 0x14, 0x22, 0x41, 
  0x7F, 0x40, 0x40, 0x40, 0x40, 
  0x7F, 0x02, 0x1C, 0x02, 0x7F, 
  0x7F, 0x04, 0x08, 0x10, 0x7F, 
  0x3E, 0x41, 0x41, 0x41, 0x3E, 
  0x7F, 0x09, 0x09, 0x09, 0x06, 
  0x3E, 0x41, 0x51, 0x21, 0x5E, 
  0x7F, 0x09, 0x19, 0x29, 0x46, 
  0x26, 0x49, 0x49, 0x49, 0x32, 
  0x03, 0x01, 0x7F, 0x01, 0x03, 
  0x3F, 0x40, 0x40, 0x40, 0x3F, 
  0x1F, 0x20, 0x40, 0x20, 0x1F, 
  0x3F, 0x40, 0x38, 0x40, 0x3F, 
  0x63, 0x14, 0x08, 0x14, 0x63, 
  0x03, 0x04, 0x78, 0x04, 0x03, 
  0x61, 0x59, 0x49, 0x4D, 0x43, 
  0x00, 0x7F, 0x41, 0x41, 0x41, 
  0x02, 0x04, 0x08, 0x10, 0x20, 
  0x00, 0x41, 0x41, 0x41, 0x7F, 
  0x04, 0x02, 0x01, 0x02, 0x04, 
  0x40, 0x40, 0x40, 0x40, 0x40, 
  0x00, 0x03, 0x07, 0x08, 0x00, 
  0x20, 0x54, 0x54, 0x78, 0x40, 
  0x7F, 0x28, 0x44, 0x44, 0x38, 
  0x38, 0x44, 0x44, 0x44, 0x28, 
  0x38, 0x44, 0x44, 0x28, 0x7F, 
  0x38, 0x54, 0x54, 0x54, 0x18, 
  0x00, 0x08, 0x7E, 0x09, 0x02, 
  0x18, 0xA4, 0xA4, 0x9C, 0x78, 
  0x7F, 0x08, 0x04, 0x04, 0x78, 
  0x00, 0x44, 0x7D, 0x40, 0x00, 
  0x20, 0x40, 0x40, 0x3D, 0x00, 
  0x7F, 0x10, 0x28, 0x44, 0x00, 
  0x00, 0x41, 0x7F, 0x40, 0x00, 
  0x7C, 0x04, 0x78, 0x04, 0x78, 
  0x7C, 0x08, 0x04, 0x04, 0x78, 
  0x38, 0x44, 0x44, 0x44, 0x38, 
  0xFC, 0x18, 0x24, 0x24, 0x18, 
  0x18, 0x24, 0x24, 0x18, 0xFC, 
  0x7C, 0x08, 0x04, 0x04, 0x08, 
  0x48, 0x54, 0x54, 0x54, 0x24, 
  0x04, 0x04, 0x3F, 0x44, 0x24, 
  0x3C, 0x40, 0x40, 0x20, 0x7C, 
  0x1C, 0x20, 0x40, 0x20, 0x1C, 
  0x3C, 0x40, 0x30, 0x40, 0x3C, 
  0x44, 0x28, 0x10, 0x28, 0x44, 
  0x4C, 0x90, 0x90, 0x90, 0x7C, 
  0x44, 0x64, 0x54, 0x4C, 0x44, 
  0x00, 0x08, 0x36, 0x41, 0x00, 
  0x00, 0x00, 0x77, 0x00, 0x00, 
  0x00, 0x41, 0x36, 0x08, 0x00, 
  0x02, 0x01, 0x02, 0x04, 0x02, 
  0x3C, 0x26, 0x23, 0x26, 0x3C, 
  0x1E, 0xA1, 0xA1, 0x61, 0x12, 
  0x3A, 0x40, 0x40, 0x20, 0x7A, 
  0x38, 0x54, 0x54, 0x55, 0x59, 
  0x21, 0x55, 0x55, 0x79, 0x41, 
  0x21, 0x54, 0x54, 0x78, 0x41, 
  0x21, 0x55, 0x54, 0x78, 0x40, 
  0x20, 0x54, 0x55, 0x79, 0x40, 
  0x0C, 0x1E, 0x52, 0x72, 0x12, 
  0x39, 0x55, 0x55, 0x55, 0x59, 
  0x39, 0x54, 0x54, 0x54, 0x59, 
  0x39, 0x55, 0x54, 0x54, 0x58, 
  0x00, 0x00, 0x45, 0x7C, 0x41, 
  0x00, 0x02, 0x45, 0x7D, 0x42, 
  0x00, 0x01, 0x45, 0x7C, 0x40, 
  0xF0, 0x29, 0x24, 0x29, 0xF0, 
  0xF0, 0x28, 0x25, 0x28, 0xF0, 
  0x7C, 0x54, 0x55, 0x45, 0x00, 
  0x20, 0x54, 0x54, 0x7C, 0x54, 
  0x7C, 0x0A, 0x09, 0x7F, 0x49, 
  0x32, 0x49, 0x49, 0x49, 0x32, 
  0x32, 0x48, 0x48, 0x48, 0x32, 
  0x32, 0x4A, 0x48, 0x48, 0x30, 
  0x3A, 0x41, 0x41, 0x21, 0x7A, 
  0x3A, 0x42, 0x40, 0x20, 0x78, 
  0x00, 0x9D, 0xA0, 0xA0, 0x7D, 
  0x39, 0x44, 0x44, 0x44, 0x39, 
  0x3D, 0x40, 0x40, 0x40, 0x3D, 
  0x3C, 0x24, 0xFF, 0x24, 0x24, 
  0x48, 0x7E, 0x49, 0x43, 0x66, 
  0x2B, 0x2F, 0xFC, 0x2F, 0x2B, 
  0xFF, 0x09, 0x29, 0xF6, 0x20, 
  0xC0, 0x88, 0x7E, 0x09, 0x03, 
  0x20, 0x54, 0x54, 0x79, 0x41, 
  0x00, 0x00, 0x44, 0x7D, 0x41, 
  0x30, 0x48, 0x48, 0x4A, 0x32, 
  0x38, 0x40, 0x40, 0x22, 0x7A, 
  0x00, 0x7A, 0x0A, 0x0A, 0x72, 
  0x7D, 0x0D, 0x19, 0x31, 0x7D, 
  0x26, 0x29, 0x29, 0x2F, 0x28, 
  0x26, 0x29, 0x29, 0x29, 0x26, 
  0x30, 0x48, 0x4D, 0x40, 0x20, 
  0x38, 0x08, 0x08, 0x08, 0x08, 
  0x08, 0x08, 0x08, 0x08, 0x38, 
  0x2F, 0x10, 0xC8, 0xAC, 0xBA, 
  0x2F, 0x10, 0x28, 0x34, 0xFA, 
  0x00, 0x00, 0x7B, 0x00, 0x00, 
  0x08, 0x14, 0x2A, 0x14, 0x22, 
  0x22, 0x14, 0x2A, 0x14, 0x08, 
  0xAA, 0x00, 0x55, 0x00, 0xAA, 
  0xAA, 0x55, 0xAA, 0x55, 0xAA, 
  0x00, 0x00, 0x00, 0xFF, 0x00, 
  0x10, 0x10, 0x10, 0xFF, 0x00, 
  0x14, 0x14, 0x14, 0xFF, 0x00, 
  0x10, 0x10, 0xFF, 0x00, 0xFF, 
  0x10, 0x10, 0xF0, 0x10, 0xF0, 
  0x14, 0x14, 0x14, 0xFC, 0x00, 
  0x14, 0x14, 0xF7, 0x00, 0xFF, 
  0x00, 0x00, 0xFF, 0x00, 0xFF, 
  0x14, 0x14, 0xF4, 0x04, 0xFC, 
  0x14, 0x14, 0x17, 0x10, 0x1F, 
  0x10, 0x10, 0x1F, 0x10, 0x1F, 
  0x14, 0x14, 0x14, 0x1F, 0x00, 
  0x10, 0x10, 0x10, 0xF0, 0x00, 
  0x00, 0x00, 0x00, 0x1F, 0x10, 
  0x10, 0x10, 0x10, 0x1F, 0x10, 
  0x10, 0x10, 0x10, 0xF0, 0x10, 
  0x00, 0x00, 0x00, 0xFF, 0x10, 
  0x10, 0x10, 0x10, 0x10, 0x10, 
  0x10, 0x10, 0x10, 0xFF, 0x10, 
  0x00, 0x00, 0x00, 0xFF, 0x14, 
  0x00, 0x00, 0xFF, 0x00, 0xFF, 
  0x00, 0x00, 0x1F, 0x10, 0x17, 
  0x00, 0x00, 0xFC, 0x04, 0xF4, 
  0x14, 0x14, 0x17, 0x10, 0x17, 
  0x14, 0x14, 0xF4, 0x04, 0xF4, 
  0x00, 0x00, 0xFF, 0x00, 0xF7, 
  0x14, 0x14, 0x14, 0x14, 0x14, 
  0x14, 0x14, 0xF7, 0x00, 0xF7, 
  0x14, 0x14, 0x14, 0x17, 0x14, 
  0x10, 0x10, 0x1F, 0x10, 0x1F, 
  0x14, 0x14, 0x14, 0xF4, 0x14, 
  0x10, 0x10, 0xF0, 0x10, 0xF0, 
  0x00, 0x00, 0x1F, 0x10, 0x1F, 
  0x00, 0x00, 0x00, 0x1F, 0x14, 
  0x00, 0x00, 0x00, 0xFC, 0x14, 
  0x00, 0x00, 0xF0, 0x10, 0xF0, 
  0x10, 0x10, 0xFF, 0x10, 0xFF, 
  0x14, 0x14, 0x14, 0xFF, 0x14, 
  0x10, 0x10, 0x10, 0x1F, 0x00, 
  0x00, 0x00, 0x00, 0xF0, 0x10, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 
  0xFF, 0xFF, 0xFF, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xFF, 0xFF, 
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
  0x38, 0x44, 0x44, 0x38, 0x44, 
  0x7C, 0x2A, 0x2A, 0x3E, 0x14, 
  0x7E, 0x02, 0x02, 0x06, 0x06, 
  0x02, 0x7E, 0x02, 0x7E, 0x02, 
  0x63, 0x55, 0x49, 0x41, 0x63, 
  0x38, 0x44, 0x44, 0x3C, 0x04, 
  0x40, 0x7E, 0x20, 0x1E, 0x20, 
  0x06, 0x02, 0x7E, 0x02, 0x02, 
  0x99, 0xA5, 0xE7, 0xA5, 0x99, 
  0x1C, 0x2A, 0x49, 0x2A, 0x1C, 
  0x4C, 0x72, 0x01, 0x72, 0x4C, 
  0x30, 0x4A, 0x4D, 0x4D, 0x30, 
  0x30, 0x48, 0x78, 0x48, 0x30, 
  0xBC, 0x62, 0x5A, 0x46, 0x3D, 
  0x3E, 0x49, 0x49, 0x49, 0x00, 
  0x7E, 0x01, 0x01, 0x01, 0x7E, 
  0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 
  0x44, 0x44, 0x5F, 0x44, 0x44, 
  0x40, 0x51, 0x4A, 0x44, 0x40, 
  0x40, 0x44, 0x4A, 0x51, 0x40, 
  0x00, 0x00, 0xFF, 0x01, 0x03, 
  0xE0, 0x80, 0xFF, 0x00, 0x00, 
  0x08, 0x08, 0x6B, 0x6B, 0x08,
  0x36, 0x12, 0x36, 0x24, 0x36, 
  0x06, 0x0F, 0x09, 0x0F, 0x06, 
  0x00, 0x00, 0x18, 0x18, 0x00, 
  0x00, 0x00, 0x10, 0x10, 0x00, 
  0x30, 0x40, 0xFF, 0x01, 0x01, 
  0x00, 0x1F, 0x01, 0x01, 0x1E, 
  0x00, 0x19, 0x1D, 0x17, 0x12, 
  0x00, 0x3C, 0x3C, 0x3C, 0x3C, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
};
