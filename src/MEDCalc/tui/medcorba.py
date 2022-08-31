# Copyright (C) 2015-2022  CEA/DEN, EDF R&D
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

# Initializing some CORBA stuff

# Remember SALOME definitions:
# ---------------------------
#
# componentName = Name of the component (a library lib<componentName>Engine
# should exist with a C function named <componentName>Engine_factory, and the
# component should be registered in the catalog MEDCatalog.xml).
#
# corbaModule = Name of the corba module that contains the IDL
# specifications of the component (name as defined in the idl file)
#
# containerType = Name of the container factory
#
factoryComponentName = "MEDFactory"
medComponentName = "FIELDS"
#corbaModule   = "MEDCALC"
containerType = "FactoryServer"

import salome
if salome.lcc is None:
  salome.salome_init()
  pass

import MEDCALC   # corbaModule
med_engine = salome.lcc.FindOrLoadComponent(containerType,medComponentName)
factory = salome.lcc.FindOrLoadComponent(containerType,factoryComponentName)
# The factory is not the main CORBA component of the SALOME module FIELDS
# (i.e. the engine associated to the study), but the CORBA
# entry point for MED fields operations (i.e. a CORBA component
# reachable through the LifeCycleCORBA). This entry point is used to
# get the other SALOME CORBA components required for MED field
# operations, in particular the dataManager and the calculator
