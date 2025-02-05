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

#include "MEDCouplingMeshFieldFactoryComponentServ.hxx"
#include "MEDCouplingMeshFieldFactoryComponent.hxx"

#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace MEDCoupling;

int mainCorbaServ(int argc, char **argv);

int mainCorbaServ(int argc, char **argv)
{
  CORBA::ORB_var orb=CORBA::ORB_init(argc,argv);
  CORBA::Object_var obj=orb->resolve_initial_references("RootPOA");
  PortableServer::POA_var poa=PortableServer::POA::_narrow(obj);
  PortableServer::POAManager_var mgr=poa->the_POAManager();
  mgr->activate();
  SALOME_TEST::MEDCouplingMeshFieldFactoryComponent *compo=new SALOME_TEST::MEDCouplingMeshFieldFactoryComponent(orb);
  SALOME_TEST::MEDCouplingMeshFieldFactory_var compoPtr=compo->_this();
  CORBA::String_var str=orb->object_to_string(compoPtr);
  std::ofstream stream(SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFileNameForIOR().c_str());
  stream << str << std::endl;
  stream.close();
  orb->run();
  delete compo;
  return 0;
}

int main(int argc, char* argv[])
{
  return mainCorbaServ(argc,argv);
}
