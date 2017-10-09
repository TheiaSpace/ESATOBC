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

#include "ESATEPSSubsystem.h"
#include <ESATI2CMaster.h>
#include <Wire.h>

void ESATEPSSubsystem::begin()
{
  newTelemetryPacket = false;
}

word ESATEPSSubsystem::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

void ESATEPSSubsystem::handleTelecommand(ESATCCSDSPacket& packet)
{
  (void) I2CMaster.writeTelecommand(Wire,
                                    ADDRESS,
                                    packet,
                                    TRIES,
                                    MILLISECONDS_BETWEEN_RETRIES);
}

boolean ESATEPSSubsystem::readTelemetry(ESATCCSDSPacket& packet)
{
  if (!newTelemetryPacket)
  {
    return false;
  }
  newTelemetryPacket = false;
  const boolean gotTelemetry =
    I2CMaster.readTelemetry(Wire,
                            ADDRESS,
                            HOUSEKEEPING,
                            packet,
                            TRIES,
                            MILLISECONDS_BETWEEN_RETRIES);
  return gotTelemetry;
}

boolean ESATEPSSubsystem::telemetryAvailable()
{
  return newTelemetryPacket;
}

void ESATEPSSubsystem::update()
{
  newTelemetryPacket = true;
}

ESATEPSSubsystem EPSSubsystem;
