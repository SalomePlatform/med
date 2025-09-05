# Copyright (C) 2011-2025  CEA, EDF
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

def LoadDataSource(filename):
  dataManager = medcalc.medcorba.factory.getDataManager()
  handler = dataManager.loadDatasource(filename)
  from medcalc.medevents import notifyGui_addDatasource
  notifyGui_addDatasource(handler.id, filename)
  return handler.id
#

def LoadImageAsDataSource(filename):
  # get temp file name to generate med file from image
  import tempfile
  temp = tempfile.NamedTemporaryFile(suffix='.med')
  medfilename = temp.name
  temp.close()

  from .medimages import FieldBuilder
  builder = FieldBuilder()
  builder.image2med(filename, medfilename)
  return LoadDataSource(medfilename)
#

def GetFirstMeshFromDataSource(datasource_id):
  dataManager = medcalc.medcorba.factory.getDataManager()
  handlerLst = dataManager.getMeshHandlerList(datasource_id)
  if not len(handlerLst):
    return -1
  return handlerLst[0].id
#

def GetFirstFieldFromMesh(mesh_id):
  dataManager = medcalc.medcorba.factory.getDataManager()
  series = dataManager.getFieldseriesListOnMesh(mesh_id)
  if not len(series):
    return -1
  fields = dataManager.getFieldListInFieldseries(series[0].id)
  if not len(fields):
    return -1
  return fields[0].id
#
