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

class ESATSubsystem
{
  public:
    virtual ~ESATSubsystem() {};

    // Start the subsystem.
    virtual void begin() = 0;

    // Return the application process identifier of this subsystem.
    // Each subsystem must have a unique 11-bit application process identifier.
    virtual word getApplicationProcessIdentifier() = 0;

    // Handle a telecommand.
    virtual void handleTelecommand(ESATCCSDSPacket& telecommand) = 0;

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    virtual boolean readTelemetry(ESATCCSDSPacket& packet) = 0;

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    virtual boolean readTelecommand(ESATCCSDSPacket& packet) = 0;

    // Return true if there is new telemetry available;
    // Otherwise return false.
    virtual boolean telemetryAvailable() = 0;

    // Update the subsystem.
    virtual void update() = 0;

    // Send a telemetry packet to this subsystem.
    virtual void writeTelemetry(ESATCCSDSPacket& packet) = 0;
};

#endif
