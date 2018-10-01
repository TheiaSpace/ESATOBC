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

#include "ESAT_OBC-telecommands/ESAT_OBCEraseStoredTelemetryTelecommand.h"
#include "ESAT_TelemetryStorage.h"

const ESAT_SemanticVersionNumber ESAT_OBCEraseStoredTelemetryTelecommandClass::INTERFACE_VERSION_NUMBER(4, 0, 0);

boolean ESAT_OBCEraseStoredTelemetryTelecommandClass::accept(const ESAT_CCSDSSecondaryHeader secondaryHeader) const
{
  if (!INTERFACE_VERSION_NUMBER.isForwardCompatibleWith(secondaryHeader.majorVersionNumber,
                                                        secondaryHeader.minorVersionNumber,
                                                        secondaryHeader.patchVersionNumber))
  {
    return false;
  }
  if (secondaryHeader.packetIdentifier != ERASE_STORED_TELEMETRY)
  {
    return false;
  }
  return true;
}

boolean ESAT_OBCEraseStoredTelemetryTelecommandClass::consume(ESAT_CCSDSPacket packet)
{
  const ESAT_CCSDSSecondaryHeader secondaryHeader =
    packet.readSecondaryHeader();
  if (accept(secondaryHeader))
  {
    return handle(packet);
  }
  else
  {
    return false;
  }
}

boolean ESAT_OBCEraseStoredTelemetryTelecommandClass::handle(const ESAT_CCSDSPacket packet) const
{
  (void) packet;
  ESAT_TelemetryStorage.erase();
  return true;
}

ESAT_OBCEraseStoredTelemetryTelecommandClass ESAT_OBCEraseStoredTelemetryTelecommand;
