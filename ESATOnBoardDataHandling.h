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

#ifndef ESATOnBoardDataHandling_h
#define ESATOnBoardDataHandling_h

#include <Energia.h>
#include <ESATCCSDSPacket.h>
#include "ESATSubsystem.h"

class ESATOnBoardDataHandling
{
  public:
    enum TelemetryType
    {
      HOUSEKEEPING_TELEMETRY = 1,
      EVENT_TELEMETRY = 2,
    };

    ESATOnBoardDataHandling();

    // Begin the registered subsystems.
    void beginSubsystems();

    // Dispatch a command on the registered subsystems.
    void dispatchTelecommand(ESATCCSDSPacket& packet);

    // Read an incomming telecommand and write it into a packet.
    // Return true if there was a valid telecommand available;
    // otherwise return false.
    boolean readTelecommand(ESATCCSDSPacket& packet);

    // Register a subsystem.
    void registerSubsystem(ESATSubsystem& subsystem);

    // Read the next available telemetry packet and write it into the
    // provided packet object.
    // Return true if there was a valid telemetry packet available;
    // otherwise return false.
    boolean readSubsystemsTelemetry(ESATCCSDSPacket& packet);

    // Send a telemetry packet.
    void sendTelemetry(ESATCCSDSPacket& packet);

    // Store a telemetry packet.
    void storeTelemetry(ESATCCSDSPacket& packet);

    // Update the registered subsystems.
    void updateSubsystems();

  private:
    static const byte MAXIMUM_NUMBER_OF_SUBSYSTEMS = 16;
    ESATSubsystem* subsystems[MAXIMUM_NUMBER_OF_SUBSYSTEMS];
    byte numberOfSubsystems;
    byte telemetryIndex;
};

extern ESATOnBoardDataHandling OnBoardDataHandling;

#endif
