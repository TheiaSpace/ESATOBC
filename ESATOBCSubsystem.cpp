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
  case DOWNLOAD_TELEMETRY:
      handleDownloadTelemetry(packet);
      break;
    default:
      break;
  }
}

void ESATOBCSubsystem::handleSetTimeCommand(ESATCCSDSPacket& packet)
{
  if(packet.readBytesAvailable() < 6)
  {
    return;
  }
  ESATTimestamp Timestamp;
  Timestamp.year = packet.readByte();
  Timestamp.month = packet.readByte();
  Timestamp.day = packet.readByte();
  Timestamp.hours = packet.readByte();
  Timestamp.minutes = packet.readByte();
  Timestamp.seconds = packet.readByte();
  Clock.write(Timestamp);
}

void ESATOBCSubsystem::handleStoreTelemetry(ESATCCSDSPacket& packet)
{
  if(packet.readBytesAvailable() < 1)
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
  if(packet.readBytesAvailable() < 12)
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
}

boolean ESATOBCSubsystem::readTelemetry(ESATCCSDSPacket& packet)
{
  boolean result;
  packet.clear();
  
  if(newHousekeepingTelemetryPacket)
  {
    newHousekeepingTelemetryPacket = false;
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
    packet.writeBoolean(storeTelemetry);
    packet.writeBoolean(Storage.error);
    Storage.error = false;
    packet.writeBoolean(Clock.error);
    Clock.error = false;
    packet.updatePacketDataLength();
    if (packet.readPacketDataLength() > packet.packetDataBufferLength)
    {
      result = false;
    }
    result = true;
  }
  else
  {
    result = readStoredTelemetry(packet);
  }
  if(result)
  {
    telemetryPacketSequenceCount = telemetryPacketSequenceCount + 1;
  }
  return result;
}



boolean ESATOBCSubsystem::readStoredTelemetry(ESATCCSDSPacket& packet)
{
  word telemetryLength;
  ESATTimestamp TelemetryTimestamp;
  boolean telemetryFound = false;
  
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
          telemetryLength = Storage.readLine(&TelemetryTimestamp, packet.buffer, packet.bufferLength);
          Storage.saveCurrentLinePosition();
          if((telemetryLength > 0)&&(telemetryLength == packet.readPacketLength()))
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
    return true;
  }
  else
  {
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

ESATOBCSubsystem OBCSubsystem;
