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

#include "ESATTelemetryStorage.h"

const char ESATTelemetryStorage::TELEMETRY_FILE[] = "telemetry";

void ESATTelemetryStorage::beginReading(const ESATTimestamp begin,
                                        const ESATTimestamp end)
{
  beginTimestamp = begin;
  endTimestamp = end;
}

void ESATTelemetryStorage::endReading()
{
  file.close();
}

void ESATTelemetryStorage::erase()
{
  const boolean correctRemoval = SD.remove((char*) TELEMETRY_FILE);
  if (!correctRemoval)
  {
    error = true;
  }
}

boolean ESATTelemetryStorage::read(ESATCCSDSPacket& packet)
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
  while (file.available() > 0)
  {
    const boolean correctRead = packet.readFrom(file);
    if (!correctRead)
    {
      error = true;
      return false;
    }
    packet.rewind();
    const ESATCCSDSSecondaryHeader secondaryHeader =
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

void ESATTelemetryStorage::write(ESATCCSDSPacket& packet)
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
  const boolean correctWrite = packet.writeTo(file);
  if (!correctWrite)
  {
    error = true;
  }
  file.close();
}

ESATTelemetryStorage TelemetryStorage;
