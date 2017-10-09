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

#ifndef ESATCOMMSSubsystem_h
#define ESATCOMMSSubsystem_h

#include <Energia.h>
#include "ESATCommand.h"
#include "ESATSubsystem.h"

class ESATCOMMSSubsystem: public ESATSubsystem
{
  public:
    // Start the communications subsystem.
    void begin();

    // Return the identifier of this subsystem.
    word getApplicationProcessIdentifier();

    // Handle a telecommand.
    void handleTelecommand(ESATCCSDSPacket& packet);

    // Fill a packet with the next available telecommand.
    void readTelecommand(ESATCCSDSPacket& packet);

    // Fill a packet with the next telemetry packet available.
    void readTelemetry(ESATCCSDSPacket& packet);

    // Return true if there is new telemetry available;
    // Otherwise return false.
    boolean telemetryAvailable();

    // Update the subsystem.
    void update();

    // Write a telemetry packet.
    void writePacket(ESATCCSDSPacket& packet);

  private:
    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 3;

    // Read a packet from an input stream.
    void readPacketFrom(Stream& input, ESATCCSDSPacket& packet);

    // Write a telemetry packet to an output stream.
    void writePacketTo(Stream& output, ESATCCSDSPacket& packet);
};

extern ESATCOMMSSubsystem COMMSSubsystem;

#endif
