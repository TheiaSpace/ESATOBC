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

#ifndef ESATSubsystemManager_h
#define ESATSubsystemManager_h

#include <Energia.h>
#include "ESATCommand.h"
#include "ESATSubsystem.h"

class ESATSubsystemManager
{
  public:
    enum TelemetryType
    {
      HOUSEKEEPING_TELEMETRY = 1,
      EVENT_TELEMETRY = 2,
    };

    ESATSubsystemManager();

    // Begin the registered subsystems.
    void beginSubsystems();

    // Dispatch a command on the registered subsystems.
    void dispatchCommand(byte subsystemIdentifier, byte commandCode, String parameters);

    // Read an incomming telecommand.
    ESATCommand readCommand();

    // Register the default subsystems.
    void registerDefaultSubsystems();

    // Register a subsystem.
    void registerSubsystem(ESATSubsystem* subsystem);

    // Read the telemetry vector from the registered subsystems.
    String readSubsystemsTelemetry();

    // Send a telemetry packet.
    void sendTelemetry(String telemetry, byte type, byte subsystemIdentifier);

    // Store a telemetry packet.
    void storeTelemetry(String telemetry);

    // Update the registered subsystems.
    void updateSubsystems();

  private:
    static const byte maximumNumberOfSubsystems = 16;
    ESATSubsystem* subsystems[maximumNumberOfSubsystems];
    byte numberOfSubsystems;

    // Build a telemetry packet.
    String buildPacket(String content, byte type, byte subsystemIdentifier);
};

extern ESATSubsystemManager SubsystemManager;

#endif
