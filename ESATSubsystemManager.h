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
#include "ESATSubsystem.h"

class ESATSubsystemManager
{
  public:
    ESATSubsystemManager();

    // Begin the registered subsystems.
    void beginSubsystems();

    // Dispatch a command on the registered subsystems.
    void dispatchCommand(byte subsystemIdentifier, byte commandCode, String parameters);

    // Register the default subsystems.
    void registerDefaultSubsystems();

    // Register a subsystem.
    void registerSubsystem(ESATSubsystem* subsystem);

    // Read the telemetry vector from the registered subsystems.
    String readSubsystemsTelemetry();

    // Update the registered subsystems.
    void updateSubsystems();

  private:
    static const byte maximumNumberOfSubsystems = 16;
    ESATSubsystem* subsystems[maximumNumberOfSubsystems];
    byte numberOfSubsystems;
};

extern ESATSubsystemManager SubsystemManager;

#endif
