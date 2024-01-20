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

// Authors : Guillaume Boulant (EDF) - 01/06/2011

#ifndef _MED_DATAMANAGER_I_HXX_
#define _MED_DATAMANAGER_I_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDDataManager)
#include "SALOME_GenericObj_i.hh"

#include "MEDCouplingRemapper.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingFieldFloat.hxx"
using namespace MEDCoupling;

/*! This map is used for lifecycle management of fields used in this operator */
#include <map>
#include <string>

typedef std::map<long,MEDCALC::DatasourceHandler*> DatasourceHandlerMap;
typedef std::map<long,MEDCALC::DatasourceHandler*>::iterator DatasourceHandlerMapIterator;

typedef std::map<long,MEDCALC::MeshHandler*> MeshHandlerMap;
typedef std::map<long,MEDCALC::MeshHandler*>::iterator MeshHandlerMapIterator;

typedef std::map<long,MEDCALC::FieldseriesHandler*> FieldseriesHandlerMap;
typedef std::map<long,MEDCALC::FieldseriesHandler*>::iterator FieldseriesHandlerMapIterator;

typedef std::map<long,MEDCALC::FieldHandler*> FieldHandlerMap;
typedef std::map<long,MEDCALC::FieldHandler*>::iterator FieldHandlerMapIterator;
typedef std::map<long,bool> FieldPersistencyMap;
typedef std::map<long,bool>::iterator FieldPersistencyMapIterator;

typedef std::map<long,MEDCouplingFieldDouble*> FieldDoubleMap;
typedef std::map<long,MEDCouplingFieldDouble*>::iterator FieldDoubleMapIterator;

typedef std::map<long,MEDCouplingUMesh*> MeshMap;
typedef std::map<long,MEDCouplingUMesh*>::iterator MeshMapIterator;

#include "MEDCALC.hxx"
class MEDDataManager_i: public POA_MEDCALC::MEDDataManager,
                                       public SALOME::GenericObj_i
{
public:

  //
  // ===========================================================
  // Functions specified in the IDL interface
  // ===========================================================
  //
  // -----------------------------------------------------------
  // Datasource management
  MEDCALC_EXPORT MEDCALC::DatasourceHandler * loadDatasource(const char *filepath);
  MEDCALC_EXPORT MEDCALC::DatasourceHandler * getDatasourceHandler(const char *filepath);
  MEDCALC_EXPORT MEDCALC::DatasourceHandler * getDatasourceHandlerFromID(CORBA::Long sourceId);

  // -----------------------------------------------------------
  // Mesh management
  MEDCALC_EXPORT MEDCALC::MeshHandlerList * getMeshHandlerList(CORBA::Long datasourceId);
  MEDCALC_EXPORT MEDCALC::MeshHandler * getMeshHandler(CORBA::Long meshId);

  // -----------------------------------------------------------
  // Field management
  MEDCALC_EXPORT MEDCALC::FieldseriesHandlerList * getFieldseriesListOnMesh(CORBA::Long meshId);
  MEDCALC_EXPORT MEDCALC::FieldHandlerList * getFieldListInFieldseries(CORBA::Long fieldseriesId);

  MEDCALC_EXPORT CORBA::Long getFieldIdAtTimestamp(CORBA::Long fieldseriesId, double timestamp);

  MEDCALC_EXPORT MEDCALC::FieldHandler *     getFieldHandler(CORBA::Long fieldHandlerId);
  MEDCALC_EXPORT char *                    getFieldRepresentation(CORBA::Long fieldHandlerId);
  MEDCALC_EXPORT MEDCALC::FieldHandlerList * getFieldHandlerList();

  MEDCALC_EXPORT void      saveFields(const char * filepath,
                                       const MEDCALC::FieldIdList & fieldIdList);
  MEDCALC_EXPORT void      markAsPersistent(CORBA::Long fieldHandlerId, bool persistent);
  MEDCALC_EXPORT void      savePersistentFields(const char * filepath);


  MEDCALC_EXPORT void updateFieldMetadata(CORBA::Long  fieldHandlerId,
                           const char * fieldname,
                           CORBA::Long  iteration,
                           CORBA::Long  order,
                           const char * source);

  MEDCALC_EXPORT void changeUnderlyingMesh(CORBA::Long fieldHandlerId, CORBA::Long meshHandlerId);
  MEDCALC_EXPORT MEDCALC::FieldHandler* interpolateField(CORBA::Long fieldHandlerId, CORBA::Long meshHandlerId, const MEDCALC::InterpolationParameters& params);

  MEDCALC_EXPORT void   setEventListenerIOR(const char * ior);
  MEDCALC_EXPORT char * getEventListenerIOR();

  MEDCALC_EXPORT void serverlog();

  MEDCALC_EXPORT void cleanUp();

  //
  // ===========================================================
  // Other public functions (non available via CORBA)
  // ===========================================================
  //
  static long LONG_UNDEFINED;
  MEDCALC_EXPORT static MEDDataManager_i * getInstance();
  MEDCALC_EXPORT MEDCouplingFieldDouble *  getFieldDouble(const MEDCALC::FieldHandler * fieldHandler);
  MEDCALC_EXPORT MEDCALC::FieldHandler *     addField(MEDCouplingFieldDouble * fieldDouble,
		                                       long meshHandlerId=LONG_UNDEFINED);

private:
  MEDDataManager_i();
  ~MEDDataManager_i();

  // The MEDDataManager is a singleton, whose instance can be obtained
  // using the getInstance static method.
  static MEDDataManager_i * _instance;

  long _sourceLastId;
  long _meshLastId;
  long _fieldseriesLastId;
  long _fieldLastId;
  DatasourceHandlerMap _datasourceHandlerMap;
  MeshHandlerMap _meshHandlerMap;
  FieldseriesHandlerMap _fieldseriesHandlerMap;
  FieldHandlerMap _fieldHandlerMap;
  FieldDoubleMap _fieldDoubleMap;
  MeshMap _meshMap;
  FieldPersistencyMap _fieldPersistencyMap;

  std::string _medEventListenerIOR;

  MEDCALC::FieldHandler * newFieldHandler(const char * fieldname,
                                          const char * meshname,
                                          TypeOfField  type,
                                          long         iteration,
                                          long         order,
                                          const char * source);

  MEDCALC::FieldHandler * updateFieldHandler(CORBA::Long fieldHandlerId,
                                             const char * fieldname,
                                             long         iteration,
                                             long         order,
                                             const char * source);

  std::string  file_to_source(const char * filepath);
  std::string  source_to_file(const char * source);
  bool isSourceInFile(const char * sourceName);
  long getDatasourceId(const char *filepath);

  MEDCouplingUMesh * getUMesh(long meshHandlerId);
  long getUMeshId(const MEDCouplingMesh * mesh);

  double getFieldTimestamp(CORBA::Long fieldHandlerId);

  INTERP_KERNEL::IntersectionType _getIntersectionType(const char* intersType);
  MEDCoupling::NatureOfField _getNatureOfField(const char* fieldNature);

};

#endif // _MED_DATAMANAGER_I_HXX_
