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
#include "ESATADCSSubsystem.h"
#include "ESATClock.h"
#include "ESATCOMMSSubsystem.h"
#include "ESATEPSSubsystem.h"
#include <ESATI2C.h>
#include <ESATTimer.h>
#include "ESATStorage.h"
#include <ESATUtil.h>
#include <USBSerial.h>

#define flash SEGMENT_C

void ESATOBCSubsystem::begin()
{
  storeTelemetry = false;
  USB.begin();
  Serial.begin(115200);
  Storage.begin();
  I2C.begin();
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
  USB.println(Clock.read());
}

void ESATOBCSubsystem::handleStoreTelemetry(String parameters)
{
  storeTelemetry = !!parameters.toInt();
}

byte ESATOBCSubsystem::loadIdentifier()
{
  Flash.read(flash, &identifier, sizeof(identifier));
}

String ESATOBCSubsystem::readTelemetry()
{
  const unsigned int load = 100 * Timer.ellapsedMilliseconds() / Timer.period;
  const byte status =
    (Clock.alive << CLOCK_OFFSET)
    | (ADCSSubsystem.inertialMeasurementUnitAlive << IMU_OFFSET)
    | (EPSSubsystem.responding << EPS_OFFSET)
    | (Storage.working << STORAGE_OFFSET)
    | ((COMMSSubsystem.status & COMMS_MASK) << COMMS_OFFSET);
  String telemetry = Util.intToHexadecimal(status)
    + Util.intToHexadecimal(load);
  return telemetry;
}

void ESATOBCSubsystem::update()
{
}

ESATOBCSubsystem OBCSubsystem;
