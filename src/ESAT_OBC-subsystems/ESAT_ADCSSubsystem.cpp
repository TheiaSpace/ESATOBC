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

#include "ESAT_OBC-subsystems/ESAT_ADCSSubsystem.h"

// This subsystem either communicates with the ADCS board through the
// I2C bus or wraps around ESAT_ADCS from the ESATADCS library.  The
// implementation used depends on the definition of
// ESAT_ADCS_CODE_RUNNING_IN_ADCS: if it is defined, the I2C code is
// generated; otherwise, the library wrapper code is generated.
// ESAT_ADCS_CODE_RUNNING_IN_ADCS is defined when the option ADCS=ADCS
// is passed to Arduino ("ADCS code running in: ADCS").

#ifdef ESAT_ADCS_CODE_RUNNING_IN_ADCS
#include <ESAT_I2CMaster.h>
#else
#include <ESAT_ADCS.h>
#endif /* ESAT_ADCS_CODE_RUNNING_IN_ADCS */


void ESAT_ADCSSubsystemClass::begin()
{
#ifdef ESAT_ADCS_CODE_RUNNING_IN_ADCS
#else
  ESAT_ADCS.begin();
#endif /* ESAT_ADCS_CODE_RUNNING_IN_ADCS */
}

word ESAT_ADCSSubsystemClass::getApplicationProcessIdentifier()
{
#ifdef ESAT_ADCS_CODE_RUNNING_IN_ADCS
  return APPLICATION_PROCESS_IDENTIFIER;
#else
  return ESAT_ADCS.getApplicationProcessIdentifier();
#endif /* ESAT_ADCS_CODE_RUNNING_IN_ADCS */
}

void ESAT_ADCSSubsystemClass::handleTelecommand(ESAT_CCSDSPacket& packet)
{
#ifdef ESAT_ADCS_CODE_RUNNING_IN_ADCS
  packet.rewind();
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  if (primaryHeader.packetType != primaryHeader.TELECOMMAND)
  {
    return;
  }
  (void) ESAT_I2CMaster.writePacket(packet, ADDRESS);
#else
  ESAT_ADCS.handleTelecommand(packet);
#endif /* ESAT_ADCS_CODE_RUNNING_IN_ADCS */
}

boolean ESAT_ADCSSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  (void) packet;
  return false;
}

boolean ESAT_ADCSSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
#ifdef ESAT_ADCS_CODE_RUNNING_IN_ADCS
  newTelemetryPacket =
    ESAT_I2CMaster.readNextTelemetry(packet, ADDRESS);
  return newTelemetryPacket;
#else
  return ESAT_ADCS.readTelemetry(packet);
#endif /* ESAT_ADCS_CODE_RUNNING_IN_ADCS */
}

boolean ESAT_ADCSSubsystemClass::telemetryAvailable()
{
#ifdef ESAT_ADCS_CODE_RUNNING_IN_ADCS
  return newTelemetryPacket;
#else
  return ESAT_ADCS.telemetryAvailable();
#endif /* ESAT_ADCS_CODE_RUNNING_IN_ADCS */
}

void ESAT_ADCSSubsystemClass::update()
{
#ifdef ESAT_ADCS_CODE_RUNNING_IN_ADCS
  newTelemetryPacket = ESAT_I2CMaster.resetTelemetryQueue(ADDRESS);
#else
  ESAT_ADCS.update();
#endif /* ESAT_ADCS_CODE_RUNNING_IN_ADCS */
}

void ESAT_ADCSSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
  (void) packet;
}

ESAT_ADCSSubsystemClass ESAT_ADCSSubsystem;
