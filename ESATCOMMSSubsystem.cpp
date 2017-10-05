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
  writePacketTo(Serial, packet);
}

void ESATCOMMSSubsystem::readTelecommand(ESATCCSDSPacket& packet)
{
  if (Serial.available())
  {
    readPacketFrom(Serial, packet);
  }
  else if (USB.available())
  {
    readPacketFrom(USB, packet);
  }
}

void ESATCOMMSSubsystem::readPacketFrom(Stream& input,
                                        ESATCCSDSPacket& packet)
{
  packet.clear();
  if (packet.bufferLength < packet.PRIMARY_HEADER_LENGTH)
  {
    return;
  }
  if (input.available() == 0)
  {
    return;
  }
  const byte headerBytesRead =
    input.readBytes((char*) packet.buffer, packet.PRIMARY_HEADER_LENGTH);
  if (headerBytesRead < packet.PRIMARY_HEADER_LENGTH)
  {
    packet.clear();
    return;
  }
  const word packetDataLength = packet.readPacketDataLength();
  const long packetLength = packetDataLength + packet.PRIMARY_HEADER_LENGTH;
  if (packetLength > packet.bufferLength)
  {
    packet.clear();
    return;
  }
  const word packetDataBytesRead =
    input.readBytes((char*) packet.buffer, packetDataLength);
  if (packetDataBytesRead != packetDataLength)
  {
    packet.clear();
    return;
  }
}

void ESATCOMMSSubsystem::readTelemetry(ESATCCSDSPacket& packet)
{
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
  writePacketTo(Serial, packet);
  writePacketTo(USB, packet);
}

void ESATCOMMSSubsystem::writePacketTo(Stream& output,
                                       ESATCCSDSPacket& packet)
{
  const long packetLength =
    packet.PRIMARY_HEADER_LENGTH + packet.readPacketDataLength();
  for (long i = 0; i < packetLength; i++)
  {
    output.write(packet.buffer[i]);
  }
}

ESATCOMMSSubsystem COMMSSubsystem;
