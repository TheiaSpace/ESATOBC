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

#include "ESATOBCSubsystem.h"
#include <Wire.h>
#include "ESATADCSSubsystem.h"
#include "ESATClock.h"
#include "ESATCOMMSSubsystem.h"
#include "ESATEPSSubsystem.h"
#include <ESATTimer.h>
#include "ESATStorage.h"
#include <ESATUtil.h>
#include <USBSerial.h>

void ESATOBCSubsystem::begin()
{
  newTelemetryPacket = false;
  telemetryPacketSequenceCount = 0;
  storeTelemetry = false;
  USB.begin();
  Serial.begin(115200);
  Storage.begin();
  Wire.begin();
  Clock.begin();
}

word ESATOBCSubsystem::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

void ESATOBCSubsystem::handleTelecommand(ESATCCSDSPacket& packet)
{
  packet.rewind();
  const byte majorVersionNumber = packet.readByte();
  const byte minorVersionNumber = packet.readByte();
  const byte patchVersionNumber = packet.readByte();
  if (majorVersionNumber < MAJOR_VERSION_NUMBER)
  {
    return;
  }
  const byte commandCode = packet.readByte();
  switch (commandCode)
  {
    case SET_TIME:
      handleSetTimeCommand(packet);
      break;
  case STORE_TELEMETRY:
      handleStoreTelemetry(packet);
      break;
    default:
      break;
  }
}

void ESATOBCSubsystem::handleSetTimeCommand(ESATCCSDSPacket& packet)
{
}

void ESATOBCSubsystem::handleStoreTelemetry(ESATCCSDSPacket& packet)
{
  const byte parameter = packet.readByte();
  if (parameter > 0)
  {
    storeTelemetry = true;
  }
  else
  {
    storeTelemetry = false;
  }
}

void ESATOBCSubsystem::readTelemetry(ESATCCSDSPacket& packet)
{
  newTelemetryPacket = false;
  packet.clear();
  packet.writePacketVersionNumber(0);
  packet.writePacketType(packet.TELEMETRY);
  packet.writeSecondaryHeaderFlag(packet.SECONDARY_HEADER_IS_PRESENT);
  packet.writeApplicationProcessIdentifier(getApplicationProcessIdentifier());
  packet.writeSequenceFlags(packet.UNSEGMENTED_USER_DATA);
  packet.writePacketSequenceCount(telemetryPacketSequenceCount);
  packet.writeByte(MAJOR_VERSION_NUMBER);
  packet.writeByte(MINOR_VERSION_NUMBER);
  packet.writeByte(PATCH_VERSION_NUMBER);
  packet.writeByte(HOUSEKEEPING);
  const unsigned int load = 100 * Timer.ellapsedMilliseconds() / Timer.period;
  packet.writeByte(load);
  if (storeTelemetry)
  {
    packet.writeByte(1);
  }
  else
  {
    packet.writeByte(0);
  }
  telemetryPacketSequenceCount = telemetryPacketSequenceCount + 1;
}

boolean ESATOBCSubsystem::telemetryAvailable()
{
  return newTelemetryPacket;
}

void ESATOBCSubsystem::update()
{
  newTelemetryPacket = true;
}

ESATOBCSubsystem OBCSubsystem;
