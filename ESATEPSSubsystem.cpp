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
#include <ESATI2C.h>
#include <ESATUtil.h>
#include <Wire.h>

void ESATEPSSubsystem::begin()
{
}

byte ESATEPSSubsystem::getStartOrder()
{
  return 1;
}

byte ESATEPSSubsystem::getSubsystemIdentifier()
{
  return 2;
}

void ESATEPSSubsystem::handleCommand(byte commandCode, String parameters)
{
  (void) I2C.write(address, commandCode, parameters.toInt());
}

String ESATEPSSubsystem::readTelemetry()
{
  String data;
  const unsigned int telemetrySize = 25;
  Wire.beginTransmission(address);
  const byte bytesRead = Wire.requestFrom(address, 2 * telemetrySize);
  delay(10);
  if (bytesRead > 0)
  {
    for (int i = 0; i < 2 * telemetrySize; i++)
    {
      data += Util.byteToHexadecimal(byte(Wire.read()));
      delay(5);
    };
    responding = true;
  }
  else
  {
    for (int i = 0; i < 2 * telemetrySize; i++)
    {
      data += Util.byteToHexadecimal(byte(0));
    };
    responding = false;
  }
  return data;
}

void ESATEPSSubsystem::update()
{
}

ESATEPSSubsystem EPSSubsystem;