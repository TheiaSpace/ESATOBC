/*
 * Copyright (C) 2017, 2018 Theia Space, Universidad Politécnica de Madrid
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

#ifndef ESAT_Subsystem_h
#define ESAT_Subsystem_h

#include <Arduino.h>
#include <ESAT_CCSDSPacket.h>

// Common subsystem interface from the point of view of the on-board
// data handling function.
// ESAT_OnBoardDataHandling operates on the subsystems on the main loop:
// - first, it calls readTelecommand() on all the subsystems;
// - for each telecommand read, it calls handleTelecommand() on the
//   telecommand target subsystem.
// - after that, it calls update() on all the subsystems;
// - then, it calls availableTelemetry() on each subsystem and,
//   while there is telemetry available, it calls readTelemetry();
// - finally, it calls writeTelemetry() on all the subsystems.
class ESAT_Subsystem
{
  public:
    // Next subsystem in the list of registered subsystems.
    // ESAT_OnBoardDataHandling uses this to keep a linked
    // list of registered subsystems: it can traverse
    // the list by going from one subsystem to the next subsystem
    // until reaching the end of the list at nullptr.
    // Only ESAT_OnBoardDataHandling should care about this.
    ESAT_Subsystem* nextSubsystem = nullptr;

    virtual ~ESAT_Subsystem() {};

    // Return the application process identifier of this subsystem.
    // Each subsystem must have a unique 11-bit application process
    // identifier.
    // ESAT_OnBoardDataHandling uses this to identify the subsystem.
    virtual word getApplicationProcessIdentifier() = 0;

    // Handle a telecommand.
    // OnBoardDataHandling.dispatchTelecommand() forwards telecommands
    // to their respective ESAT_Subsystem modules, identified by the
    // value returned by getApplicationProcessIdentifier().
    virtual void handleTelecommand(ESAT_CCSDSPacket& telecommand) = 0;

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    // Called from ESAT_OnBoardDataHandling.readTelecommand().
    virtual boolean readTelecommand(ESAT_CCSDSPacket& packet) = 0;

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    // Called from ESAT_OnBoardDataHandling.readSubsystemsTelemetry().
    virtual boolean readTelemetry(ESAT_CCSDSPacket& packet) = 0;

    // Update the subsystem.
    // Called from ESAT_OnBoardDataHandling.updateSubsystems().
    virtual void update() = 0;

    // Send a telemetry packet to this subsystem.
    // Called from ESAT_OnBoardDataHandling.writeTelemetry().
    virtual void writeTelemetry(ESAT_CCSDSPacket& packet) = 0;
};

#endif /* ESAT_Subsystem_h */
