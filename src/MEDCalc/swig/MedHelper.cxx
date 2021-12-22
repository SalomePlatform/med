// Copyright (C) 2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MedHelper.h"

#include "SALOME_KernelServices.hxx"

#include "MED_Component_Generator.hxx"
#include "MEDFactory_Component_Generator.hxx"

std::string BuildMEDInstance()
{
  Engines::EngineComponent_var zeRef = RetrieveMEDInstance();
  CORBA::String_var ior = KERNEL::getORB()->object_to_string(zeRef);
  return std::string(ior.in());
}

std::string BuildMEDFactoryInstance()
{
  Engines::EngineComponent_var zeRef = RetrieveMEDFactoryInstance();
  CORBA::String_var ior = KERNEL::getORB()->object_to_string(zeRef);
  return std::string(ior.in());
}

