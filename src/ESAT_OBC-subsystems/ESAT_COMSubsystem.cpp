/*
 * Copyright (C) 2020 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_OBC-subsystems/ESAT_COMSubsystem.h"
#include "ESAT_OBC-hardware/ESAT_OBCClock.h"
#include <ESAT_I2CMaster.h>

void ESAT_COMSubsystemClass::begin()
{
    setTime();
}

word ESAT_COMSubsystemClass::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

void ESAT_COMSubsystemClass::handleTelecommand(ESAT_CCSDSPacket& packet)
{
  // We pass telecommand packets to the COM board with our
  // CCSDS-Space-Packet-over-I2C protocol.
  (void) ESAT_I2CMaster.writePacket(packet,
                                    ADDRESS,
                                    MICROSECONDS_BETWEEN_CHUNKS);
									// TODO
									// Check microseconds
}

boolean ESAT_COMSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
	
  // We read telecommand packets from the COM board with our
  // CCSDS-Space-Packet-over-I2C protocol.
  
  // TODO
  // May need a while loop to empty the incoming telecommand buffer
  return ESAT_I2CMaster.readTelecommand(packet,
										ADDRESS);
}

boolean ESAT_COMSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
	return ESAT_I2CMaster.readNextTelemetry(packet, ADDRESS);
}


void ESAT_COMSubsystemClass::setTime()
{
  // To set the time of the COM board, we send it the telecommand to
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
  handleTelecommand(packet);
}


void ESAT_COMSubsystemClass::update()
{
  (void) ESAT_I2CMaster.resetTelemetryQueue(ADDRESS);
}

void ESAT_COMSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
  (void) ESAT_I2CMaster.writePacket(packet,
                                    ADDRESS,
                                    MICROSECONDS_BETWEEN_CHUNKS);
}

ESAT_COMSubsystemClass ESAT_COMSubsystem;
