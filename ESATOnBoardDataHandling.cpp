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

#include "ESATOnBoardDataHandling.h"
#include "ESATADCSSubsystem.h"
#include "ESATClock.h"
#include "ESATCOMMSSubsystem.h"
#include "ESATEPSSubsystem.h"
#include "ESATOBCSubsystem.h"
#include "ESATStorage.h"

ESATOnBoardDataHandling::ESATOnBoardDataHandling():
  numberOfSubsystems(0),
  telemetryIndex(0)
{
}

void ESATOnBoardDataHandling::beginSubsystems()
{
  telemetryIndex = 0;
  for (unsigned i = 0; i < numberOfSubsystems; ++i)
  {
    subsystems[i]->begin();
  }
}

void ESATOnBoardDataHandling::dispatchTelecommand(ESATCCSDSPacket& packet)
{
  const word applicationProcessIdentifier =
    packet.readApplicationProcessIdentifier();
  for (unsigned int i = 0; i < numberOfSubsystems; ++i)
  {
    const word subsystemsApplicationProcessIdentifier =
      subsystems[i]->getApplicationProcessIdentifier() & 0x07FF;
    if (subsystemsApplicationProcessIdentifier == applicationProcessIdentifier)
    {
      subsystems[i]->handleTelecommand(packet);
      return;
    }
  }
}

boolean ESATOnBoardDataHandling::readTelecommand(ESATCCSDSPacket& packet)
{
  return COMMSSubsystem.readTelecommand(packet);
}

boolean ESATOnBoardDataHandling::readSubsystemsTelemetry(ESATCCSDSPacket& packet)
{
  packet.clear();
  while (telemetryIndex < numberOfSubsystems)
  {
    if (subsystems[telemetryIndex]->telemetryAvailable())
    {
      const boolean successfulRead =
        subsystems[telemetryIndex]->readTelemetry(packet);
      if (successfulRead && (packet.readPacketType() == packet.TELEMETRY))
      {
        return true;
      }
    }
    else
    {
      telemetryIndex = telemetryIndex + 1;
    }
  }
  return false;
}

void ESATOnBoardDataHandling::registerSubsystem(ESATSubsystem& subsystem)
{
  subsystems[numberOfSubsystems] = &subsystem;
  numberOfSubsystems = numberOfSubsystems + 1;
}

void ESATOnBoardDataHandling::sendTelemetry(ESATCCSDSPacket& packet)
{
  COMMSSubsystem.writePacket(packet);
}

void ESATOnBoardDataHandling::storeTelemetry(ESATCCSDSPacket& packet)
{
  ESATTimestamp Timestamp = Clock.read();
  if(Clock.error)
  {
    return;
  }
    
  Storage.write(Timestamp, packet);
}

void ESATOnBoardDataHandling::updateSubsystems()
{
  for (int i = 0; i < numberOfSubsystems; ++i)
  {
    subsystems[i]->update();
  }
  telemetryIndex = 0;
}

ESATOnBoardDataHandling OnBoardDataHandling;
