/*
 * Copyright (C) 2017, 2018, 2019, 2021 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_OBC-subsystems/ESAT_OBCSubsystem.h"
#include "ESAT_OBC-hardware/ESAT_OBCLED.h"
#include "ESAT_OBC-hardware/ESAT_TelemetryStorage.h"
#include "ESAT_OBC-telecommands/ESAT_OBCDisableTelemetryTelecommand.h"
#include "ESAT_OBC-telecommands/ESAT_OBCDownloadStoredTelemetryTelecommand.h"
#include "ESAT_OBC-telecommands/ESAT_OBCEnableTelemetryTelecommand.h"
#include "ESAT_OBC-telecommands/ESAT_OBCEraseStoredTelemetryTelecommand.h"
#include "ESAT_OBC-telecommands/ESAT_OBCSetTimeTelecommand.h"
#include "ESAT_OBC-telecommands/ESAT_OBCStoreTelemetryTelecommand.h"
#include "ESAT_OBC-telemetry/ESAT_OBCHousekeepingTelemetry.h"
#include "ESAT_OBC-telemetry/ESAT_OBCLinesTelemetry.h"
#include "ESAT_OBC-telemetry/ESAT_OBCProcessorTelemetry.h"
#include <ESAT_Timer.h>
#include <ESAT_Timestamp.h>

void ESAT_OBCSubsystemClass::addTelecommand(ESAT_CCSDSTelecommandPacketHandler& telecommand)
{
  telecommandPacketDispatcher.add(telecommand);
}

void ESAT_OBCSubsystemClass::addTelemetry(ESAT_CCSDSTelemetryPacketContents& telemetry)
{
  telemetryPacketBuilder.add(telemetry);
  enableTelemetry(telemetry.packetIdentifier());
}

void ESAT_OBCSubsystemClass::begin()
{
  beginHardware();
  beginTelemetry();
  beginTelecommands();  
}

void ESAT_OBCSubsystemClass::beginHardware()
{
  storeTelemetry = false;
  ESAT_OBCLED.begin();
}

void ESAT_OBCSubsystemClass::beginTelemetry()
{
  // The list of enabled telemetry packets is persistent and must be
  // read from a configuration file.  The housekeeping telemetry will
  // be enabled on startup regardless of what the configuration file
  // says, though, so that the satellite isn't silent.
  readEnabledTelemetryList();
  addTelemetry(ESAT_OBCHousekeepingTelemetry);
  enableTelemetry(ESAT_OBCHousekeepingTelemetry.packetIdentifier());
  addTelemetry(ESAT_OBCLinesTelemetry);
  addTelemetry(ESAT_OBCProcessorTelemetry);
}

void ESAT_OBCSubsystemClass::beginTelecommands()
{
  addTelecommand(ESAT_OBCSetTimeTelecommand);
  addTelecommand(ESAT_OBCStoreTelemetryTelecommand);
  addTelecommand(ESAT_OBCDownloadStoredTelemetryTelecommand);
  addTelecommand(ESAT_OBCEraseStoredTelemetryTelecommand);
  addTelecommand(ESAT_OBCEnableTelemetryTelecommand);
  addTelecommand(ESAT_OBCDisableTelemetryTelecommand);
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
  // A telecommand packet dispatcher hides the complexity of dispatching
  // and handling telecommands.
  (void) telecommandPacketDispatcher.dispatch(packet);
}

void ESAT_OBCSubsystemClass::readEnabledTelemetryList()
{
  // The enabled telemetry list will be empty if reading it from
  // storage fails for some reason (for example, if the configuration
  // file is missing).  This means that, by default, all telemetry
  // packets are disabled.
  File file = SD.open(ENABLED_TELEMETRY_FILENAME, FILE_READ);
  (void) enabledTelemetry.readFrom(file);
  file.close();
}

boolean ESAT_OBCSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  // The OBC subsystem doesn't produce telecommands at this moment.
  (void) packet;
  return false;
}

boolean ESAT_OBCSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
  // The OBC subsystem produces telemetry of two kinds:
  // - its own telemetry;
  // - stored telemetry.
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
  // If there aren't more stored telemetry packets to be read,
  // we must ensure that the telemetry storage module is free.
  if (!correctRead)
  {
    ESAT_TelemetryStorage.endReading();
  }
  return correctRead;
}

boolean ESAT_OBCSubsystemClass::telemetryAvailable()
{
  // The OBC subsystem produces telemetry of two kinds:
  // - its own telemetry;
  // - stored telemetry.
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
  // The OBC has a few periodic tasks that it must do on each cycle:
  // - reset the list of pending telemetry packets;
  const ESAT_FlagContainer availableTelemetry =
    telemetryPacketBuilder.available();
  const ESAT_FlagContainer availableAndEnabledTelemetry =
    availableTelemetry & enabledTelemetry;
  pendingTelemetry = availableAndEnabledTelemetry;
  // - toggle the OBC LED.
  ESAT_OBCLED.toggle();
}

void ESAT_OBCSubsystemClass::writeEnabledTelemetryList()
{
  File file = SD.open(ENABLED_TELEMETRY_FILENAME, FILE_WRITE);
  (void) enabledTelemetry.writeTo(file);
  file.close();
}

void ESAT_OBCSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
  if (storeTelemetry && !ESAT_TelemetryStorage.reading())
  {
    ESAT_TelemetryStorage.write(packet);
  }
}

ESAT_OBCSubsystemClass ESAT_OBCSubsystem;
