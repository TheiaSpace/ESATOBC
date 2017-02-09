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
#include <ESATI2C.h>
#include <ESATUtil.h>

byte ESATClock::BCDToBinary(byte value)
{
  return value - 6 * (value >> 4);
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
  byte rawTime[7];
  (void) I2C.read(address, timeRegister, rawTime, sizeof(rawTime));
  String timestamp = "20"
                   + format(rawTime[6], 2)
                   + format(rawTime[5], 2)
                   + format(rawTime[4], 2)
                   + format(rawTime[2], 2)
                   + format(rawTime[1], 2)
                   + format(rawTime[0] & 0x7F, 2);
  return timestamp;
}

void ESATClock::write(String time)
{
  const byte year = time.substring(0, 4).toInt() - 2000;
  const byte month = time.substring(4, 6).toInt();
  const byte day = time.substring(6, 8).toInt();
  const byte hours = time.substring(8, 10).toInt();
  const byte minutes = time.substring(10, 12).toInt();
  const byte seconds = time.substring(12, 14).toInt();
  byte timestamp[] = {
    binaryToBCD(seconds),
    binaryToBCD(minutes),
    binaryToBCD(hours),
    binaryToBCD(day),
    binaryToBCD(month),
    binaryToBCD(year),
    0
  };
  (void) I2C.write(address, timeRegister, timestamp, sizeof(timestamp));
}

ESATClock Clock;
