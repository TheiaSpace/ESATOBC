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

#include "ESATOBCClock.h"
#include <ESATUtil.h>
#include <Wire.h>

ESATTimestamp ESATOBCClock::read()
{
  Wire.beginTransmission(ADDRESS);
  Wire.write(TIME_REGISTER);
  const byte errorCode = Wire.endTransmission();
  if (errorCode != 0)
  {
    error = true;
    return ESATTimestamp();
  }
  const byte bytesToRead = 7;
  const byte bytesRead = Wire.requestFrom(ADDRESS, bytesToRead);
  if (bytesRead != bytesToRead)
  {
    error = true;
    return ESATTimestamp();
  }
  const byte seconds = Wire.read();
  const byte minutes = Wire.read();
  const byte hours = Wire.read();
  const byte weekDay = Wire.read();
  const byte day = Wire.read();
  const byte month = Wire.read();
  const byte year = Wire.read();
  return ESATTimestamp(2000 + Util.decodeBinaryCodedDecimalByte(year),
                       Util.decodeBinaryCodedDecimalByte(month),
                       Util.decodeBinaryCodedDecimalByte(day),
                       Util.decodeBinaryCodedDecimalByte(hours),
                       Util.decodeBinaryCodedDecimalByte(minutes),
                       Util.decodeBinaryCodedDecimalByte(seconds & 0x7F));
}

void ESATOBCClock::write(ESATTimestamp timestamp)
{
  Wire.beginTransmission(ADDRESS);
  Wire.write(TIME_REGISTER);
  Wire.write(Util.encodeBinaryCodedDecimalByte(timestamp.seconds));
  Wire.write(Util.encodeBinaryCodedDecimalByte(timestamp.minutes));
  Wire.write(Util.encodeBinaryCodedDecimalByte(timestamp.hours));
  // Day of the week not used.
  Wire.write(Util.encodeBinaryCodedDecimalByte(2));
  Wire.write(Util.encodeBinaryCodedDecimalByte(timestamp.day));
  Wire.write(Util.encodeBinaryCodedDecimalByte(timestamp.month));
  Wire.write(Util.encodeBinaryCodedDecimalByte(timestamp.year % 100));
  Wire.write(0);
  const byte errorCode = Wire.endTransmission();
  if (errorCode != 0)
  {
    error = true;
  }
}

ESATOBCClock OBCClock;
