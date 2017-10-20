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

#include <Arduino.h>
#include <ESATKISSStream.h>
#include "ESATSubsystem.h"

// Interface to the Wifi subsystem from the point of view of the
// on-board data handling subsystem.  There is a global instance:
// WifiSubsystem.  This should be the only instance of
// ESATWifiSubsystem.  Register the Wifi subsystem on the on-board
// data handling with
// OnBoardDataHandling.registerSubsystem(WifiSubsystem).
class ESATWifiSubsystem: public ESATSubsystem
{
  public:
    // Start the communications subsystem.
    void begin();

    // Return the identifier of this subsystem.
    word getApplicationProcessIdentifier();

    // Handle a telecommand.
    void handleTelecommand(ESATCCSDSPacket& packet);

    // Fill a packet with the next available telecommand.
    // Return true if there was a pending telecommand;
    // otherwise return false.
    boolean readTelecommand(ESATCCSDSPacket& packet);

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelemetry(ESATCCSDSPacket& packet);

    // Specify a buffer to accumulate the incomming telecommands.
    void setTelecommandBuffer(byte buffer[], unsigned long bufferLength);

    // Return true if there is new telemetry available;
    // Otherwise return false.
    boolean telemetryAvailable();

    // Update the subsystem.
    void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESATCCSDSPacket& packet);

  private:
    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 3;

    // Version number of the subsystem's interface.
    static const byte MAJOR_VERSION_NUMBER = 2;
    static const byte MINOR_VERSION_NUMBER = 0;
    static const byte PATCH_VERSION_NUMBER = 0;

    // Command code sent for connecting to the wireless network and
    // ground segment server.
    static const byte CONNECT = 0x00;

    // This line is pulled up when the Wifi board is not connected
    // to the server and it is pulled down when the Wifi board is
    // connected to the server.
    static const byte NOT_CONNECTED_SIGNAL_PIN = ESP0;

    // Decode KISS frames with telecommands with this stream.
    ESATKISSStream telecommandDecoder;

    // Return true if the Wifi board is connected to the server;
    // otherwise return false.
    boolean isConnected();
};

// Global instance of ESATWifiSubsystem.  Register WifiSubsystem on the
// on-board data handling module with
// OnBoardDataHandling.registerSubsystem(WifiSubsystem).
extern ESATWifiSubsystem WifiSubsystem;

#endif
