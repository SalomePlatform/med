// Copyright (C) 2007-2024  CEA, EDF
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

#include "MEDCalculatorBasicsTest.hxx"
#include "MEDCalculatorBrowserLiteStruct.hxx"
#include "MEDCalculatorDBRangeSelection.hxx"
#include "MEDCalculatorDBField.hxx"

#include "SPythonParser.hxx"
#include "SPythonInterpreter.hxx"

#include "MEDLoader.hxx"

#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingFieldDouble.hxx"

#include <iostream>

void MEDCoupling::MEDCalculatorBasicsTest::testLightStruct1()
{
  const char fname[]="/export/home/geay/MEDStdAlone/Salome5/V5_1_main/MED/MED_INSTALL_DEBUG/share/salome/resources/med/TimeStamps_import22.med";
  MEDCalculatorBrowserLiteStruct lt(fname);
  lt.selectAllFields();
  //std::cout << lt.str() << std::endl;
}

void MEDCoupling::MEDCalculatorBasicsTest::testRangeSelection1()
{
  MEDCalculatorDBRangeSelection sel1(":");
  std::vector<std::size_t> v=sel1.getIds(6);
  CPPUNIT_ASSERT_EQUAL(6,(int)v.size());
  const int expected1[6]={0,1,2,3,4,5};
  CPPUNIT_ASSERT(std::equal(expected1,expected1+6,v.begin()));
  MEDCalculatorDBRangeSelection sel2(":-1");
  v=sel2.getIds(6);
  CPPUNIT_ASSERT_EQUAL(5,(int)v.size());
  CPPUNIT_ASSERT(std::equal(expected1,expected1+5,v.begin()));
  MEDCalculatorDBRangeSelection sel3("2:4");
  v=sel3.getIds(6);
  CPPUNIT_ASSERT_EQUAL(2,(int)v.size());
  CPPUNIT_ASSERT(std::equal(expected1+2,expected1+4,v.begin()));
  MEDCalculatorDBRangeSelection sel4(":3");
  v=sel4.getIds(6);
  CPPUNIT_ASSERT_EQUAL(3,(int)v.size());
  CPPUNIT_ASSERT(std::equal(expected1,expected1+3,v.begin()));
  //
  bool exc=false;
  MEDCalculatorDBRangeSelection sel5("2");
  try {  sel5="3:1"; } catch(INTERP_KERNEL::Exception& e) { exc=true; }
  CPPUNIT_ASSERT(exc); exc=false;
  try {  sel5="4:5:7"; } catch(INTERP_KERNEL::Exception& e) { exc=true; }
  CPPUNIT_ASSERT(exc); exc=false;
  try {  sel5="4:5r"; } catch(INTERP_KERNEL::Exception& e) { exc=true; }
  CPPUNIT_ASSERT(exc); exc=false;
  try {  sel5="4:ry5"; } catch(INTERP_KERNEL::Exception& e) { exc=true; }
  CPPUNIT_ASSERT(exc); exc=false;
  try {  sel5="4r:7"; } catch(INTERP_KERNEL::Exception& e) { exc=true; }
  CPPUNIT_ASSERT(exc); exc=false;
  try {  sel5="r4:7"; } catch(INTERP_KERNEL::Exception& e) { exc=true; }
  CPPUNIT_ASSERT(exc); exc=false;
  sel5="3:7";
  try {  v=sel5.getIds(6); } catch(INTERP_KERNEL::Exception& e) { exc=true; }
  CPPUNIT_ASSERT(exc); exc=false;
}

void MEDCoupling::MEDCalculatorBasicsTest::testDBField1()
{
  const char fName[]="hfile1.med";
  generateAFile1(fName);
  MEDCalculatorBrowserLiteStruct lt(fName);
  lt.selectAllFields();
  MEDCalculatorDBFieldReal *Power=new MEDCalculatorDBFieldReal(lt.getField(0));
  MEDCalculatorDBFieldReal *Power2=new MEDCalculatorDBFieldReal(lt.getField(0));
  MEDCalculatorDBFieldReal *Power3=(MEDCalculatorDBFieldReal *)((*Power)+(*Power2));
  Power3->decrRef();
  Power2->decrRef();
  Power->decrRef();
  //
  MEDCalculatorDBRangeSelection t1("2:4"),t2("6:8");
  MEDCalculatorDBRangeSelection p;
  MEDCalculatorDBRangeSelection c1,c2;
  //
  Power=new MEDCalculatorDBFieldReal(lt.getField(0));
  Power2=(*Power)(t1,p,c1);
  Power3=(*Power)(t2,p,c1);
  MEDCalculatorDBField *Power4=(*Power2)+(*Power3);
  Power4->decrRef();
  Power3->decrRef();
  Power2->decrRef();
  Power->decrRef();
}

void MEDCoupling::MEDCalculatorBasicsTest::generateAFile1(const char *fName)
{
  double targetCoords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  mcIdType targetConn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  MEDCouplingUMesh *m=MEDCouplingUMesh::New();
  m->setName("AMesh");
  m->setMeshDimension(2);
  m->allocateCells(5);
  m->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn);
  m->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,targetConn+4);
  m->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,targetConn+7);
  m->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn+10);
  m->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn+14);
  m->finishInsertingCells();
  DataArrayDouble *myCoords=DataArrayDouble::New();
  myCoords->alloc(9,2);
  myCoords->setInfoOnComponent(0,"x [cm]");
  myCoords->setInfoOnComponent(1,"y [cm]");
  std::copy(targetCoords,targetCoords+18,myCoords->getPointer());
  m->setCoords(myCoords);
  myCoords->decrRef();
  //
  WriteUMesh(fName,m,true);
  static const int nbOfTimeSteps=10;
  static const int nbOfComponents=7;
  MEDCouplingFieldDouble *f=MEDCouplingFieldDouble::New(ON_CELLS,ONE_TIME);
  f->setName("Power");
  f->setMesh(m);
  DataArrayDouble *da=DataArrayDouble::New();
  da->alloc(5,7);
  f->setArray(da);
  da->setInfoOnComponent(0,"aaa [a]"); da->setInfoOnComponent(1,"bbb [b]"); da->setInfoOnComponent(2,"ccc [c]");
  da->setInfoOnComponent(3,"ddd [d]"); da->setInfoOnComponent(4,"eee [e]"); da->setInfoOnComponent(5,"fff [f]");
  da->setInfoOnComponent(6,"ggg [g]");
  f->checkConsistencyLight();
  for(int i=0;i<nbOfTimeSteps;i++)
    {
      double *pt=da->getPointer();
      for(int j=0;j<5;j++)
        for(int k=0;k<nbOfComponents;k++,pt++)
          *pt=(i+1)*100.+(j+1)*10.+k+1;
      f->setTime(i*0.1,i,-i);
      WriteFieldUsingAlreadyWrittenMesh(fName,f);
    }
  da->decrRef();
  f->decrRef();
  m->decrRef();
}

void MEDCoupling::MEDCalculatorBasicsTest::testSPython1()
{
  std::string s1,s2;
  bool b;
  s1="12:3";
  CPPUNIT_ASSERT(MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT_EQUAL(s2,s1);
  s1="12:";
  CPPUNIT_ASSERT(MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT_EQUAL(s2,s1);
  s1="12";
  CPPUNIT_ASSERT(MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  CPPUNIT_ASSERT(b);
  CPPUNIT_ASSERT_EQUAL(s2,s1);
  s1="";
  CPPUNIT_ASSERT(!MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  s1="          ";
  CPPUNIT_ASSERT(!MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  s1=":";
  CPPUNIT_ASSERT(MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT_EQUAL(s2,s1);
  s1=":12";
  CPPUNIT_ASSERT(MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT_EQUAL(s2,s1);
  s1=":12:";
  CPPUNIT_ASSERT(!MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  s1=":12   23";
  CPPUNIT_ASSERT(!MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  // some ' ' char
  s1="  12  : 3  ";
  CPPUNIT_ASSERT(MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT_EQUAL(s2,std::string("12:3"));
  s1="  12  : 3";
  CPPUNIT_ASSERT(MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT_EQUAL(s2,std::string("12:3"));
  s1="  12  :3  ";
  CPPUNIT_ASSERT(MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT_EQUAL(s2,std::string("12:3"));
  s1="  12: 3  ";
  CPPUNIT_ASSERT(MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT_EQUAL(s2,std::string("12:3"));
  s1="12  : 3  ";
  CPPUNIT_ASSERT(MEDCoupling::SPythonParser::isElementInParenthesisMatching(s1,s2,b));
  CPPUNIT_ASSERT(!b);
  CPPUNIT_ASSERT_EQUAL(s2,std::string("12:3"));
}

void MEDCoupling::MEDCalculatorBasicsTest::testSPython2()
{
  std::string s1,s2;
  s1="  12:3  , : ,4  ";
  CPPUNIT_ASSERT(MEDCoupling::SPythonParser::isParenthesisMatching(s1,s2));
  CPPUNIT_ASSERT_EQUAL(s2,std::string("\"12:3\",\":\",4"));
}

void MEDCoupling::MEDCalculatorBasicsTest::testSPython3()
{
  std::string s1,s2;
  s1="(  12:3  , : ,4  )";
  s2=MEDCoupling::SPythonInterpreter::strip(s1);
  CPPUNIT_ASSERT_EQUAL(s2,std::string("(12:3,:,4)"));
  s1=" (  12:3  , : ,4  )  ";
  s2=MEDCoupling::SPythonInterpreter::strip(s1);
  CPPUNIT_ASSERT_EQUAL(s2,std::string("(12:3,:,4)"));
}
