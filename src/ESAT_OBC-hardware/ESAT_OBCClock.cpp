/*
 * Copyright (C) 2017, 2018 Theia Space, Universidad Politécnica de Madrid
 *
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

#include "ESAT_OBC-hardware/ESAT_OBCClock.h"
#include <ESAT_Util.h>
#include <Wire.h>

ESAT_Timestamp ESAT_OBCClockClass::read()
{
  // The on-board real-time clock stores the time in seven consecutive
  // registers starting at TIME_REGISTER in the following order:
  // - seconds (from 0 to 59);
  // - minutes (from 0 to 59);
  // - hours (from 0 to 23);
  // - day of week (we ignore it);
  // - day of month (from 1 to 31);
  // - month (from 1 to 12);
  // - year (from 0 to 99).
  // As this code was written during the first quarter of the XXI century,
  // we assume that the actual year goes from 2000 to 2099.  For a more
  // extended range of years, we would need to store the extra information
  // elsewhere.
  Wire.beginTransmission(ADDRESS);
  Wire.write(TIME_REGISTER);
  const byte errorCode = Wire.endTransmission();
  if (errorCode != 0)
  {
    error = true;
    return ESAT_Timestamp();
  }
  const byte bytesToRead = 7;
  const byte bytesRead = Wire.requestFrom(ADDRESS, bytesToRead);
  if (bytesRead != bytesToRead)
  {
    error = true;
    return ESAT_Timestamp();
  }
  const byte seconds = Wire.read();
  const byte minutes = Wire.read();
  const byte hours = Wire.read();
  const byte weekDay = Wire.read();
  (void) weekDay; // Unused.
  const byte day = Wire.read();
  const byte month = Wire.read();
  const byte year = Wire.read();
  return ESAT_Timestamp(2000 + ESAT_Util.decodeBinaryCodedDecimalByte(year),
                        ESAT_Util.decodeBinaryCodedDecimalByte(month),
                        ESAT_Util.decodeBinaryCodedDecimalByte(day),
                        ESAT_Util.decodeBinaryCodedDecimalByte(hours),
                        ESAT_Util.decodeBinaryCodedDecimalByte(minutes),
                        ESAT_Util.decodeBinaryCodedDecimalByte(seconds & 0x7F));
}

void ESAT_OBCClockClass::write(ESAT_Timestamp timestamp)
{
  // The on-board real-time clock stores the time in seven consecutive
  // registers starting at TIME_REGISTER in the following order:
  // - seconds (from 0 to 59);
  // - minutes (from 0 to 59);
  // - hours (from 0 to 23);
  // - day of week (we don't use it, so we write a dummy value);
  // - day of month (from 1 to 31);
  // - month (from 1 to 12);
  // - year (from 0 to 99).
  // As this code was written during the first quarter of the XXI century,
  // we assume that the actual year goes from 2000 to 2099.  For a more
  // extended range of years, we would need to store the extra information
  // elsewhere.
  Wire.beginTransmission(ADDRESS);
  Wire.write(TIME_REGISTER);
  Wire.write(ESAT_Util.encodeBinaryCodedDecimalByte(timestamp.seconds));
  Wire.write(ESAT_Util.encodeBinaryCodedDecimalByte(timestamp.minutes));
  Wire.write(ESAT_Util.encodeBinaryCodedDecimalByte(timestamp.hours));
  // Day of the week not used.
  Wire.write(ESAT_Util.encodeBinaryCodedDecimalByte(2));
  Wire.write(ESAT_Util.encodeBinaryCodedDecimalByte(timestamp.day));
  Wire.write(ESAT_Util.encodeBinaryCodedDecimalByte(timestamp.month));
  Wire.write(ESAT_Util.encodeBinaryCodedDecimalByte(timestamp.year % 100));
  Wire.write(0);
  const byte errorCode = Wire.endTransmission();
  if (errorCode != 0)
  {
    error = true;
  }
}

ESAT_OBCClockClass ESAT_OBCClock;
