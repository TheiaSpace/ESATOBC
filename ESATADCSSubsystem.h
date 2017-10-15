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

#ifndef ESATADCSSubsystem_h
#define ESATADCSSubsystem_h

#include <Arduino.h>
#include "ESATSubsystem.h"

// Interface to the ADCS (attitude determination and control
// subsystem) from the point of view of the on-board data handling
// subsystem.  There is a global instance: ADCSSubsystem.  This should
// be the only instance of ESATADCSSubsystem.  Register the EPS
// subsystem on the on-board data handling with
// OnBoardDataHandling.registerSubsystem(ADCSSubsystem).
class ESATADCSSubsystem: public ESATSubsystem
{
  public:
    // Start this subsystem.
    void begin();

    // Return the identifier of this subsystem.
    word getApplicationProcessIdentifier();

    // Handle a telecommand.
    void handleTelecommand(ESATCCSDSPacket& packet);

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelecommand(ESATCCSDSPacket& packet);

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelemetry(ESATCCSDSPacket& packet);

    // Return true if there is new telemetry available;
    // Otherwise return false.
    boolean telemetryAvailable();

    // Update the subsystem.
    void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESATCCSDSPacket& packet);
};

// Global instance of ESATADCSSubsystem.  Register ADCSSubsystem on
// the on-board data handling module with
// OnBoardDataHandling.registerSubsystem(ADCSSubsystem).
extern ESATADCSSubsystem ADCSSubsystem;

#endif
