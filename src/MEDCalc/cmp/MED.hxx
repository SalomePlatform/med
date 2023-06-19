// Copyright (C) 2015-2023  CEA, EDF
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

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MED_Gen)
#include CORBA_SERVER_HEADER(MEDDataManager)
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include <SALOME_Component_i.hxx>

#include <map>
#include <string>

#include "MEDCALC.hxx"

class MEDCALC_EXPORT MED :
  public POA_MED_ORB::MED_Gen,
  public Engines_Component_i
{
public:
  MED(CORBA::ORB_ptr orb,
      PortableServer::POA_ptr poa,
      PortableServer::ObjectId* contId,
      const char* instanceName,
      const char* interfaceName,
	  bool checkNS = true);
  virtual ~MED();

  MED_ORB::status addDatasourceToStudy(const MEDCALC::DatasourceHandler& datasourceHandler);

  MED_ORB::status registerPresentationField(CORBA::Long fieldId,
                                            const char* name,
                                            const char* type,
                                            const char* ico,
                                            CORBA::Long presentationId);

  MED_ORB::status registerPresentationMesh(CORBA::Long meshId,
                                           const char* name,
                                           const char* type,
                                           const char* ico,
                                           CORBA::Long presentationId);

  MED_ORB::status unregisterPresentation(CORBA::Long presentationId);

//  // Caller owns the returned list, and is responsible for the list deletion.
//  MED_ORB::PresentationsList* getSiblingPresentations(CORBA::Long presentationId);

  // Get all presentations registered in the study
  MED_ORB::PresentationsList* getStudyPresentations();

  char* getStudyPresentationEntry(CORBA::Long presentationId);

  void cleanUp();

  /*! Dump the study as a Python file */
  virtual Engines::TMPFile* DumpPython(CORBA::Boolean isPublished,
                                       CORBA::Boolean isMultiFile,
                                       CORBA::Boolean& isValidScript);

  // For tooltips
  virtual CORBA::Boolean hasObjectInfo();
  virtual char* getObjectInfo(const char* entry);

  // Get Study
  virtual SALOMEDS::Study_var getStudyServant() = 0;

 private:
  std::map<long, std::string> _fieldSeriesEntries;
  std::map<long, std::string> _meshEntries;
};

#endif
