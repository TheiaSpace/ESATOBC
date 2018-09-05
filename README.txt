Copyright (C) 2017-2018 Theia Space, Universidad Politécnica de Madrid

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


ESAT OBC library version 4.1.0.

On-Board Computer (OBC) for ESAT.

This software runs in the ESAT OBC board.  Use it with Arduino 1.8.0
or newer together with the Arduino core for MSP430-based ESAT boards
(Theia Space's ESAT Boards (MSP430)) 2.2.0 or a newer 2.x.y version
and the ESAT utility library (ESATUtil) version 2.1.0 or a newer 2.x.y
version.  Use ESAT-OBC as the target board.  Depending on your ADCS
module, select ADCS Code Running in ADCS (option ADCS=ADCS, for newer
ADCS boards with a standalone microprocessor) or ADCS Code Running in
OBC (option ADCS=OBC, for legacy ADCS boards controlled by the OBC
board).  For option ADCS=OBC, use the ESAT ADCS library (ESATADCS)
version 3.2.0 or a newer 3.x.y version.

See the example OBC program (examples/OBC/OBC.ino).  This program uses
the modules of the OBC library.

The src/ directory contains the OBC library, which consists of the
following modules:


# ESAT_OnBoardDataHandling

On-Board Data Handling (OBDH), which is the main function of the OBC.
This library provides the general OBDH library functionality.


# ESAT_Subsystem

The OBDH library manages subsystems by means of objects that implement
the ESAT_Subsystem interface.


# ESAT_ADCSSubsystem

Attitude Determination and Control Subsystem.  Either communicates
with the ADCS board (option ADCS=ADCS, for using together with newer
ADCS boards with a standalone microprocessor) or wraps the ESAT_ADCS
library (option ADCS=OBC, for using together with legacy ADCS boards
controlled by the OBC board).


# ESAT_EPSSubsystem

Electrical Power Subsystem.  Communicates with the EPS board.


# ESAT_OBCSubsystem

On-Board Computer subsystem.


# ESAT_WifiSubsystem

Wifi communications subsystem.  Communicates with the Wifi board.


# ESAT_OBCClock

Access to the real-time clock mounted on the OBC board.


# ESAT_OBCLED

Control the on-board heartbeat led.


# ESAT_TelemetryStorage

Access to the memory card mounted on the OBC board for telemetry
storage.
