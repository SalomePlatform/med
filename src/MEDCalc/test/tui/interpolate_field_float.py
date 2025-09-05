#!/usr/bin/env python

import os
import sys
from salome.kernel import salome

salome.salome_init()

###
### FIELDS component
###

import salome.pvsimple as pvs

import medcalc
medcalc.medconsole.setConsoleGlobals(globals())
from salome.kernel import MEDCALC
from medcalc.medconsole import accessField

from medcalc_testutils import GetMEDFileDirTUI

source_file = os.path.join(GetMEDFileDirTUI(), "source_1.med")
target_file = os.path.join(GetMEDFileDirTUI(), "target_1.med")

source_id = medcalc.LoadDataSource(source_file)

presentation_id = medcalc.MakeMeshView(medcalc.GetFirstMeshFromDataSource(source_id), viewMode=MEDCALC.VIEW_MODE_REPLACE)

target_id = medcalc.LoadDataSource(target_file)

presentation_id = medcalc.MakeMeshView(medcalc.GetFirstMeshFromDataSource(target_id), viewMode=MEDCALC.VIEW_MODE_REPLACE)

result_id = medcalc.InterpolateField(fieldId=0,meshId=1,precision=1e-12,
                                     defaultValue=0,reverse=0,
                                     method='P0P0',nature='IntensiveConservation',intersectionType='Triangulation')

presentation_id = medcalc.MakeScalarMap(accessField(result_id), viewMode=MEDCALC.VIEW_MODE_REPLACE)

source = pvs.GetActiveSource()

presentation_id = medcalc.MakeMeshView(0, viewMode=MEDCALC.VIEW_MODE_OVERLAP)

if source:
  mini, maxi = source.CellData.GetArray("Mesh_field").GetRange()
  mini_ref = 0.5294822392944354
  maxi_ref = 0.8495972706827812

  assert abs(mini-mini_ref)<1e-5
  assert abs(maxi-maxi_ref)<1e-5

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
