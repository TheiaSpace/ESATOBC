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
#include "ESATTimestamp.h"
#include "ESATClock.h"
#include <ESATTimer.h>
#include <ESATTelemetryStorage.h>

void ESATOBCSubsystem::begin()
{
  newHousekeepingTelemetryPacket = false;
  telemetryPacketSequenceCount = 0;
  downloadTelemetry = false;
  storeTelemetry = false;
  Clock.begin();
}

word ESATOBCSubsystem::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

void ESATOBCSubsystem::handleTelecommand(ESATCCSDSPacket& packet)
{
  packet.rewind();
  if (packet.readApplicationProcessIdentifier()
      != APPLICATION_PROCESS_IDENTIFIER)
  {
    return;
  }
  if (packet.readPacketType() != packet.TELECOMMAND)
  {
    return;
  }
  if (packet.readPacketDataLength() < MINIMUM_COMMAND_PAYLOAD_DATA_LENGTH)
  {
    return;
  }
  const ESATCCSDSSecondaryHeader secondaryHeader = packet.readSecondaryHeader();
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
    case DOWNLOAD_TELEMETRY:
      handleDownloadTelemetry(packet);
      break;
    default:
      break;
  }
}

void ESATOBCSubsystem::handleSetTimeCommand(ESATCCSDSPacket& packet)
{
  if(packet.availableBytesToRead() < 7)
  {
    return;
  }
  ESATTimestamp Timestamp;
  Timestamp.year = packet.readWord() - 2000;
  Timestamp.month = packet.readByte();
  Timestamp.day = packet.readByte();
  Timestamp.hours = packet.readByte();
  Timestamp.minutes = packet.readByte();
  Timestamp.seconds = packet.readByte();
  Clock.write(Timestamp);
}

void ESATOBCSubsystem::handleStoreTelemetry(ESATCCSDSPacket& packet)
{
  if(packet.availableBytesToRead() < 1)
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

void ESATOBCSubsystem::handleDownloadTelemetry(ESATCCSDSPacket& packet)
{
  const ESATTimestamp beginTimestamp = packet.readTimestamp();
  const ESATTimestamp endTimestamp = packet.readTimestamp();
  ESATTelemetryStorage.beginReading(beginTimestamp, endTimestamp);
  downloadTelemetry = true;
}

boolean ESATOBCSubsystem::readHousekeepingTelemetry(ESATCCSDSPacket& packet)
{
  newHousekeepingTelemetryPacket = false;
  packet.writePacketVersionNumber(0);
  packet.writePacketType(packet.TELEMETRY);
  packet.writeSecondaryHeaderFlag(packet.SECONDARY_HEADER_IS_PRESENT);
  packet.writeSequenceFlags(packet.UNSEGMENTED_USER_DATA);
  packet.writePacketSequenceCount(telemetryPacketSequenceCount);
  ESATCCSDSSecondaryHeader secondaryHeader;
  secondaryHeader.preamble =
    secondaryHeader.CALENDAR_SEGMENTED_TIME_CODE_MONTH_DAY_VARIANT_1_SECOND_RESOLUTION;
  secondaryHeader.timestamp = Clock.read();
  secondaryHeader.majorVersionNumber = MAJOR_VERSION_NUMBER;
  secondaryHeader.minorVersionNumber = MINOR_VERSION_NUMBER;
  secondaryHeader.patchVersionNumber = PATCH_VERSION_NUMBER;
  secondaryHeader.packetIdentifier = HOUSEKEEPING;
  packet.writeSecondaryHeader(secondaryHeader);
  const unsigned int load = 100 * Timer.ellapsedMilliseconds() / Timer.period;
  packet.writeByte(load);
  packet.writeWord(telemetryPacketSequenceCount - 1);
  packet.writeBoolean(storeTelemetry);
  packet.writeBoolean(TelemetryStorage.error);
  TelemetryStorage.error = false;
  packet.writeBoolean(Clock.error);
  Clock.error = false;
  packet.updatePacketDataLength();
  telemetryPacketSequenceCount = telemetryPacketSequenceCount + 1;
  return true;
}

boolean ESATOBCSubsystem::readTelecommand(ESATCCSDSPacket& packet)
{
  return false;
}

boolean ESATOBCSubsystem::readTelemetry(ESATCCSDSPacket& packet)
{
  if (newHousekeepingTelemetryPacket)
  {
    return readHousekeepingTelemetry(packet);
  }
  if (downloadTelemetry)
  {
    return readStoredTelemetry(packet);
  }
}

boolean ESATOBCSubsystem::readStoredTelemetry(ESATCCSDSPacket& packet)
{
  const boolean correctRead = TelemetryStorage.read(packet);
  if (!correctRead)
  {
    TelemetryStorage.endReading();
    downloadTelemetry = false;
  }
  return correctRead;
}

boolean ESATOBCSubsystem::telemetryAvailable()
{
  if (newHousekeepingTelemetryPacket)
  {
    return true;
  }
  if (downloadTelemetry)
  {
    return true;
  }
  return false;
}

void ESATOBCSubsystem::update()
{
  newHousekeepingTelemetryPacket = true;
}

void ESATOBCSubsystem::writeTelemetry(ESATCCSDSPacket& packet)
{
  if (storeTelemetry)
  {
    TelemetryStorage.write(packet);
  }
}

ESATOBCSubsystem OBCSubsystem;
