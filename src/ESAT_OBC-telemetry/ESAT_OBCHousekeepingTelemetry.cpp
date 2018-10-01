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

#include "ESAT_OBC-telemetry/ESAT_OBCHousekeepingTelemetry.h"
#include "ESAT_OBC-peripherals/ESAT_OBCClock.h"
#include "ESAT_OBC-peripherals/ESAT_TelemetryStorage.h"
#include "ESAT_OBC-subsystems/ESAT_OBCSubsystem.h"
#include <ESAT_Timer.h>

boolean ESAT_OBCHousekeepingTelemetryClass::available()
{
  return true;
}

byte ESAT_OBCHousekeepingTelemetryClass::packetIdentifier()
{
  return PACKET_IDENTIFIER;
}

boolean ESAT_OBCHousekeepingTelemetryClass::fillUserData(ESAT_CCSDSPacket& packet)
{
  packet.writeByte(ESAT_Timer.load());
  packet.writeBoolean(ESAT_OBCSubsystem.storeTelemetry);
  packet.writeBoolean(ESAT_TelemetryStorage.error);
  ESAT_TelemetryStorage.error = false;
  packet.writeBoolean(ESAT_OBCClock.error);
  ESAT_OBCClock.error = false;
  if (packet.triedToWriteBeyondCapacity())
  {
    return false;
  }
  else
  {
    return true;
  }
}

ESAT_OBCHousekeepingTelemetryClass ESAT_OBCHousekeepingTelemetry;
