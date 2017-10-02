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
  Wire.beginTransmission(address);
  Wire.write(timeRegister);
  const byte errorCode = Wire.endTransmission();
  if (errorCode != 0)
  {
    alive = false;
    return "0000-00-00T00:00:00";
  }
  const byte bytesToRead = 6;
  const byte bytesRead = Wire.requestFrom(address, bytesToRead);
  if (bytesRead != bytesToRead)
  {
    alive = false;
    return "0000-00-00T00:00:00";
  }
  const byte seconds = Wire.read();
  const byte minutes = Wire.read();
  const byte hours = Wire.read();
  const byte day = Wire.read();
  const byte month = Wire.read();
  const byte year = Wire.read();
  alive = true;
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

String ESATClock::getDateWithoutDashes(String timestamp){
  String date;
  if(timestamp.length() != timestampLength)
  {
    date = "00000000";
  }
  else
  {
    date = timestamp.substring(0, 10);
    date.replace("-","");
  }
  return date;
}

ESATClock Clock;

ESATTimeStamp::ESATTimeStamp(byte Hour, byte Minute, byte Second, 
                   byte Year, byte Month, byte Day)
{
  hours = Hour;
  minutes = Minute;
  seconds = Second;
  year = Year;
  month = Month;
  day = Day;  
}

ESATTimeStamp::ESATTimeStamp(String time)
{
  year = time.substring(0, 4).toInt() - 2000;
  month = time.substring(5, 7).toInt();
  day = time.substring(8, 10).toInt();
  hours = time.substring(11, 13).toInt();
  minutes = time.substring(14, 16).toInt();
  seconds = time.substring(17, 19).toInt();
}

String ESATTimeStamp::toStringTimeStamp()
{
  return "20"
    + Util.pad(String(year % 100), '0', 2)
    + "-"
    + Util.pad(String(month % 100), '0', 2)
    + "-"
    + Util.pad(String(day % 100), '0', 2)
    + "T"
    + Util.pad(String(hours % 100), '0', 2)
    + ":"
    + Util.pad(String(minutes % 100), '0', 2)
    + ":"
    + Util.pad(String(seconds % 100), '0', 2);
  
}
boolean ESATTimeStamp::isHigherThan(ESATTimeStamp timeStamp)
{
  if(timeStamp.year > year)
  {
    return false;
  }
  else if(timeStamp.month > month)
  {
    return false;
  }
  else if(timeStamp.day > day)
  {
    return false;
  }
  else if(timeStamp.hours > hours)
  {
    return false;
  }
  else if(timeStamp.minutes > minutes)
  {
    return false;
  }
  else if(timeStamp.seconds > seconds)
  {
    return false;
  }
  else
  {
    return true; 
  }
}



