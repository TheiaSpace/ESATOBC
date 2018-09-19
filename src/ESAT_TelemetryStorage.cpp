/*
 * Copyright (C) 2017-2018 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_TelemetryStorage.h"
#include <ESAT_CCSDSPacketFromKISSFrameReader.h>
#include <ESAT_CCSDSPacketToKISSFrameWriter.h>

const char ESAT_TelemetryStorageClass::TELEMETRY_FILE[] = "telem_db";

void ESAT_TelemetryStorageClass::beginReading(const ESAT_Timestamp begin,
                                              const ESAT_Timestamp end)
{
  beginTimestamp = begin;
  endTimestamp = end;
}

void ESAT_TelemetryStorageClass::endReading()
{
  file.close();
}

void ESAT_TelemetryStorageClass::erase()
{
  const boolean correctRemoval = SD.remove((char*) TELEMETRY_FILE);
  if (!correctRemoval)
  {
    error = true;
  }
}

boolean ESAT_TelemetryStorageClass::read(ESAT_CCSDSPacket& packet)
{
  if (!file)
  {
    file = SD.open(TELEMETRY_FILE, FILE_READ);
    if (!file)
    {
      error = true;
      return false;
    }
  }
  const unsigned long bufferLength =
    ESAT_KISSStream::frameLength(ESAT_CCSDSPrimaryHeader::LENGTH
                                 + packet.capacity());
  byte buffer[bufferLength];
  ESAT_CCSDSPacketFromKISSFrameReader reader(file, buffer, bufferLength);
  while (file.available() > 0)
  {
    const boolean correctPacket = reader.read(packet);
    if (!correctPacket)
    {
      error = true;
      return false;
    }
    packet.rewind();
    const ESAT_CCSDSSecondaryHeader secondaryHeader =
      packet.readSecondaryHeader();
    packet.rewind();
    if ((beginTimestamp <= secondaryHeader.timestamp)
        && (secondaryHeader.timestamp <= endTimestamp))
    {
      return true;
    }
  }
  return false;
}

void ESAT_TelemetryStorageClass::write(ESAT_CCSDSPacket& packet)
{
  if (file)
  {
    error = true;
    return;
  }
  file = SD.open(TELEMETRY_FILE, FILE_APPEND);
  if (!file)
  {
    error = true;
    return;
  }
  ESAT_CCSDSPacketToKISSFrameWriter writer(file);
  const boolean correctWrite = writer.unbufferedWrite(packet);
  if (!correctWrite)
  {
    error = true;
  }
  file.close();
}

ESAT_TelemetryStorageClass ESAT_TelemetryStorage;
