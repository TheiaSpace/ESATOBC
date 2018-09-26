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

#include "ESAT_EPSSubsystem.h"
#include "ESAT_OBCClock.h"
#include <ESAT_I2CMaster.h>
#include <Wire.h>
#include <USBSerial.h>

void ESAT_EPSSubsystemClass::begin()
{
  newTelemetryPacket = false;
  setTime();
}

word ESAT_EPSSubsystemClass::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

void ESAT_EPSSubsystemClass::handleTelecommand(ESAT_CCSDSPacket& packet)
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
  (void) ESAT_I2CMaster.writePacket(packet,
                                    ADDRESS,
                                    MICROSECONDS_BETWEEN_CHUNKS);
}

boolean ESAT_EPSSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  (void) packet;
  return false;
}

boolean ESAT_EPSSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
  // ESATEPS 2.0.0 used the preliminary (0.0.0) version of our I2C
  // protocol and didn't support next-packet telemetry requests.
  // If the protocol version number is 0.0.0, we will request just one
  // housekeeping telemetry packet per cycle; for newer protocol
  // versions, we will do a next-packet telemetry request.
  const ESAT_SemanticVersionNumber protocolVersionNumber =
    ESAT_I2CMaster.readProtocolVersionNumber(ADDRESS);
  if (protocolVersionNumber == ESAT_SemanticVersionNumber(0, 0, 0))
  {
    newTelemetryPacket = false;
    return ESAT_I2CMaster.readNamedTelemetry(packet, HOUSEKEEPING, ADDRESS);
  }
  else
  {
    newTelemetryPacket =
      ESAT_I2CMaster.readNextTelemetry(packet, ADDRESS);
    return newTelemetryPacket;
  }
}

void ESAT_EPSSubsystemClass::setTime()
{
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
  (void) ESAT_I2CMaster.writePacket(packet,
                                    ADDRESS,
                                    MICROSECONDS_BETWEEN_CHUNKS);
}

boolean ESAT_EPSSubsystemClass::telemetryAvailable()
{
  return newTelemetryPacket;
}

void ESAT_EPSSubsystemClass::update()
{
  // ESATEPS 2.0.0 used the preliminary (0.0.0) version of our I2C
  // protocol and didn't support next-packet telemetry requests.
  // If the protocol version number is 0.0.0, we will request just one
  // housekeeping telemetry packet per cycle; for newer protocol
  // versions, we will start a new series of next-packet telemetry
  // requests.
  const ESAT_SemanticVersionNumber protocolVersionNumber =
    ESAT_I2CMaster.readProtocolVersionNumber(ADDRESS);
  if (protocolVersionNumber == ESAT_SemanticVersionNumber(0, 0, 0))
  {
    newTelemetryPacket = true;
  }
  else
  {
    newTelemetryPacket = ESAT_I2CMaster.resetTelemetryQueue(ADDRESS);
  }
}

void ESAT_EPSSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
  (void) packet;
}

ESAT_EPSSubsystemClass ESAT_EPSSubsystem;
