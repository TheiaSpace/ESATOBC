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

#include "ESATWifiSubsystem.h"
#include "ESATOBCClock.h"

void ESATWifiSubsystem::begin()
{
  pinMode(NOT_CONNECTED_SIGNAL_PIN, INPUT_PULLUP);
  const byte packetDataBufferLength = ESATCCSDSSecondaryHeader::LENGTH;
  byte packetData[packetDataBufferLength];
  ESATCCSDSPacket packet(packetData, packetDataBufferLength);
  packet.writePacketVersionNumber(0);
  packet.writePacketType(packet.TELECOMMAND);
  packet.writeSecondaryHeaderFlag(packet.SECONDARY_HEADER_IS_PRESENT);
  packet.writeApplicationProcessIdentifier(APPLICATION_PROCESS_IDENTIFIER);
  packet.writeSequenceFlags(packet.UNSEGMENTED_USER_DATA);
  packet.writePacketSequenceCount(0);
  ESATCCSDSSecondaryHeader secondaryHeader;
  secondaryHeader.preamble =
    secondaryHeader.CALENDAR_SEGMENTED_TIME_CODE_MONTH_DAY_VARIANT_1_SECOND_RESOLUTION;
  secondaryHeader.timestamp = OBCClock.read();
  secondaryHeader.majorVersionNumber = MAJOR_VERSION_NUMBER;
  secondaryHeader.minorVersionNumber = MINOR_VERSION_NUMBER;
  secondaryHeader.patchVersionNumber = PATCH_VERSION_NUMBER;
  secondaryHeader.packetIdentifier = CONNECT;
  packet.writeSecondaryHeader(secondaryHeader);
  packet.updatePacketDataLength();
  const unsigned long encoderBufferLength =
    ESATKISSStream::frameLength(packet.PRIMARY_HEADER_LENGTH
                                + packet.readPacketDataLength());
  byte encoderBuffer[encoderBufferLength];
  ESATKISSStream encoder(Serial, encoderBuffer, encoderBufferLength);
  (void) encoder.beginFrame();
  (void) packet.writeTo(encoder);
  (void) encoder.endFrame();
}

word ESATWifiSubsystem::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

void ESATWifiSubsystem::handleTelecommand(ESATCCSDSPacket& packet)
{
  const unsigned long encoderBufferLength =
    ESATKISSStream::frameLength(packet.PRIMARY_HEADER_LENGTH
                                + packet.readPacketDataLength());
  byte encoderBuffer[encoderBufferLength];
  ESATKISSStream encoder(Serial, encoderBuffer, encoderBufferLength);
  (void) encoder.beginFrame();
  (void) packet.writeTo(encoder);
  (void) encoder.endFrame();
}

boolean ESATWifiSubsystem::isConnected()
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

boolean ESATWifiSubsystem::readTelecommand(ESATCCSDSPacket& packet)
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
  if (packet.readPacketType() != packet.TELECOMMAND)
  {
    return false;
  }
  return true;
}

boolean ESATWifiSubsystem::readTelemetry(ESATCCSDSPacket& packet)
{
  return false;
}

boolean ESATWifiSubsystem::telemetryAvailable()
{
  return false;
}

void ESATWifiSubsystem::setTelecommandBuffer(byte buffer[],
                                             const unsigned long bufferLength)
{
  telecommandDecoder = ESATKISSStream(Serial, buffer, bufferLength);
}

void ESATWifiSubsystem::update()
{
}

void ESATWifiSubsystem::writeTelemetry(ESATCCSDSPacket& packet)
{
  if (!isConnected())
  {
    return;
  }
  const unsigned long encoderBufferLength =
    ESATKISSStream::frameLength(packet.PRIMARY_HEADER_LENGTH
                                + packet.readPacketDataLength());
  byte encoderBuffer[encoderBufferLength];
  ESATKISSStream encoder(Serial, encoderBuffer, encoderBufferLength);
  (void) encoder.beginFrame();
  (void) packet.writeTo(encoder);
  (void) encoder.endFrame();
}

ESATWifiSubsystem WifiSubsystem;
