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

#ifndef ESAT_EPSSubsystem_h
#define ESAT_EPSSubsystem_h

#include <Arduino.h>
#include <ESAT_CCSDSPacketBuilder.h>
#include "ESAT_Subsystem.h"

// Interface to the EPS (electrical power subsystem) from the point of
// view of the on-board data handling subsystem.  There is a global
// instance: EPS_Subsystem.  This should be the only instance of
// ESAT_EPSSubsystemClass.  Register the EPS subsystem on the on-board
// data handling with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_EPSSubsystem).
class ESAT_EPSSubsystemClass: public ESAT_Subsystem
{
  public:
    // Start this subsystem.
    void begin();

    // Return the identifier of this subsystem.
    word getApplicationProcessIdentifier();

    // Handle a telecommand.
    void handleTelecommand(ESAT_CCSDSPacket& telecommand);

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelecommand(ESAT_CCSDSPacket& packet);

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelemetry(ESAT_CCSDSPacket& telemetry);

    // Return true if a new telemetry packet is available.
    boolean telemetryAvailable();

    // Update the subsystem.
    void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESAT_CCSDSPacket& packet);

  private:
    // I2C address of the EPS board.
    static const byte ADDRESS = 1;

    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 1;

    // Version numbers of the subsystem interface.
    static const byte MAJOR_VERSION_NUMBER = 2;
    static const byte MINOR_VERSION_NUMBER = 0;
    static const byte PATCH_VERSION_NUMBER = 0;

    // Time after I2C write operations to let the EPS board process
    // the requests.
    static const byte MILLISECONDS_AFTER_WRITES = 1;

    // Time between I2C telecommand and telemetry transfer attempts.
    static const word MILLISECONDS_BETWEEN_ATTEMPTS = 20;

    // Number of attempts for I2C telecommand and telemetry transfers.
    static const byte ATTEMPTS = 100;

    // True when there is a new telemetry packet available
    // (after update()); false otherwise (after readTelemetry()).
    boolean newTelemetryPacket;

    // Set the time of the EPS board.
    void setTime();

  private:
    // Command code for setting the time of the EPS clock.
    static const byte SET_CURRENT_TIME = 0x00;

    // Use this to build telecommands.
    ESAT_CCSDSPacketBuilder telecommandBuilder;
};

// Global instance of ESAT_EPSSubsystemClass.  Register
// ESAT_EPSSubsystem on the on-board data handling module with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_EPSSubsystem).
extern ESAT_EPSSubsystemClass ESAT_EPSSubsystem;

#endif /* ESAT_EPSSubsystem_h */
