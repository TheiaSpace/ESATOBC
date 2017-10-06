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

#ifndef ESATEPSSubsystem_h
#define ESATEPSSubsystem_h

#include <Energia.h>
#include "ESATSubsystem.h"

class ESATEPSSubsystem: public ESATSubsystem
{
  public:
    // Start this subsystem.
    void begin();

    // Return the identifier of this subsystem.
    word getApplicationProcessIdentifier();

    // Handle a telecommand.
    void handleTelecommand(ESATCCSDSPacket& telecommand);

    // Fill a packet with the next telemetry packet available.
    // Set the error flag on error.
    void readTelemetry(ESATCCSDSPacket& telemetry);

    // Return true if a new telemetry packet is available.
    boolean telemetryAvailable();

    // Update the subsystem.
    void update();

  private:
    // I2C register numbers of the EPS board.
    enum RegisterNumbers
    {
      TELECOMMAND_PRIMARY_HEADER = 0,
      TELECOMMAND_PACKET_DATA = 1,
      TELECOMMAND_STATUS = 2,
      TELEMETRY_REQUEST = 3,
      TELEMETRY_STATUS = 4,
      TELEMETRY_VECTOR = 5,
    };

    // Identifier numbers of the telemetry packets.
    enum TelemetryPacketIdentifier
    {
      HOUSEKEEPING = 0,
    };

    // I2C address of the EPS board.
    static const byte ADDRESS = 2;

    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 1;

    // I2C messages are at most 16 bytes long.
    static const byte I2C_CHUNK_LENGTH = 16;

    boolean newTelemetryPacket;
};

extern ESATEPSSubsystem EPSSubsystem;

#endif
