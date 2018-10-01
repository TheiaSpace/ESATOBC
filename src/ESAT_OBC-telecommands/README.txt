Copyright (C) 2018 Theia Space, Universidad Politécnica de Madrid

This file is part of Theia Space's ESAT OBC library.

Theia Space's ESAT OBC library is free software: you can
redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either
version 3 of the License, or (at your option) any later version.

Theia Space's ESAT OBC library is distributed in the hope that it
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Theia Space's ESAT OBC library.  If not, see
<http://www.gnu.org/licenses/>.


OBC telecommand handlers provide telecommand responses.  Each
telecommand handler implements the ESAT_CCSDSPacketConsumer interface
from ESATUtil and responds to one or several telecommands.  They are
used by ESAT_OBCSubsystem.


# ESAT_OBCSetTimeTelecommand

Telecommand handler for OBC_SET_TIME (0x00): set the time of the
real-time clock of the ESAT OBC board.


# ESAT_OBCStoreTelemetry

Telecommand handler for OBC_STORE_TELEMETRY (0x01): enable or disable
the storage of telemetry packets in the SD card of the ESAT OBC board.


# ESAT_OBCDownloadStoredTelemetry

Telecommand handler for OBC_DOWNLOAD_STORED_TELEMETRY (0x02): download
telemetry packets stored in the SD card of the ESAT OBC board.


# ESAT_OBCEraseStoredTelemetry

Telecommand handler for OBC_ERASE_STORED_TELEMETRY (0x03): erase the
telemetry packets stored in the SD card of the ESAT OBC board.


# ESAT_OBCEnableTelemetryTelecommand

Telecommand handler for OBC_ENABLE_TELEMETRY (0x04): enable the
generation of a telemetry packet by ESAT_OBCSubsystem.


# ESAT_OBCDisableTelemetryTelecommand

Telecommand handler for OBC_DISABLE_TELEMETRY (0x05): disable the
generation of a telemetry packet by ESAT_OBCSubsystem.
