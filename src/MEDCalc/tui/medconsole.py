# Copyright (C) 2012-2022  CEA/DEN, EDF R&D
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

import medcalc
import MEDCALC

dataManager = medcalc.medcorba.factory.getDataManager()

# IMPORTANT NOTE:
# the pyConsoleGlobals variable should hold the globals() dictionary of
# the python console context
pyConsoleGlobals = None

#-----
# This function is to be called from the working python console to
# specify the globals() dictionary (used in fieldtools for stat analysis)
#
# >>> medcalc.setConsoleGlobals(globals())
#
def setConsoleGlobals(pyconsole_globals):
  global pyConsoleGlobals
  pyConsoleGlobals = pyconsole_globals
#

import SALOME
def saveWorkspace(filename):
  """
  Dump your job in a med file. Only the fields marked as persistent
  are saved in the specified file.
  """
  try:
    dataManager.savePersistentFields(filename)
  except SALOME.SALOME_Exception as ex:
    medcalc.err(ex.details.text)
#

# Clean workspace
from .medevents import notifyGui_cleanWorkspace
def cleanWorkspace():
  dvars = pyConsoleGlobals
  if dvars is None:
    return
  all_keys = []
  for varkey, var in list(dvars.items()):
    if isinstance(var, medcalc.FieldProxy):
      all_keys.append("%s"%varkey)
  if len(all_keys) > 0:
    exec("del "+",".join(all_keys), pyConsoleGlobals)
  notifyGui_cleanWorkspace()
#

# Remove variable from console
from .medevents import notifyGui_removeFromWorkspace
def removeFromWorkspace(fieldProxy):
  dvars = pyConsoleGlobals
  if dvars is None:
    return
  for varkey, var in list(dvars.items()):
    if isinstance(var, medcalc.FieldProxy) and var.id == fieldProxy.id:
      exec(("del %s"%varkey), pyConsoleGlobals)
  notifyGui_removeFromWorkspace(fieldProxy.id)
#

# Get list a field with information using the commands ls and/or la
def getEnvironment(local=True, remote=False):
  """
  This function return the status of the medcalc context, i.e. the
  list of fields defined in this python session.
  """
  status=""
  if local is True:
    dvars = pyConsoleGlobals
    if dvars is None:
      medcalc.wrn("The stat function required the specification of the python context")
      medcalc.inf("Type this command \"import medcalc; medcalc.setConsoleGlobals(globals())")
    if remote is True:
      status="========= Fields used in the current context ===\n"
    for varkey in list(dvars.keys()):
      var = dvars[varkey]
      if isinstance(var, medcalc.FieldProxy):
        status+="%s \t(id=%s, name=%s)\n"%(varkey,var.id,var.fieldname)

  if remote is True:
    if local is True:
      status+="\n========= Fields available in the data manager ===\n"
    fieldHandlerList = dataManager.getFieldHandlerList()
    for fieldHandler in fieldHandlerList:
      status+="id=%s\tname\t= %s\n\tmesh\t= %s\n\t(it,dt)\t= (%s,%s)\n\tsource\t= %s\n"%(
        fieldHandler.id,
        fieldHandler.fieldname,
        fieldHandler.meshname,
        fieldHandler.iteration,
        fieldHandler.order,
        fieldHandler.source)
      status+="---------\n"

    if len(fieldHandlerList) > 0:
      status+="(use 'f=accessField(id)' to get a field in the current context)"

  return status
#

# For simpler typing, one can create a python command for status
# (avoid to type "print(getEnvironment())")
class ListFields(object):
  """
  A stat object displays the status of the med operating context, i.e. the
  list of fields defined in this python session.
  """
  def __init__(self, all=False):
    self.__local  = True
    self.__remote = all
    # all = True means that the local metadata (fieldproxy) and the
    # remote metadata on the engine (MEDCouplingFieldDouble) are
    # displayed by the stat command. Otherwise, only the local
    # metadata are displayed.
  #
  def __repr__(self):
    return getEnvironment(self.__local, self.__remote)
  #
#
# Creating the commands list (ls) and list all (la)
ls = ListFields(all=False)
la = ListFields(all=True)

#
# Add variable to workspace
from .medevents import notifyGui_putInWorkspace
def putInWorkspace(fieldProxy):
  """
  This function puts a reference to this field in the GUI data
  model. When a field is referenced in the GUI data model, then it
  belongs to the workspace. When the workspace is saved, all the
  field that belongs to the workspace are saved.
  """
  dataManager.markAsPersistent(fieldProxy.id, True)
  notifyGui_putInWorkspace(fieldProxy.id)
#

def accessField(fieldHandlerId):
  """
  This return a field proxy on the field identified by the specified
  field handler id.
  """
  return medcalc.newFieldProxy(fieldHandlerId)
#

def view(field):
  """
  Display the field at its timestep
  """
  medcalc.MakeScalarMap(field, viewMode=MEDCALC.VIEW_MODE_REPLACE, scalarBarRange=MEDCALC.SCALAR_BAR_CURRENT_TIMESTEP)
  pass
