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

#ifndef ESATTelemetryManager_h
#define ESATTelemetryManager_h

#include <Energia.h>

class ESATTelemetryManager
{
  public:
    // Send a telemetry packet.
    void send(String telemetry, byte type, byte subsystemIdentifier);

    // Store a telemetry packet.
    void store(String telemetry);

    enum TelemetryType
    {
      HOUSEKEEPING_TELEMETRY = 1,
      EVENT_TELEMETRY = 2,
    };

  private:
    // Build a telemetry packet.
    String buildPacket(String content, byte type, byte subsystemIdentifier);
};

extern ESATTelemetryManager TelemetryManager;

#endif
