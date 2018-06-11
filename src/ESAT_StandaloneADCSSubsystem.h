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

#ifndef ESAT_StandaloneADCSSubsystem_h
#define ESAT_StandaloneADCSSubsystem_h

#include <Arduino.h>
#include "ESAT_Subsystem.h"

// Interface to the ADCS (attitude determination and control
// subsystem) from the point of view of the on-board data handling
// subsystem.  There is a global instance: ESAT_StandaloneADCSSubsystem.  This
// should be the only instance of ESAT_StandaloneADCSSubsystemClass.  Register
// the EPS subsystem on the on-board data handling with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_StandaloneADCSSubsystem).
class ESAT_StandaloneADCSSubsystemClass: public ESAT_Subsystem
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

    // Return true if there is new telemetry available;
    // Otherwise return false.
    boolean telemetryAvailable();

    // Update the subsystem.
    void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESAT_CCSDSPacket& packet);

  private:
    // I2C address of the ADCS board.
    static const byte ADDRESS = 2;

    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 2;

    // Version numbers for I2C communication.
    static const byte MAJOR_VERSION_NUMBER = 3;
    static const byte MINOR_VERSION_NUMBER = 2;
    static const byte PATCH_VERSION_NUMBER = 0;

    // Time after I2C write operations to let the ADCS board process
    // the requests.
    static const byte MILLISECONDS_AFTER_WRITES = 1;

    // Time between I2C telecommand and telemetry transfer attempts.
    static const word MILLISECONDS_BETWEEN_ATTEMPTS = 20;

    // Number of attempts for I2C telecommand and telemetry transfers.
    static const byte ATTEMPTS = 100;

    // True when there is a new telemetry packet available
    // (after update()); false otherwise (after readTelemetry()).
    boolean newTelemetryPacket;
};

// Global instance of ESAT_StandaloneADCSSubsystemClass.  Register
// ESAT_StandaloneADCSSubsystem on the on-board data handling module with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_StandaloneADCSSubsystem).
extern ESAT_StandaloneADCSSubsystemClass ESAT_StandaloneADCSSubsystem;

#endif /* ESAT_StandaloneADCSSubsystem_h */
