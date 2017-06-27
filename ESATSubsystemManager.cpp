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

#include "ESATSubsystemManager.h"
#include "ESATADCSSubsystem.h"
#include "ESATClock.h"
#include "ESATCOMMSSubsystem.h"
#include "ESATEPSSubsystem.h"
#include "ESATOBCSubsystem.h"
#include "ESATStorage.h"
#include "ESATTelemetryManager.h"
#include <ESATUtil.h>

ESATSubsystemManager::ESATSubsystemManager(): numberOfSubsystems(0)
{
}

void ESATSubsystemManager::beginSubsystems()
{
  ESATSubsystem* sortedSubsystems[numberOfSubsystems];
  memcpy(sortedSubsystems, subsystems, sizeof(sortedSubsystems));
  for (int i = 1; i < numberOfSubsystems; ++i)
  {
    ESATSubsystem* currentSubsystem = sortedSubsystems[i];
    int j = i - 1;
    while ((j >= 0) && (sortedSubsystems[j]->getStartOrder() >
                        currentSubsystem->getStartOrder()))
    {
      sortedSubsystems[j+1] = sortedSubsystems[j];
      j = j - 1;
    }
    sortedSubsystems[j+1] = currentSubsystem;
  }
  for (unsigned i = 0; i < numberOfSubsystems; ++i)
  {
    sortedSubsystems[i]->begin();
  }
}

String ESATSubsystemManager::buildPacket(String content, byte type, byte subsystemIdentifier)
{
  String checksum = "FFFF";
  String packet = String(int(subsystemIdentifier), DEC).substring(0, 1)
                + Util.byteToHexadecimal(byte(content.length()))
                + Util.byteToHexadecimal(type)
                + content
                + checksum;
  return packet;
}

void ESATSubsystemManager::dispatchCommand(byte subsystemIdentifier, byte commandCode, String parameters)
{
  for (unsigned int i = 0; i < numberOfSubsystems; ++i)
  {
    if (subsystems[i]->getSubsystemIdentifier() == subsystemIdentifier)
    {
      subsystems[i]->handleCommand(commandCode, parameters);
      return;
    }
  }
}

ESATCommand ESATSubsystemManager::readCommand()
{
  return COMMSSubsystem.readCommand();
}

String ESATSubsystemManager::readSubsystemsTelemetry()
{
  String telemetry = "";
  for (unsigned int i = 0; i < numberOfSubsystems; ++i)
  {
    telemetry = telemetry + subsystems[i]->readTelemetry();
  }
  return telemetry;
}

void ESATSubsystemManager::registerDefaultSubsystems()
{
  registerSubsystem(&EPSSubsystem);
  registerSubsystem(&ADCSSubsystem);
  registerSubsystem(&COMMSSubsystem);
  registerSubsystem(&OBCSubsystem);
}

void ESATSubsystemManager::registerSubsystem(ESATSubsystem* subsystem)
{
  subsystems[numberOfSubsystems] = subsystem;
  numberOfSubsystems = numberOfSubsystems + 1;
}

void ESATSubsystemManager::sendTelemetry(String telemetry, byte type, byte subsystemIdentifier)
{
  String packet = buildPacket(telemetry, type, subsystemIdentifier);
  COMMSSubsystem.writePacket(packet);
}

void ESATSubsystemManager::storeTelemetry(String telemetry)
{
  String timestamp = Clock.read();
  String filename = timestamp.substring(0, 8)
                  + ".txt";
  Storage.write(filename, timestamp + " " + telemetry);
}

void ESATSubsystemManager::updateSubsystems()
{
  for (unsigned int i = 0; i < numberOfSubsystems; ++i)
  {
    subsystems[i]->update();
  }
}

ESATSubsystemManager SubsystemManager;
