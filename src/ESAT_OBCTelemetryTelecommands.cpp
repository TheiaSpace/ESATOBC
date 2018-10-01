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

#include "ESAT_OBCTelemetryTelecommands.h"
#include "ESAT_OBCSubsystem.h"

const ESAT_SemanticVersionNumber ESAT_OBCTelemetryTelecommandsClass::INTERFACE_VERSION_NUMBER(4, 1, 0);

boolean ESAT_OBCTelemetryTelecommandsClass::consume(ESAT_CCSDSPacket packet)
{
  const ESAT_CCSDSSecondaryHeader secondaryHeader =
    packet.readSecondaryHeader();
  if (!INTERFACE_VERSION_NUMBER.isForwardCompatibleWith(secondaryHeader.majorVersionNumber,
                                                        secondaryHeader.minorVersionNumber,
                                                        secondaryHeader.patchVersionNumber))
  {
    return false;
  }
  switch (secondaryHeader.packetIdentifier)
  {
    case ENABLE_TELEMETRY:
      return handleEnableTelemetryTelecommand(packet);
      break;
    case DISABLE_TELEMETRY:
      return handleDisableTelemetryTelecommand(packet);
      break;
    default:
      return false;
      break;
  }
  return false;
}

boolean ESAT_OBCTelemetryTelecommandsClass::handleEnableTelemetryTelecommand(ESAT_CCSDSPacket packet)
{
  const byte identifier = packet.readByte();
  if (packet.triedToReadBeyondLength())
  {
    (void) identifier; // Unused.
    return false;
  }
  else
  {
    ESAT_OBCSubsystem.enableTelemetry(identifier);
    return true;
  }
}

boolean ESAT_OBCTelemetryTelecommandsClass::handleDisableTelemetryTelecommand(ESAT_CCSDSPacket packet)
{
  const byte identifier = packet.readByte();
  if (packet.triedToReadBeyondLength())
  {
    (void) identifier; // Unused;
    return false;
  }
  else
  {
    ESAT_OBCSubsystem.disableTelemetry(identifier);
    return true;
  }
}

ESAT_OBCTelemetryTelecommandsClass ESAT_OBCTelemetryTelecommands;
