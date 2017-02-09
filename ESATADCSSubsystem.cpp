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

#include "ESATADCSSubsystem.h"
#include <ESATADCS.h>

void ESATADCSSubsystem::begin()
{
  ADCS.begin();
}

byte ESATADCSSubsystem::getStartOrder()
{
  return 3;
}

byte ESATADCSSubsystem::getSubsystemIdentifier()
{
  return 3;
}

void ESATADCSSubsystem::handleCommand(byte commandCode, String parameters)
{
  ADCS.handleCommand(commandCode, parameters);
}

String ESATADCSSubsystem::readTelemetry()
{
  return ADCS.readTelemetry();
}

void ESATADCSSubsystem::update()
{
  ADCS.update();
}

ESATADCSSubsystem ADCSSubsystem;
