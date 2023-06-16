#!/usr/bin/env python3
# Copyright (C) 2007-2023  CEA/DEN, EDF R&D
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

# This file is a set of basic use case to test (from the python
# context) the functions developed in the MEDCALC engine and the
# associated MEDCALC CORBA interface (MEDDataManager and
# MEDCalaculator).
#
# (gboulant - 16/6/2011)
#

# WARN: this scripts is a unit tests runner for testing the SALOME
# MEDCALC CORBA components and it should stay self-consistent. Then,
# it's on purpose that the script does not use the xmed python
# package. Conversely, some (small) parts of this code could be
# redundant with code from the xmed package.

#
# ===============================================================
# Initializing some CORBA stuff
# ===============================================================
#

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
componentName = "MEDFactory"
corbaModule   = "MEDCALC"
containerType = "FactoryServer"

import salome
if salome.lcc is None:
    salome.salome_init()
__import__(corbaModule)
factory=salome.lcc.FindOrLoadComponent(containerType,componentName)
# This is not the main CORBA component of the SALOME module FIELDS
# (i.e. the engine associated to the study), but the CORBA
# entry point for MED fields operations (i.e. a CORBA component
# reachable throughout the LifeCycleCORBA). This entry point is used to
# get the other SALOME CORBA components required for MED field
# operations, in particular the dataManager and the calculator

#
# ==================================================
# Helper functions to localize tests files and get data
# ==================================================
#
import os

try:
    FIELDS_ROOT_DIR=os.environ["FIELDS_ROOT_DIR"]
except KeyError as e:
    raise RuntimeError("FIELDS_ROOT_DIR should be defined to load the test data")

RESDIR=os.path.join(FIELDS_ROOT_DIR,"share","salome","resources","fields","medcalc_testfiles")

def getFilePath(filename):
    """
    Returns the absolute path for a given file base name. The base
    name must match with a file contained in the test files directory.
    """
    filepath = os.path.join(RESDIR,filename)
    if not os.path.exists(filepath):
        raise RuntimeError("The file %s does not exists"%filepath)
    return filepath

testFileName = "smallmesh_varfield.med"
testMeshName = "My2DMesh"
testFieldName= "testfield2"
testFieldIt  = 1
testFieldDt  = 1
testTypeOfField = 1 # On nodes
testFilePath = getFilePath(testFileName)

#
# ==================================================
# Basic use cases of the MEDDataManager
# ==================================================
#
def TEST_getDataManager():
    dataManager = factory.getDataManager()
    if "loadDatasource" not in dir(dataManager):
        return False
    return True

def TEST_loadDatasource():
    dataManager = factory.getDataManager()
    datasource = dataManager.loadDatasource(testFilePath)
    if datasource.name != testFileName:
        print("ERR: datasource.name=%s (should be %s)"%(datasource.name,testFilePath))
        return False

    # We try to load the file twice. It should not load twice and
    # return the same datasource as previously registered (same id).
    sourceid_ref = datasource.id
    datasource = dataManager.loadDatasource(testFilePath)
    if datasource.id != sourceid_ref:
        print("ERR: datasource.id=%s (should be %s)"%(datasource.id,sourceid_ref))
        return False

    return True

def TEST_getFieldHandlerList():
    dataManager = factory.getDataManager()
    datasource = dataManager.loadDatasource(testFilePath)
    fieldHandlerList = dataManager.getFieldHandlerList()
    if fieldHandlerList is None or len(fieldHandlerList) == 0:
        return False
    return True

def TEST_getFieldRepresentation():
    dataManager = factory.getDataManager()
    datasource = dataManager.loadDatasource(testFilePath)
    fieldHandlerList = dataManager.getFieldHandlerList()
    fieldHandler0 = fieldHandlerList[0]

    print(dataManager.getFieldRepresentation(fieldHandler0.id))
    return True

def TEST_updateFieldMetadata():
    dataManager = factory.getDataManager()
    datasource = dataManager.loadDatasource(testFilePath)
    fieldHandlerList = dataManager.getFieldHandlerList()
    fieldHandler0 = fieldHandlerList[0]

    fieldid = fieldHandler0.id
    newname = fieldHandler0.fieldname + " modified"

    dataManager.updateFieldMetadata(fieldid, newname,
                                    fieldHandler0.iteration,
                                    fieldHandler0.order,
                                    fieldHandler0.source)

    fieldHandlerModified = dataManager.getFieldHandler(fieldid)
    print(fieldHandlerModified)

    if fieldHandlerModified.fieldname != newname:
        print("ERR: the name is %s (should be %s)"%(fieldHandlerModified.fieldname,newname))
        return False
    return True

def TEST_saveFields():
    dataManager = factory.getDataManager()
    datasource = dataManager.loadDatasource(testFilePath)
    fieldHandlerList = dataManager.getFieldHandlerList()
    fieldHandler0 = fieldHandlerList[0]
    fieldIdList = [fieldHandler0.id]
    filepath = "/tmp/test_xmed_saveFields.med"

    print("fieldIdList = %s"%fieldIdList)
    print("filepath = %s"%filepath)

    dataManager.saveFields(filepath,fieldIdList)
    # We just control that the file exists. But we should reload the
    # contents to check the fields
    import os
    if not os.path.exists(filepath):
        print("ERR: the file %s does not exist"%(filepath))
        return False
    return True

#
# ==================================================
# Use cases of the MEDDataManager for data loading
# ==================================================
#
def TEST_MEDDataManager_getMeshHandlerList():
    dataManager = factory.getDataManager()
    datasourceHandler = dataManager.loadDatasource(testFilePath)
    meshHandlerList = dataManager.getMeshHandlerList(datasourceHandler.id)
    print(meshHandlerList)

    if len(meshHandlerList) == 0:
        return False
    return True

def TEST_MEDDataManager_getMesh():
    dataManager = factory.getDataManager()
    datasourceHandler = dataManager.loadDatasource(testFilePath)
    meshHandlerList = dataManager.getMeshHandlerList(datasourceHandler.id)
    for mRef in meshHandlerList:
        meshId = mRef.id
        mRes = dataManager.getMeshHandler(meshId)
        print(mRes)
        if ( mRes.name != mRef.name ) or ( mRes.sourceid != mRef.sourceid):
            return False
    return True

def TEST_MEDDataManager_getFieldseriesListOnMesh():
    dataManager = factory.getDataManager()
    datasourceHandler = dataManager.loadDatasource(testFilePath)

    meshHandlerList = dataManager.getMeshHandlerList(datasourceHandler.id)
    # We look for the fieldseries defined on the first mesh of the list
    meshId = meshHandlerList[0].id
    fieldseriesList = dataManager.getFieldseriesListOnMesh(meshId)
    print(fieldseriesList)

    if len(fieldseriesList) == 0:
        return False
    return True

def TEST_MEDDataManager_getFieldListInFieldseries():
    dataManager = factory.getDataManager()
    testFilePath = os.path.join(RESDIR,testFileName)

    testFilePath  = getFilePath("timeseries.med")
    datasourceHandler = dataManager.loadDatasource(testFilePath)

    meshHandlerList = dataManager.getMeshHandlerList(datasourceHandler.id)
    # We look for the fieldseries defined on the first mesh of the list
    meshId = meshHandlerList[0].id
    fieldseriesList = dataManager.getFieldseriesListOnMesh(meshId)
    # We look for the fields defined in the first fieldseries,
    # i.e. the time steps for this field.
    fieldseriesId = fieldseriesList[0].id
    fieldList = dataManager.getFieldListInFieldseries(fieldseriesId)
    print(fieldList)

    if len(fieldList) == 0:
        return False
    return True

def TEST_MEDDataManager_getFieldIdAtTimestamp():
    dataManager = factory.getDataManager()
    testFilePath = os.path.join(RESDIR,testFileName)

    testFilePath  = getFilePath("timeseries.med")
    datasourceHandler = dataManager.loadDatasource(testFilePath)

    meshHandlerList = dataManager.getMeshHandlerList(datasourceHandler.id)
    # We look for the fieldseries defined on the first mesh of the list
    meshId = meshHandlerList[0].id
    fieldseriesList = dataManager.getFieldseriesListOnMesh(meshId)
    # We look for the fields defined in the first fieldseries,
    # i.e. the time steps for this field.
    fieldseriesId = fieldseriesList[0].id
    requestedTimestamp8 = 8
    fieldId8 = dataManager.getFieldIdAtTimestamp(fieldseriesId, requestedTimestamp8)
    timestamp8 = dataManager.getFieldTimestamp(fieldId8)
    print("fieldId8: ", fieldId8)
    print("timestamp8: ", timestamp8)
    requestedTimestamp9 = 9
    fieldId9 = dataManager.getFieldIdAtTimestamp(fieldseriesId, 9)
    timestamp9 = dataManager.getFieldTimestamp(fieldId9)
    print("fieldId9: ", fieldId9)
    print("timestamp9: ", timestamp9)

    if (timestamp8!=requestedTimestamp8):
      return False
    if (timestamp9!=requestedTimestamp9):
      return False
    return True

#
# ==================================================
# Use cases of the MEDCalculator
# ==================================================
#
def TEST_Calculator_basics():
    dataManager = factory.getDataManager()
    datasource = dataManager.loadDatasource(testFilePath)
    fieldHandlerList = dataManager.getFieldHandlerList()

    # Try to operate on the two first fields
    fieldHandler0 = fieldHandlerList[0]
    fieldHandler1 = fieldHandlerList[1]
    print(fieldHandler0)
    print(fieldHandler1)

    calculator = factory.getCalculator()
    add = calculator.add(fieldHandler0, fieldHandler1)
    print(add)
    sub = calculator.sub(fieldHandler0, fieldHandler1)
    print(sub)
    mul = calculator.mul(fieldHandler0, fieldHandler1)
    print(mul)
    div = calculator.div(fieldHandler0, fieldHandler1)
    print(div)
    #power = calculator.pow(fieldHandler0, 2)
    # print(power)
    linear = calculator.lin(fieldHandler0, 3,2)
    print(linear)

    return True

def TEST_Calculator_applyFunc():
    dataManager = factory.getDataManager()
    datasource = dataManager.loadDatasource(testFilePath)
    fieldHandlerList = dataManager.getFieldHandlerList()
    fieldHandler = fieldHandlerList[0]

    # In this example, "u" stands for the whole field
    calculator = factory.getCalculator()
    import MEDCALC
    nbResultingComponent = MEDCALC.NBCOMP_DEFAULT
    res = calculator.fct(fieldHandler,"abs(u)",nbResultingComponent);
    print(res)

    # In this example, "a" stands for the first component
    nbResultingComponent = 1
    res = calculator.fct(fieldHandler,"a+2",nbResultingComponent)
    print(res)

    return True

#
# ==================================================
# Use cases of the MEDDataManager that need MEDCalculator
# ==================================================
#
def TEST_markAsPersistent():
    dataManager = factory.getDataManager()
    datasource = dataManager.loadDatasource(testFilePath)
    fieldHandlerList = dataManager.getFieldHandlerList()
    fieldHandler0 = fieldHandlerList[0]
    fieldHandler1 = fieldHandlerList[1]

    calculator = factory.getCalculator()
    add = calculator.add(fieldHandler0, fieldHandler1)

    filepath = "/tmp/test_xmed_markAsPersistent.med"
    dataManager.markAsPersistent(add.id, True)
    dataManager.savePersistentFields(filepath)
    import os
    if not os.path.exists(filepath):
        print("ERR: the file %s does not exist"%(filepath))
        return False
    return True

#
# =============================================================
# Unit tests runner
# =============================================================
#
import unittest
from salome.kernel import pyunittester
class MyTestSuite(unittest.TestCase):

    # === MEDDataManager (core functions)
    def test_getDataManager(self):
        self.assertTrue(TEST_getDataManager())

    def test_loadDatasource(self):
        self.assertTrue(TEST_loadDatasource())

    def test_getFieldHandlerList(self):
        self.assertTrue(TEST_getFieldHandlerList())

    def test_getFieldRepresentation(self):
        self.assertTrue(TEST_getFieldRepresentation())

    def test_updateFieldMetadata(self):
        self.assertTrue(TEST_updateFieldMetadata())

    def test_saveFields(self):
        self.assertTrue(TEST_saveFields())

    # === MEDDataManager (data request functions)
    def test_MEDDataManager_getMeshHandlerList(self):
        self.assertTrue(TEST_MEDDataManager_getMeshHandlerList())

    def test_MEDDataManager_getMesh(self):
        self.assertTrue(TEST_MEDDataManager_getMesh())

    def test_MEDDataManager_getFieldseriesListOnMesh(self):
        self.assertTrue(TEST_MEDDataManager_getFieldseriesListOnMesh())

    def test_MEDDataManager_getFieldListInFieldseries(self):
        self.assertTrue(TEST_MEDDataManager_getFieldListInFieldseries())

    def test_MEDDataManager_getFieldIdAtTimestamp(self):
        self.assertTrue(TEST_MEDDataManager_getFieldIdAtTimestamp())

    # === MEDCalculator (need MEDDataManager)
    def test_Calculator_basics(self):
        self.assertTrue(TEST_Calculator_basics())

    def test_Calculator_applyFunc(self):
        self.assertTrue(TEST_Calculator_applyFunc())

    # === MEDDataManager (need MEDCalculator)
    def test_markAsPersistent(self):
        self.assertTrue(TEST_markAsPersistent())

def myunittests():
    pyunittester.run(MyTestSuite)

if __name__ == "__main__":
    myunittests()
