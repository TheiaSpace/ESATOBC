/*
 * Copyright (C) 2017, 2018 Theia Space, Universidad Politécnica de Madrid
 *
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

#include "ESAT_OBC-subsystems/ESAT_WifiSubsystem.h"
#include "ESAT_OBC-peripherals/ESAT_OBCClock.h"

void ESAT_WifiSubsystemClass::begin(byte wifiReaderBuffer[],
                                    const unsigned long wifiReaderBufferLength,
                                    byte packetDataBuffer[],
                                    const unsigned long packetDataBufferLength)
{
  beginControlLines();
  beginWifiBridge(wifiReaderBuffer,
                  wifiReaderBufferLength,
                  packetDataBuffer,
                  packetDataBufferLength);
  connect();
}

void ESAT_WifiSubsystemClass::beginControlLines()
{
  pinMode(NOT_CONNECTED_SIGNAL_PIN, INPUT_PULLUP);
  digitalWrite(RESET_TELEMETRY_QUEUE_SIGNAL_PIN, HIGH);
  pinMode(RESET_TELEMETRY_QUEUE_SIGNAL_PIN, OUTPUT);
}

void ESAT_WifiSubsystemClass::beginReadingTelemetry()
{
  digitalWrite(RESET_TELEMETRY_QUEUE_SIGNAL_PIN, LOW);
  delayMicroseconds(TELEMETRY_QUEUE_RESET_DELAY);
  digitalWrite(RESET_TELEMETRY_QUEUE_SIGNAL_PIN, HIGH);
  delayMicroseconds(TELEMETRY_QUEUE_RESET_DELAY);
  readingTelemetry = true;
}

void ESAT_WifiSubsystemClass::beginWifiBridge(byte wifiReaderBuffer[],
                                              const unsigned long wifiReaderBufferLength,
                                              byte packetDataBuffer[],
                                              const unsigned long packetDataBufferLength)
{
  wifiReader = ESAT_CCSDSPacketFromKISSFrameReader(SerialWifi,
                                                   wifiReaderBuffer,
                                                   wifiReaderBufferLength);
  wifiWriter = ESAT_CCSDSPacketToKISSFrameWriter(SerialWifi);
  bufferedPacket = ESAT_CCSDSPacket(packetDataBuffer,
                                    packetDataBufferLength);
}

void ESAT_WifiSubsystemClass::connect()
{
  const byte packetDataBufferLength = ESAT_CCSDSSecondaryHeader::LENGTH;
  byte packetData[packetDataBufferLength];
  ESAT_CCSDSPacket packet(packetData, sizeof(packetData));
  packet.writeTelecommandHeaders(getApplicationProcessIdentifier(),
                                 0,
                                 ESAT_OBCClock.read(),
                                 MAJOR_VERSION_NUMBER,
                                 MINOR_VERSION_NUMBER,
                                 PATCH_VERSION_NUMBER,
                                 CONNECT);
  (void) wifiWriter.unbufferedWrite(packet);
}

word ESAT_WifiSubsystemClass::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

void ESAT_WifiSubsystemClass::handleTelecommand(ESAT_CCSDSPacket& packet)
{
  packet.rewind();
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  if (primaryHeader.packetType != primaryHeader.TELECOMMAND)
  {
    return;
  }
  if (primaryHeader.applicationProcessIdentifier
      != getApplicationProcessIdentifier())
  {
    return;
  }
  (void) wifiWriter.unbufferedWrite(packet);
}

boolean ESAT_WifiSubsystemClass::isConnected()
{
  if (digitalRead(NOT_CONNECTED_SIGNAL_PIN) == HIGH)
  {
    return false;
  }
  else
  {
    return true;
  }
}

boolean ESAT_WifiSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  // If a telecommand packet is already buffered, use it.
  if (telecommandAlreadyBuffered())
  {
    const boolean gotPacket = bufferedPacket.copyTo(packet);
    bufferedPacket.flush();
    return gotPacket;
  }
  // If there isn't a buffered telecommand packet, read a new packet.
  else
  {
    const boolean gotPacket = wifiReader.read(packet);
    if (!gotPacket)
    {
      return false;
    }
    const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
    // If we got a telecommand packet, report success.
    if (primaryHeader.packetType == primaryHeader.TELECOMMAND)
    {
      return true;
    }
    // If we got a telemetry packet, save it to the buffered packet
    // and report failure.
    else
    {
      (void) packet.copyTo(bufferedPacket);
      return false;
    }
  }
}

boolean ESAT_WifiSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
  // If a telemetry packet is already buffered, use it.
  if (telemetryAlreadyBuffered())
  {
    const boolean gotPacket = bufferedPacket.copyTo(packet);
    bufferedPacket.flush();
    return gotPacket;
  }
  // If there isn't a buffered telemetry packet, read a new packet.
  else
  {
    const boolean gotPacket = wifiReader.read(packet);
    if (!gotPacket)
    {
      return false;
    }
    const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
    // If we got a telemetry packet, report success.
    if (primaryHeader.packetType == primaryHeader.TELEMETRY)
    {
      return true;
    }
    // If we got a telecommand packet, save it to the buffered packet,
    // stop reading telemetry and report failure.
    else
    {
      (void) packet.copyTo(bufferedPacket);
      readingTelemetry = false;
      return false;
    }
  }
}

boolean ESAT_WifiSubsystemClass::telemetryAvailable()
{
  return readingTelemetry;
}

boolean ESAT_WifiSubsystemClass::telecommandAlreadyBuffered() const
{
  const ESAT_CCSDSPrimaryHeader primaryHeader =
    bufferedPacket.readPrimaryHeader();
  if ((primaryHeader.packetType == primaryHeader.TELECOMMAND)
      && (primaryHeader.packetDataLength > 0))
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean ESAT_WifiSubsystemClass::telemetryAlreadyBuffered() const
{
  const ESAT_CCSDSPrimaryHeader primaryHeader =
    bufferedPacket.readPrimaryHeader();
  if ((primaryHeader.packetType == primaryHeader.TELEMETRY)
      && (primaryHeader.packetDataLength > 0))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void ESAT_WifiSubsystemClass::update()
{
  beginReadingTelemetry();
}

void ESAT_WifiSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
  if (!isConnected())
  {
    return;
  }
  packet.rewind();
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  if (primaryHeader.packetType != primaryHeader.TELEMETRY)
  {
    return;
  }
  (void) wifiWriter.unbufferedWrite(packet);
}

ESAT_WifiSubsystemClass ESAT_WifiSubsystem;
