// Copyright (C) 2015-2024  CEA, EDF
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

#include "MED.hxx"
#include "MEDFactoryClient.hxx"
#include <MEDCalcConstants.hxx>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)

#include <SALOMEDS_SObject.hxx>
#include <Utils_ExceptHandlers.hxx>
#include <SALOME_LifeCycleCORBA.hxx>
#include <SALOME_NamingService.hxx>
#include <SALOME_KernelServices.hxx>
#include <SALOMEDSImpl_AttributeParameter.hxx>

#include <string>
#include <sstream>

/*!
  \brief Constructor

  Creates an instance of the MED component engine

  \param orb reference to the ORB
  \param poa reference to the POA
  \param contId CORBA object ID, pointing to the owner SALOME container
  \param instanceName SALOME component instance name
  \param interfaceName SALOME component interface name
*/
MED::MED(CORBA::ORB_ptr orb,
         PortableServer::POA_ptr poa,
         PortableServer::ObjectId* contId,
         const char* instanceName,
         const char* interfaceName,
		 bool checkNS)
  : Engines_Component_i(orb, poa, contId, instanceName, interfaceName, false, checkNS),
    _fieldSeriesEntries(),
    _meshEntries()
{
  _thisObj = this;
  _id = _poa->activate_object(_thisObj); // register and activate this servant object
}

MED::~MED()
{
  // nothing to do
}

MED_ORB::status
MED::addDatasourceToStudy(const MEDCALC::DatasourceHandler& datasourceHandler)
{
  // set exception handler to catch unexpected CORBA exceptions
  Unexpect aCatch(SALOME_SalomeException);

  SALOMEDS::Study_var aStudy = KERNEL::getStudyServant();
  // check if reference to study is valid
  if (!CORBA::is_nil(aStudy)) {
    // get full object path
    std::string fullName = CORBA::string_dup(datasourceHandler.name);
    // check if the object with the same name is already registered in the study
    SALOMEDS::SObject_var sobj = aStudy->FindObjectByPath(fullName.c_str());
    if (CORBA::is_nil(sobj)) {
      // object is not registered yet -> register
      SALOMEDS::GenericAttribute_var anAttr;
      SALOMEDS::AttributeParameter_var aParam;
      SALOMEDS::StudyBuilder_var studyBuilder = aStudy->NewBuilder();
      SALOMEDS::UseCaseBuilder_var useCaseBuilder = aStudy->GetUseCaseBuilder();

      // find MED component; create it if not found
      SALOMEDS::SComponent_var father = aStudy->FindComponent("FIELDS");
      if (CORBA::is_nil(father)) {
        // create component
        father = studyBuilder->NewComponent("FIELDS");
        // set name attribute
        father->SetAttrString("AttributeName", "FIELDS");
        // set icon attribute
        father->SetAttrString("AttributePixMap", "ICO_MED");
        // register component in the study
        studyBuilder->DefineComponentInstance(father, MED_Gen::_this());
        // add component to the use case tree
        // (to support tree representation customization and drag-n-drop)
        useCaseBuilder->SetRootCurrent();
        useCaseBuilder->Append(father); // component object is added as the top level item
      }

      // create new sub-object, as a child of the component object
      SALOMEDS::SObject_var soDatasource = studyBuilder->NewObject(father);
      soDatasource->SetAttrString("AttributeName", fullName.c_str());
      soDatasource->SetAttrString("AttributePixMap", "ICO_DATASOURCE");
      anAttr = studyBuilder->FindOrCreateAttribute(soDatasource, "AttributeParameter");
      aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
      aParam->SetInt(SOURCE_ID, datasourceHandler.id);
      useCaseBuilder->AppendTo(soDatasource->GetFather(), soDatasource);

      // We can add the meshes as children of the datasource
      MEDCALC::MeshHandlerList* meshHandlerList =
        MEDFactoryClient::getDataManager()->getMeshHandlerList(datasourceHandler.id);

      for(CORBA::ULong iMesh=0; iMesh<meshHandlerList->length(); iMesh++) {
        MEDCALC::MeshHandler meshHandler = (*meshHandlerList)[iMesh];
        SALOMEDS::SObject_var soMesh = studyBuilder->NewObject(soDatasource);
        _meshEntries[meshHandler.id] = soMesh->GetID();
        soMesh->SetAttrString("AttributeName", meshHandler.name);
        soMesh->SetAttrString("AttributePixMap", "ICO_DATASOURCE_MESH");
        anAttr = studyBuilder->FindOrCreateAttribute(soMesh, "AttributeParameter");
        aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
        aParam->SetInt(MESH_ID, meshHandler.id);
        aParam->SetBool(IS_IN_WORKSPACE, false);
        useCaseBuilder->AppendTo(soMesh->GetFather(), soMesh);

        // We add the field timeseries defined on this mesh, as children of the mesh SObject
        MEDCALC::FieldseriesHandlerList * fieldseriesHandlerList =
          MEDFactoryClient::getDataManager()->getFieldseriesListOnMesh(meshHandler.id);

        for(CORBA::ULong iFieldseries=0; iFieldseries<fieldseriesHandlerList->length(); iFieldseries++) {
          MEDCALC::FieldseriesHandler fieldseriesHandler = (*fieldseriesHandlerList)[iFieldseries];
          SALOMEDS::SObject_var soFieldseries = studyBuilder->NewObject(soMesh);
          _fieldSeriesEntries[fieldseriesHandler.id] = soFieldseries->GetID();

          std::string label(fieldseriesHandler.name);
          label +=" ("+std::string(mapTypeOfFieldLabel[fieldseriesHandler.type])+")";
          soFieldseries->SetAttrString("AttributeName", label.c_str());
          soFieldseries->SetAttrString("AttributePixMap", "ICO_DATASOURCE_FIELD");
          anAttr = studyBuilder->FindOrCreateAttribute(soFieldseries, "AttributeParameter");
          aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
          aParam->SetInt(FIELD_SERIES_ID, fieldseriesHandler.id);
          //aParam->SetInt(FIELD_ID, fieldseriesHandler.id);
          aParam->SetBool(IS_IN_WORKSPACE, false);

          useCaseBuilder->AppendTo(soFieldseries->GetFather(), soFieldseries);
          soFieldseries->UnRegister();
        }
        soMesh->UnRegister();
      }

      // cleanup
      father->UnRegister();
      soDatasource->UnRegister();
    }
  }

  return MED_ORB::OP_OK;
}

MED_ORB::status
MED::registerPresentationField(CORBA::Long fieldId,
                               const char* name,
                               const char* type,
                               const char* ico,
                               CORBA::Long presentationId)
{
  // set exception handler to catch unexpected CORBA exceptions
  Unexpect aCatch(SALOME_SalomeException);

  MEDCALC::FieldHandler_var fldHandler = MEDFactoryClient::getDataManager()->getFieldHandler(fieldId);
  int fieldSeriesId = fldHandler->fieldseriesId;
  if (fieldSeriesId < 0){
      std::cerr << "MED::registerPresentationField(): Error getting field handler\n";
      return MED_ORB::OP_ERROR ;
    }

  if (_fieldSeriesEntries.find(fieldSeriesId) == _fieldSeriesEntries.end()) {
    std::cerr << "MED::registerPresentationField(): Field series not found\n";
    return MED_ORB::OP_ERROR ;
  }
  std::string entry = _fieldSeriesEntries[fieldSeriesId];
  SALOMEDS::Study_var aStudy = KERNEL::getStudyServant();
  SALOMEDS::SObject_var sobject = aStudy->FindObjectID(entry.c_str());
  SALOMEDS::SObject_ptr soFieldseries = sobject._retn();

  if (soFieldseries->IsNull()) {
    std::cerr << "MED::registerPresentationField(): Entry not found\n";
    return MED_ORB::OP_ERROR;
  }

  SALOMEDS::StudyBuilder_var studyBuilder = aStudy->NewBuilder();
  SALOMEDS::UseCaseBuilder_var useCaseBuilder = aStudy->GetUseCaseBuilder();
  SALOMEDS::SObject_var soPresentation = studyBuilder->NewObject(soFieldseries);
  
  soPresentation->SetAttrString("AttributeName", name);
  soPresentation->SetAttrString("AttributePixMap", ico);

  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeParameter_var aParam;
  anAttr = studyBuilder->FindOrCreateAttribute(soPresentation, "AttributeParameter");
  aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
  aParam->SetInt(FIELD_ID, fieldId);
  aParam->SetBool(IS_PRESENTATION, true);
  aParam->SetInt(PRESENTATION_ID, presentationId);
  aParam->SetString(PRESENTATION_TYPE, type);

  useCaseBuilder->AppendTo(soPresentation->GetFather(), soPresentation);

  return MED_ORB::OP_OK;
}

MED_ORB::status
MED::registerPresentationMesh(CORBA::Long meshId,
                              const char* name,
                              const char* type,
                              const char* ico,
                              CORBA::Long presentationId)
{
  // set exception handler to catch unexpected CORBA exceptions
  Unexpect aCatch(SALOME_SalomeException);

  MEDCALC::MeshHandler_var meshHandler = MEDFactoryClient::getDataManager()->getMeshHandler(meshId);
  if (meshHandler->id < 0){
      std::cerr << "MED::registerPresentationMesh(): Error getting mesh handler\n";
      return MED_ORB::OP_ERROR ;
    }

  if (_meshEntries.find(meshHandler->id) == _meshEntries.end()) {
    std::cerr << "MED::registerPresentationMesh(): mesh not found\n";
    return MED_ORB::OP_ERROR ;
  }
  std::string entry = _meshEntries[meshHandler->id];
  SALOMEDS::Study_var aStudy = KERNEL::getStudyServant();
  SALOMEDS::SObject_var sobject = aStudy->FindObjectID(entry.c_str());
  SALOMEDS::SObject_ptr soMesh = sobject._retn();

  if (soMesh->IsNull()) {
    std::cerr << "MED::registerPresentationMesh(): Entry not found\n";
    return MED_ORB::OP_ERROR;
  }

  SALOMEDS::StudyBuilder_var studyBuilder = aStudy->NewBuilder();
  SALOMEDS::UseCaseBuilder_var useCaseBuilder = aStudy->GetUseCaseBuilder();
  SALOMEDS::SObject_var soPresentation = studyBuilder->NewObject(soMesh);
  useCaseBuilder->AppendTo(soPresentation->GetFather(), soPresentation);

  soPresentation->SetAttrString("AttributeName", name);
  soPresentation->SetAttrString("AttributePixMap", ico);

  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeParameter_var aParam;
  anAttr = studyBuilder->FindOrCreateAttribute(soPresentation, "AttributeParameter");
  aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
  aParam->SetInt(MESH_ID, meshId);
  aParam->SetBool(IS_PRESENTATION, true);
  aParam->SetInt(PRESENTATION_ID, presentationId);
  aParam->SetString(PRESENTATION_TYPE, type);

  return MED_ORB::OP_OK;
}


MED_ORB::status
MED::unregisterPresentation(CORBA::Long presentationId)
{
  // set exception handler to catch unexpected CORBA exceptions
  Unexpect aCatch(SALOME_SalomeException);

  SALOMEDS::Study_var aStudy = KERNEL::getStudyServant();
  SALOMEDS::StudyBuilder_var studyBuilder = aStudy->NewBuilder();
  SALOMEDS::UseCaseBuilder_var useCaseBuilder = aStudy->GetUseCaseBuilder();

  SALOMEDS::GenericAttribute_var anAttribute;
  SALOMEDS::SComponent_var father = aStudy->FindComponent("FIELDS");
  SALOMEDS::ChildIterator_var it = aStudy->NewChildIterator(father);
  for (it->InitEx(true); it->More(); it->Next()) {
    SALOMEDS::SObject_var child(it->Value());

    if (child->FindAttribute(anAttribute, "AttributeParameter")) {
      SALOMEDS::AttributeParameter_var attrParam = SALOMEDS::AttributeParameter::_narrow(anAttribute);
      if (!attrParam->IsSet(IS_PRESENTATION, PT_BOOLEAN) || !attrParam->GetBool(IS_PRESENTATION) || !attrParam->IsSet(PRESENTATION_ID, PT_INTEGER))
        continue;

      if (presentationId == attrParam->GetInt(PRESENTATION_ID)) {
        // remove object from study
        studyBuilder->RemoveObjectWithChildren(child);
        // remove object from use case tree
        useCaseBuilder->Remove(child);
      }
    }
  }

  return MED_ORB::OP_OK;
}

MED_ORB::PresentationsList*
MED::getStudyPresentations()
{
  // set exception handler to catch unexpected CORBA exceptions
  Unexpect aCatch(SALOME_SalomeException);

  MED_ORB::PresentationsList* presList = new MED_ORB::PresentationsList;

  SALOMEDS::Study_var aStudy = KERNEL::getStudyServant();
  SALOMEDS::StudyBuilder_var studyBuilder = aStudy->NewBuilder();
  SALOMEDS::UseCaseBuilder_var useCaseBuilder = aStudy->GetUseCaseBuilder();

  SALOMEDS::GenericAttribute_var anAttribute;
  SALOMEDS::SComponent_var father = aStudy->FindComponent("FIELDS");
  SALOMEDS::ChildIterator_var it = aStudy->NewChildIterator(father);
  for (it->InitEx(true); it->More(); it->Next())
    {
      SALOMEDS::SObject_var child(it->Value());
      if (child->FindAttribute(anAttribute, "AttributeParameter"))
        {
          SALOMEDS::AttributeParameter_var attrParam = SALOMEDS::AttributeParameter::_narrow(anAttribute);
          if (!attrParam->IsSet(IS_PRESENTATION, PT_BOOLEAN) || !attrParam->GetBool(IS_PRESENTATION) || !attrParam->IsSet(PRESENTATION_ID, PT_INTEGER))
            continue;

          CORBA::ULong size = presList->length();
          presList->length(size+1);
          (*presList)[size] = attrParam->GetInt(PRESENTATION_ID);
        }
    }
  return presList;
}

char*
MED::getStudyPresentationEntry(CORBA::Long presentationId)
{
  // set exception handler to catch unexpected CORBA exceptions
  Unexpect aCatch(SALOME_SalomeException);

  //MED_ORB::PresentationsList* presList = new MED_ORB::PresentationsList; // todo: unused

  SALOMEDS::Study_var aStudy = KERNEL::getStudyServant();
  SALOMEDS::StudyBuilder_var studyBuilder = aStudy->NewBuilder();
  SALOMEDS::UseCaseBuilder_var useCaseBuilder = aStudy->GetUseCaseBuilder();

  SALOMEDS::GenericAttribute_var anAttribute;
  SALOMEDS::SComponent_var father = aStudy->FindComponent("FIELDS");
  SALOMEDS::ChildIterator_var it = aStudy->NewChildIterator(father);
  for (it->InitEx(true); it->More(); it->Next())
    {
      SALOMEDS::SObject_var child(it->Value());
      if (child->FindAttribute(anAttribute, "AttributeParameter"))
        {
          SALOMEDS::AttributeParameter_var attrParam = SALOMEDS::AttributeParameter::_narrow(anAttribute);
          if (!attrParam->IsSet(IS_PRESENTATION, PT_BOOLEAN) || !attrParam->GetBool(IS_PRESENTATION) || !attrParam->IsSet(PRESENTATION_ID, PT_INTEGER))
            continue;

          if (attrParam->GetInt(PRESENTATION_ID) == presentationId)
            return CORBA::string_dup(child->GetID());
        }
    }
  return CORBA::string_dup("");
}


Engines::TMPFile*
MED::DumpPython(CORBA::Boolean /*isPublished*/,
                CORBA::Boolean /*isMultiFile*/,
                CORBA::Boolean& isValidScript)
{
  SALOMEDS::SObject_var aSO = KERNEL::getStudyServant()->FindComponent("FIELDS");
  if(CORBA::is_nil(aSO)) {
    std::cerr << "Error: Cannot find component MED\n";
    return new Engines::TMPFile(0);
  }

  std::string aScript;

  MEDCALC::CommandsList* history = MEDFactoryClient::getCommandsHistoryManager()->getCommandsHistory();
  for (CORBA::ULong i = 0; i < history->length(); ++i) {
    aScript += (*history)[i];
    aScript += "\n";
  }

  int aLen = aScript.size();
  unsigned char* aBuffer = new unsigned char[aLen+1];
  strcpy((char*)aBuffer, aScript.c_str());

  CORBA::Octet* anOctetBuf =  (CORBA::Octet*)aBuffer;
  Engines::TMPFile_var aStreamFile = new Engines::TMPFile(aLen+1, aLen+1, anOctetBuf, 1);

  isValidScript = true;
  return aStreamFile._retn();
}

CORBA::Boolean
MED::hasObjectInfo()
{
  return true;
}

char*
MED::getObjectInfo(const char* entry)
{
  SALOMEDS::SObject_var aSObj = KERNEL::getStudyServant()->FindObjectID( entry );
  SALOMEDS::SObject_var aResultSObj;
  if (aSObj->ReferencedObject(aResultSObj))
    aSObj = aResultSObj;

  if (aSObj->_is_nil())
    return CORBA::string_dup("unknown");

  SALOMEDS::GenericAttribute_var anAttribute;

  std::string name("unknown");
  if (aSObj->FindAttribute(anAttribute, "AttributeName")) {
    SALOMEDS::AttributeName_var attrName = SALOMEDS::AttributeName::_narrow(anAttribute);
    name = std::string(attrName->Value());
  }

  bool isInWorkspace = false;
  //bool isPresentation = false;
  int sourceId = -1;
  int meshId = -1;
  int fieldSeriesId = -1;
  int fieldId = -1;
  int presentationId = -1;
  if (aSObj->FindAttribute(anAttribute, "AttributeParameter")) {
    SALOMEDS::AttributeParameter_var attrParam = SALOMEDS::AttributeParameter::_narrow(anAttribute);
    if (attrParam->IsSet(IS_IN_WORKSPACE, PT_BOOLEAN))
      isInWorkspace = attrParam->GetBool(IS_IN_WORKSPACE);
    //if (attrParam->IsSet(IS_PRESENTATION, PT_BOOLEAN))
    //  isPresentation = attrParam->GetBool(IS_PRESENTATION);
    if (attrParam->IsSet(SOURCE_ID, PT_INTEGER))
      sourceId = attrParam->GetInt(SOURCE_ID);
    if (attrParam->IsSet(MESH_ID, PT_INTEGER))
      meshId = attrParam->GetInt(MESH_ID);
    if (attrParam->IsSet(FIELD_SERIES_ID, PT_INTEGER))
      fieldSeriesId = attrParam->GetInt(FIELD_SERIES_ID);
    if (attrParam->IsSet(FIELD_ID, PT_INTEGER))
      fieldId = attrParam->GetInt(FIELD_ID);
    if (attrParam->IsSet(PRESENTATION_ID, PT_INTEGER))
      presentationId = attrParam->GetInt(PRESENTATION_ID);
  }

  if (!aSObj->_is_nil() )
    aSObj->UnRegister();

  std::ostringstream oss;
  if (sourceId > -1)
    oss << "Source id: " << sourceId << std::endl;
  if (meshId > -1)
    oss << "Mesh id: " << meshId << std::endl;
  if (fieldSeriesId > -1)
    oss << "Field series id: " << fieldSeriesId << std::endl;
  if (fieldId > -1)
    oss << "Field id: " << fieldId << std::endl;
  //oss << "Is presentation: " << isPresentation << std::endl;
  if (presentationId > -1)
    oss << "Presentation id: " << presentationId << std::endl;
  oss << "Is in workspace: " << isInWorkspace << std::endl;

  return CORBA::string_dup(oss.str().c_str());
}

void MED::cleanUp()
{
  _fieldSeriesEntries.clear();
  _meshEntries.clear();
}
