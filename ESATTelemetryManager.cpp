 /*
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

#include "ESATTelemetryManager.h"
#include "ESATClock.h"
#include "ESATCOMMSSubsystem.h"
#include "ESATStorage.h"
#include <ESATUtil.h>

String ESATTelemetryManager::buildPacket(String content, byte type, byte subsystemIdentifier)
{
  String checksum = "FFFF";
  String packet = String(int(subsystemIdentifier), DEC).substring(0, 1)
                + Util.byteToHexadecimal(byte(content.length()))
                + Util.byteToHexadecimal(type)
                + content
                + checksum;
  return packet;
}

void ESATTelemetryManager::send(String telemetry, byte type, byte subsystemIdentifier)
{
  String packet = buildPacket(telemetry, type, subsystemIdentifier);
  COMMSSubsystem.writePacket(packet);
}

void ESATTelemetryManager::store(String telemetry)
{
  String timestamp = Clock.read();
  String filename = timestamp.substring(0, 8)
                  + ".txt";
  Storage.write(filename, timestamp + " " + telemetry);
}

ESATTelemetryManager TelemetryManager;
