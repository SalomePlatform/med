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

#include "MEDDataManager_i.hxx"
#include "SALOME_KernelServices.hxx"
#include "Basics_DirUtils.hxx"
#include "Basics_Utils.hxx"

#include "MEDLoader.hxx"
using namespace MEDCoupling;

#include <string>
#include <vector>
using namespace std;

MEDDataManager_i * MEDDataManager_i::_instance = NULL;
long MEDDataManager_i::LONG_UNDEFINED = -1;

MEDDataManager_i * MEDDataManager_i::getInstance() {
  // _GBO_ we will certainly need to define one single DataManager per
  // SALOME study and not one singleton for the whole session
  if ( _instance == NULL ) {
    _instance = new MEDDataManager_i();
  }
  return _instance;
}

#define IOR_UNDEF "IOR UNDEFINED"
MEDDataManager_i::MEDDataManager_i()
{
  LOG("Creating a MEDDataManager_i instance");
  _fieldLastId = 0;
  _sourceLastId = 0;
  _meshLastId = 0;
  _fieldseriesLastId = 0;
}

MEDDataManager_i::~MEDDataManager_i()
{
  LOG("Deleting MEDDataManager_i instance");
}

void MEDDataManager_i::cleanUp()
{
  _fieldLastId = 0;
  _sourceLastId = 0;
  _meshLastId = 0;
  _fieldseriesLastId = 0;

  // [ABN] Is it the proper way?
  _datasourceHandlerMap.clear();
  _meshHandlerMap.clear();
  _fieldseriesHandlerMap.clear();
  _fieldHandlerMap.clear();
  _fieldDoubleMap.clear();
  _meshMap.clear();
  _fieldPersistencyMap.clear();
}


std::string MEDDataManager_i::file_to_source(const char * filepath)
{
  string source("file://");
  source.append(filepath);
  return source;
}

std::string MEDDataManager_i::source_to_file(const char * source)
{
  string filepath(source);
  filepath.replace(0,7,"");
  return filepath;
}

/*!
 * Check if a source (from its uri name) is in a file or in memory
 */
bool MEDDataManager_i::isSourceInFile(const char * sourceName)
{
  std::string sourceString(sourceName);
  return (sourceString.substr(0, 7) == std::string("file://"));
}



/*!
 * This function loads the meta-data from the specified med file and
 * returns the associated datasource handler. The data source handler
 * is a key to retrieve all information concerning the data (meshes,
 * fields).
 */
MEDCALC::DatasourceHandler * MEDDataManager_i::loadDatasource(const char *filepath) {

  // We first check that this datasource is not already registered
  long sourceid = getDatasourceId(filepath);
  if ( sourceid != LONG_UNDEFINED ) {
      // The file is already registered under the identifier sourceid
      LOG("WRN: The file "<<filepath<<" is already registered with id="<<ToString(sourceid));
      return new MEDCALC::DatasourceHandler(*_datasourceHandlerMap[sourceid]);
  }

  // Then we check that the file is readable by MEDLoader
  CheckFileForRead(filepath);

  // Initialise the datasource handler
  MEDCALC::DatasourceHandler * datasourceHandler = new MEDCALC::DatasourceHandler();
  datasourceHandler->id = _sourceLastId; _sourceLastId++;
  datasourceHandler->name = (Kernel_Utils::GetBaseName(filepath)).c_str();
  std::string tmp(file_to_source(filepath));
  datasourceHandler->uri = CORBA::string_dup(tmp.c_str());
  _datasourceHandlerMap[datasourceHandler->id] = datasourceHandler;

  // We start by read the list of meshes (spatial supports of fields)
  vector<string> meshNames = GetMeshNames(filepath);
  int nbOfMeshes = meshNames.size();
  for (int iMesh = 0; iMesh < nbOfMeshes; iMesh++) {
      const char * meshName = meshNames[iMesh].c_str();
      LOG("name of mesh " << iMesh << " = " << meshName);

      MEDCALC::MeshHandler * meshHandler = new MEDCALC::MeshHandler();
      meshHandler->id       = _meshLastId; _meshLastId++;
      meshHandler->name     = meshName;
      meshHandler->sourceid = datasourceHandler->id;

      _meshHandlerMap[meshHandler->id] = meshHandler;

      // For each mesh, we can read the list of the names of the
      // associated fields, i.e. fields whose spatial support is this
      // mesh.
      vector<string> fieldNames = GetAllFieldNamesOnMesh(filepath,
          meshName);
      int nbOfFields = fieldNames.size();
      for (int iField = 0; iField < nbOfFields; iField++) {
          const char * fieldName = fieldNames[iField].c_str();
          LOG("-- name of field " << iField << " = " << fieldName);

          // A field name could identify several MEDCoupling fields, that
          // differ by their spatial discretization on the mesh (values on
          // cells, values on nodes, ...). This spatial discretization is
          // specified by the TypeOfField that is an integer value in this
          // list:
          // 0 = ON_CELLS
          // 1 = ON_NODES
          // 2 = ON_GAUSS_PT
          // 3 = ON_GAUSS_NE

          // As a consequence, before loading values of a field, we have
          // to determine the types of spatial discretization defined for
          // this field and to choose one.

          vector<TypeOfField> listOfTypes = GetTypesOfField(filepath,
              meshName,
              fieldName);
          int nbOfTypes = listOfTypes.size();
          for (int iType = 0; iType < nbOfTypes; iType++) {
              LOG("---- type "<<iType<<" of field "<<iField<< " = " << listOfTypes[iType]);

              // Then, we can get the iterations associated to this field on
              // this type of spatial discretization:
              std::vector< std::pair<int,int> > fieldIterations;

              if (listOfTypes[iType] == MEDCoupling::ON_CELLS || listOfTypes[iType] == MEDCoupling::ON_NODES)
                fieldIterations = GetFieldIterations(listOfTypes[iType],
                      filepath, meshName, fieldName);
              else
                {
                  LOG("---- WARNING - field " << fieldName << " is not on CELLS or on NODES");
                  typedef std::vector< std::pair< std::pair<int,int>, double> > TimeVec;
                  TimeVec fieldIterTime = GetAllFieldIterations(filepath, fieldName);
                  for (TimeVec::const_iterator it = fieldIterTime.begin(); it != fieldIterTime.end(); ++it)
                    fieldIterations.push_back(it->first);
                }

              int nbFieldIterations = fieldIterations.size();
              LOG("---- nb. iterations = " << nbFieldIterations);

              // We can define the timeseries of fields (fieldseries) for
              // this type. A fieldseries is a macro object that handle the whole
              // set of time iterations of a field.
              MEDCALC::FieldseriesHandler * fieldseriesHandler = new MEDCALC::FieldseriesHandler();
              fieldseriesHandler->id     = _fieldseriesLastId; _fieldseriesLastId++;
              fieldseriesHandler->name   = fieldName;
              fieldseriesHandler->type   = listOfTypes[iType];
              fieldseriesHandler->meshid = meshHandler->id;
              fieldseriesHandler->nbIter = nbFieldIterations;
              _fieldseriesHandlerMap[fieldseriesHandler->id] = fieldseriesHandler;

              // We can then load meta-data concerning all iterations
              for (int iterationIdx=0; iterationIdx<nbFieldIterations; iterationIdx++) {

                  int iteration = fieldIterations[iterationIdx].first;
                  int order = fieldIterations[iterationIdx].second;

                  const char * source = datasourceHandler->uri;
                  MEDCALC::FieldHandler * fieldHandler = newFieldHandler(fieldName,
                      meshName,
                      listOfTypes[iType],
                      iteration,
                      order,
                      source);

                  fieldHandler->meshid = meshHandler->id;
                  fieldHandler->fieldseriesId = fieldseriesHandler->id;
                  _fieldHandlerMap[fieldHandler->id] = fieldHandler;
                  //    LOG("=== Storing " << fieldName << " (" << fieldHandler->id << ")");
              }
          }
      }
  }

  return new MEDCALC::DatasourceHandler(*datasourceHandler);
}

long MEDDataManager_i::getDatasourceId(const char *filepath) {
  std::string uri(file_to_source(filepath));
  DatasourceHandlerMapIterator it = _datasourceHandlerMap.begin();
  while ( it != _datasourceHandlerMap.end() ) {
    if ( strcmp(it->second->uri,uri.c_str()) == 0 ) {
      return it->first;
    }
    ++it;
  }
  return LONG_UNDEFINED;
}

MEDCALC::DatasourceHandler*
MEDDataManager_i::getDatasourceHandler(const char *filepath)
{
  std::string uri(file_to_source(filepath));
  DatasourceHandlerMapIterator it = _datasourceHandlerMap.begin();
  while ( it != _datasourceHandlerMap.end() ) {
    if ( strcmp(it->second->uri,uri.c_str()) == 0 ) {
      return it->second;
    }
    ++it;
  }
  return NULL;
}

MEDCALC::DatasourceHandler*
MEDDataManager_i::getDatasourceHandlerFromID(CORBA::Long sourceid)
{
  DatasourceHandlerMapIterator it = _datasourceHandlerMap.find(sourceid);
  if (it != _datasourceHandlerMap.end())
    {
      return it->second;
    }
  return NULL;
}

MEDCALC::MeshHandler * MEDDataManager_i::getMeshHandler(CORBA::Long meshId) {
  if ( _meshHandlerMap.count(meshId) == 0 ) {
    std::string message =
      std::string("The mesh of id=") + ToString(meshId) +
      std::string(" does not exist in the data manager");
    LOG(message);
    throw KERNEL::createSalomeException(message.c_str());
  }
  return new MEDCALC::MeshHandler(*(_meshHandlerMap[meshId]));
}


/*!
 * This function returns the list of mesh handlers associated to the
 * specified datasource. It corresponds to the list of meshes defined
 * in the datasource.
 */
MEDCALC::MeshHandlerList * MEDDataManager_i::getMeshHandlerList(CORBA::Long datasourceId) {

  // We initiate a list with the maximum length
  MEDCALC::MeshHandlerList_var meshHandlerList = new MEDCALC::MeshHandlerList();
  meshHandlerList->length(_meshHandlerMap.size());

  // Scan the map looking for meshes associated to the specified datasource
  int itemIdx = 0;
  MeshHandlerMapIterator meshIt;
  for ( meshIt=_meshHandlerMap.begin(); meshIt != _meshHandlerMap.end(); meshIt++) {
    if ( meshIt->second->sourceid == datasourceId ) {
      meshHandlerList[itemIdx] = *(meshIt->second);
      itemIdx++;
    }
  }

  // Adjust the length to the real number of elements
  meshHandlerList->length(itemIdx);
  return meshHandlerList._retn();
}

/*!
 * This function returns the list of fieldseries defined on the
 * specified mesh.
 */
MEDCALC::FieldseriesHandlerList * MEDDataManager_i::getFieldseriesListOnMesh(CORBA::Long meshId) {
  // We initiate a list with the maximum length
  MEDCALC::FieldseriesHandlerList_var
    fieldseriesHandlerList = new MEDCALC::FieldseriesHandlerList();
  fieldseriesHandlerList->length(_fieldseriesHandlerMap.size());

  // Scan the map looking for fieldseries defined on the specified mesh
  int itemIdx = 0;
  FieldseriesHandlerMapIterator it;
  for ( it=_fieldseriesHandlerMap.begin(); it != _fieldseriesHandlerMap.end(); it++) {
    if ( it->second->meshid == meshId ) {
      fieldseriesHandlerList[itemIdx] = *(it->second);
      itemIdx++;
    }
  }

  // Adjust the length to the real number of elements
  fieldseriesHandlerList->length(itemIdx);
  return fieldseriesHandlerList._retn();
}

/*!
 * A fieldseries is a timeseries of fields. Then the list of fields is
 * the different time iterations defined for the specified field id.
 */
MEDCALC::FieldHandlerList * MEDDataManager_i::getFieldListInFieldseries(CORBA::Long fieldseriesId) {
  // We initiate a list with the maximum length
  MEDCALC::FieldHandlerList_var fieldHandlerList = new MEDCALC::FieldHandlerList();
  fieldHandlerList->length(_fieldHandlerMap.size());

  // Scan the map looking for field defined on the specified mesh
  int itemIdx = 0;
  FieldHandlerMapIterator it;
  for ( it=_fieldHandlerMap.begin(); it != _fieldHandlerMap.end(); it++) {
    if ( it->second->fieldseriesId == fieldseriesId ) {
      fieldHandlerList[itemIdx] = *(it->second);
      itemIdx++;
    }
  }

  // Adjust the length to the real number of elements
  fieldHandlerList->length(itemIdx);
  return fieldHandlerList._retn();
}

/*!
 *  Get the field id of a field series at a given timestep
 */
CORBA::Long MEDDataManager_i::getFieldIdAtTimestamp(CORBA::Long fieldseriesId, double timestampOfPvAnimation) {
  // Scan the map looking for field defined on the specified mesh at given timestep
  int itemIdx = 0;
  CORBA::Long fieldId = 0;
  double timestampOfField;
  FieldHandlerMapIterator it;
  for ( it=_fieldHandlerMap.begin(); it != _fieldHandlerMap.end(); it++) {
    if ( it->second->fieldseriesId == fieldseriesId ) {
      if (itemIdx == 0) {
        // set the fieldId to the first one in case it is not found
        fieldId = it->second->id;
      }
      timestampOfField = getFieldTimestamp(it->second->id);
      if (timestampOfField == timestampOfPvAnimation) {
        fieldId = it->second->id;
        break;
      }
      itemIdx++;
    }
  }
  return fieldId;
}

/*!
 * This returns the whole set of fields handlers for all datasource
 * that have been loaded using loadDatasource.
 */
MEDCALC::FieldHandlerList * MEDDataManager_i::getFieldHandlerList() {
  MEDCALC::FieldHandlerList_var fieldHandlerSeq = new MEDCALC::FieldHandlerList();
  fieldHandlerSeq->length(_fieldHandlerMap.size());

  int sequenceId = 0;
  FieldHandlerMapIterator fieldIt;
  for ( fieldIt=_fieldHandlerMap.begin(); fieldIt != _fieldHandlerMap.end(); fieldIt++) {
    fieldHandlerSeq[sequenceId] = *(fieldIt->second);
    sequenceId++;
  }
  return fieldHandlerSeq._retn();
}

/*!
 * This returns a copy of the fieldHandler associated to the specified id.
 */
MEDCALC::FieldHandler * MEDDataManager_i::getFieldHandler(CORBA::Long fieldHandlerId) {
//  LOG("getFieldHandler: START")

  FieldHandlerMapIterator fieldIt = _fieldHandlerMap.find(fieldHandlerId);
  if ( fieldIt != _fieldHandlerMap.end() ) {
    // >>> WARNING: CORBA struct specification indicates that the
    // assignment acts as a destructor for the structure that its
    // pointed to. The values of the fields are copied first in the new
    // structure that receives the assignment and finally the initial
    // structure is destroyed. In the present case, WE WANT to keep
    // the initial fieldHandler in the map. We must then make a deep
    // copy of the structure found in the map and return the copy. The
    // CORBA struct specification indicates that a deep copy can be
    // done using the copy constructor.  <<<
    return new MEDCALC::FieldHandler(*(fieldIt->second));
  }
  return NULL;
}

/*!
 * This returns a string representation of the field associated to the specified id.
 */
char * MEDDataManager_i::getFieldRepresentation(CORBA::Long fieldHandlerId) {
  LOG("getFieldRepresentation: START")
  MEDCALC::FieldHandler * fieldHandler = getFieldHandler(fieldHandlerId);
  MEDCouplingFieldDouble* fieldDouble = getFieldDouble(fieldHandler);
  return CORBA::string_dup(fieldDouble->getArray()->repr().c_str());
}

void MEDDataManager_i::saveFields(const char * filepath,
          const MEDCALC::FieldIdList & fieldIdList)
{
  LOG("saveFields to : " << filepath);

  // We first have to check if the target filepath is writable
  // (segmentation fault in med otherwise)
  if (!Kernel_Utils::IsWritable(Kernel_Utils::GetDirName(std::string(filepath)))) {
    std::string message =
      std::string("The target filepath ") +
      std::string(filepath) +
      std::string(" is not writable");
    LOG(message);
    throw KERNEL::createSalomeException(message.c_str());
  }

  if ( fieldIdList.length() == 0 ) {
    throw KERNEL::createSalomeException("No fields to save");
  }

  // Consider the first field to initiate the med file
  CORBA::Long fieldHandlerId = fieldIdList[0];
  MEDCALC::FieldHandler * fieldHandler = getFieldHandler(fieldHandlerId);
  MEDCouplingFieldDouble* fieldDouble = getFieldDouble(fieldHandler);

  try {
    bool writeFromScratch = true;
    WriteField(filepath, fieldDouble, writeFromScratch);

    writeFromScratch = false;
    for(CORBA::ULong i=1; i<fieldIdList.length(); i++) {
      fieldHandlerId = fieldIdList[i];
      fieldHandler = getFieldHandler(fieldHandlerId);
      fieldDouble = getFieldDouble(fieldHandler);
      WriteField(filepath, fieldDouble, writeFromScratch);
    }
  }
  catch (INTERP_KERNEL::Exception &ex) {
    std::string message =
      std::string("Error when saving file ") +
      std::string(filepath) + std::string(" : ") + ex.what();
    throw KERNEL::createSalomeException(message.c_str());
  }
  catch (const std::exception& ex) {
    std::string message =
      std::string("Error when saving file ") +
      std::string(filepath) + std::string(" : ") + ex.what();
    throw KERNEL::createSalomeException(message.c_str());
  }

}

/*!
 * This function must be used to indicate that the field with the
 * specified id must be considered as persistent (if persistent is
 * true) or not persistent (if persistent is false). If a field is
 * marked as persistent, then it is automatically saved when the
 * function savePersistentFields is called.
 */
void MEDDataManager_i::markAsPersistent(CORBA::Long fieldHandlerId, bool persistent) {
  LOG("mark as persistent : id="<<fieldHandlerId);
  _fieldPersistencyMap[fieldHandlerId] = persistent;
}

void MEDDataManager_i::savePersistentFields(const char * filepath) {
  LOG("savePersistentFields to : " << filepath);
  std::vector<long> listId;

  FieldPersistencyMapIterator mapIt;
  for ( mapIt = _fieldPersistencyMap.begin(); mapIt != _fieldPersistencyMap.end(); mapIt++) {
    if ( mapIt->second == true ) {
      listId.push_back(mapIt->first);
    }
  }

  MEDCALC::FieldIdList fieldIdList;
  fieldIdList.length(listId.size());
  for (int i=0; i<(int)listId.size(); i++) {
    fieldIdList[i] = CORBA::Long(listId[i]);
  }

  try {
    this->saveFields(filepath, fieldIdList);
  }
  catch (const SALOME::SALOME_Exception & ex) {
    throw ex;
  }
  catch (const std::exception& ex) {
    std::string message =
      std::string("Error when saving file ") +
      std::string(filepath) + std::string(" : ") + ex.what();
    throw KERNEL::createSalomeException(message.c_str());
  }
}

/*!
 * This function is responsible for creating the FieldHandler
 * instances. You must use this function because it manages
 * automatically the identifier value (autoincrementation of a static
 * variable)
 */
MEDCALC::FieldHandler * MEDDataManager_i::newFieldHandler(const char * fieldname,
              const char * meshname,
              TypeOfField  type,
              long         iteration,
              long         order,
              const char * source)
{
  MEDCALC::FieldHandler * fieldHandler = new MEDCALC::FieldHandler();
  fieldHandler->id = _fieldLastId; _fieldLastId++;
  fieldHandler->fieldname = fieldname;
  fieldHandler->meshname  = meshname;
  fieldHandler->type      = type;
  fieldHandler->iteration = iteration;
  fieldHandler->order     = order;
  fieldHandler->source    = source;
  return fieldHandler;
}

/*!
 * This updates the metadata of the field identified by its id with
 * the data of the given field handler. Returns a copy of the updated
 * handler (that should be identical to the given field handler for
 * all data but not for the id that is an invariant for all session
 * long).
 * WARN: you should be warned that this function could leave the data
 * model in a non-coherent state, by example if you change the mesh
 * name while the mesh has not been updated.
 */
MEDCALC::FieldHandler * MEDDataManager_i::updateFieldHandler(CORBA::Long fieldHandlerId,
                 const char * fieldname,
                 long         iteration,
                 long         order,
                 const char * source) {
  FieldHandlerMapIterator fieldIt = _fieldHandlerMap.find(fieldHandlerId);
  if ( fieldIt != _fieldHandlerMap.end() ) {
    // Update the attributes
    // >>> WARN: note that the id of a handler registered in the map
    // SHOULD NEVER be modified because it is the identifier used in
    // the whole application for this field all the session long.
    // <<<
    fieldIt->second->fieldname = fieldname;
    fieldIt->second->iteration = iteration;
    fieldIt->second->order     = order;
    fieldIt->second->source    = source;
    // Return a copy
    return new MEDCALC::FieldHandler(*fieldIt->second);
  }
  return NULL;
}

MEDCouplingUMesh * MEDDataManager_i::getUMesh(long meshHandlerId) {

  LOG("getUMesh: START")

  MEDCouplingUMesh * myMesh = NULL;
  if ( _meshMap.count(meshHandlerId) > 0 ) {
    // The mesh has been found in the map
    myMesh = _meshMap[meshHandlerId];
  } else {
    // The mesh is not loaded yet ==> load it and register it in the map
    LOG("getUMesh: the mesh must be loaded. meshid="<<meshHandlerId);
    if ( _meshHandlerMap[meshHandlerId] == NULL ) {
      std::string message =
  std::string("No mesh for id=") + ToString(meshHandlerId);
      LOG("getUMesh: "<<message);
      throw KERNEL::createSalomeException(message.c_str());
    }

    long sourceid = _meshHandlerMap[meshHandlerId]->sourceid;
    std::string filepath(source_to_file((_datasourceHandlerMap[sourceid])->uri));
    const char * meshName = _meshHandlerMap[meshHandlerId]->name;
    int meshDimRelToMax = 0;
    myMesh = ReadUMeshFromFile(filepath,meshName,meshDimRelToMax);
    _meshMap[meshHandlerId] = myMesh;
  }
  return myMesh;
}

/**
 * Try to retrieve the id of the specified mesh, i.e. the key it is
 * registered with in the internal meshes map.
 */
long MEDDataManager_i::getUMeshId(const MEDCouplingMesh * mesh) {
  bool found = false;
  MeshMapIterator it = _meshMap.begin();
  while ( it != _meshMap.end() ) {
    found = (it->second == mesh);
    if (found) {
      return it->first;
    }
    ++it;
  }
  return LONG_UNDEFINED;
}

/**
 * Get the timestamp associated to a field
 */
double MEDDataManager_i::getFieldTimestamp(CORBA::Long fieldHandlerId)
{
  LOG("getFieldTimestamp(" << fieldHandlerId << ")");
  double timestamp(0);
  MEDCALC::FieldHandler * fieldHandler = getFieldHandler(fieldHandlerId);

  // if the field is in a file, get the timestamp by MEDLoader tiny information
  long meshHandlerId = fieldHandler->meshid;
  long sourceid = _meshHandlerMap[meshHandlerId]->sourceid;
  if (isSourceInFile(fieldHandler->source))
  {
    const char * meshSourceUri = (_datasourceHandlerMap[sourceid])->uri;
    std::string fileName(source_to_file(meshSourceUri));
    const char * fieldName = fieldHandler->fieldname;
    //const char * meshName = _meshHandlerMap[meshHandlerId]->name; // todo: unused
    timestamp = GetTimeAttachedOnFieldIteration(fileName.c_str(), fieldName, fieldHandler->iteration, fieldHandler->order);
    LOG("timestamp in med file is " << timestamp);
  }
  else
  {
    // else get the timestamp with MEDCouplingField
    int iteration, order;
    // WARN: note that the variables "iteration" and "order" are passed
    // by reference to the function getTime (see documentation of
    // MEDCouplingField). As a consequence, the values of these
    // variables are updated by this function call. This is the means to
    // retrieve the iteration and order of the field.
    MEDCouplingFieldDouble* fieldDouble = getFieldDouble(fieldHandler);
    timestamp = fieldDouble->getTime(iteration, order);
    LOG("timestamp in MEDCouplingFieldDouble is " << timestamp);
  }
  return timestamp;
}

/*!
 * This method returns the physical data of the specified field,
 * i.e. the MEDCoupling field associated to the specified field
 * handler. If the field source is a file and the data ar not loaded
 * yet, the this function load the data from the file in a MEDCoupling
 * field instance. Otherwise, it just returns the MEDCoupling field
 * instance.
 */
MEDCouplingFieldDouble * MEDDataManager_i::getFieldDouble(const MEDCALC::FieldHandler * fieldHandler)
{

  LOG("getFieldDouble: START with id="<<fieldHandler->id);

  if ( _fieldDoubleMap.count(fieldHandler->id) > 0 ) {
  // The MEDCoupling field data are already loaded. Just return the
  // reference of the MEDCouplingFieldDouble pointer
    return _fieldDoubleMap[fieldHandler->id];
  }

  // The MEDCoupling field data are not loaded yet. Load the data and
  // register the MEDCoupling field in our internal map an all the
  // associated data if needed (i.e. the underlying mesh).

  // At this step, the mesh handler needs a meshid correctly
  // set. Normally, we should arrive at this step only in the case
  // where the field is loaded from a file ==> the meshid is defined
  // (see the loadDatasource function).
  //
  // >>>> __GBO__ TO BE CHECKED AND SERIOUSLY TESTED. There at least
  // one case where we can arrive here with no previous call to
  // loadDataSource: for example the field handler list can be obtained
  // from a call to addFieldsFromFile instead of loadDataSource (see
  // for example the getFieldRepresentation service of the
  // dataManager, that comes here and then calls getUMesh where we
  // need a map initialized only in loadDataSource) <<<<
  long meshid = fieldHandler->meshid;

  // We first have to check if the associated mesh is already loaded
  // and to load it if needed. The loaded meshes are registered in a
  // map whose key is the mesh handler id. This checking is
  // automatically done by the function getUMesh. It's important to do
  // it before the loading of field data to prevent from the case
  // where the mesh would not have been loaded already (in the
  // previous field loading).
  MEDCouplingUMesh * myMesh =this->getUMesh(meshid);

  long sourceid = _meshHandlerMap[meshid]->sourceid;

  std::string filepath(source_to_file((_datasourceHandlerMap[sourceid])->uri));
  std::string meshName(myMesh->getName());
  LOG("getFieldDouble: field "<<fieldHandler->fieldname<<" loaded from file "<<filepath);
  TypeOfField type = (TypeOfField)fieldHandler->type;
  int meshDimRelToMax = 0;
  MCAuto<MEDCouplingField> myFieldTmpp(ReadField(type,
                filepath,
                meshName,
                meshDimRelToMax,
                std::string(fieldHandler->fieldname),
                fieldHandler->iteration,
                fieldHandler->order));
  MCAuto<MEDCouplingFieldDouble> myField(DynamicCast<MEDCouplingField,MEDCouplingFieldDouble>(myFieldTmpp));

  // trying float field
  if (!myField){
    MCAuto<MEDCouplingFieldFloat> myFieldFloat(DynamicCast<MEDCouplingField,MEDCouplingFieldFloat>(myFieldTmpp));
    if (myFieldFloat){
      myField = myFieldFloat->convertToDblField();
      LOG("getFieldDouble: field "<<fieldHandler->fieldname<<" was read as float and converted to double.");
    }
  }

  myField->setMesh(myMesh);
  _fieldDoubleMap[fieldHandler->id] = myField.retn();
  return myField;
}

/*!
 * This adds the specified MEDCoupling field in the collection managed
 * by this DataManager. The associated FieldHandler is returned. This
 * is typically used in a context where the MEDCoupling field is
 * created from scratch, for example by operations in the
 * MEDCalculator.
 * @param[in] fieldDouble   the MEDCouplingFieldDouble instance to add
 * @param[in] meshHandlerId the id of the meshHandler this filed is associated to.
 * @return a copy of the FieldHandler registered in the internal map for this field.
 */
MEDCALC::FieldHandler * MEDDataManager_i::addField(MEDCouplingFieldDouble * fieldDouble,
             long meshHandlerId)
{
  std::string fieldName(fieldDouble->getName());
  std::string meshName(fieldDouble->getMesh()->getName());
  TypeOfField  type      = fieldDouble->getTypeOfField();

  int iteration, order;
  // WARN: note that the variables "iteration" and "order" are passed
  // by reference to the function getTime (see documentation of
  // MEDCouplingField). As a consequence, the values of these
  // variables are updated by this function call. This is the means to
  // retrieve the iteration and order of the field.
  /*double timestamp = */fieldDouble->getTime(iteration, order); // todo: unused

  // For the fields that are created in memory (by operations for
  // example), the convention for the source attribute is to specify
  // the fielddouble name, because this name describes the operation
  // the field has been created with.
  string * source = new string("mem://"); source->append(fieldName);
  MEDCALC::FieldHandler * fieldHandler = newFieldHandler(fieldName.c_str(),
                                                       meshName.c_str(),
                                                       type,
                                                       iteration,
                                                       order,
                                                       source->c_str());

  if ( meshHandlerId == LONG_UNDEFINED ) {
    // We have to guess the id of the underlying mesh to preserve data
    // integrity (a fieldHandler must have an attribute that contains
    // the id of its underlying mesh):
    //
    // WARNING: it's better to let the client code (the one who calls the
    // function addField) to specify this meshid. This guess procedure is
    // not reliable, it's just to have a second chance.
    //
    LOG("addField: The mesh id is not defined. Trying to guess from the mesh name "<<meshName);
    long meshid = this->getUMeshId(fieldDouble->getMesh());
    fieldHandler->meshid = meshid;
    if ( meshid == LONG_UNDEFINED ) {
      // No mesh has been found in the internal map
      LOG("addField: The mesh id for the mesh "<<meshName<<" can't be retrieved from the field "<<fieldName);
      // _GBO_ : Maybe it could be better to raise an exception
    }
  }
  else {
    fieldHandler->meshid = meshHandlerId;
  }

  _fieldHandlerMap[fieldHandler->id] = fieldHandler;
  _fieldDoubleMap[fieldHandler->id] = fieldDouble;
  // >>> WARNING: CORBA structure assignment specification ==> return
  // >>> a deep copy to avoid the destruction of the fieldHandler
  // >>> registered in the map (assignment acts as a destructor for
  // >>> CORBA struct).
  return new MEDCALC::FieldHandler(*fieldHandler);
}

/*!
 * This function updates the meta-data "fieldname" associated to the
 * specified field.
 */
void MEDDataManager_i::updateFieldMetadata(CORBA::Long  fieldHandlerId,
             const char * fieldname,
             CORBA::Long  iteration,
             CORBA::Long  order,
             const char * source)
{
  // We have to update the field handler registered in the internal
  // map AND the associated fieldDouble loaded in memory.
  MEDCALC::FieldHandler * fieldHandler = getFieldHandler(fieldHandlerId);
  updateFieldHandler(fieldHandlerId,fieldname,iteration,order,source);

  MEDCouplingFieldDouble* fieldDouble = getFieldDouble(fieldHandler);
  fieldDouble->setName(fieldname);

  // _GBO_ TO BE IMPLEMENTED: iteration and order
}

/**
 * This can be used to associate to the specified field another mesh
 * support than its current one. This is typically needed to operate 2
 * fields defined on the same mesh but coming from different med
 * files. In this case, the underlying meshes are different mesh
 * objects (from the MEDCoupling point of view) and then no operation
 * can be allowed by MEDCoupling. The operation of course fails if the
 * new mesh is not identical to the old one.
 */
void MEDDataManager_i::changeUnderlyingMesh(CORBA::Long fieldHandlerId, CORBA::Long meshHandlerId) {

  MEDCALC::FieldHandler * fieldHandler = getFieldHandler(fieldHandlerId);
  MEDCouplingFieldDouble* fieldDouble = getFieldDouble(fieldHandler);
  MEDCouplingMesh * newMesh = getUMesh(meshHandlerId);

  try {
    fieldDouble->changeUnderlyingMesh(newMesh,10,1e-12);
  }
  catch (INTERP_KERNEL::Exception &ex) {
    std::string * message = new std::string("Error when changing the underlying mesh : ");
    message->append(ex.what());
    throw KERNEL::createSalomeException(message->c_str());
  }

  // The change of mesh is OK, then we can update the meta-data
  _fieldHandlerMap[fieldHandlerId]->meshid = meshHandlerId;
  _fieldHandlerMap[fieldHandlerId]->meshname = _meshHandlerMap[meshHandlerId]->name;


  // WARN: if this field has already been request by the tui for
  // manipulation (in a fieldproxy), then the data should be
  // synchronized
}

INTERP_KERNEL::IntersectionType MEDDataManager_i::_getIntersectionType(const char* intersType) {
  std::string type(intersType);
  if (type == "Triangulation") {
    return INTERP_KERNEL::Triangulation;
  }
  else if (type == "Convex") {
    return INTERP_KERNEL::Convex;
  }
  else if (type == "Geometric2D") {
    return INTERP_KERNEL::Geometric2D;
  }
  else if (type == "PointLocator") {
    return INTERP_KERNEL::PointLocator;
  }
  else if (type == "Barycentric") {
    return INTERP_KERNEL::Barycentric;
  }
  else if (type == "BarycentricGeo2D") {
    return INTERP_KERNEL::BarycentricGeo2D;
  }

  std::string message("Error when trying to interpolate field: ");
  message.append("Unrecognized intersection type: ");
  message.append(type);
  throw KERNEL::createSalomeException(message.c_str());
}

MEDCoupling::NatureOfField MEDDataManager_i::_getNatureOfField(const char* fieldNature) {
  std::string nature(fieldNature);
  if (nature == "NoNature") {
    return NoNature;
  }
  else if (nature == "IntensiveMaximum") {
    return IntensiveMaximum;
  }
  else if (nature == "ExtensiveMaximum") {
    return ExtensiveMaximum;
  }
  else if (nature == "ExtensiveConservation") {
    return ExtensiveConservation;
  }
  else if (nature == "IntensiveConservation") {
    return IntensiveConservation;
  }

  std::string message("Error when trying to interpolate field: ");
  message.append("Unrecognized field nature: ");
  message.append(nature);
  throw KERNEL::createSalomeException(message.c_str());
}

MEDCALC::FieldHandler* MEDDataManager_i::interpolateField(CORBA::Long fieldHandlerId, CORBA::Long meshHandlerId, const MEDCALC::InterpolationParameters& params) {
  MEDCALC::FieldHandler* fieldHandler = getFieldHandler(fieldHandlerId);
  MEDCouplingFieldDouble* sourceField = getFieldDouble(fieldHandler);
  MEDCouplingMesh* sourceMesh = getUMesh(fieldHandler->meshid);
  MEDCouplingMesh* targetMesh = getUMesh(meshHandlerId);

  double precision = params.precision;
  INTERP_KERNEL::IntersectionType interpType = this->_getIntersectionType(params.intersectionType);
  std::string method(params.method);
  double defaultValue = params.defaultValue;
  bool reverse = params.reverse;
  MEDCoupling::NatureOfField nature = this->_getNatureOfField(params.nature);

  // 1. Build remapper between sourceMesh and targetMesh (compute interpolation matrix)
  MEDCouplingRemapper remapper;
  remapper.setPrecision(precision);
  remapper.setIntersectionType(interpType);
  remapper.prepare(sourceMesh, targetMesh, method.c_str());

  // 2. Apply interpolation to the field
  sourceField->setNature(nature);
  MEDCouplingFieldDouble* targetField = NULL;
  if (reverse) {
    targetField = remapper.reverseTransferField(sourceField, defaultValue);
  } else {
    targetField = remapper.transferField(sourceField, defaultValue);
  }
  targetField->setMesh(targetMesh);
  targetField->setName(targetMesh->getName() + "_field");

  // 3. Create and register field handler
  MEDCALC::FieldHandler* fieldResultHandler = this->addField(targetField, this->getUMeshId(targetField->getMesh()));
  return fieldResultHandler;
}



/*!
 * This functions display the internal data of the MEDDataManager on
 * the server side (data in the SALOME container).
 */
void MEDDataManager_i::serverlog() {

  LOG("==== Field Handler Map ====================================================");
  LOG("Size = "<<_fieldHandlerMap.size());
  FieldHandlerMapIterator fhmIt;
  for ( fhmIt = _fieldHandlerMap.begin(); fhmIt != _fieldHandlerMap.end(); fhmIt++) {
    LOG("------------------------------------- id = "<<ToString(fhmIt->first));
    LOG("- id        \t= "<<fhmIt->second->id);
    LOG("- fieldname \t= "<<fhmIt->second->fieldname);
    LOG("- meshname  \t= "<<fhmIt->second->meshname);
  }

  LOG("==== Field Double Map ====================================================");
  LOG("Size = "<<_fieldDoubleMap.size());
  FieldDoubleMapIterator fdmIt;
  for ( fdmIt = _fieldDoubleMap.begin(); fdmIt != _fieldDoubleMap.end(); fdmIt++) {
    LOG("------------------------------------- id = "<<ToString(fdmIt->first));
    LOG("- fieldname \t= "<<fdmIt->second->getName());
    LOG("- meshname  \t= "<<fdmIt->second->getMesh()->getName());
  }
}

/*!
 * The event listener is created inside the GUI by the
 * WorkspaceController. This function is called by the WorkspaceController to
 * store the event listener IOR for the time of the session. Then this
 * IOR can be available to any point of the application that can
 * request the data manager (the python console for example).
 */
void MEDDataManager_i::setEventListenerIOR(const char * ior) {
  _medEventListenerIOR = ior;
}
/*!
 * Return the IOR of the event listener that resides in the
 * GUI. Having the IOR, you can restore the CORBA object by using:
 *
 * In a python SALOME context:
 *
 * >>> import salome
 * >>> salome.salome_init()
 * >>> myobject = salome.orb.string_to_object(ior)
 *
 * In a C++ SALOME context: (to do if needed)
 */
char * MEDDataManager_i::getEventListenerIOR() {
  if ( _medEventListenerIOR == "" ) {
    throw KERNEL::createSalomeException("The event listener IOR is not defined");
  }
  // WARN: return a copy because the pointer memory will be released
  // (CORBA specification)
  return CORBA::string_dup( _medEventListenerIOR.c_str() );
}
