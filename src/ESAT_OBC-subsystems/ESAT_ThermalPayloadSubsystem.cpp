/*
 * Copyright (C) 2019 Theia Space, Universidad Politécnica de Madrid.
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

 /*
  Thermal payload version.
  00 - Skeleton for your own Thermla Payload subsystem.
 */

#include "ESAT_ThermalPayloadSubsystem.h"

// Global variable to select if the Thermal Payload is registered
// as a subsystem or not. This implies to use or leave
// unused and not initialized the OBC uC PINs used for the PL.
boolean __attribute__((weak)) ESAT_ThermalPayloadSubsystemClass::enabled = true;

void __attribute__((weak)) ESAT_ThermalPayloadSubsystemClass::begin()
{
}

// Return the application process identifier of this subsystem.
// Each subsystem must have a unique 11-bit application process
// identifier.
// OnBoardDataHandling uses this to identify the subsystem.
word __attribute__((weak)) ESAT_ThermalPayloadSubsystemClass::getApplicationProcessIdentifier()
{
  return 5;
}

// Handle a telecommand.
// OnBoardDataHandling.dispatchTelecommand() forwards telecommands
// to their respective ESATSubsystem modules, identified by the
// value returned by getApplicationProcessIdentifier().
void __attribute__((weak)) ESAT_ThermalPayloadSubsystemClass::handleTelecommand(ESAT_CCSDSPacket& packet)
{
  // We will see how to handle a telecommand packet in coming sessions.
  // A command to stop and start the blinking of the LED would be
  // interesting, isn't it?
}

// Fill a packet with the next telemetry packet available.
// Return true if the operation was successful;
// otherwise return false.
// Called from OnBoardDataHandling.readSubsystemsTelemetry().
boolean __attribute__((weak)) ESAT_ThermalPayloadSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
  // Here we should fill a telemetry packet.
  // We will see how to do it in coming sessions.
  // At the moment we are going to show information through the
  // USB as a debugging tool.
  return false;
}

// Update the subsystem.
// Called from OnBoardDataHandling.updateSubsystems().
void __attribute__((weak)) ESAT_ThermalPayloadSubsystemClass::update()
{
}

// Fill a packet with the next telecommand packet available.
// Return true if the operation was successful;
// otherwise return false.
// Called from OnBoardDataHandling.readTelecommand().
boolean __attribute__((weak)) ESAT_ThermalPayloadSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  // Nothing to do here.
  return false;
}

// Send a telemetry packet to this subsystem.
// Called from OnBoardDataHandling.writeTelemetry().
void __attribute__((weak)) ESAT_ThermalPayloadSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
  // Nothing to do here.
}

// Unique global instance of ESAT_ThermalPayloadSubsystemClass.
ESAT_ThermalPayloadSubsystemClass ESAT_ThermalPayloadSubsystem;
