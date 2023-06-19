// Copyright (C) 2007-2023  CEA, EDF
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

// Authors : Guillaume Boulant (EDF) - 01/06/2011

#include "MEDFactory_Session_i.hxx"
#include "MEDFactory_No_Session_i.hxx"

extern "C"
{
	PortableServer::ObjectId* MEDFactoryEngine_factory(
		CORBA::ORB_ptr orb,
		PortableServer::POA_ptr poa,
		PortableServer::ObjectId* contId,
		const char* instanceName,
		const char* interfaceName)
	{
		MESSAGE("PortableServer::ObjectId * MEDEngine_factory()");
		SCRUTE(interfaceName);
		CORBA::Object_var o = poa->id_to_reference(*contId);
		Engines::Container_var cont = Engines::Container::_narrow(o);
		if(cont->is_SSL_mode())
		{
			MEDFactory_No_Session_i* factory = new MEDFactory_No_Session_i(orb, poa, contId,
				instanceName,
				interfaceName);
			return factory->getId();
		}
		else
		{
			MEDFactory_i* factory = new MEDFactory_Session_i(orb, poa, contId,
				instanceName,
				interfaceName);
			return factory->getId();
		}
	}
}
