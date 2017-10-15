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

#include <Arduino.h>
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

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelecommand(ESATCCSDSPacket& packet);

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelemetry(ESATCCSDSPacket& telemetry);

    // Return true if a new telemetry packet is available.
    boolean telemetryAvailable();

    // Update the subsystem.
    void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESATCCSDSPacket& packet);

  private:
    // Identifier numbers of the telemetry packets.
    enum TelemetryPacketIdentifier
    {
      HOUSEKEEPING = 0,
    };

    // I2C address of the EPS board.
    static const byte ADDRESS = 2;

    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 1;

    // Version numbers of the subsystem interface.
    static const byte MAJOR_VERSION_NUMBER = 2;
    static const byte MINOR_VERSION_NUMBER = 0;
    static const byte PATCH_VERSION_NUMBER = 0;

    // Time between I2C telecommand and telemetry transfer retries.
    static const word MILLISECONDS_BETWEEN_RETRIES = 10;

    // Number of tries for I2C telecommand and telemetry transfers.
    static const byte TRIES = 10;

    // True when there is a new telemetry packet available
    // (after update()); false otherwise (after readTelemetry()).
    boolean newTelemetryPacket;

    // Set the time of the EPS board.
    void setTime();
};

extern ESATEPSSubsystem EPSSubsystem;

#endif
