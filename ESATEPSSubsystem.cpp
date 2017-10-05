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
#include <Wire.h>

void ESATEPSSubsystem::begin()
{
  newTelemetryPacket = false;
}

word ESATEPSSubsystem::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

void ESATEPSSubsystem::handleTelecommand(ESATCCSDSPacket& telecommand)
{
  const word packetDataLength = telecommand.readPacketDataLength();
  const word packetLength =
    packetDataLength + telecommand.PRIMARY_HEADER_LENGTH;
  Wire.beginTransmission(ADDRESS);
  Wire.write(TELECOMMAND_CONTROL);
  for (int index = 0; index < packetLength; index++)
  {
    Wire.write(telecommand.buffer[index]);
  }
  (void) Wire.endTransmission();
}

void ESATEPSSubsystem::readTelemetry(ESATCCSDSPacket& packet)
{
  newTelemetryPacket = false;
  packet.clear();
  Wire.beginTransmission(ADDRESS);
  (void) Wire.write(TELEMETRY_CONTROL);
  (void) Wire.write(HOUSEKEEPING);
  (void) Wire.write(true);
  (void) Wire.write(0);
  (void) Wire.write(0);
  const byte headerTelemetryControlWriteStatus = Wire.endTransmission();
  if (headerTelemetryControlWriteStatus != 0)
  {
    alive = false;
    return;
  }
  Wire.beginTransmission(ADDRESS);
  (void) Wire.write(TELEMETRY_VECTOR);
  const byte headerTelemetryVectorWriteStatus = Wire.endTransmission();
  if (headerTelemetryVectorWriteStatus != 0)
  {
    alive = false;
    return;
  }
  const byte headerBytesRead =
    Wire.requestFrom(ADDRESS, packet.PRIMARY_HEADER_LENGTH);
  if (headerBytesRead != packet.PRIMARY_HEADER_LENGTH)
  {
    alive = false;
    return;
  }
  for (int i = 0; i < packet.PRIMARY_HEADER_LENGTH; i++)
  {
    packet.buffer[i] = Wire.read();
  }
  const word packetDataLength = packet.readPacketDataLength();
  const long packetLength = packet.PRIMARY_HEADER_LENGTH + packetDataLength;
  if (packetLength > packet.bufferLength)
  {
    packet.clear();
    return;
  }
  long readIndex = packet.PRIMARY_HEADER_LENGTH;
  while(readIndex < packetLength)
  {
    Wire.beginTransmission(ADDRESS);
    (void) Wire.write(TELEMETRY_CONTROL);
    (void) Wire.write(HOUSEKEEPING);
    (void) Wire.write(false);
    (void) Wire.write(highByte(readIndex));
    (void) Wire.write(lowByte(readIndex));
    const byte telemetryControlWriteStatus = Wire.endTransmission();
    if (telemetryControlWriteStatus != 0)
    {
      alive = false;
      packet.clear();
      return;
    }
    Wire.beginTransmission(ADDRESS);
    (void) Wire.write(TELEMETRY_VECTOR);
    const byte telemetryVectorWriteStatus = Wire.endTransmission();
    if (telemetryVectorWriteStatus != 0)
    {
      alive = false;
      packet.clear();
      return;
    }
    byte bytesToRead = BUFFER_LENGTH;
    if ((readIndex + bytesToRead) > packetLength)
    {
      bytesToRead = packetLength - readIndex;
    }
    const byte bytesRead = Wire.requestFrom(int(ADDRESS), int(bytesToRead));
    if (bytesRead != bytesToRead)
    {
      alive = false;
      packet.clear();
      return;
    }
    for (int i = 0; i < bytesToRead; i++)
    {
      packet.buffer[readIndex + i] = Wire.read();
    }
    readIndex = readIndex + bytesRead;
  }
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
