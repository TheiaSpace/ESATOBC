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

#include <Arduino.h>
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

    // Disable reception of telecommands from the USB interface.
    void disableUSBTelecommands();

    // Disable emission of telecommands through the USB interface.
    void disableUSBTelemetry();

    // Dispatch a command on the registered subsystems.
    void dispatchTelecommand(ESATCCSDSPacket& packet);

    // Enable reception of telecommands from the USB interface.
    void enableUSBTelecommands();

    // Enable emission of telemetry through the USB interface.
    void enableUSBTelemetry();

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

    // Update the registered subsystems.
    void updateSubsystems();

    // Write a telemetry packet to the subsystems that handle
    // telemetry.  For example, a communications subsystem may
    // transmit the packet to the ground station.
    void writeTelemetry(ESATCCSDSPacket& packet);

  private:
    static const byte MAXIMUM_NUMBER_OF_SUBSYSTEMS = 16;
    ESATSubsystem* subsystems[MAXIMUM_NUMBER_OF_SUBSYSTEMS];
    byte numberOfSubsystems;
    byte telecommandIndex;
    byte telemetryIndex;

    // True if the reception of telecommands from the USB interface is
    // enabled; false otherwise.
    boolean usbTelecommandsEnabled;

    // True if the emission of telemetry through the USB interface is
    // enabled; false otherwise.
    boolean usbTelemetryEnabled;

    // Read a telecommand from the USB interface.
    boolean readTelecommandFromUSB(ESATCCSDSPacket& packet);

    // Write a telemetry packet to the USB interface.
    void writeTelemetryToUSB(ESATCCSDSPacket& packet);
};

extern ESATOnBoardDataHandling OnBoardDataHandling;

#endif
