/*
 * This file is part of Theia Space's ESAT OBC library.
 *
 * Theia Space's ESAT OBC library is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Theia Space's ESAT OBC library is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Theia Space's ESAT OBC library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "ESATClock.h"
#include <ESATUtil.h>
#include <Wire.h>


byte ESATClock::BCDToBinary(byte value)
{
  return value - 6 * (value >> 4);
}

void ESATClock::begin()
{
  (void) read();
}

byte ESATClock::binaryToBCD(byte value)
{
  return value + 6 * (value / 10);
}

ESATTimestamp ESATClock::read()
{
  ESATTimestamp timestamp;
  Wire.beginTransmission(address);
  Wire.write(timeRegister);
  const byte errorCode = Wire.endTransmission();
  if (errorCode != 0)
  {
    alive = false;
    return timestamp;
  }
  const byte bytesToRead = 6;
  const byte bytesRead = Wire.requestFrom(address, bytesToRead);
  if (bytesRead != bytesToRead)
  {
    alive = false;
    return timestamp;
  }
  const byte seconds = Wire.read();
  const byte minutes = Wire.read();
  const byte hours = Wire.read();
  const byte day = Wire.read();
  const byte month = Wire.read();
  const byte year = Wire.read();
  alive = true;
  timestamp.update(BCDToBinary(year),BCDToBinary(month),BCDToBinary(day),
                   BCDToBinary(hours),BCDToBinary(minutes),BCDToBinary(seconds & 0x7F));
  return timestamp;
}

void ESATClock::write(String time)
{
  const byte year = time.substring(0, 4).toInt() - 2000;
  const byte month = time.substring(5, 7).toInt();
  const byte day = time.substring(8, 10).toInt();
  const byte hours = time.substring(11, 13).toInt();
  const byte minutes = time.substring(14, 16).toInt();
  const byte seconds = time.substring(17, 19).toInt();
  Wire.beginTransmission(address);
  Wire.write(timeRegister);
  Wire.write(binaryToBCD(seconds));
  Wire.write(binaryToBCD(minutes));
  Wire.write(binaryToBCD(hours));
  Wire.write(binaryToBCD(day));
  Wire.write(binaryToBCD(month));
  Wire.write(binaryToBCD(year));
  Wire.write(0);
  const byte errorCode = Wire.endTransmission();
  if (errorCode == 0)
  {
    alive = true;
  }
  else
  {
    alive = false;
  }
}



ESATClock Clock;



