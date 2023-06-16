// Copyright (C) 2007-2023  CEA/DEN, EDF R&D
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

#include "MEDCouplingMeshFieldFactoryComponentClt.hxx"
#include "MEDCouplingMeshFieldFactoryComponent.hxx"
#include "MEDCouplingUMeshClient.hxx"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestRunner.h>

#include CORBA_CLIENT_HEADER(MEDCouplingCorbaServantTest)

#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace MEDCoupling;

CPPUNIT_TEST_SUITE_REGISTRATION( SALOME_TEST::MEDCouplingCorbaServBasicsTestClt );

int main(int argc, char* argv[])
{
  CPPUNIT_NS::TestResult controller;
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        
#ifdef WIN32
  CPPUNIT_NS::TextTestProgressListener progress;
#else
  CPPUNIT_NS::BriefTestProgressListener progress;
#endif
  controller.addListener( &progress );
  SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::_argc=argc;
  SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::_argv=argv;
  CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( suite );
  runner.run( controller);
  // ---  Print test in a compiler compatible format.
  std::ofstream testFile;
  testFile.open("UnitTestsResult", std::ios::out |  std::ios::trunc);
  //CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
  CPPUNIT_NS::CompilerOutputter outputter( &result, testFile );
  outputter.write(); 
  // ---  Run the tests.
  bool wasSucessful = result.wasSuccessful();
  testFile.close();
  // ---  Return error code 1 if the one of test failed.
  return wasSucessful ? 0 : 1;
}
