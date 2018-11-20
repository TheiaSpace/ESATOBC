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
#include "ESAT_OBC-hardware/ESAT_OBCClock.h"

void ESAT_WifiSubsystemClass::begin(byte readerBuffer[],
                                    const unsigned long readerBufferLength,
                                    byte packetDataBuffer[],
                                    const unsigned long packetDataBufferLength)
{
  readingTelemetry = false;
  beginControlLines();
  beginWifiBridge(readerBuffer,
                  readerBufferLength,
                  packetDataBuffer,
                  packetDataBufferLength);
  setTime();
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

void ESAT_WifiSubsystemClass::beginWifiBridge(byte readerBuffer[],
                                              const unsigned long readerBufferLength,
                                              byte packetDataBuffer[],
                                              const unsigned long packetDataBufferLength)
{
  // We pass packets around in KISS frames.
  wifiReader = ESAT_CCSDSPacketFromKISSFrameReader(SerialWifi,
                                                   readerBuffer,
                                                   readerBufferLength);
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
  // This is more complicated than it could be because we buffer
  // incoming packets to be able to read telecommands and telemetry
  // at different stages.
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
    // If didn't get any packet, report failure.
    if (!gotPacket)
    {
      return false;
    }
    // If we got a telecommand packet, report success.
    if (packet.isTelecommand())
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
  // This is more complicated than it could be because we buffer
  // incoming packets to be able to read telecommands and telemetry
  // at different stages.
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
    // If didn't get any packet, stop reading telemetry and report
    // failure.
    if (!gotPacket)
    {
      readingTelemetry = false;
      return false;
    }
    // If we got a telemetry packet, report success.
    if (packet.isTelemetry())
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

void ESAT_WifiSubsystemClass::setTime()
{
  // To set the time of the Wifi board, we send it the telecommand to
  // do so with our CCSDS-Space-Packet-over-I2C protocol.
  const byte packetDataBufferLength =
    ESAT_CCSDSSecondaryHeader::LENGTH + 7;
  byte packetDataBuffer[packetDataBufferLength];
  ESAT_CCSDSPacket packet(packetDataBuffer, packetDataBufferLength);
  const ESAT_Timestamp timestamp = ESAT_OBCClock.read();
  packet.writeTelecommandHeaders(getApplicationProcessIdentifier(),
                                 0,
                                 timestamp,
                                 MAJOR_VERSION_NUMBER,
                                 MINOR_VERSION_NUMBER,
                                 PATCH_VERSION_NUMBER,
                                 SET_CURRENT_TIME);
  packet.writeTimestamp(timestamp);
  (void) wifiWriter.unbufferedWrite(packet);
}

boolean ESAT_WifiSubsystemClass::telemetryAvailable()
{
  return readingTelemetry;
}

boolean ESAT_WifiSubsystemClass::telecommandAlreadyBuffered() const
{
  if (bufferedPacket.isTelecommand()
      && (bufferedPacket.packetDataLength() > 0))
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
  if (bufferedPacket.isTelemetry()
      && (bufferedPacket.packetDataLength() > 0))
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
  // We want to start reading a new series of telemetry packets on
  // each cycle, but only if we have a buffer for storing them.
  if (bufferedPacket.capacity() > 0)
  {
    beginReadingTelemetry();
  }
}

void ESAT_WifiSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
  // The Wifi board is generally responsive, but it may block when
  // trying to connect to the ground station server.  The serial
  // buffer of the Wifi board has limited capacity and, if we start
  // writing telemetry packets while the Wifi board is blocking, we
  // could overload it and make it drop packets, making it impossible
  // to command.  We need to have spare buffer capacity to be able to
  // command the Wifi board, so that's why we will only write
  // telemetry packets if the Wifi board is connected and therefore
  // not blocking.
  if (!isConnected())
  {
    return;
  }
  (void) wifiWriter.unbufferedWrite(packet);
}

ESAT_WifiSubsystemClass ESAT_WifiSubsystem;
