# Copyright (C) 2015-2025  CEA, EDF
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
import SALOME
import salome

dataManager = medcalc.medcorba.factory.getDataManager()

# Initializing the notification system (events from components to GUI)

# The MEDEventListener is created in the GUI (WorkspaceController) and
# a reference is transmitted to the python context throw its IOR. The
# transmission consists in initializing the variable below from the
# GUI (who knows the IOR) when importing medcalc in the python console
# (see WorkspaceController)
__eventListener = None
def connectEventListener():
  global __eventListener
  try:
    eventListenerIOR = dataManager.getEventListenerIOR()
    __eventListener = salome.orb.string_to_object(eventListenerIOR)
  except SALOME.SALOME_Exception as e:
    medcalc.wrn("The event listener is not running yet")
    msg ="When you'll have loaded the MED GUI, "
    msg+="call explicitly \"medcalc.medevents.connectEventListener()\" "
    msg+="to connect the GUI event listener"
    medcalc.inf(msg)
    __eventListener = None
  except Exception as e:
    medcalc.err("An unknown error occurs. Check if this ior=%s is valid."%eventListenerIOR)
    print(e)
#

def eventListenerIsRunning():
  global __eventListener
  if __eventListener is not None:
    return True

  # Try to define the event listener
  connectEventListener()
  if __eventListener is None:
    # it definitely does not work
    medcalc.wrn("the GUI is not loaded yet and will not be notified of the modification")
    return False

  return True
#

def processMedEvent(event):
  __eventListener.processMedEvent(event)
#

# One can try to connect to the eventListener, but it will fail if the
# MED GUI is not loaded. That does not matter, because the event
# listener is used only to notify the GUI of some event. If the GUI is
# not loaded, there is no use of notification.
connectEventListener()


#
# ===================================================================
# Functions for events notification
# ===================================================================
#
# Note that these functions are not part of the class FieldProxy so
# that they could be used in another context than the FieldProxy instances
import MEDCALC

def __notifyGui(eventType, dataId=-1, filename="", presentationId=-1, msg=""):
  medEvent = MEDCALC.MedEvent(eventType, dataId, filename, presentationId, msg)
  if not eventListenerIsRunning(): return

  # Notify the GUI of the update event
  processMedEvent(medEvent)
#

def notifyGui_updateField(fieldId):
  """
  This function must be used to notify the GUI that the field
  meta-data have changed so it could update the gui
  presentations of this field.
  """
  __notifyGui(MEDCALC.EVENT_UPDATE_FIELD, dataId=fieldId)
#

def notifyGui_putInWorkspace(fieldId):
  __notifyGui(MEDCALC.EVENT_PUT_IN_WORKSPACE, dataId=fieldId)
#
def notifyGui_removeFromWorkspace(fieldId):
  __notifyGui(MEDCALC.EVENT_REMOVE_FROM_WORKSPACE, dataId=fieldId)

def notifyGui_cleanWorkspace():
  __notifyGui(MEDCALC.EVENT_CLEAN_WORKSPACE)
#
def notifyGui_addDatasource(handlerId, filename):
  __notifyGui(MEDCALC.EVENT_ADD_DATASOURCE, dataId=handlerId, filename=filename)
#
def notifyGui_addPresentation(fieldId, presId):
  __notifyGui(MEDCALC.EVENT_ADD_PRESENTATION, dataId=fieldId, presentationId=presId)
#
def notifyGui_removePresentation(presId):
  __notifyGui(MEDCALC.EVENT_REMOVE_PRESENTATION, presentationId=presId)
#
def notifyGui_modifyPresentation(presId):
  __notifyGui(MEDCALC.EVENT_MODIFY_PRESENTATION, presentationId=presId)
#
def notifyGui_visibilityChanged(presId):
  __notifyGui(MEDCALC.EVENT_VISIBILITY_CHANGED, presentationId=presId)

def notifyGui_playQtTestingScenario(filename):
  __notifyGui(MEDCALC.EVENT_PLAY_TEST, filename=filename)
#
def notifyGui_termination():
  __notifyGui(MEDCALC.EVENT_QUIT_SALOME)

def notifyGui_error(msg):
  __notifyGui(MEDCALC.EVENT_ERROR, msg=msg)

def notifyGui_changeUnderlyingMesh(fieldId):
  __notifyGui(MEDCALC.EVENT_CHANGE_UNDERLYING_MESH, dataId=fieldId)

def notifyGui_interpolateField(fieldId):
  __notifyGui(MEDCALC.EVENT_INTERPOLATE_FIELD, dataId=fieldId)
