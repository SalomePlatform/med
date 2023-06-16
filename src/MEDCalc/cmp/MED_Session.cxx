// Copyright (C) 2015-2023  CEA/DEN, EDF R&D
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

#include "MED_Session.hxx"

MED_Session::MED_Session(CORBA::ORB_ptr orb,
                        PortableServer::POA_ptr poa,
                        PortableServer::ObjectId *contId,
                        const char *instanceName,
                        const char *interfaceName):MED(orb,poa,contId,instanceName,interfaceName, true)
{
  name_service = new SALOME_NamingService(_orb);
}

MED_Session::~MED_Session()
{

}

SALOMEDS::Study_var MED_Session::getStudyServant()
{
  static SALOMEDS::Study_var aStudy;
  if(CORBA::is_nil(aStudy))
  {
    CORBA::Object_ptr anObject = name_service->Resolve("/Study");
    aStudy = SALOMEDS::Study::_narrow(anObject);
  }
  return aStudy;
}

#include "MED_No_Session.hxx"

extern "C"
{
  /*!
    \brief Exportable factory function: create an instance of the MED component engine
    \param orb reference to the ORB
    \param poa reference to the POA
    \param contId CORBA object ID, pointing to the owner SALOME container
    \param instanceName SALOME component instance name
    \param interfaceName SALOME component interface name
    \return CORBA object identifier of the registered servant
  */
  PortableServer::ObjectId* FIELDSEngine_factory(CORBA::ORB_ptr orb,
                                              PortableServer::POA_ptr poa,
                                              PortableServer::ObjectId* contId,
                                              const char* instanceName,
                                              const char* interfaceName)
  {
    CORBA::Object_var o = poa->id_to_reference(*contId);
		Engines::Container_var cont = Engines::Container::_narrow(o);
		if(cont->is_SSL_mode())
		{
      MED_No_Session* component = new MED_No_Session(orb, poa, contId, instanceName, interfaceName);
      return component->getId();
    }
    else
    {
      MED* component = new MED_Session(orb, poa, contId, instanceName, interfaceName);
      return component->getId();
    }
  }
}
