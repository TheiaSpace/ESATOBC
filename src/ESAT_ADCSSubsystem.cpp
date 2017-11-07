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

#include "ESAT_ADCSSubsystem.h"
#include <ESAT_ADCS.h>

void ESAT_ADCSSubsystemClass::begin(const word period)
{
  ESAT_ADCS.begin(period);
}

word ESAT_ADCSSubsystemClass::getApplicationProcessIdentifier()
{
  return ESAT_ADCS.getApplicationProcessIdentifier();
}

void ESAT_ADCSSubsystemClass::handleTelecommand(ESAT_CCSDSPacket& packet)
{
  ESAT_ADCS.handleTelecommand(packet);
}

boolean ESAT_ADCSSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  return false;
}

boolean ESAT_ADCSSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
  return ESAT_ADCS.readTelemetry(packet);
}

boolean ESAT_ADCSSubsystemClass::telemetryAvailable()
{
  return ESAT_ADCS.telemetryAvailable();
}

void ESAT_ADCSSubsystemClass::update()
{
  ESAT_ADCS.update();
}

void ESAT_ADCSSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
}

ESAT_ADCSSubsystemClass ESAT_ADCSSubsystem;
