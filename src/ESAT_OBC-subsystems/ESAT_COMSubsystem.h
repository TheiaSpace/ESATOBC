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

#ifndef ESAT_COMSubsystem_h
#define ESAT_COMSubsystem_h

#include <Arduino.h>
#include "ESAT_OBC-subsystems/ESAT_Subsystem.h"

// Interface to the COM (radio communications) subsystem from the point 
// of view of the on-board data handling subsystem.  There is a global 
// instance: ESAT_COMSubsystem.  This should be the only instance of
// ESAT_COMSubsystemClass.  Register the ESAT COM subsystem on the
// on-board data handling with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_COMSubsystem).
class ESAT_COMSubsystemClass: public ESAT_Subsystem
{
  public:
    // Start the communications subsystem.
    void begin();

    // Return the identifier of this subsystem.
    word getApplicationProcessIdentifier();

    // Handle a telecommand.
    void handleTelecommand(ESAT_CCSDSPacket& packet);

    // Fill a packet with the next available telecommand.
    // Return true if there was a pending telecommand;
    // otherwise return false.
    boolean readTelecommand(ESAT_CCSDSPacket& packet);

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelemetry(ESAT_CCSDSPacket& packet);

    void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESAT_CCSDSPacket& packet);

  private:
  
    // I2C address of the COM board.
    static const byte ADDRESS = 3;
	
    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 5;

    // Version number of the subsystem's interface.
    static const byte MAJOR_VERSION_NUMBER = 1;
    static const byte MINOR_VERSION_NUMBER = 0;
    static const byte PATCH_VERSION_NUMBER = 0;
	
	// Wait this number of microseconds between successive chunks when
    // writing packets to the COM board.
    static const word MICROSECONDS_BETWEEN_CHUNKS = 1000;

    // Command code for setting the time of the COM clock.
    static const byte SET_CURRENT_TIME = 0x06;

    // Set the time of the Wifi board.
    void setTime();
};

// Global instance of ESAT_COMSubsystemClass. Register COMSubsystem
// on the on-board data handling module with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_COMSubsystem).
extern ESAT_COMSubsystemClass ESAT_COMSubsystem;

#endif /* ESAT_COMSubsystem_h */
