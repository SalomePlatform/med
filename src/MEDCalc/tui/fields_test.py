#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2018-2024  CEA, EDF
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
import unittest

class TestFields(unittest.TestCase):

    def setUp(self):
        import salome
        salome.salome_init()

    def processGuiEvents(self):
        import salome
        if salome.sg.hasDesktop():
            salome.sg.updateObjBrowser();
            import SalomePyQt
            SalomePyQt.SalomePyQt().processEvents()

    def test_fields(self):
        """Quick test for Fields module"""

        print()
        print('Testing Fields module')

        # ---- initialize Fields
        print('... Initialize Fields module')
        import medcalc
        medcalc.medconsole.setConsoleGlobals(globals())
        from medcalc.fieldproxy import FieldProxy

        # ---- get root dir
        fields_root_dir = os.getenv('FIELDS_ROOT_DIR', '')
        self.assertNotEqual(fields_root_dir, '', 'FIELDS_ROOT_DIR should be defined to load the test data')

        print('... Load MED file')
        # ---- load med file
        file_path = os.path.join(fields_root_dir, 'share', 'salome', 'resources', 'fields', 'medcalc_testfiles', 'smallmesh_varfield.med')
        medcalc.medio.LoadDataSource(file_path)
        self.processGuiEvents()

        print('... Get fields')
        # ---- get fields
        fields = medcalc.medevents.dataManager.getFieldHandlerList()
        self.assertTrue(len(fields) >= 2) # two make this working on repeating calls
        self.processGuiEvents()

        # **** get first field
        field_1 = fields[0]
        self.assertIsNotNone(field_1)
        self.assertEqual(field_1.meshid, 0)
        self.assertEqual(field_1.meshname, 'My2DMesh')
        self.assertEqual(field_1.fieldname, 'testfield1')
        self.assertEqual(field_1.type, 1)
        self.assertEqual(field_1.iteration, 1)
        self.assertEqual(field_1.order, 1)
        self.assertEqual(field_1.source, 'file://'+file_path)
        self.processGuiEvents()

        # **** get second field
        field_2 = fields[1]
        self.assertIsNotNone(field_2)
        self.assertEqual(field_2.meshid, 0)
        self.assertEqual(field_2.meshname, 'My2DMesh')
        self.assertEqual(field_2.fieldname, 'testfield2')
        self.assertEqual(field_2.type, 1)
        self.assertEqual(field_2.iteration, 1)
        self.assertEqual(field_2.order, 1)
        self.assertEqual(field_2.source, 'file://'+file_path)
        self.processGuiEvents()

        # ---- add two fields

        # **** add two fields using field handler
        print('... Add two fields using field handler directly')
        add_field_1 = medcalc.fieldproxy.calculator.add(field_1, field_2)
        self.assertIsNotNone(add_field_1)
        self.assertEqual(add_field_1.meshid, 0)
        self.assertEqual(add_field_1.meshname, 'My2DMesh')
        self.assertEqual(add_field_1.fieldname, 'testfield1+testfield2')
        self.assertEqual(add_field_1.type, 1)
        self.assertEqual(add_field_1.iteration, 1)
        self.assertEqual(add_field_1.order, 1)
        self.assertEqual(add_field_1.source, 'mem://testfield1+testfield2')
        self.processGuiEvents()

        # **** add two fields using proxies
        print('... Add two fields with field proxy')
        proxy_1 = FieldProxy(field_1)
        proxy_2 = FieldProxy(field_2)
        add_field_2 = proxy_1 + proxy_2
        self.assertIsNotNone(add_field_2)
        self.assertEqual(add_field_2.meshid, 0)
        self.assertEqual(add_field_2.meshname, 'My2DMesh')
        self.assertEqual(add_field_2.fieldname, 'testfield1+testfield2')
        self.assertEqual(add_field_2.type, 1)
        self.assertEqual(add_field_2.iteration, 1)
        self.assertEqual(add_field_2.order, 1)
        self.assertEqual(add_field_2.source, 'mem://testfield1+testfield2')
        self.processGuiEvents()

if __name__ == '__main__':
    exit = True
    # Keep Salome GUI alive
    if salome.sg.hasDesktop():
      exit=False
    unittest.main(exit=exit)
