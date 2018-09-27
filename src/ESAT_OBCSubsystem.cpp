/*
 * Copyright (C) 2017, 2018 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_OBCSubsystem.h"
#include "ESAT_OBCClock.h"
#include "ESAT_OBCHousekeepingTelemetry.h"
#include "ESAT_OBCLED.h"
#include "ESAT_OBCLinesTelemetry.h"
#include "ESAT_TelemetryStorage.h"
#include <ESAT_Timer.h>
#include <ESAT_Timestamp.h>

void ESAT_OBCSubsystemClass::begin()
{
  storeTelemetry = false;
  telemetryPacketBuilder =
    ESAT_CCSDSTelemetryPacketBuilder(getApplicationProcessIdentifier(),
                                     MAJOR_VERSION_NUMBER,
                                     MINOR_VERSION_NUMBER,
                                     PATCH_VERSION_NUMBER,
                                     ESAT_OBCClock);
  enabledTelemetry.clearAll();
  pendingTelemetry.clearAll();
  telemetryPacketBuilder.add(ESAT_OBCHousekeepingTelemetry);
  telemetryPacketBuilder.add(ESAT_OBCLinesTelemetry);
  enabledTelemetry.set(ESAT_OBCHousekeepingTelemetry.packetIdentifier());
  enabledTelemetry.clear(ESAT_OBCLinesTelemetry.packetIdentifier());
  ESAT_OBCLED.begin();
}

void ESAT_OBCSubsystemClass::disableTelemetry(const byte identifier)
{
  enabledTelemetry.clear(identifier);
}

void ESAT_OBCSubsystemClass::enableTelemetry(const byte identifier)
{
  enabledTelemetry.set(identifier);
}

word ESAT_OBCSubsystemClass::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

void ESAT_OBCSubsystemClass::handleTelecommand(ESAT_CCSDSPacket& packet)
{
  packet.rewind();
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  if (primaryHeader.applicationProcessIdentifier
      != getApplicationProcessIdentifier())
  {
    return;
  }
  if (primaryHeader.packetType != primaryHeader.TELECOMMAND)
  {
    return;
  }
  if (primaryHeader.packetDataLength < ESAT_CCSDSSecondaryHeader::LENGTH)
  {
    return;
  }
  const ESAT_CCSDSSecondaryHeader secondaryHeader =
    packet.readSecondaryHeader();
  if (secondaryHeader.majorVersionNumber < MAJOR_VERSION_NUMBER)
  {
    return;
  }
  switch (secondaryHeader.packetIdentifier)
  {
    case SET_TIME:
      handleSetTimeCommand(packet);
      break;
    case STORE_TELEMETRY:
      handleStoreTelemetry(packet);
      break;
    case DOWNLOAD_STORED_TELEMETRY:
      handleDownloadStoredTelemetry(packet);
      break;
    case ERASE_STORED_TELEMETRY:
      handleEraseStoredTelemetry(packet);
      break;
    case ENABLE_TELEMETRY:
      handleEnableTelemetry(packet);
      break;
    case DISABLE_TELEMETRY:
      handleDisableTelemetry(packet);
      break;
    default:
      break;
  }
}

void ESAT_OBCSubsystemClass::handleSetTimeCommand(ESAT_CCSDSPacket& packet)
{
  if (packet.available() < 7)
  {
    return;
  }
  ESAT_OBCClock.write(packet.readTimestamp());
}

void ESAT_OBCSubsystemClass::handleStoreTelemetry(ESAT_CCSDSPacket& packet)
{
  if (packet.available() < 1)
  {
    return;
  }
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

void ESAT_OBCSubsystemClass::handleDownloadStoredTelemetry(ESAT_CCSDSPacket& packet)
{
  const ESAT_Timestamp beginTimestamp = packet.readTimestamp();
  const ESAT_Timestamp endTimestamp = packet.readTimestamp();
  ESAT_TelemetryStorage.beginReading(beginTimestamp, endTimestamp);
}

void ESAT_OBCSubsystemClass::handleEraseStoredTelemetry(ESAT_CCSDSPacket& packet)
{
  (void) packet;
  ESAT_TelemetryStorage.erase();
}

void ESAT_OBCSubsystemClass::handleEnableTelemetry(ESAT_CCSDSPacket& packet)
{
  if (packet.available() < 1)
  {
    return;
  }
  const byte identifier = packet.readByte();
  enabledTelemetry.set(identifier);
}

void ESAT_OBCSubsystemClass::handleDisableTelemetry(ESAT_CCSDSPacket& packet)
{
  if (packet.available() < 1)
  {
    return;
  }
  const byte identifier = packet.readByte();
  enabledTelemetry.clear(identifier);
}

boolean ESAT_OBCSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  (void) packet;
  return false;
}

boolean ESAT_OBCSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
  if (pendingTelemetry.available() > 0)
  {
    const byte identifier = byte(pendingTelemetry.readNext());
    pendingTelemetry.clear(identifier);
    return telemetryPacketBuilder.build(packet, identifier);
  }
  return readStoredTelemetry(packet);
  return false;
}

boolean ESAT_OBCSubsystemClass::readStoredTelemetry(ESAT_CCSDSPacket& packet)
{
  const boolean correctRead = ESAT_TelemetryStorage.read(packet);
  if (!correctRead)
  {
    ESAT_TelemetryStorage.endReading();
  }
  return correctRead;
}

boolean ESAT_OBCSubsystemClass::telemetryAvailable()
{
  if (pendingTelemetry.available() > 0)
  {
    return true;
  }
  if (ESAT_TelemetryStorage.reading())
  {
    return true;
  }
  return false;
}

void ESAT_OBCSubsystemClass::update()
{
  const ESAT_FlagContainer availableTelemetry =
    telemetryPacketBuilder.available();
  const ESAT_FlagContainer availableAndEnabledTelemetry =
    availableTelemetry & enabledTelemetry;
  pendingTelemetry = availableAndEnabledTelemetry;
  ESAT_OBCLED.toggle();
}

void ESAT_OBCSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
  if (storeTelemetry && !ESAT_TelemetryStorage.reading())
  {
    ESAT_TelemetryStorage.write(packet);
  }
}

ESAT_OBCSubsystemClass ESAT_OBCSubsystem;
