# Copyright (C) 2016-2024  CEA, EDF
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import os
from time import sleep

import medcalc
medcalc.medconsole.setConsoleGlobals(globals())
import MEDCALC
from medcalc.medconsole import accessField

from medcalc_testutils import GetMEDFileDirTUI

datafile = os.path.join(GetMEDFileDirTUI(), "agitateur.med")
source_id = medcalc.LoadDataSource(datafile)

# Field 55 = VITESSE_ELEM_DOM (ON_CELLS) at timestamp 0
# Field 65 = VITESSE_ELEM_DOM (ON_CELLS) at timestamp 10
presentation_id = medcalc.MakeVectorField(accessField(65), viewMode=MEDCALC.VIEW_MODE_REPLACE,
                                          colorMap=MEDCALC.COLOR_MAP_BLUE_TO_RED_RAINBOW,
                                          scalarBarRange=MEDCALC.SCALAR_BAR_CURRENT_TIMESTEP
                                          )
sleep(1)
medcalc.RemovePresentation(presentation_id)
sleep(1)
