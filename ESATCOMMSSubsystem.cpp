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

#include "ESATCOMMSSubsystem.h"
#include <USBSerial.h>

void ESATCOMMSSubsystem::begin()
{
}

word ESATCOMMSSubsystem::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

void ESATCOMMSSubsystem::handleTelecommand(ESATCCSDSPacket& packet)
{
  (void) packet.writeTo(Serial);
}

boolean ESATCOMMSSubsystem::readTelecommand(ESATCCSDSPacket& packet)
{
  boolean gotPacket = false;
  if (Serial.available())
  {
    gotPacket = packet.readFrom(Serial);
  }
  else if (USB.available())
  {
    gotPacket = packet.readFrom(USB);
  }
  if (!gotPacket)
  {
    return false;
  }
  if (packet.readPacketType() != packet.TELECOMMAND)
  {
    return false;
  }
  return true;
}

boolean ESATCOMMSSubsystem::readTelemetry(ESATCCSDSPacket& packet)
{
  return false;
}

boolean ESATCOMMSSubsystem::telemetryAvailable()
{
  return false;
}

void ESATCOMMSSubsystem::update()
{
}

void ESATCOMMSSubsystem::writePacket(ESATCCSDSPacket& packet)
{
  (void) packet.writeTo(Serial);
  (void) packet.writeTo(USB);
}

ESATCOMMSSubsystem COMMSSubsystem;
