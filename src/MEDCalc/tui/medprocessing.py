# Copyright (C) 2011-2022  CEA/DEN, EDF R&D
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
import SALOME
from medcalc.medevents import notifyGui_error


def ChangeUnderlyingMesh(fieldId, meshId):
  try:
    dataManager = medcalc.medcorba.factory.getDataManager()
    fieldHandler = dataManager.getFieldHandler(fieldId)

    # We don't modify the original field but create first a duplicate
    duplicate = medcalc.medcorba.factory.getCalculator().dup(fieldHandler)
    dataManager.changeUnderlyingMesh(duplicate.id, meshId)

    from medcalc.medevents import notifyGui_changeUnderlyingMesh
    notifyGui_changeUnderlyingMesh(duplicate.id)
    return duplicate.id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while changing underlying mesh:\n" + e.details.text)
    raise Exception(e.details.text)
#

def InterpolateField(fieldId,
                     meshId,
                     precision,
                     defaultValue,
                     reverse,
                     method,
                     nature,
                     intersectionType
                     ):
  params = MEDCALC.InterpolationParameters(precision, defaultValue, reverse, method, nature, intersectionType)
  try:
    dataManager = medcalc.medcorba.factory.getDataManager()
    fieldHandler = dataManager.interpolateField(fieldId, meshId, params)
    from medcalc.medevents import notifyGui_interpolateField
    notifyGui_interpolateField(fieldHandler.id)
    return fieldHandler.id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occurred while interpolating field:\n" + e.details.text)
    raise Exception(e.details.text)
#
