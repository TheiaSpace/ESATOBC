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
  (void) ESAT_I2CMaster.writeTelecommand(Wire,
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
  ESAT_CCSDSPrimaryHeader primaryHeader;
  primaryHeader.packetVersionNumber = 0;
  primaryHeader.packetType = primaryHeader.TELECOMMAND;
  primaryHeader.secondaryHeaderFlag = primaryHeader.SECONDARY_HEADER_IS_PRESENT;
  primaryHeader.applicationProcessIdentifier =
    getApplicationProcessIdentifier();
  primaryHeader.sequenceFlags = primaryHeader.UNSEGMENTED_USER_DATA;
  primaryHeader.packetSequenceCount = 0;
  packet.writePrimaryHeader(primaryHeader);
  ESAT_CCSDSSecondaryHeader secondaryHeader;
  secondaryHeader.preamble =
    secondaryHeader.CALENDAR_SEGMENTED_TIME_CODE_MONTH_DAY_VARIANT_1_SECOND_RESOLUTION;
  secondaryHeader.timestamp = ESAT_OBCClock.read();
  secondaryHeader.majorVersionNumber = MAJOR_VERSION_NUMBER;
  secondaryHeader.minorVersionNumber = MINOR_VERSION_NUMBER;
  secondaryHeader.patchVersionNumber = PATCH_VERSION_NUMBER;
  secondaryHeader.packetIdentifier = SET_CURRENT_TIME;
  packet.writeSecondaryHeader(secondaryHeader);
  packet.writeTimestamp(secondaryHeader.timestamp);
  (void) ESAT_I2CMaster.writeTelecommand(Wire,
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
  newTelemetryPacket = true;
}

void ESAT_EPSSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
}

ESAT_EPSSubsystemClass ESAT_EPSSubsystem;
