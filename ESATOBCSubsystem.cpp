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
  lastStoredTelemetryDownloadedTimestamp.year = packet.readByte();
  lastStoredTelemetryDownloadedTimestamp.month = packet.readByte();
  lastStoredTelemetryDownloadedTimestamp.day = packet.readByte();
  lastStoredTelemetryDownloadedTimestamp.hours = packet.readByte();
  lastStoredTelemetryDownloadedTimestamp.minutes = packet.readByte();
  lastStoredTelemetryDownloadedTimestamp.seconds = packet.readByte();
  
  downloadStoredTelemetryToTimestamp.year = packet.readByte();
  downloadStoredTelemetryToTimestamp.month = packet.readByte();
  downloadStoredTelemetryToTimestamp.day = packet.readByte();
  downloadStoredTelemetryToTimestamp.hours = packet.readByte();
  downloadStoredTelemetryToTimestamp.minutes = packet.readByte();
  downloadStoredTelemetryToTimestamp.seconds = packet.readByte();
  
  if(downloadStoredTelemetryToTimestamp > lastStoredTelemetryDownloadedTimestamp)
  {
    downloadStoredTelemetry = true;  
    Storage.resetLinePosition();
    }
  else{
    downloadStoredTelemetry = false;
  }
}

void ESATOBCSubsystem::readTelemetry(ESATCCSDSPacket& packet)
{
  packet.clear();
  
  if(newHousekeepingTelemetryPacket)
  {
    newHousekeepingTelemetryPacket = false;
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
    packet.writeBoolean(storeTelemetry);
    packet.writeBoolean(Storage.error);
    Storage.error = false;
    packet.writeBoolean(Clock.error);
    Clock.error = false;
    telemetryPacketSequenceCount = telemetryPacketSequenceCount + 1;
  }
  else
  {
    readStoredTelemetry(packet);    
  }    
  
  
  
}

void ESATOBCSubsystem::readStoredTelemetry(ESATCCSDSPacket& packet)
{
  static const byte MAX_TELEMETRY_SIZE = packet.bufferLength;
  char cdate[lastStoredTelemetryDownloadedTimestamp.charTimestampLength] = "";
  // char telemetry[MAX_TELEMETRY_SIZE*2 + 1] = "";
  word telemetryLength;
  ESATTimestamp TelemetryTimestamp;
  boolean telemetryFound = false;
  
  while(downloadStoredTelemetry)
  {
    if(downloadStoredTelemetryToTimestamp > lastStoredTelemetryDownloadedTimestamp)
    {
      lastStoredTelemetryDownloadedTimestamp.getDateWithoutDashes(cdate);
      strcat(cdate, ".txt");
      if(Storage.fileExists(cdate))
      {
        Storage.openReadFile(cdate);
        Storage.goToSavedPosition();
        while(Storage.available())
        {
          // telemetryLength = Storage.readLine(cdate, telemetry, MAX_TELEMETRY_SIZE);
          Storage.saveCurrentLinePosition();
          if(TelemetryTimestamp.update(cdate) == TelemetryTimestamp.VALID_TIMESTAMP)
          {
            if(TelemetryTimestamp > lastStoredTelemetryDownloadedTimestamp)
            {
              lastStoredTelemetryDownloadedTimestamp.update(TelemetryTimestamp);
              if(downloadStoredTelemetryToTimestamp > lastStoredTelemetryDownloadedTimestamp)
              {
                // Habria que resolver como mandar la fecha de la telemetria (guardada en cdate)
                // packet.writeCharPacket(telemetry);
                telemetryFound = true;
              }
              else
              {
                downloadStoredTelemetry = false;
              }
              break;
            }
          }
        }
        Storage.closeReadFile();
        if (telemetryFound)
        {
          break;
        }      
      }
      lastStoredTelemetryDownloadedTimestamp.incrementDay();
      Storage.resetLinePosition();
    }
    else{
      downloadStoredTelemetry = false;
    }
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
