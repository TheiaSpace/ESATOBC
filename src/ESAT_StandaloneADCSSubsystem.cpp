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

#include "ESAT_StandaloneADCSSubsystem.h"
#include <ESAT_I2CMaster.h>
#include <Wire.h>

void ESAT_StandaloneADCSSubsystemClass::begin()
{
}

word ESAT_StandaloneADCSSubsystemClass::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

void ESAT_StandaloneADCSSubsystemClass::handleTelecommand(ESAT_CCSDSPacket& packet)
{
  packet.rewind();
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  if (primaryHeader.packetType != primaryHeader.TELECOMMAND)
  {
    return;
  }
  (void) ESAT_I2CMaster.writePacket(Wire,
                                    ADDRESS,
                                    packet,
                                    MILLISECONDS_AFTER_WRITES,
                                    ATTEMPTS,
                                    MILLISECONDS_BETWEEN_ATTEMPTS);
}

boolean ESAT_StandaloneADCSSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  return false;
}

boolean ESAT_StandaloneADCSSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
  newTelemetryPacket =
    ESAT_I2CMaster.readTelemetry(Wire,
                                 ADDRESS,
                                 packet,
                                 MILLISECONDS_AFTER_WRITES,
                                 ATTEMPTS,
                                 MILLISECONDS_BETWEEN_ATTEMPTS);
  return newTelemetryPacket;
}

boolean ESAT_StandaloneADCSSubsystemClass::telemetryAvailable()
{
  return newTelemetryPacket;
}

void ESAT_StandaloneADCSSubsystemClass::update()
{
  newTelemetryPacket = ESAT_I2CMaster.resetTelemetryQueue(Wire, ADDRESS);
}

void ESAT_StandaloneADCSSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
}

ESAT_StandaloneADCSSubsystemClass ESAT_StandaloneADCSSubsystem;
