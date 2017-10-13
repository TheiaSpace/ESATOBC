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

#include "ESATStorage.h"
#include "ESATTimestamp.h"
#include <SD.h>

const char ESATStorage::TELEMETRY_FILE[] = "telemetry";

void ESATStorage::begin()
{
  const boolean correctBegin = SD.begin(PIN);
  if (!correctBegin)
  {
    error = true;
  }
}

void ESATStorage::write(ESATCCSDSPacket& packet)
{
  File file = SD.open(TELEMETRY_FILE, FILE_WRITE);
  if (!file)
  {
    error = true;
    return;
  }
  const boolean correctSeek = file.seek(file.size());
  if (!correctSeek)
  {
    file.close();
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

ESATStorage Storage;
