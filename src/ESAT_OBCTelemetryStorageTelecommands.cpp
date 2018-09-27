/*
 * Copyright (C) 2018 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_OBCTelemetryStorageTelecommands.h"
#include "ESAT_OBCSubsystem.h"
#include "ESAT_TelemetryStorage.h"

boolean ESAT_OBCTelemetryStorageTelecommandsClass::consume(ESAT_CCSDSPacket packet)
{
  const ESAT_CCSDSSecondaryHeader secondaryHeader =
    packet.readSecondaryHeader();
  switch (secondaryHeader.packetIdentifier)
  {
    case STORE_TELEMETRY:
      return handleStoreTelemetryTelecommand(packet);
      break;
    case DOWNLOAD_STORED_TELEMETRY:
      return handleDownloadStoredTelemetryTelecommand(packet);
      break;
    case ERASE_STORED_TELEMETRY:
      return handleEraseStoredTelemetryTelecommand(packet);
      break;
    default:
      return false;
      break;
  }
  return false;
}

boolean ESAT_OBCTelemetryStorageTelecommandsClass::handleStoreTelemetryTelecommand(ESAT_CCSDSPacket packet)
{
  const byte parameter = packet.readByte();
  if (packet.triedToReadBeyondLength())
  {
    return false;
  }
  else
  {
    if (parameter > 0)
    {
      ESAT_OBCSubsystem.storeTelemetry = true;
    }
    else
    {
      ESAT_OBCSubsystem.storeTelemetry = false;
    }
    return true;
  }
}

boolean ESAT_OBCTelemetryStorageTelecommandsClass::handleDownloadStoredTelemetryTelecommand(ESAT_CCSDSPacket packet)
{
  const ESAT_Timestamp beginTimestamp = packet.readTimestamp();
  const ESAT_Timestamp endTimestamp = packet.readTimestamp();
  if (packet.triedToReadBeyondLength())
  {
    return false;
  }
  else
  {
    ESAT_TelemetryStorage.beginReading(beginTimestamp, endTimestamp);
    return true;
  }
}

boolean ESAT_OBCTelemetryStorageTelecommandsClass::handleEraseStoredTelemetryTelecommand(ESAT_CCSDSPacket packet)
{
  (void) packet;
  ESAT_TelemetryStorage.erase();
  return true;
}

ESAT_OBCTelemetryStorageTelecommandsClass ESAT_OBCTelemetryStorageTelecommands;
