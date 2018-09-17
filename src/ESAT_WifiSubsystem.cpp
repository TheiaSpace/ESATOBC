/*
 * Copyright (C) 2017-2018 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_WifiSubsystem.h"
#include "ESAT_OBCClock.h"

void ESAT_WifiSubsystemClass::begin(byte buffer[],
                                    const unsigned long bufferLength)
{
  beginConnectionSensor();
  beginTelecommandDecoder(buffer, bufferLength);
  connect();
}

void ESAT_WifiSubsystemClass::beginConnectionSensor()
{
  pinMode(NOT_CONNECTED_SIGNAL_PIN, INPUT_PULLUP);
}

void ESAT_WifiSubsystemClass::beginTelecommandDecoder(byte buffer[],
                                                      const unsigned long bufferLength)
{
  telecommandDecoder = ESAT_KISSStream(SerialWifi, buffer, bufferLength);
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
  const unsigned long encoderBufferLength =
    ESAT_KISSStream::frameLength(packet.length());
  byte encoderBuffer[encoderBufferLength];
  ESAT_KISSStream encoder(SerialWifi, encoderBuffer, sizeof(encoderBuffer));
  (void) encoder.beginFrame();
  (void) packet.writeTo(encoder);
  (void) encoder.endFrame();
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
  const unsigned long encoderBufferLength =
    ESAT_KISSStream::frameLength(packet.length());
  byte encoderBuffer[encoderBufferLength];
  ESAT_KISSStream encoder(SerialWifi, encoderBuffer, sizeof(encoderBuffer));
  (void) encoder.beginFrame();
  (void) packet.writeTo(encoder);
  (void) encoder.endFrame();
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
  const boolean gotFrame = telecommandDecoder.receiveFrame();
  if (!gotFrame)
  {
    return false;
  }
  const boolean gotPacket = packet.readFrom(telecommandDecoder);
  if (!gotPacket)
  {
    return false;
  }
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  if (primaryHeader.packetType == primaryHeader.TELECOMMAND)
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean ESAT_WifiSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
  (void) packet;
  return false;
}

boolean ESAT_WifiSubsystemClass::telemetryAvailable()
{
  return false;
}

void ESAT_WifiSubsystemClass::update()
{
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
  ESAT_KISSStream encoder(SerialWifi);
  (void) encoder.beginFrame();
  (void) packet.writeTo(encoder);
  (void) encoder.endFrame();
}

ESAT_WifiSubsystemClass ESAT_WifiSubsystem;
