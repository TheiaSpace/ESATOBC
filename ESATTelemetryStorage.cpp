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

void ESATTelemetryStorage::beginReading()
{
  if (file)
  {
    error = true;
    return;
  }
  file = SD.open(TELEMETRY_FILE, FILE_READ);
  if (!file)
  {
    error = true;
  }
}

void ESATTelemetryStorage::beginWriting()
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
  }
}

void ESATTelemetryStorage::endReading()
{
  file.close();
}

void ESATTelemetryStorage::endWriting()
{
  file.close();
}

boolean ESATTelemetryStorage::read(ESATCCSDSPacket& packet)
{
  const boolean correctRead = packet.readFrom(file);
  if (!correctRead)
  {
    error = true;
  }
  return correctRead;
}

void ESATTelemetryStorage::write(ESATCCSDSPacket& packet)
{
  const boolean correctWrite = packet.writeTo(file);
  if (!correctWrite)
  {
    error = true;
  }
}

ESATTelemetryStorage TelemetryStorage;
