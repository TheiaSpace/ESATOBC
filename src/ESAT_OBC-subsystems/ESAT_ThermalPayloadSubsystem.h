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

#ifndef ESAT_ThermalPayloadSubsystem_h
#define ESAT_ThermalPayloadSubsystem_h

#include <Arduino.h>
#include <ESAT_OnBoardDataHandling.h>

// Interface to the Thermal Payload from the point of view of the
// on-board data handling subsystem.  There is a global instance:
// ESAT_ThermalPayloadSubsystem.  This should be the only instance of
// ESAT_ThermalPayloadSubsystemClass.  Register the OBC subsystem on
// the on-board data handling with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_ThermalPayloadSubsystem).
class ESAT_ThermalPayloadSubsystemClass: public ESAT_Subsystem
{
  public:
    // The main program checks the value of this variable to decide
    // whether or not to use the thermal payload.
    // This implies to use or leave unused and not initialized the OBC
    // microcontroller pins used for the payload.
    static boolean enabled;

    // Start this subsystem.
    void begin();

    // Return the identifier of this subsystem.
    word getApplicationProcessIdentifier();

    // Handle a telecommand.
    void handleTelecommand(ESAT_CCSDSPacket& packet);

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelecommand(ESAT_CCSDSPacket& packet);

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelemetry(ESAT_CCSDSPacket& packet);

    // Update the subsystem.
    void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESAT_CCSDSPacket& packet);
};

// Global instance of ESAT_ThermalPayloadSubsystemClass.  Register
// ESAT_ThermalPayloadSubsystem on the on-board data handling module with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_ThermalPayloadSubsystem).
extern ESAT_ThermalPayloadSubsystemClass ESAT_ThermalPayloadSubsystem;

#endif /* ESAT_ThermalPayloadSubsystem_h */
