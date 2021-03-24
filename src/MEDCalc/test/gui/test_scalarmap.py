# Copyright (C) 2011-2021  CEA/DEN, EDF R&D
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

""" Test of the scalarmap.

This script is to be passed as an argument of the ./salome command and will be executed within the SALOME
Python console.
"""

import os, sys
import SalomePyQt

from medcalc_testutils import GetScenarioDirGUI

sgPyQt = SalomePyQt.SalomePyQt()
sgPyQt.activateModule('MED')

import medcalc  # After module activation !!
localTestDir = sys.argv[1]
medcalc.PlayQtTestingScenario(os.path.join(localTestDir, 'test_scalarmap.xml'))

medcalc.RequestSALOMETermination()  # not equivalent to quit()!
