// Copyright (C) 2015  CEA/DEN, EDF R&D
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

#ifndef _MED_HXX_
#define _MED_HXX_

#ifdef WIN32
# if defined MEDENGINE_EXPORTS || defined MEDEngine_EXPORTS
#  define MEDENGINE_EXPORT __declspec( dllexport )
# else
#  define MEDENGINE_EXPORT __declspec( dllimport )
# endif
#else
# define MEDENGINE_EXPORT
#endif

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MED_Gen)
#include CORBA_SERVER_HEADER(MEDDataManager)
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include <SALOME_Component_i.hxx>
#include <SALOMEDS_Study.hxx>

#include <map>
#include <string>

class MEDENGINE_EXPORT MED :
  public POA_MED_ORB::MED_Gen,
  public Engines_Component_i
{
public:
  MED(CORBA::ORB_ptr orb,
      PortableServer::POA_ptr poa,
      PortableServer::ObjectId* contId,
      const char* instanceName,
      const char* interfaceName);
  virtual ~MED();

  MED_ORB::status addDatasourceToStudy(SALOMEDS::Study_ptr study,
                                       const MEDCALC::DatasourceHandler& datasourceHandler);

  MED_ORB::status registerPresentation(SALOMEDS::Study_ptr study,
                                       CORBA::Long fieldId,
                                       const char* name,
                                       const char* label);

  /*! Dump the study as a Python file */
  virtual Engines::TMPFile* DumpPython(CORBA::Object_ptr theStudy,
                                       CORBA::Boolean isPublished,
                                       CORBA::Boolean isMultiFile,
                                       CORBA::Boolean& isValidScript);

 private:
  std::map<long, std::string> _fieldSeriesEntries;
};

extern "C"
MEDENGINE_EXPORT
PortableServer::ObjectId* MEDEngine_factory( CORBA::ORB_ptr orb,
                                             PortableServer::POA_ptr poa,
                                             PortableServer::ObjectId* contId,
                                             const char* instanceName,
                                             const char* interfaceName );

#endif