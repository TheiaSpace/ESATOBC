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
  Wire.beginTransmission(ADDRESS);
  Wire.write(TIME_REGISTER);
  const byte errorCode = Wire.endTransmission();
  if (errorCode != 0)
  {
    error = true;
    return timestamp;
  }
  const byte bytesToRead = 7;
  const byte bytesRead = Wire.requestFrom(ADDRESS, bytesToRead);
  if (bytesRead != bytesToRead)
  {
    error = true;
    return timestamp;
  }
  const byte seconds = Wire.read();
  const byte minutes = Wire.read();
  const byte hours = Wire.read();
  const byte weekDay = Wire.read();
  const byte day = Wire.read();
  const byte month = Wire.read();
  const byte year = Wire.read();
  timestamp.update(BCDToBinary(year),BCDToBinary(month),BCDToBinary(day),
                   BCDToBinary(hours),BCDToBinary(minutes),BCDToBinary(seconds & 0x7F));
  return timestamp;
}

void ESATClock::write(ESATTimestamp timestamp)
{
  Wire.beginTransmission(ADDRESS);
  Wire.write(TIME_REGISTER);
  Wire.write(binaryToBCD(timestamp.seconds));
  Wire.write(binaryToBCD(timestamp.minutes));
  Wire.write(binaryToBCD(timestamp.hours));
  // Day of the week not used
  Wire.write(binaryToBCD(2));
  Wire.write(binaryToBCD(timestamp.day));
  Wire.write(binaryToBCD(timestamp.month));
  Wire.write(binaryToBCD(timestamp.year));
  Wire.write(0);
  const byte errorCode = Wire.endTransmission();
  if (errorCode != 0)
  {
    error = true;
  }
}



ESATClock Clock;



