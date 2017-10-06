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
#include <MspFlash.h>
#include <Wire.h>
#include "ESATADCSSubsystem.h"
#include "ESATClock.h"
#include "ESATCOMMSSubsystem.h"
#include "ESATEPSSubsystem.h"
#include <ESATTimer.h>
#include "ESATStorage.h"
#include <ESATUtil.h>
#include <USBSerial.h>

#define flash SEGMENT_C

void ESATOBCSubsystem::begin()
{
  storeTelemetry = true;
  downloadStoredTelemetry = false;
  USB.begin();
  Serial.begin(115200);
  Storage.begin();
  Wire.begin();
  Clock.begin();
  loadIdentifier();
}

byte ESATOBCSubsystem::getStartOrder()
{
  return 0;
}

byte ESATOBCSubsystem::getSubsystemIdentifier()
{
  return 1;
}

void ESATOBCSubsystem::handleCommand(byte opcode, String parameters)
{
  switch (opcode)
  {
    case STORE_ID:
      handleStoreIdCommand(parameters);
      break;
    case SET_TIME:
      handleSetTimeCommand(parameters);
      break;
  case STORE_TELEMETRY:
      handleStoreTelemetry(parameters);
      break;
  case DOWNLOAD_TELEMETRY:
      handleDownloadTelemetry(parameters);
      break;
    default:
      break;
  }
}

void ESATOBCSubsystem::handleStoreIdCommand(String parameters)
{
  byte id = parameters.toInt();
  Flash.erase(flash);
  Flash.write(flash, &id, sizeof(id));
  identifier = id;
}

void ESATOBCSubsystem::handleSetTimeCommand(String parameters)
{
  Clock.write(parameters);
  Timestamp ts = Clock.read();
  char cts[ts.charTimestampLength];
  ts.toStringTimeStamp(cts);
  USB.println(cts);
}

void ESATOBCSubsystem::handleStoreTelemetry(String parameters)
{
  storeTelemetry = !!parameters.toInt();
}

void ESATOBCSubsystem::handleDownloadTelemetry(String parameters)
{
  static const byte timestampLength = 19;
  if(parameters.length() != timestampLength*2)
  {
    downloadStoredTelemetry = false;
    return;
  }
  if(downloadStoredTelemetryFromTimestamp.update(parameters.substring(0,timestampLength).c_str())
     == 
     downloadStoredTelemetryFromTimestamp.INVALID_TIMESTAMP)
  {
    downloadStoredTelemetry = false;
    return;    
  }
  if(downloadStoredTelemetryToTimestamp.update(parameters.substring(timestampLength,timestampLength*2).c_str()) 
     == 
     downloadStoredTelemetryToTimestamp.INVALID_TIMESTAMP)
  {
    downloadStoredTelemetry = false;
    return;     
  }
  if(downloadStoredTelemetryToTimestamp > downloadStoredTelemetryFromTimestamp)
  {
    downloadStoredTelemetry = true;  
    lastStoredTelemetryDownloadedTimestamp.update(downloadStoredTelemetryFromTimestamp);
    Storage.resetLinePosition();
    }
  else{
    downloadStoredTelemetry = false;
  }
}

byte ESATOBCSubsystem::loadIdentifier()
{
  Flash.read(flash, &identifier, sizeof(identifier));
  return identifier;
}

String ESATOBCSubsystem::readTelemetry()
{
  const unsigned int load = 100 * Timer.ellapsedMilliseconds() / Timer.period;
  const byte status =
    (Clock.alive << CLOCK_OFFSET)
    | (ADCSSubsystem.inertialMeasurementUnitAlive << IMU_OFFSET)
    | (EPSSubsystem.alive << EPS_OFFSET)
    | (Storage.alive << STORAGE_OFFSET)
    | ((COMMSSubsystem.status & COMMS_MASK) << COMMS_OFFSET);
  String telemetry = Util.intToHexadecimal(status)
    + Util.intToHexadecimal(load);
  return telemetry;
}

void ESATOBCSubsystem::update()
{
  static const byte MAX_TELEMETRY_SIZE = 255;
  char cdate[lastStoredTelemetryDownloadedTimestamp.charDateLength + 4] = "";
  char telemetry[MAX_TELEMETRY_SIZE + 1] = "";
  Timestamp TelemetryTimestamp;
  boolean telemetryFound = false;
  
  while(downloadStoredTelemetry){
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
          Storage.readLine(telemetry, MAX_TELEMETRY_SIZE);
          Storage.saveCurrentLinePosition();
          if(TelemetryTimestamp.update(telemetry) == TelemetryTimestamp.VALID_TIMESTAMP)
          {
            if(TelemetryTimestamp > lastStoredTelemetryDownloadedTimestamp)
            {
              lastStoredTelemetryDownloadedTimestamp.update(TelemetryTimestamp);
              if(downloadStoredTelemetryToTimestamp > lastStoredTelemetryDownloadedTimestamp)
              {
                // Update TM package with telemetrt string
                USB.println(telemetry);
                telemetryFound = true;
              }
              break;
            }
          }
        }
        Storage.closeReadFile();
        lastStoredTelemetryDownloadedTimestamp.incrementDay();
        Storage.resetLinePosition();        
      }
      else{
        lastStoredTelemetryDownloadedTimestamp.incrementDay();
        Storage.resetLinePosition();
      }
    }
    else{
      downloadStoredTelemetry = false;
    }
  }
  
  
}

ESATOBCSubsystem OBCSubsystem;
