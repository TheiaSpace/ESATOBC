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

String ESATClock::format(byte BCDNumber, byte length)
{
  const byte binaryNumber = BCDToBinary(BCDNumber);
  String unpaddedText = String(int(binaryNumber), DEC);
  String paddedText = Util.pad(unpaddedText, '0', 2);
  return paddedText;
}

String ESATClock::read()
{
  Wire.beginTransmission(ADDRESS);
  Wire.write(TIME_REGISTER);
  const byte errorCode = Wire.endTransmission();
  if (errorCode != 0)
  {
    error = true;
    return "00000000000000";
  }
  const byte bytesToRead = 6;
  const byte bytesRead = Wire.requestFrom(ADDRESS, bytesToRead);
  if (bytesRead != bytesToRead)
  {
    error = true;
    return "00000000000000";
  }
  const byte seconds = Wire.read();
  const byte minutes = Wire.read();
  const byte hours = Wire.read();
  const byte day = Wire.read();
  const byte month = Wire.read();
  const byte year = Wire.read();
  return "20"
    + format(year, 2)
    + "-"
    + format(month, 2)
    + "-"
    + format(day, 2)
    + "T"
    + format(hours, 2)
    + ":"
    + format(minutes, 2)
    + ":"
    + format(seconds & 0x7F, 2);
}

void ESATClock::write(String time)
{
  const byte year = time.substring(0, 4).toInt() - 2000;
  const byte month = time.substring(5, 7).toInt();
  const byte day = time.substring(8, 10).toInt();
  const byte hours = time.substring(11, 13).toInt();
  const byte minutes = time.substring(14, 16).toInt();
  const byte seconds = time.substring(17, 19).toInt();
  Wire.beginTransmission(ADDRESS);
  Wire.write(TIME_REGISTER);
  Wire.write(binaryToBCD(seconds));
  Wire.write(binaryToBCD(minutes));
  Wire.write(binaryToBCD(hours));
  Wire.write(binaryToBCD(day));
  Wire.write(binaryToBCD(month));
  Wire.write(binaryToBCD(year));
  Wire.write(0);
  const byte errorCode = Wire.endTransmission();
  if (errorCode != 0)
  {
    error = true;
  }
}

ESATClock Clock;
