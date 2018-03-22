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

#include "ESAT_EPSSubsystem.h"
#include "ESAT_OBCClock.h"
#include <ESAT_I2CMaster.h>
#include <Wire.h>
#include <USBSerial.h>

void ESAT_EPSSubsystemClass::begin()
{
  newTelemetryPacket = false;
  telecommandBuilder =
    ESAT_CCSDSPacketBuilder(APPLICATION_PROCESS_IDENTIFIER,
                            MAJOR_VERSION_NUMBER,
                            MINOR_VERSION_NUMBER,
                            PATCH_VERSION_NUMBER,
                            ESAT_CCSDSPrimaryHeader::TELECOMMAND,
                            0,
                            ESAT_OBCClock);
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
  (void) ESAT_I2CMaster.writePacket(Wire,
                                    ADDRESS,
                                    packet,
                                    MILLISECONDS_AFTER_WRITES,
                                    ATTEMPTS,
                                    MILLISECONDS_BETWEEN_ATTEMPTS);
}

boolean ESAT_EPSSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  return false;
}

boolean ESAT_EPSSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
  newTelemetryPacket =
    ESAT_I2CMaster.readTelemetry(Wire,
                                 ADDRESS,
                                 packet,
                                 MILLISECONDS_AFTER_WRITES,
                                 ATTEMPTS,
                                 MILLISECONDS_BETWEEN_ATTEMPTS);
  return newTelemetryPacket;
}

void ESAT_EPSSubsystemClass::setTime()
{
  const byte packetDataBufferLength =
    ESAT_CCSDSSecondaryHeader::LENGTH + 7;
  byte packetDataBuffer[packetDataBufferLength];
  ESAT_CCSDSPacket packet(packetDataBuffer, packetDataBufferLength);
  const boolean headersCorrect =
    telecommandBuilder.fillHeaders(packet, SET_CURRENT_TIME);
  if (!headersCorrect)
  {
    return;
  }
  packet.writeTimestamp(ESAT_OBCClock.read());
  telecommandBuilder.incrementPacketSequenceCount();
  (void) ESAT_I2CMaster.writePacket(Wire,
                                    ADDRESS,
                                    packet,
                                    MILLISECONDS_AFTER_WRITES,
                                    ATTEMPTS,
                                    MILLISECONDS_BETWEEN_ATTEMPTS);
}

boolean ESAT_EPSSubsystemClass::telemetryAvailable()
{
  return newTelemetryPacket;
}

void ESAT_EPSSubsystemClass::update()
{
  newTelemetryPacket = ESAT_I2CMaster.resetTelemetryQueue(Wire, ADDRESS);
}

void ESAT_EPSSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
}

ESAT_EPSSubsystemClass ESAT_EPSSubsystem;
