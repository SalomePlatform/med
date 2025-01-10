# Copyright (C) 2016-2025  CEA, EDF
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
# Author: A. Bruneton (CEA)

import unittest, os, shutil, sys
from posixpath import basename

class MEDGUITest(unittest.TestCase):
  def __init__(self, methodName='runTest'):
    unittest.TestCase.__init__(self, methodName=methodName)
    self._tmpDir = ""
    self._removeDir = True

  def setUp(self):
    import tempfile
    self._tmpDir = tempfile.mkdtemp(prefix="med_gui_tests_")
    self._removeDir = True  # reset for each new test in the TestCase

  def tearDown(self):
    import shutil
    unittest.TestCase.tearDown(self)
    if self._removeDir:
      shutil.rmtree(self._tmpDir, False)

  def getTestName(self):
    """ Return name of the test being currently executed. """
    return self.id().split(".")[-1]

  def launchSalomeWithScript(self, scriptname):
    """ TODO: review this - what is the nicest way to launch SALOME GUI from a Python script? """
    from salome_instance import SalomeInstance
    from medcalc_testutils import GetScriptDirGUI
    args = "args:%s" % self._tmpDir
    pth = os.path.join(GetScriptDirGUI(), scriptname)
    # Launch SALOME with the test script:
    inst = SalomeInstance.start(with_gui=True, args=[pth, args])
    # And make sure SALOME is stopped before running next one:
    inst.stop()
    status = 0 # TODO review this
    if status:
      raise Exception("SALOME exited abnormally for this test!")

  def compareSnapshot(self, basename):
    """ Compare the screenshot in the current temporary test directory with the reference baseline.
    Assert if not matching. """
    import filecmp
    from medcalc_testutils import GetBaselineDirGUI

    base_pth = os.path.join(GetBaselineDirGUI(), basename)
    gen_path = os.path.join(self._tmpDir, basename)
    print(base_pth, gen_path)
    try:
      ret = filecmp.cmp(base_pth, gen_path, shallow=False)
    except OSError:
      import traceback
      traceback.print_exc()
      ret = False
    if not ret:
      # Keep file if comparison fails
      self._removeDir = False
      self.assertTrue(ret, "[%s] -- Failed screenshot equality, or unable to open baseline file - directory is kept alive: %s" % (self.getTestName(), self._tmpDir))
    return ret

  def prepareScenario(self, scenario, baseline, med_file):
    """ Copy scenario to current temporary test dir and substitute paths inside """
    from medcalc_testutils import GetScenarioDirGUI, GetMEDFileDirGUI
    scen_path = os.path.join(GetScenarioDirGUI(), scenario)
    scen_pth2 = os.path.join(self._tmpDir, scenario)
    try:
      shutil.copy(scen_path, scen_pth2)
    except IOError:
      raise Exception("Could not copy test scenario '%s' to local test directory!" % scen_path)
    with open(scen_pth2,'r') as f:
      filedata = f.read()
    filedata = filedata.replace("/tmp/%s" % baseline, "%s/%s" % (self._tmpDir, baseline))
    filedata = filedata.replace("/tmp/%s" % med_file, os.path.join(GetMEDFileDirGUI(), med_file))
    with open(scen_pth2,'w') as f:
      f.write(filedata)

  ##
  ## Now the tests themselves
  ##
  def testScalarMap(self):
    baseline = "test_scalarmap.png"
    med_file = "test_scalarmap.med"  # will change with Cedric's files
    scenario = "test_scalarmap.xml"
    self.prepareScenario(scenario, baseline, med_file)
    self.launchSalomeWithScript("test_scalarmap.py")
    self.compareSnapshot(baseline)

  def testScalarMap2(self):
    baseline = "test_scalarmap.png"
    med_file = "test_scalarmap.med"  # will change
    scenario = "test_scalarmap.xml"
    self.prepareScenario(scenario, baseline, med_file)
    self.launchSalomeWithScript("test_scalarmap.py")
    self.compareSnapshot(baseline)

if __name__ == "__main__":
  suite = unittest.TestSuite()
  suite.addTest(MEDGUITest('testScalarMap'))
  suite.addTest(MEDGUITest('testScalarMap2'))
#  suite.addTest(MEDGUITest('testIsoContour'))

  runner = unittest.TextTestRunner()
  ret = not runner.run(suite).wasSuccessful()
  sys.exit(ret)
