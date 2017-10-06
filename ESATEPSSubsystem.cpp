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

void ESATEPSSubsystem::handleTelecommand(ESATCCSDSPacket& packet)
{
  const word packetDataLength = packet.readPacketDataLength();
  Wire.beginTransmission(ADDRESS);
  Wire.write(TELECOMMAND_PRIMARY_HEADER);
  for (int index = 0; index < packet.PRIMARY_HEADER_LENGTH; index++)
  {
    Wire.write(packet.buffer[index]);
  }
  const byte headerWriteStatus = Wire.endTransmission();
  if (headerWriteStatus != 0)
  {
    return;
  }
  packet.rewind();
  long index = 0;
  while (index < packetDataLength)
  {
    Wire.beginTransmission(ADDRESS);
    (void) Wire.write(TELECOMMAND_PACKET_DATA);
    byte bytesToWrite;
    if (index + (I2C_CHUNK_LENGTH - 1) <= packetDataLength)
    {
      bytesToWrite = (I2C_CHUNK_LENGTH - 1);
    }
    else
    {
      bytesToWrite = packetDataLength - index;
    }
    for (int i = 0; i < bytesToWrite; i++)
    {
      (void) Wire.write(packet.readByte());
    }
    const byte dataWriteStatus = Wire.endTransmission();
    if (dataWriteStatus != 0)
    {
      return;
    }
    index = index + bytesToWrite;
  }
}

void ESATEPSSubsystem::readTelemetry(ESATCCSDSPacket& packet)
{
  newTelemetryPacket = false;
  packet.clear();
  Wire.beginTransmission(ADDRESS);
  (void) Wire.write(TELEMETRY_REQUEST);
  (void) Wire.write(HOUSEKEEPING);
  const byte headerTelemetryControlWriteStatus = Wire.endTransmission();
  if (headerTelemetryControlWriteStatus != 0)
  {
    return;
  }
  Wire.beginTransmission(ADDRESS);
  (void) Wire.write(TELEMETRY_VECTOR);
  const byte headerTelemetryVectorWriteStatus = Wire.endTransmission();
  if (headerTelemetryVectorWriteStatus != 0)
  {
    return;
  }
  const byte headerBytesRead =
    Wire.requestFrom(ADDRESS, packet.PRIMARY_HEADER_LENGTH);
  if (headerBytesRead != packet.PRIMARY_HEADER_LENGTH)
  {
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
    byte bytesToRead = I2C_CHUNK_LENGTH;
    if ((readIndex + bytesToRead) > packetLength)
    {
      bytesToRead = packetLength - readIndex;
    }
    const byte bytesRead = Wire.requestFrom(int(ADDRESS), int(bytesToRead));
    if (bytesRead != bytesToRead)
    {
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
