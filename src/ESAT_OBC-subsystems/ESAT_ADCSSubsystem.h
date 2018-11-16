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

#ifndef ESAT_ADCSSubsystem_h
#define ESAT_ADCSSubsystem_h

#include <Arduino.h>
#include "ESAT_OBC-subsystems/ESAT_Subsystem.h"

// Interface to the ADCS (attitude determination and control
// subsystem) from the point of view of the on-board data handling
// subsystem.  There is a global instance: ESAT_ADCSSubsystem.  This
// should be the only instance of ESAT_ADCSSubsystemClass.  Register
// the ADCS subsystem on the on-board data handling with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_ADCSSubsystem).
// If the macro ESAT_ADCS_CODE_RUNNING_IN_ADCS is defined, this module
// communicates with the of the ADCS board; if it is undefined, this
// module works as wrapper around ESAT_ADCS.
// The I2C bus must be configured before using this library.  You must
// have called Wire.begin() before using this library.
class ESAT_ADCSSubsystemClass: public ESAT_Subsystem
{
  public:
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

    // Deprecated method; don't use it.
    // Return true if there is new telemetry available;
    // Otherwise return false.
    boolean telemetryAvailable() __attribute__((deprecated));

    // Update the subsystem.
    void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESAT_CCSDSPacket& packet);

#ifdef ESAT_ADCS_CODE_RUNNING_IN_ADCS
  private:
    // I2C address of the ADCS board.
    static const byte ADDRESS = 2;

    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 2;

    // True when there is a new telemetry packet available
    // (after update()); false otherwise (after readTelemetry()).
    boolean newTelemetryPacket;

    // False before the first call to setTime() and true afterward.
    boolean timeIsSet;

#endif /* ESAT_ADCS_CODE_RUNNING_IN_ADCS */

    // Version numbers of the telecommand for setting the time.
    static const byte SET_TIME_MAJOR_VERSION_NUMBER = 3;
    static const byte SET_TIME_MINOR_VERSION_NUMBER = 3;
    static const byte SET_TIME_PATCH_VERSION_NUMBER = 0;

    // Packet identifier of the telecommand for setting the time.
    static const byte SET_TIME_PACKET_IDENTIFIER = 0xC0;

    // Set the time of the ADCS.
    void setTime();
};

// Global instance of ESAT_ADCSSubsystemClass.  Register
// ESAT_ADCSSubsystem on the on-board data handling module with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_ADCSSubsystem).
extern ESAT_ADCSSubsystemClass ESAT_ADCSSubsystem;

#endif /* ESAT_ADCSSubsystem_h */
