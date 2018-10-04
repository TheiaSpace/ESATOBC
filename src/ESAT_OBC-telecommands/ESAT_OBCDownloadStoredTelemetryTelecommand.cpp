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

#include "ESAT_OBC-telecommands/ESAT_OBCDownloadStoredTelemetryTelecommand.h"
#include "ESAT_OBC-peripherals/ESAT_TelemetryStorage.h"

boolean ESAT_OBCDownloadStoredTelemetryTelecommandClass::handleUserData(ESAT_CCSDSPacket packet)
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

byte ESAT_OBCDownloadStoredTelemetryTelecommandClass::packetIdentifier()
{
  return PACKET_IDENTIFIER;
}

ESAT_SemanticVersionNumber ESAT_OBCDownloadStoredTelemetryTelecommandClass::versionNumber()
{
  return ESAT_SemanticVersionNumber(MAJOR_VERSION_NUMBER,
                                    MINOR_VERSION_NUMBER,
                                    PATCH_VERSION_NUMBER);
}

ESAT_OBCDownloadStoredTelemetryTelecommandClass ESAT_OBCDownloadStoredTelemetryTelecommand;
