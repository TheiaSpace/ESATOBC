Copyright (C) 2019 Theia Space, Universidad Politécnica de Madrid

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


ESAT OBC subsystems.  ESAT_OnBoardDataHandling uses them.


# ESAT_Subsystem

The on-board data handling library manages subsystems by means of
objects that implement the ESAT_Subsystem interface.


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


# ESAT_ThermalPayloadSubsystem

Thermal Payload subsystem.  Drives the Thermal Payload board.


# ESAT_WifiSubsystem

Wifi communications subsystem.  Communicates with the Wifi board.
