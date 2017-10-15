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

#ifndef ESATSubsystem_h
#define ESATSubsystem_h

#include <Arduino.h>
#include <ESATCCSDSPacket.h>

// Common subsystem interface from the point of view of the on-board
// data handling function.
// OnBoardDataHandling operates on the subsystems:
// - just once during the setup stage, it calls begin() on all the
//   subsystems.
// - then, on a loop, it performs several actions:
//   * first, it calls readTelecommand() on all the subsystems;
//   * for each telecommand read, it calls handleTelecommand() on the
//     telecommand target subsystem.
//   * after that, it calls update() on all the subsystems;
//   * then, it calls availableTelemetry() on each subsystem and,
//     while there is telemetry available, it calls readTelemetry();
//   * finally, it calls writeTelemetry() on all the subsystems.
class ESATSubsystem
{
  public:
    virtual ~ESATSubsystem() {};

    // Start the subsystem.
    // Called from OnBoardDataHandling.beginSubsystems().
    virtual void begin() = 0;

    // Return the application process identifier of this subsystem.
    // Each subsystem must have a unique 11-bit application process
    // identifier.
    // OnBoardDataHandling uses this to identify the subsystem.
    virtual word getApplicationProcessIdentifier() = 0;

    // Handle a telecommand.
    // OnBoardDataHandling.dispatchTelecommand() forwards telecommands
    // to their respective ESATSubsystem modules, identified by the
    // value returned by getApplicationProcessIdentifier().
    virtual void handleTelecommand(ESATCCSDSPacket& telecommand) = 0;

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    // Called from OnBoardDataHandling.readSubsystemsTelemetry().
    virtual boolean readTelemetry(ESATCCSDSPacket& packet) = 0;

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    // Called from OnBoardDataHandling.readTelecommand().
    virtual boolean readTelecommand(ESATCCSDSPacket& packet) = 0;

    // Return true if there is new telemetry available;
    // Otherwise return false.
    // Called from OnBoardDataHandling.readSubsystemsTelemetry().
    virtual boolean telemetryAvailable() = 0;

    // Update the subsystem.
    // Called from OnBoardDataHandling.updateSubsystems().
    virtual void update() = 0;

    // Send a telemetry packet to this subsystem.
    // Called from OnBoardDataHandling.writeTelemetry().
    virtual void writeTelemetry(ESATCCSDSPacket& packet) = 0;
};

#endif
