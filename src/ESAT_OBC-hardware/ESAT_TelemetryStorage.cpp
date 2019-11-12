/*
 * Copyright (C) 2017, 2018, 2019 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_OBC-hardware/ESAT_TelemetryStorage.h"
#include <ESAT_CCSDSPacketFromKISSFrameReader.h>
#include <ESAT_CCSDSPacketToKISSFrameWriter.h>

// The filename of the telemetry archive cannot exceed 8 characters
// due to filesystem limitations.
const char ESAT_TelemetryStorageClass::TELEMETRY_FILE[] = "telem_db";

void ESAT_TelemetryStorageClass::beginReading(const ESAT_Timestamp begin,
                                              const ESAT_Timestamp end)
{
  beginTimestamp = begin;
  endTimestamp = end;
  // We must open the telemetry archive in read mode.
  // Close it first if it is already open.
  if (file)
  {
    file.close();
  }
  file = SD.open(TELEMETRY_FILE, FILE_READ);
  readingInProgress = true;
}

void ESAT_TelemetryStorageClass::endReading()
{
  // We must close the telemetry archive when we are finished with
  // reading it so that it can be used for a new reading session
  // or for writing new packets.
  file.close();
  readingInProgress = false;
}

void ESAT_TelemetryStorageClass::erase()
{
  // Erasing the stored telemetry is as simple as removing the
  // telemetry archive.
  // A failure to remove the telemetry archive is a hardware error.
  const boolean correctRemoval = SD.remove((char*) TELEMETRY_FILE);
  if (!correctRemoval)
  {
    error = true;
  }
}

boolean ESAT_TelemetryStorageClass::read(ESAT_CCSDSPacket& packet)
{

  // If we didn't call beginReading(), we aren't ready to read
  // telemetry, but this in itself isn't a hardware error, so we don't
  // set the error flag.
  if (!readingInProgress)
  {
    return false;
  }
  // It is a hardware error if we couldn't open the telemetry archive.
  if (!file)
  {
    error = true;
    return false;
  }
  // If everything went well, we can try to read the next packet.
  // Instead of naked packets, we store them in KISS frames, so
  // we must extract packets from frames.
  const unsigned long bufferLength =
    ESAT_CCSDSPrimaryHeader::LENGTH + packet.capacity();
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

boolean ESAT_TelemetryStorageClass::reading() const
{
  return readingInProgress;
}

unsigned long ESAT_TelemetryStorageClass::size()
{
  if (file)
  {
    return file.size();
  }
  else
  {
    // If the telemetry store file isn't already open, just open it to
    // get its size and close it back to leave it as we found it.
    file = SD.open(TELEMETRY_FILE, FILE_READ);
    if (!file)
    {
      error = true;
      return 0;
    }
    else
    {
      const unsigned long fileSize = file.size();
      file.close();
      return fileSize;
    }
  }
}

void ESAT_TelemetryStorageClass::write(ESAT_CCSDSPacket& packet)
{
  // We cannot write telemetry packets to the telemetry archive while
  // we are reading it.
  if (readingInProgress)
  {
    return;
  }
  // The file shouldn't be open.
  if (file)
  {
    return;
  }
  // We open and close the telemetry archive every time we write a new
  // packet.  This is simpler than tracking the state of the file from
  // call to call.
  // A failure to open the file is a hardware error.
  file = SD.open(TELEMETRY_FILE, FILE_APPEND);
  if (!file)
  {
    error = true;
    return;
  }
  // We don't store naked packets, but KISS frames containing the
  // packets.  This helps when there is a small data corruption: if we
  // stored naked packets and the packet data length field of one
  // packet didn't match the actually stored packet data length, all
  // subsequent packets would be affected and couldn't be read, so
  // they would be as good as lost; with KISS frames, we limit the
  // data loss to the affected packet.
  ESAT_CCSDSPacketToKISSFrameWriter writer(file);
  const boolean correctWrite = writer.unbufferedWrite(packet);
  if (!correctWrite)
  {
    error = true;
  }
  file.close();
}

ESAT_TelemetryStorageClass ESAT_TelemetryStorage;
