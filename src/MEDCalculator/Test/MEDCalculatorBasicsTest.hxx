// Copyright (C) 2007-2025  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Author : Anthony Geay (CEA/DEN)

#ifndef __MEDCALCULATORBASICSTEST_HXX__
#define __MEDCALCULATORBASICSTEST_HXX__

#include <cppunit/extensions/HelperMacros.h>

namespace MEDCoupling
{
  class MEDCalculatorBasicsTest : public CppUnit::TestFixture
  {
    CPPUNIT_TEST_SUITE(MEDCalculatorBasicsTest);
    // CPPUNIT_TEST( testLightStruct1 );
    // CPPUNIT_TEST( testRangeSelection1 );
    CPPUNIT_TEST( testDBField1 );
    CPPUNIT_TEST( testSPython1 );
    CPPUNIT_TEST( testSPython2 );
    CPPUNIT_TEST( testSPython3 );
    CPPUNIT_TEST_SUITE_END();
  public:
    void testLightStruct1();
    void testRangeSelection1();
    void testDBField1();
    void testSPython1();
    void testSPython2();
    void testSPython3();
  private:
    static void generateAFile1(const char *fName);
  };
}

#endif
