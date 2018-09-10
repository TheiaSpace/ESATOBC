/*
 * Copyright (C) 2017-2018 Theia Space, Universidad Politécnica de Madrid
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

#ifndef ESAT_OBCSubsystem_h
#define ESAT_OBCSubsystem_h

#include <Arduino.h>
#include <ESAT_CCSDSTelemetryPacketBuilder.h>
#include <ESAT_FlagContainer.h>
#include "ESAT_Subsystem.h"

// Interface to the OBC (on-board computer subsystem) from the point
// of view of the on-board data handling subsystem.  There is a global
// instance: ESAT_OBCSubsystem.  This should be the only instance of
// ESAT_OBCSubsystemClass.  Register the OBC subsystem on the on-board
// data handling with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_OBCSubsystem).
class ESAT_OBCSubsystemClass: public ESAT_Subsystem
{
  public:
    // True when commanded to store telemetry.  False when commanded
    // not to store telemetry.
    boolean storeTelemetry;

    // Start the OBC.
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
    virtual void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESAT_CCSDSPacket& packet);

  private:
    // Command codes.
    enum CommandCode
    {
      SET_TIME = 0x00,
      STORE_TELEMETRY = 0x01,
      DOWNLOAD_STORED_TELEMETRY = 0x02,
      ERASE_STORED_TELEMETRY = 0x03,
    };

    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 0;

    // Version numbers.
    static const byte MAJOR_VERSION_NUMBER = 4;
    static const byte MINOR_VERSION_NUMBER = 0;
    static const byte PATCH_VERSION_NUMBER = 0;

    // List of available telemetry packet identifiers.
    ESAT_FlagContainer availableTelemetry;

    // True if we were commanded to download telemetry; false
    // otherwise.
    boolean downloadStoredTelemetry;

    // List of enabled telemetry packet identifiers.
    ESAT_FlagContainer enabledTelemetry;

    // List of pending telemetry packet identifiers.
    ESAT_FlagContainer pendingTelemetry;

    // Telemetry packet builder.
    ESAT_CCSDSTelemetryPacketBuilder telemetryPacketBuilder;

    // Command handlers.
    void handleSetTimeCommand(ESAT_CCSDSPacket& packet);
    void handleSetModeCommand(ESAT_CCSDSPacket& packet);
    void handleStoreTelemetry(ESAT_CCSDSPacket& packet);
    void handleDownloadStoredTelemetry(ESAT_CCSDSPacket& packet);
    void handleEraseStoredTelemetry(ESAT_CCSDSPacket& packet);

    // Read the next stored telemetry packet and fill the given packet buffer.
    // Return true on success; otherwise return false.
    // Set downloadTelemetry to false on unsuccessful read.
    boolean readStoredTelemetry(ESAT_CCSDSPacket& packet);
};

// Global instance of ESAT_OBCSubsystem.  Register the OBC subsystem
// on the on-board data handling module with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_OBCSubsystem).
extern ESAT_OBCSubsystemClass ESAT_OBCSubsystem;

#endif /* ESAT_OBCSubsystem_h */
