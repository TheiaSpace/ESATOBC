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

#include "ESAT_OBCSubsystem.h"
#include "ESAT_OBCClock.h"
#include "ESAT_TelemetryStorage.h"
#include <ESAT_Timer.h>
#include <ESAT_Timestamp.h>

void ESAT_OBCSubsystemClass::begin()
{
  newHousekeepingTelemetryPacket = false;
  downloadStoredTelemetry = false;
  telemetryPacketSequenceCount = 0;
  storeTelemetry = false;
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
  downloadStoredTelemetry = true;
}

void ESAT_OBCSubsystemClass::handleEraseStoredTelemetry(ESAT_CCSDSPacket& packet)
{
  (void) packet;
  ESAT_TelemetryStorage.erase();
}

boolean ESAT_OBCSubsystemClass::readHousekeepingTelemetry(ESAT_CCSDSPacket& packet)
{
  newHousekeepingTelemetryPacket = false;
  const byte userDataLength = 4;
  if (packet.capacity() < (ESAT_CCSDSSecondaryHeader::LENGTH + userDataLength))
  {
    return false;
  }
  packet.writeTelemetryHeaders(getApplicationProcessIdentifier(),
                               telemetryPacketSequenceCount,
                               ESAT_OBCClock.read(),
                               MAJOR_VERSION_NUMBER,
                               MINOR_VERSION_NUMBER,
                               PATCH_VERSION_NUMBER,
                               HOUSEKEEPING);
  packet.writeByte(ESAT_Timer.load());
  packet.writeBoolean(storeTelemetry);
  packet.writeBoolean(ESAT_TelemetryStorage.error);
  ESAT_TelemetryStorage.error = false;
  packet.writeBoolean(ESAT_OBCClock.error);
  ESAT_OBCClock.error = false;
  telemetryPacketSequenceCount = telemetryPacketSequenceCount + 1;
  return true;
}

boolean ESAT_OBCSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  (void) packet;
  return false;
}

boolean ESAT_OBCSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
  if (newHousekeepingTelemetryPacket)
  {
    return readHousekeepingTelemetry(packet);
  }
  if (downloadStoredTelemetry)
  {
    return readStoredTelemetry(packet);
  }
  (void) packet;
  return false;
}

boolean ESAT_OBCSubsystemClass::readStoredTelemetry(ESAT_CCSDSPacket& packet)
{
  const boolean correctRead = ESAT_TelemetryStorage.read(packet);
  if (!correctRead)
  {
    ESAT_TelemetryStorage.endReading();
    downloadStoredTelemetry = false;
  }
  return correctRead;
}

boolean ESAT_OBCSubsystemClass::telemetryAvailable()
{
  if (newHousekeepingTelemetryPacket)
  {
    return true;
  }
  if (downloadStoredTelemetry)
  {
    return true;
  }
  return false;
}

void ESAT_OBCSubsystemClass::update()
{
  newHousekeepingTelemetryPacket = true;
}

void ESAT_OBCSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
  if (storeTelemetry && !downloadStoredTelemetry)
  {
    ESAT_TelemetryStorage.write(packet);
  }
}

ESAT_OBCSubsystemClass ESAT_OBCSubsystem;
