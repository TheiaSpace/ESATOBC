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
#include "ESATTimestamp.h"
#include "ESATClock.h"
#include "ESATCOMMSSubsystem.h"
#include "ESATEPSSubsystem.h"
#include <ESATTimer.h>
#include "ESATStorage.h"
#include <ESATUtil.h>
#include <USBSerial.h>

void ESATOBCSubsystem::begin()
{
  newHousekeepingTelemetryPacket = false;
  telemetryPacketSequenceCount = 0;
  storeTelemetry = false;
  downloadStoredTelemetry = false;
  Storage.begin();
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
  if(packet.availableBytesToRead() < 14)
  {
    downloadStoredTelemetry = false;
    return;
  }
  downloadStoredTelemetryFromTimestamp.year = packet.readWord() - 2000;
  downloadStoredTelemetryFromTimestamp.month = packet.readByte();
  downloadStoredTelemetryFromTimestamp.day = packet.readByte();
  downloadStoredTelemetryFromTimestamp.hours = packet.readByte();
  downloadStoredTelemetryFromTimestamp.minutes = packet.readByte();
  downloadStoredTelemetryFromTimestamp.seconds = packet.readByte();

  downloadStoredTelemetryToTimestamp.year = packet.readWord() - 2000;
  downloadStoredTelemetryToTimestamp.month = packet.readByte();
  downloadStoredTelemetryToTimestamp.day = packet.readByte();
  downloadStoredTelemetryToTimestamp.hours = packet.readByte();
  downloadStoredTelemetryToTimestamp.minutes = packet.readByte();
  downloadStoredTelemetryToTimestamp.seconds = packet.readByte();

  downloadStoredTelemetry = true;
  Storage.resetLinePosition();
  downloadStoredTelemetryUpdated = true;
}

boolean ESATOBCSubsystem::readTelecommand(ESATCCSDSPacket& packet)
{
  return false;
}

boolean ESATOBCSubsystem::readTelemetry(ESATCCSDSPacket& packet)
{
  boolean result;

  if(newHousekeepingTelemetryPacket)
  {
    newHousekeepingTelemetryPacket = false;
    ESATTimestamp Timestamp = Clock.read();
    if(Clock.error)
    {
      return false;
    }
    // Primary and secondary header
    prepareNewPacket(Timestamp, packet, packet.TELEMETRY, HOUSEKEEPING);
    // User data
    const unsigned int load = 100 * Timer.ellapsedMilliseconds() / Timer.period;
    packet.writeByte(load);
    packet.writeWord(telemetryPacketSequenceCount - 1);
    packet.writeBoolean(storeTelemetry);
    packet.writeBoolean(Storage.error);
    Storage.error = false;
    packet.writeBoolean(Clock.error);
    Clock.error = false;
    // End of user data
    result = closePacket(packet);
  }
  else
  {
    result = readStoredTelemetry(packet);
  }
  return result;
}



boolean ESATOBCSubsystem::readStoredTelemetry(ESATCCSDSPacket& packet)
{
  word telemetryLength;
  ESATTimestamp TelemetryTimestamp;
  boolean telemetryFound = false;
  static boolean previousStoreTelemetryValue;

  if(downloadStoredTelemetryUpdated)
  {
    previousStoreTelemetryValue = storeTelemetry;
    downloadStoredTelemetryUpdated = false;
  }
  while(downloadStoredTelemetry)
  {
    if(downloadStoredTelemetryToTimestamp >= downloadStoredTelemetryFromTimestamp)
    {
      if(Storage.fileExists(downloadStoredTelemetryFromTimestamp))
      {
        Storage.openReadFile(downloadStoredTelemetryFromTimestamp);
        Storage.goToSavedPosition();
        while(Storage.available())
        {
          telemetryLength = Storage.read(&TelemetryTimestamp, packet);
          Storage.saveCurrentLinePosition();
          if(telemetryLength > 0)
          {
            if(telemetryLength == packet.readPacketDataLength() + packet.PRIMARY_HEADER_LENGTH)
            {
              if(TelemetryTimestamp >= downloadStoredTelemetryFromTimestamp)
              {
                if(downloadStoredTelemetryToTimestamp >= TelemetryTimestamp)
                {
                  // Gestionar paquete
                  telemetryFound = true;
                  break;
                }
              }
            }
          }
        }
        Storage.closeReadFile();
        if (telemetryFound)
        {
          break;
        }
      }
      downloadStoredTelemetryFromTimestamp.incrementDay();
      Storage.resetLinePosition();
    }
    else{
      packet.clear();
      downloadStoredTelemetry = false;
    }
  }
  if(telemetryFound)
  {
    storeTelemetry = false;
    return true;
  }
  else
  {
    storeTelemetry = previousStoreTelemetryValue;
    return false;
  }
}

boolean ESATOBCSubsystem::telemetryAvailable()
{
  return newHousekeepingTelemetryPacket || downloadStoredTelemetry;
}

void ESATOBCSubsystem::update()
{
  newHousekeepingTelemetryPacket = true;
}

void ESATOBCSubsystem::writeTelemetry(ESATCCSDSPacket& packet)
{
  if (storeTelemetry)
  {
    Storage.write(Clock.read(), packet);
  }
}

void ESATOBCSubsystem::prepareNewPacket(ESATTimestamp Timestamp, ESATCCSDSPacket& packet, byte packetType, byte packetID)
{
  packet.clear();
  // Primary header
  packet.writePacketVersionNumber(0);
  switch(packetType)
  {
    case packet.TELEMETRY:
      packet.writePacketType((ESATCCSDSPacket::PacketType)packetType);
      packet.writePacketSequenceCount(telemetryPacketSequenceCount);
      break;
    case packet.TELECOMMAND:
      packet.writePacketType((ESATCCSDSPacket::PacketType)packetType);
      packet.writePacketSequenceCount(0);
      break;
  }
  packet.writeSecondaryHeaderFlag(packet.SECONDARY_HEADER_IS_PRESENT);
  packet.writeApplicationProcessIdentifier(getApplicationProcessIdentifier());
  packet.writeSequenceFlags(packet.UNSEGMENTED_USER_DATA);
  // Secondary header
  ESATCCSDSSecondaryHeader secondaryHeader;
  secondaryHeader.preamble =
    secondaryHeader.CALENDAR_SEGMENTED_TIME_CODE_MONTH_DAY_VARIANT_1_SECOND_RESOLUTION;
  secondaryHeader.timestamp = Timestamp;
  secondaryHeader.majorVersionNumber = MAJOR_VERSION_NUMBER;
  secondaryHeader.minorVersionNumber = MINOR_VERSION_NUMBER;
  secondaryHeader.patchVersionNumber = PATCH_VERSION_NUMBER;
  secondaryHeader.packetIdentifier = packetID;
  packet.writeSecondaryHeader(secondaryHeader);
}
boolean ESATOBCSubsystem::closePacket(ESATCCSDSPacket& packet)
{
  packet.updatePacketDataLength();
  if (packet.readPacketDataLength() > packet.packetDataBufferLength)
  {
    return false;
  }
  telemetryPacketSequenceCount = telemetryPacketSequenceCount + 1;
  return true;
}


ESATOBCSubsystem OBCSubsystem;
