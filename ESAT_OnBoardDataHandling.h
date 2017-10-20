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

#ifndef ESAT_OnBoardDataHandling_h
#define ESAT_OnBoardDataHandling_h

#include <Arduino.h>
#include <ESAT_CCSDSPacket.h>
#include <ESAT_KISSStream.h>
#include "ESAT_Subsystem.h"

// On-board data handling library.
// ESAT_OnBoardDataHandling operates on the subsystems (which
// implement the ESAT_Subsystem interface):
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
// Use the global instance ESAT_OnBoardDataHandling.
class ESAT_OnBoardDataHandlingClass
{
  public:
    enum TelemetryType
    {
      HOUSEKEEPING_TELEMETRY = 1,
      EVENT_TELEMETRY = 2,
    };

    ESAT_OnBoardDataHandlingClass();

    // Begin the registered subsystems.
    void beginSubsystems();

    // Disable reception of telecommands from the USB interface.
    void disableUSBTelecommands();

    // Disable emission of telecommands through the USB interface.
    void disableUSBTelemetry();

    // Dispatch a command on the registered subsystems.
    void dispatchTelecommand(ESAT_CCSDSPacket& packet);

    // Enable reception of telecommands from the USB interface.  Use
    // the buffer for accumulating the partially-received telecommands
    // from one call to readTelecommand() to the next.
    void enableUSBTelecommands(byte buffer[], unsigned long bufferLength);

    // Enable emission of telemetry through the USB interface.
    void enableUSBTelemetry();

    // Read an incomming telecommand and write it into a packet.
    // Return true if there was a valid telecommand available;
    // otherwise return false.
    boolean readTelecommand(ESAT_CCSDSPacket& packet);

    // Register a subsystem.
    void registerSubsystem(ESAT_Subsystem& subsystem);

    // Read the next available telemetry packet and write it into the
    // provided packet object.
    // Return true if there was a valid telemetry packet available;
    // otherwise return false.
    boolean readSubsystemsTelemetry(ESAT_CCSDSPacket& packet);

    // Update the registered subsystems.
    void updateSubsystems();

    // Write a telemetry packet to the subsystems that handle
    // telemetry.  For example, a communications subsystem may
    // transmit the packet to the ground station.
    void writeTelemetry(ESAT_CCSDSPacket& packet);

  private:
    static const byte MAXIMUM_NUMBER_OF_SUBSYSTEMS = 16;
    ESAT_Subsystem* subsystems[MAXIMUM_NUMBER_OF_SUBSYSTEMS];
    byte numberOfSubsystems;
    byte telecommandIndex;
    byte telemetryIndex;

    // Store incoming USB telecommands in this buffer.
    byte* usbTelecommandBuffer;

    // Length of the USB telecommand buffer.
    unsigned long usbTelecommandBufferLength;

    // Decode USB KISS frames with telecommands with this stream.
    ESAT_KISSStream usbTelecommandDecoder;

    // True if the reception of telecommands from the USB interface is
    // enabled; false otherwise.
    boolean usbTelecommandsEnabled;

    // True if the emission of telemetry through the USB interface is
    // enabled; false otherwise.
    boolean usbTelemetryEnabled;

    // Read a telecommand from the USB interface.
    boolean readTelecommandFromUSB(ESAT_CCSDSPacket& packet);

    // Write a telemetry packet to the USB interface.
    void writeTelemetryToUSB(ESAT_CCSDSPacket& packet);
};

// Global instance of the on-board data handling library.
extern ESAT_OnBoardDataHandlingClass ESAT_OnBoardDataHandling;

#endif /* ESAT_OnBoardDataHandling_h */
