// Copyright (C) 2011-2025  CEA, EDF
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

// Authors: A Bruneton (CEA), C Aguerre (EdF)

#include "MEDPyLockWrapper.hxx"
#include "MEDFactoryClient.hxx"
#include "MEDPresentation.hxx"
#include "MEDPresentationException.hxx"
#include "MEDCouplingRefCountObject.hxx"
#include <SALOME_KernelServices.hxx>
#undef LOG
#include <Basics_Utils.hxx>

#include <sstream>

#if PY_VERSION_HEX < 0x03050000
static char*
Py_EncodeLocale(const wchar_t *text, size_t *error_pos)
{
	return _Py_wchar2char(text, error_pos);
}
#endif
const std::string MEDPresentation::PROP_NAME  = "name";
const std::string MEDPresentation::PROP_NB_COMPONENTS = "nbComponents";
const std::string MEDPresentation::PROP_SELECTED_COMPONENT = "selectedComponent";
const std::string MEDPresentation::PROP_COMPONENT = "component_";
const std::string MEDPresentation::PROP_COLOR_MAP = "colorMap";
const std::string MEDPresentation::PROP_SCALAR_BAR_RANGE = "scalarBarRange";
const std::string MEDPresentation::PROP_VISIBILITY = "visibility";
const std::string MEDPresentation::PROP_SCALAR_BAR_VISIBILITY = "scalarBarVisibility";
const std::string MEDPresentation::PROP_SCALAR_BAR_MIN_VALUE = "scalarBarMinValue";
const std::string MEDPresentation::PROP_SCALAR_BAR_MAX_VALUE = "scalarBarMaxValue";
const std::string MEDPresentation::PROP_HIDE_DATA_OUTSIDE_CUSTOM_RANGE = "hideDataOutsideCustomRange";

#define PROGRAMMABLE "__programmable" 

MEDPresentation::MEDPresentation(MEDPresentation::TypeID handlerId, const std::string& name,
                                 const MEDCALC::ViewModeType viewMode,
                                 const MEDCALC::ColorMapType colorMap,
                                 const MEDCALC::ScalarBarRangeType sbRange)
    : _meshName(""), _fileName(""), _fieldName(""),
      _mcFieldType(MEDCoupling::ON_CELLS),
      _pvFieldType(""),
      _handlerId(handlerId),
      _selectedComponentIndex(-1),
      _viewMode(viewMode),
      _colorMap(colorMap),
      _sbRange(sbRange),
      _renderViewPyId(-1),  // will be set by MEDPresentationManager_i::_makePresentation()
      _globalDict(0),
      _presentationVisibility(true),
      _scalarBarVisibility(true),
      _scalarBarRangeArray{ 0.0, 100.0 },
      _programmableInitialized(false),
      _hideDataOutsideCustomRange(false),
      _nbComponents(0),
      _nbComponentsInThresholdInput(0)
{
  setStringProperty(MEDPresentation::PROP_NAME, name);

  setIntProperty(MEDPresentation::PROP_NB_COMPONENTS, 0);
  setIntProperty(MEDPresentation::PROP_SELECTED_COMPONENT, 0);

  setIntProperty(MEDPresentation::PROP_COLOR_MAP, static_cast<int>(colorMap));
  setIntProperty(MEDPresentation::PROP_SCALAR_BAR_RANGE, static_cast<int>(sbRange));
  setIntProperty(MEDPresentation::PROP_VISIBILITY, static_cast<int>(_presentationVisibility));
  setIntProperty(MEDPresentation::PROP_SCALAR_BAR_VISIBILITY, static_cast<int>(_scalarBarVisibility));
  setDoubleProperty(MEDPresentation::PROP_SCALAR_BAR_MIN_VALUE, _scalarBarRangeArray[0]);
  setDoubleProperty(MEDPresentation::PROP_SCALAR_BAR_MAX_VALUE, _scalarBarRangeArray[1]);
  setIntProperty(MEDPresentation::PROP_HIDE_DATA_OUTSIDE_CUSTOM_RANGE, static_cast<int>(_hideDataOutsideCustomRange));


  // Python variables:
  int id = GeneratePythonId();
  std::ostringstream oss_o, oss_d, oss_l, oss_s, oss_r, oss_cl, oss_p;
  oss_o << "__obj" << id;
  oss_s << "__srcObj" << id;
  oss_d << "__disp" << id;
  oss_l << "__lut" << id;
  oss_r << "__range" << id;
  oss_p << PROGRAMMABLE << id;
  _objVar = oss_o.str();
  _srcObjVar = oss_s.str();
  _dispVar = oss_d.str();
  _lutVar = oss_l.str();
  _rangeVar = oss_r.str();
  _programmableVar = oss_p.str();
}

/**
 * For most of the presentations the field name etc is required.
 * For the MEDPresentationMeshView however, the handler ID is a mesh handler ID, not a field, and the
 * treatment is specific.
 */
void
MEDPresentation::initFieldMeshInfos()
{
  MEDCALC::MEDDataManager_ptr dataManager(MEDFactoryClient::getDataManager());
  MEDCALC::FieldHandler* fieldHandler = dataManager->getFieldHandler(_handlerId);
  MEDCALC::MeshHandler* meshHandler = dataManager->getMeshHandler(fieldHandler->meshid);
  //MEDCALC::DatasourceHandler* dataSHandler = dataManager->getDatasourceHandlerFromID(meshHandler->sourceid); // todo: unused

  // get the file name of the field (or its memory information)
  extractFileName(std::string(fieldHandler->source));

  _fieldName = fieldHandler->fieldname;
  _mcFieldType = (MEDCoupling::TypeOfField) fieldHandler->type;
  _pvFieldType = getPVFieldTypeString(_mcFieldType);
  _colorByType = _pvFieldType;  // by default the same; overridden in DeflectionShape, VectorField, PointSprite and Contour, Plot3D
  _meshName = meshHandler->name;
}

void
MEDPresentation::extractFileName(const std::string& name)
{
  STDLOG("MEDPresentation::extractFileName('" << name << "')");
  _fileName = name;
  if (_fileName.substr(0, 7) != std::string("file://")) {
    STDLOG("MEDPresentation(): Data source is in memory! Saving it in tmp file.");
    // export a med file with this field
    // we could instead use CORBA to transfer the field to PARAVIS like in MEDCalculatorDBFieldReal::display()
    _fileName = std::tmpnam(NULL);
    MEDCALC::FieldIdList fieldIdList;
    fieldIdList.length(1);
    fieldIdList[0] = _handlerId;
    MEDFactoryClient::getDataManager()->saveFields(_fileName.c_str(), fieldIdList);
  }
  else
    // removing "file://"
    _fileName = _fileName.substr(7, _fileName.size());
  STDLOG("MEDPresentation::extractFileName _fileName=" << _fileName);
}

MEDPresentation::~MEDPresentation()
{
  STDLOG("~MEDPresentation(): clear display");
  {
    MEDPyLockWrapper lock;
    std::ostringstream oss;

    try {
      oss << "pvs.Hide(" << _objVar <<  ", view=" << getRenderViewVar() << ");";
      for (auto& value : _presentationThresolds) {
        if (value._active) {
          oss << "pvs.Hide(" << value._thresholdVar << ", view=" << getRenderViewVar() << ");";
        }
      }
      execPyLine(oss.str());
      // :TRICKY: The two following lines raise an exception when closing MED module
      //          after sequence: MED - load file - PARAVIS - MED - close SALOME
      //          (see Mantis #23461)
      //execPyLine(getRenderViewVar() + ".ResetCamera();");
      //execPyLine("pvs.Render();");
    }
    catch(SALOME::SALOME_Exception&) {
    }
  }
}

void
MEDPresentation::generatePipeline()
{
  // Might be more complicated in the future:

  this->internalGeneratePipeline();
}

void MEDPresentation::internalGenerateThreshold() {
  std::ostringstream oss;
  std::string inputFieldName = getFieldName();
  if (!_programmableInitialized) {
    if (_nbComponentsInThresholdInput > 1) {
      std::string typ = toScriptCellType(_colorByType);
      oss << _programmableVar << " = pvs.ProgrammableFilter(Input = " << _objVar << ");";
      oss << _programmableVar << ".Script = \"\"\"import numpy as np" << std::endl;
      oss << "import paraview.vtk.numpy_interface.dataset_adapter as dsa" << std::endl;
      oss << "input0 = inputs[0]" << std::endl;
      oss << "inputDataArray=input0." << typ << "['" << inputFieldName << "']" << std::endl;
      oss << "if type(inputDataArray) == dsa.VTKCompositeDataArray:" << std::endl;
      oss << "\tnpa = inputDataArray.GetArrays()" << std::endl;
      oss << "else:" << std::endl;
      oss << "\tnpa = inputDataArray" << std::endl;
      oss << "if type(npa) == list:" << std::endl;
      oss << "\tarrs = []" << std::endl;
      oss << "\tfor a in npa:" << std::endl;
      oss << "\t\tmgm = np.linalg.norm(a, axis = -1)" << std::endl;
      oss << "\t\tmga = mgm.reshape(mgm.size, 1)" << std::endl;
      oss << "\t\tarrs.append(mga)" << std::endl;
      oss << "\tca = dsa.VTKCompositeDataArray(arrs)" << std::endl;
      oss << "\toutput." << typ << ".append(ca, '" << inputFieldName << "_magnitude')" << std::endl;
      oss << "else:" << std::endl;
      oss << "\tmgm = np.linalg.norm(npa, axis = -1)" << std::endl;
      oss << "\tmga = mgm.reshape(mgm.size, 1)" << std::endl;
      oss << "\toutput." << typ << ".append(mga, '" << inputFieldName << "_magnitude')" << std::endl;
      for (std::vector<std::string>::size_type ii = 1; ii < _nbComponentsInThresholdInput + 1 ; ii++) {
        oss << "dataArray" << ii << " = inputDataArray[:, [" << ii - 1 << "]]" << std::endl;
        oss << "output." << typ << ".append(dataArray" << ii << ", '" << inputFieldName << "_" << ii <<  "')" << std::endl;
      }
      oss << "\"\"\"" << std::endl;
      _programmableInitialized = true;
    }
  }

  ComponentThresold& currentThreshold = _presentationThresolds[getThresholdIndex()];
  if (!currentThreshold._thresholdInitialized) {
    std::string& thInput = (_nbComponentsInThresholdInput > 1 ) ? _programmableVar : _objVar;
    std::string arrayName = getThresholdFieldName();
    oss << currentThreshold._thresholdVar << " = pvs.Threshold(Input = " << thInput << ");";
    oss << currentThreshold._thresholdVar << ".Scalars = ['" << _colorByType << "', '" << arrayName << "'];";
    oss << additionalThresholdInitializationActions();
    oss << currentThreshold._thresholdDispVar << " = pvs.Show(" << currentThreshold._thresholdVar << ", " 
      << getRenderViewVar() << ");";
    oss << currentThreshold._thresholdLutVar << " = pvs.GetColorTransferFunction('" << arrayName << "', " 
      << currentThreshold._thresholdDispVar << ", separate=True);";
    oss << "pvs.ColorBy(" << currentThreshold._thresholdDispVar << ", ('" << _colorByType << "', '"
      << arrayName << "'), separate = True);";
    oss << additionalThresholdVisualizationActions();
    currentThreshold._thresholdInitialized = true;
  }
  if (oss.str().length() > 0) {
    pushAndExecPyLine(oss.str());
  }
}

//void
//MEDPresentation::pushPyObjects(PyObjectId obj, PyObjectId disp)
//{
//  _pipeline.push_back(obj);
//  _display.push_back(disp);
//}

void
MEDPresentation::pushAndExecPyLine(const std::string & lin)
{
  execPyLine(lin);
  _pythonCmds.push_back(lin);
}

void
MEDPresentation::execPyLine(const std::string & lin)
{
  MEDPyLockWrapper lock;
  STDLOG("@@@@ MEDPresentation::execPyLine() about to exec >> " << lin);
  if(PyRun_SimpleString(lin.c_str()))
    {
      std::ostringstream oss;
      oss << "MEDPresentation::execPyLine(): following Python command failed!\n";
      oss << ">> " << lin;
      STDLOG(oss.str());
      throw KERNEL::createSalomeException(oss.str().c_str());
    }
}

void
MEDPresentation::setStringProperty(const std::string& propName, const std::string& propValue)
{
  _propertiesStr[propName] = propValue;
}

const std::string
MEDPresentation::getStringProperty(const std::string& propName) const
{
  std::map<std::string, std::string>::const_iterator it = _propertiesStr.find(propName);
  if (it != _propertiesStr.end()) {
      return (*it).second;
  }
  else {
      STDLOG("MEDPresentation::getStringProperty(): no property named " + propName);
      throw MEDPresentationException("MEDPresentation::getStringProperty(): no property named " + propName);
  }
}

void
MEDPresentation::setIntProperty(const std::string& propName, const int propValue)
{
  _propertiesInt[propName] = propValue;
}

int
MEDPresentation::getIntProperty(const std::string& propName) const
{
  std::map<std::string, int>::const_iterator it = _propertiesInt.find(propName);
  if (it != _propertiesInt.end()) {
      return (*it).second;
  }
  else {
      STDLOG("MEDPresentation::getIntProperty(): no property named " + propName);
      throw MEDPresentationException("MEDPresentation::getIntProperty(): no property named " + propName);
  }
}

 void
 MEDPresentation::dumpIntProperties() const
 {
   std::map<std::string, int>::const_iterator it = _propertiesInt.begin();
   STDLOG("@@@ Dumping INT properties");
   for(; it != _propertiesInt.end(); ++it)
     {
       std::ostringstream oss;
       oss << (*it).first << "  ->   " << (*it).second;
       STDLOG(oss.str());
     }
 }

 void
 MEDPresentation::setDoubleProperty(const std::string& propName, const double propValue)
 {
   _propertiesDouble[propName] = propValue;
 }

 double
 MEDPresentation::getDoubleProperty(const std::string& propName) const
 {
   std::map<std::string, double>::const_iterator it = _propertiesDouble.find(propName);
   if (it != _propertiesDouble.end()) {
     return (*it).second;
   }
   else {
     STDLOG("MEDPresentation::getDoubleProperty(): no property named " + propName);
     throw MEDPresentationException("MEDPresentation::getDoubleProperty(): no property named " + propName);
   }
 }

 void
  MEDPresentation::dumpDoubleProperties() const
 {
   std::map<std::string, double>::const_iterator it = _propertiesDouble.begin();
   STDLOG("@@@ Dumping DOUBLE properties");
   for (; it != _propertiesDouble.end(); ++it)
   {
     std::ostringstream oss;
     oss << (*it).first << "  ->   " << (*it).second;
     STDLOG(oss.str());
   }
 }

 void
 MEDPresentation::dumpStringProperties() const
 {
   std::map<std::string, std::string>::const_iterator it = _propertiesStr.begin();
   STDLOG("@@@ Dumping STR properties");
   for(; it != _propertiesStr.end(); ++it)
     {
       std::ostringstream oss;
       oss << (*it).first << "  ->   " << (*it).second;
       STDLOG(oss.str());
     }
 }

 void
 MEDPresentation::internalGeneratePipeline()
 {
   MEDPyLockWrapper lock;
   pushAndExecPyLine( "import salome.pvsimple as pvs;");
   pushAndExecPyLine( "import medcalc");
 }


/**
* @return a borrowed reference. Do not DECRREF!
*/
PyObject*
MEDPresentation::getPythonObjectFromMain(const char* python_var) const
{
  if (! _globalDict)
    {
      // All the calls below returns *borrowed* references
      PyObject* main_module = PyImport_AddModule((char*)"__main__");
      _globalDict = PyModule_GetDict(main_module);
    }
  return PyDict_GetItemString(_globalDict, python_var);
}

std::string
MEDPresentation::getPVFieldTypeString(MEDCoupling::TypeOfField fieldType) const
{
  switch(fieldType)
  {
    case MEDCoupling::ON_CELLS:
      return "CELLS";
    case MEDCoupling::ON_NODES:
      return "POINTS";
    case MEDCoupling::ON_GAUSS_PT:
      return "POINTS"; // because internally after application of the ELGA filter, the field will appear as a POINT field
    case MEDCoupling::ON_GAUSS_NE:
      return "POINTS"; // because internally after application of the ELNO mesh filter, the field will appear as a POINT field
    default:
      STDLOG("MEDPresentation::getPVFieldTypeString() -- Not implemented ! ELNO field?");
      return "";
  }
}

std::string
MEDPresentation::getRenderViewVar() const
{
  std::ostringstream oss;
  oss << "__view" << _renderViewPyId;
  return oss.str();
}

/*!
 * Creates the MEDReader source in the pipeline, and potentially apply GAUSS/ELNO filters.
 */
void
MEDPresentation::createSource()
{
  std::string typ;
  switch(_mcFieldType) {
    case MEDCoupling::ON_CELLS: typ = "P0"; break;
    case MEDCoupling::ON_NODES: typ = "P1"; break;
    case MEDCoupling::ON_GAUSS_PT: typ = "GAUSS"; break;
    case MEDCoupling::ON_GAUSS_NE: typ = "GSSNE"; break;
    default:
      const char * msg ="MEDPresentation::createSource(): field type not impl. yet!";
      STDLOG(msg);
      throw KERNEL::createSalomeException(msg);
  }

  std::ostringstream oss;
  oss << _srcObjVar << " = pvs.MEDReader(FileNames=r'" << _fileName << "');";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << "medcalc.SelectSourceField(" << _srcObjVar << ", '" << _meshName << "', '"
      << _fieldName << "', '" << typ << "');";
  pushAndExecPyLine(oss.str()); oss.str("");
  // Generate complete vector fields: fields with 2 components will copied into <name>_vector and
  // have a third null component added.
  oss << _srcObjVar << ".VectorsProperty = 1;";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Make sure this is set so we stick to time steps:
  pushAndExecPyLine("pvs.GetAnimationScene().PlayMode = 'Snap To TimeSteps'");

  // Deal with GAUSS fields:
  if(_mcFieldType == MEDCoupling::ON_GAUSS_PT)
    {
      std::ostringstream oss, oss2;
      oss2 << "__srcObj" << GeneratePythonId();
      oss << oss2.str() << " = pvs.ELGAfieldToPointGaussian(Input=" << _srcObjVar << ");";
      pushAndExecPyLine(oss.str()); oss.str("");
      // Now the source becomes the result of the CellDatatoPointData:
      _srcObjVar = oss2.str();
      oss << _srcObjVar << ".SelectSourceArray = ['CELLS', 'ELGA@0'];";
      pushAndExecPyLine(oss.str()); oss.str("");
    }
  if(_mcFieldType == MEDCoupling::ON_GAUSS_NE)
    {
      std::ostringstream oss, oss2;
      oss2 << "__srcObj" << GeneratePythonId();
      oss << oss2.str() << " = pvs.ELNOfieldToSurface(Input=" << _srcObjVar << ");";
      pushAndExecPyLine(oss.str()); oss.str("");
      // Now the source becomes the result of the CellDatatoPointData:
      _srcObjVar = oss2.str();
    }
}

/*
 * Set the timestamp of the animation to the timestamp of the field.
 * Especially useful when working on a field's iteration:
 * in the workspace, in the python console, or using changeUnderlyingMesh.
 */
void
MEDPresentation::setTimestamp()
{
  // get the timestamp of the field
  double timestamp = MEDFactoryClient::getDataManager()->getFieldTimestamp(_handlerId);
  STDLOG("Displaying timestamp : " << timestamp);

  std::ostringstream oss;

  // go to the right timestamp in animation (view and VCR toolbar)
  pushAndExecPyLine("pvs.GetAnimationScene().UpdateAnimationUsingDataTimeSteps()");
  oss << "pvs.GetAnimationScene().AnimationTime = " << timestamp << ";";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << "pvs.GetTimeKeeper().Time = " << timestamp << ";";
  pushAndExecPyLine(oss.str()); oss.str("");
}

void
MEDPresentation::setOrCreateRenderView()
{
  std::ostringstream oss2;

  std::string view(getRenderViewVar());
  oss2 << "pvs._DisableFirstRenderCameraReset();";
  pushAndExecPyLine(oss2.str()); oss2.str("");
  if (_viewMode == MEDCALC::VIEW_MODE_OVERLAP) {
      // this might potentially re-assign to an existing view variable, but this is OK, we
      // normally reassign exactly the same RenderView object.
      oss2 << view << " = medcalc.FindOrCreateView('RenderView');";
      pushAndExecPyLine(oss2.str()); oss2.str("");
  } else if (_viewMode == MEDCALC::VIEW_MODE_REPLACE) {
      // same as above
      oss2 << view << " = medcalc.FindOrCreateView('RenderView');";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 << "pvs.GetActiveSource() and pvs.Hide(view=" << view << ");";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 << "pvs.Render();";
      pushAndExecPyLine(oss2.str()); oss2.str("");
  } else if (_viewMode == MEDCALC::VIEW_MODE_NEW_LAYOUT) {
      oss2 <<  "nbLayouts = len(pvs.GetLayouts());";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 <<  "__layout1 = pvs.CreateLayout('Layout #%i'%(nbLayouts+1));";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 << view << " = pvs.CreateView('RenderView');";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 <<  "pvs.AssignViewToLayout(view=" << view << ", layout=__layout1, hint=0);";
      pushAndExecPyLine(oss2.str()); oss2.str("");
  } else if (_viewMode == MEDCALC::VIEW_MODE_SPLIT_VIEW) {
      oss2 <<  "__activeLayout = pvs.GetLayout();";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 << "__activeLayout.SplitHorizontal(0, 0.5);";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 << view << " = pvs.CreateView('RenderView');";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 << "pvs.AssignViewToLayout(view=" << view << ", layout=__activeLayout, hint=2);";
      pushAndExecPyLine(oss2.str()); oss2.str("");
  }
}

void
MEDPresentation::resetCameraAndRender()
{
  pushAndExecPyLine(getRenderViewVar() + ".ResetCamera();");
  pushAndExecPyLine("pvs.Render();");
}

void
MEDPresentation::selectFieldComponent()
{
  std::ostringstream oss, oss_l;
  std::string ret;
  if (!_hideDataOutsideCustomRange) {
    if (_selectedComponentIndex != -1)
    {
      oss << _lutVar << ".VectorMode = 'Component';";
      pushAndExecPyLine(oss.str()); oss.str("");
      oss << _lutVar << ".VectorComponent = " << _selectedComponentIndex << ";";
      pushAndExecPyLine(oss.str()); oss.str("");
    }
    else  // Euclidean norm
    {
      oss << _lutVar << ".VectorMode = 'Magnitude';";
      pushAndExecPyLine(oss.str()); oss.str("");
    }
  }
  else {
    // Make sure that threshold is initialized
    internalGenerateThreshold();
    // Set range
    thresholdValues();

    // Color map setting
    selectColorMap(false);

    if (!_presentationThresolds[getThresholdIndex()]._active) {
      std::vector<std::string>::size_type idx = (_nbComponentsInThresholdInput > 1) ? -1 : 0;
      for (auto& value : _presentationThresolds) {
        // Hide previous threshold
        if (value._active) {
          bool currentVisibility = _presentationVisibility;
          std::vector<std::string>::size_type currentIndex = _selectedComponentIndex;
          _presentationVisibility = false;
          _selectedComponentIndex = idx;
          visibility();
          _selectedComponentIndex = currentIndex;
          _presentationVisibility = currentVisibility;
          value._active = false;
        }
        idx++;
      }
      // Show new threshold 
      visibility();
      _presentationThresolds[getThresholdIndex()]._active = true;
      scalarBarTitle();
    }
  }
}

/**
 * Needs the LUT, so to be called after selectColorMap for the first time.
 */
void
MEDPresentation::scalarBarTitle()
{
  // get selected component name:
  std::string compoName;
  if (_selectedComponentIndex != -1)
    {
      std::ostringstream oss1;
      oss1 << MEDPresentation::PROP_COMPONENT << _selectedComponentIndex;
      compoName = getStringProperty(oss1.str());
    }
  else
    {
      if (getIntProperty(MEDPresentation::PROP_NB_COMPONENTS) == 1)
        compoName = "";
      else
        compoName = "Magnitude";
    }
  std::ostringstream oss;
  if (_hideDataOutsideCustomRange) {
    oss << "pvs.GetScalarBar(" << getLutVar() << ").Title = '" << _fieldName << "';";
  }
  oss << "pvs.GetScalarBar(" << getLutVar() << ").ComponentTitle = '" << compoName << "';";
  pushAndExecPyLine(oss.str()); oss.str("");
}

void
MEDPresentation::selectColorMap(const bool updateFieldComponent)
{
  std::ostringstream oss;

  switch (_colorMap) {
  case MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW:
    oss << getLutVar() << ".ApplyPreset('Blue to Red Rainbow',True);";
    break;
  case MEDCALC::COLOR_MAP_COOL_TO_WARM:
    oss << getLutVar() << ".ApplyPreset('Cool to Warm',True);";
    break;
  default:
    STDLOG("MEDPresentation::getColorMapCommand(): invalid colormap!");
    throw KERNEL::createSalomeException("MEDPresentation::getColorMapCommand(): invalid colormap!");
  }
  pushAndExecPyLine(oss.str());

  if(updateFieldComponent) {
    selectFieldComponent(); // somehow PV keeps the LUT parameters of the previous presentation, so better reset this.
  }
}

void
MEDPresentation::showObject()
{
  std::ostringstream oss;
  oss << _dispVar << " = pvs.Show(" << _objVar << ", " << getRenderViewVar() << ");";
  oss << _lutVar << " = pvs.GetColorTransferFunction('" << getFieldName() << "', " << _dispVar << ", separate=True);";
  pushAndExecPyLine(oss.str());
}

void
MEDPresentation::hideObject()
{
  std::ostringstream oss;
  oss <<"pvs.Hide(" << _objVar << ", " << getRenderViewVar() << ");";
  pushAndExecPyLine(oss.str());
}

void
MEDPresentation::showScalarBar()
{
  // Display Scalar Bar only if presentation is visible 
  if (_presentationVisibility) {
    std::ostringstream oss;
    oss << getDispVar() << ".SetScalarBarVisibility(" << getRenderViewVar() << ", True);";
    pushAndExecPyLine(oss.str());
  }
}

void
MEDPresentation::hideScalarBar()
{
  std::ostringstream oss;
  oss << getDispVar() << ".SetScalarBarVisibility(" << getRenderViewVar() << ", False);";
  pushAndExecPyLine(oss.str());
}

void 
MEDPresentation::scalarBarVisibility() {
  _scalarBarVisibility ? showScalarBar() : hideScalarBar();
}

void
MEDPresentation::colorBy()
{
  std::ostringstream oss;
  oss << "pvs.ColorBy(" << getDispVar() << ", ('" << _colorByType << "', '" << getFieldName() << "'), separate=True);";
  pushAndExecPyLine(oss.str());
}

void MEDPresentation::visibility() {
  std::ostringstream oss;                        
  oss << getDispVar() << ".Visibility = " << (_presentationVisibility ? "True" : "False") << ";";
  pushAndExecPyLine(oss.str());

  // Hide scalar bar with the presentation
  if (!_presentationVisibility && _scalarBarVisibility)
    hideScalarBar();

  // Show scalar bar with the presentation
  if (_presentationVisibility && _scalarBarVisibility)
    showScalarBar();
}

void MEDPresentation::threshold() {
  _hideDataOutsideCustomRange ? thresholdPresentation() : unThresholdPresentation();
}

void MEDPresentation::thresholdPresentation() {
  if (!_hideDataOutsideCustomRange)
    return;

  internalGenerateThreshold();

  // Hide _dispVar, for that temporary switch the _presentationVisibility and _hideDataOutsideCustomRange
  // flag to false and call visibility() method
  bool prevVisibility = _presentationVisibility;
  bool prevHideDataOutsideCustomRange = _hideDataOutsideCustomRange;
  _presentationVisibility = false;
  _hideDataOutsideCustomRange = false;
  visibility();
  _presentationVisibility = prevVisibility;
  _hideDataOutsideCustomRange = prevHideDataOutsideCustomRange;

  // Display _thresholdDispVar var
  visibility();

  // Select target colormap  
  selectColorMap();

  // Adapt scalar bar title
  scalarBarTitle();

  // Additional threshold actions to be done
  additionalThresholdActions();

  _presentationThresolds[getThresholdIndex()]._active = true;
}

void MEDPresentation::thresholdValues() {
  if (!_hideDataOutsideCustomRange)
    return;
  std::ostringstream oss;
  oss << _presentationThresolds[getThresholdIndex()]._thresholdVar << ".ThresholdRange = [ " << _scalarBarRangeArray[0] << ", " << _scalarBarRangeArray[1] << "];";
  oss << _presentationThresolds[getThresholdIndex()]._thresholdLutVar <<".RescaleTransferFunction(" << _scalarBarRangeArray[0] << ", " << _scalarBarRangeArray[1] << ");";
  pushAndExecPyLine(oss.str());
}

void MEDPresentation::unThresholdPresentation() {

  if (_presentationThresolds[getThresholdIndex()]._active) {
    // Hide _dispVar, for that temporary switch the _presentationVisibility to talse and _hideDataOutsideCustomRange
    // flag to true and call visibility() method
    bool prevVisibility = _presentationVisibility;
    bool prevHideDataOutsideCustomRange = _hideDataOutsideCustomRange;
    _presentationVisibility = false;
    _hideDataOutsideCustomRange = true;
    visibility();
    _presentationVisibility = prevVisibility;
    _hideDataOutsideCustomRange = prevHideDataOutsideCustomRange;

    // Range
    rescaleTransferFunction();

    // Display _dispVar var
    visibility();

    // Select target colormap  
    selectColorMap();

    // Adapt scalar bar title
    scalarBarTitle();

    // Additional unthreshold actions to be done
    additionalUnThresholdActions();

    _presentationThresolds[getThresholdIndex()]._active = false;
  }
}

int MEDPresentation::getThresholdIndex() const {
  if (_nbComponentsInThresholdInput > 1 && _nbComponentsInThresholdInput <= 3) {
    return _selectedComponentIndex + 1; 
  }
  else {
    return _selectedComponentIndex;
  }
}

/*!
  Return _dispVar or _thresholdDispVar depending on _hideDataOutsideCustomRange flag:
         _hideDataOutsideCustomRange == false : _dispVar is used
         _hideDataOutsideCustomRange == true  : _thresholdDispVar is used
*/
const std::string& MEDPresentation::getDispVar() {
  return (_hideDataOutsideCustomRange ? _presentationThresolds[getThresholdIndex()]._thresholdDispVar : _dispVar);
}

/*!
  Return _dispVar or _thresholdDispVar depending on _hideDataOutsideCustomRange flag:
         _hideDataOutsideCustomRange == false : _dispVar is used
         _hideDataOutsideCustomRange == true  : _thresholdDispVar is used
*/
const std::string& MEDPresentation::getLutVar() {
  return (_hideDataOutsideCustomRange ? _presentationThresolds[getThresholdIndex()]._thresholdLutVar : _lutVar);
}

void
MEDPresentation::rescaleTransferFunction()
{
  if (_hideDataOutsideCustomRange)
    return;

  std::ostringstream oss;
  switch(_sbRange)
  {
    case MEDCALC::SCALAR_BAR_ALL_TIMESTEPS:
      oss << _dispVar << ".RescaleTransferFunctionToDataRangeOverTime();";
      break;
    case MEDCALC::SCALAR_BAR_CURRENT_TIMESTEP:
      oss << _dispVar << ".RescaleTransferFunctionToDataRange(False);";
      break;
    case MEDCALC::SCALAR_BAR_CUSTOM_RANGE:
      oss << _lutVar << ".RescaleTransferFunction("<< _scalarBarRangeArray[0]<<", "<< _scalarBarRangeArray[1]<<");";
      break;
    default:
      STDLOG("MEDPresentation::getRescaleCommand(): invalid range!");
      throw KERNEL::createSalomeException("MEDPresentation::getRescaleCommand(): invalid range!");
  }
  pushAndExecPyLine(oss.str()); oss.str("");
  // Get min-max
  oss << _rangeVar << " = [" << _lutVar << ".RGBPoints[0], " << _lutVar << ".RGBPoints[-4]];";
  execPyLine(oss.str());

  //Update _scalarBarRange internal variable in case of rescaling to "Data Range" or "Data Range Over All Times"
  if (_sbRange == MEDCALC::SCALAR_BAR_ALL_TIMESTEPS ||
      _sbRange == MEDCALC::SCALAR_BAR_CURRENT_TIMESTEP) {
    MEDPyLockWrapper lock;
    PyObject * obj = getPythonObjectFromMain(_rangeVar.c_str());
    if (obj && PyList_Check(obj)) {
      PyObject* objL0 = PyList_GetItem(obj, 0);
      PyObject* objL1 = PyList_GetItem(obj, 1);
      if (PyFloat_Check(objL0)) {
        double min = PyFloat_AsDouble(objL0);
        _scalarBarRangeArray[0] = min;
      }
      if (PyFloat_Check(objL1)) {
        double max = PyFloat_AsDouble(objL1);
        _scalarBarRangeArray[1] = max;
      }
    }
    setDoubleProperty(MEDPresentation::PROP_SCALAR_BAR_MIN_VALUE, _scalarBarRangeArray[0]);
    setDoubleProperty(MEDPresentation::PROP_SCALAR_BAR_MAX_VALUE, _scalarBarRangeArray[1]);
  }
  // Adapt scalar bar title
  scalarBarTitle();
}

MEDCALC::PresentationVisibility
MEDPresentation::presentationStateInActiveView() {
  MEDPyLockWrapper lock;
  MEDCALC::PresentationVisibility result = MEDCALC::PRESENTATION_NOT_IN_VIEW;

  execPyLine("__isInView = ( " + getRenderViewVar() + " == pvs.GetActiveView() )");
  PyObject * obj = getPythonObjectFromMain("__isInView");
  
  if (obj && PyBool_Check(obj) && (obj == Py_True)) {
    result = _presentationVisibility ? MEDCALC::PRESENTATION_VISIBLE : MEDCALC::PRESENTATION_INVISIBLE;
  }
  return result;
}

int
MEDPresentation::GeneratePythonId()
{
  static int INIT_ID = 0;
  return INIT_ID++;
}

bool MEDPresentation::isThresoldActive() const {
  bool active = false;
  for (auto const& value : _presentationThresolds) {
    active = active || value._active;
  }
  return active;
}

bool
MEDPresentation::activateView()
{
  MEDPyLockWrapper lock;

  execPyLine("__alive = " + getRenderViewVar() + " in pvs.GetRenderViews()");
  PyObject * obj = getPythonObjectFromMain("__alive");
  bool alive = true;
  if (obj && PyBool_Check(obj))
    alive = (obj == Py_True);

  if (alive)
    // The view is still there,just activate it:
    pushAndExecPyLine("pvs.SetActiveView(" + getRenderViewVar() + ");");
  else
    {
      // The view disappeared, recreate it in a new layout. The transfer of the objects is to be done by the caller.
      std::ostringstream oss;
      oss <<  "pvs.servermanager.misc.ViewLayout(registrationGroup='layouts');";
      pushAndExecPyLine(oss.str()); oss.str("");
      oss << getRenderViewVar() << " = pvs.CreateView('RenderView');";
      pushAndExecPyLine(oss.str()); oss.str("");
    }
  return alive;
}

/**!
 * Called when the view has been recreated (because the user closed it).
 * All the objects and set up are re-shown in the new view (which is stored in the same Python variable).
 */
void
MEDPresentation::recreateViewSetup()
{
  bool prevHideDataOutsideCustomRange = _hideDataOutsideCustomRange;
  _hideDataOutsideCustomRange = false;
  showObject();
  colorBy();
  showScalarBar();
  selectColorMap();
  rescaleTransferFunction();
  _hideDataOutsideCustomRange = prevHideDataOutsideCustomRange;
  threshold();
  resetCameraAndRender();
}

std::string
MEDPresentation::paravisDump() const
{
  using namespace std;
  ostringstream oss;
  for (vector<string>::const_iterator it=_pythonCmds.begin(); it != _pythonCmds.end(); ++it)
    {
      oss << (*it);
      oss << "\n";
    }
  return oss.str();
}

/**
 * Query all available component names for the field associated with this presentation.
 * Fills in all the corresponding string properties:
 *  - PROP_COMPONENT1
 *  - PROP_COMPONENT2
 *    etc...
 *  and the number of components.
 */
void
MEDPresentation::fillAvailableFieldComponents()
{
  MEDPyLockWrapper lock;  // GIL!
  std::string typ = getScriptCellType();

  std::ostringstream oss;
  oss << "__nbCompo = " << _srcObjVar << "." << typ << ".GetArray('" <<  _fieldName << "').GetNumberOfComponents();";
  execPyLine(oss.str());
  PyObject* p_obj = getPythonObjectFromMain("__nbCompo");
  long nbCompo;
  if (p_obj && PyLong_Check(p_obj))
    nbCompo = PyLong_AS_LONG(p_obj);
  else
    {
      STDLOG("Unexpected Python error");
      throw KERNEL::createSalomeException("Unexpected Python error");
    }
  _nbComponents = nbCompo;
  _nbComponentsInThresholdInput = _nbComponents;
  setIntProperty(MEDPresentation::PROP_NB_COMPONENTS, nbCompo);

  // if the field is not a vector (2 or 3 components), select the first component of the tensor,
  // like in WidgetPresentationParameters::setComponents
  if (!(nbCompo > 1 && nbCompo <= 3))
    _selectedComponentIndex = 0;

  std::map<std::string, long> aCompoMap;
  std::vector <std::string> aCompos;
  for (long i = 0; i<nbCompo; i++)
    {
      std::ostringstream oss2;
      oss2 << "__compo = " << _srcObjVar << "." << typ << ".GetArray('" <<  _fieldName << "').GetComponentName(" << i << ");";
      execPyLine(oss2.str());
      PyObject* p_obj = getPythonObjectFromMain("__compo");
      std::string compo;
      if (p_obj && PyUnicode_Check(p_obj))
#if PY_VERSION_HEX < 0x030c0000 // See PEP-623
        compo = std::string(Py_EncodeLocale(PyUnicode_AS_UNICODE(p_obj), NULL));  // pointing to internal Python memory, so make a copy!!
#else
        compo = std::string(Py_EncodeLocale(PyUnicode_AsWideCharString(p_obj,NULL), NULL));
#endif
      else
        {
          STDLOG("Unexpected Python error");
          throw KERNEL::createSalomeException("Unexpected Python error");
        }
      if (aCompoMap.find(compo) == aCompoMap.end()) {
        aCompoMap.insert(std::pair<std::string, int>(compo, 1));
        aCompos.push_back(compo);
      }
      else {
        if (aCompoMap[compo] == 1) {
          for (std::vector<std::string>::size_type ii = 0; ii != aCompos.size(); ii++) {
            // Modify the previous occurrence
            if (aCompos[ii] == compo) {
              std::ostringstream oss_m;
              oss_m << compo << "(" << 1 << ")";
              aCompos[ii] = oss_m.str();
              break;
            }
          }
          std::ostringstream oss_n;
          oss_n << compo << "(" << 2 << ")";
          aCompos.push_back(oss_n.str());
        }
        else if (aCompoMap[compo] > 1) {
          auto val = aCompoMap[compo];
          val++;
          std::ostringstream oss_n;
          oss_n << compo << "(" << val << ")";
          aCompos.push_back(oss_n.str());
        }
        aCompoMap[compo] = aCompoMap[compo] + 1;
      }
    }
  std::string aCopy = _programmableVar;
  std::string id = aCopy.replace(aCopy.find(PROGRAMMABLE), std::string(aCopy).length() - 1, "");

  std::vector<std::string>::size_type up = (nbCompo > 1) ? nbCompo + 1 : nbCompo;
  for (std::vector<std::string>::size_type i = 0; i != up; i++) {
    std::ostringstream oss_p;
    if (i != 0 || nbCompo == 1) {
      std::vector<std::string>::size_type idx = (nbCompo > 1) ? i - 1 : i;
      oss_p << MEDPresentation::PROP_COMPONENT << idx;
      setStringProperty(oss_p.str(), aCompos[idx]);
    }
    std::ostringstream oss_thd, oss_th, oss_thl;
    oss_th << "__threshold" << id <<"_"<<i;
    oss_thd << "__thresholdDisp" << id << "_" << i;
    oss_thl << "__thresholdLut" << id << "_" << i;
    ComponentThresold ct = ComponentThresold();
    ct._thresholdVar = oss_th.str();
    ct._thresholdDispVar = oss_thd.str();
    ct._thresholdLutVar = oss_thl.str();
    _presentationThresolds.push_back(ct);
  }
}

std::string MEDPresentation::getThresholdFieldName() const {
  std::string result = getFieldName();
  if (_nbComponentsInThresholdInput > 1 && _hideDataOutsideCustomRange) {
    std::ostringstream oss;
    if (_selectedComponentIndex == -1) {
      oss << _fieldName << "_magnitude";
    }
    else {
      oss << _fieldName << "_" << _selectedComponentIndex + 1;
    }
    result = oss.str();
  }
  return result;
}

std::string MEDPresentation::getFieldName() const {
  return _fieldName;
}

std::string MEDPresentation::toScriptCellType(const std::string& pvType) {
  std::string typ = "";
  if (pvType == "CELLS") {
    typ = "CellData";
  }
  else if (pvType == "POINTS") {
    typ = "PointData";
  }
  else {
    std::string msg("Unsupported spatial discretisation: " + pvType);
    STDLOG(msg);
    throw KERNEL::createSalomeException(msg.c_str());
  }
  return typ;
}

std::string MEDPresentation::getScriptCellType() const {
  return toScriptCellType(_pvFieldType);
}

/**
 * In case where a CELLS field needs to be converted to POINT field.
 * This updates the source object to become the result of the CellDatatoPointData filter.
 */
void
MEDPresentation::applyCellToPointIfNeeded()
{
  if (_pvFieldType == "CELLS")
    {
      std::ostringstream oss, oss2;
      // Apply Cell data to point data:
      oss2 << "__srcObj" << GeneratePythonId();
      oss << oss2.str() << " = pvs.CellDatatoPointData(Input=" << _srcObjVar << ");";
      pushAndExecPyLine(oss.str()); oss.str("");
      // Now the source becomes the result of the CellDatatoPointData:
      _srcObjVar = oss2.str();
    }
}

/**
* Delete threshold filters and programmable filter
*/
void 
MEDPresentation::deleteThresholds() {
  std::ostringstream oss;
  for (auto& value : _presentationThresolds) {
    if (value._active) {
      oss << "pvs.Hide(" << value._thresholdVar << ");";
    }
    if (value._thresholdInitialized) {
      oss << "pvs.Delete(" << value._thresholdVar <<");";
      value.clear();
    }
  }
  if (_programmableInitialized) {
    oss << "pvs.Delete(" << _programmableVar << ");";
    _programmableInitialized = false;
  }
  pushAndExecPyLine(oss.str());
}

///**
// * Convert a vector field into a 3D vector field:
// *  - if the vector field is already 3D, nothing to do
// *  - if it is 2D, then add a null component
// *  - otherwise (tensor field, scalar field) throw
// */
//void
//MEDPresentation::convertTo3DVectorField()
//{
//  std::ostringstream oss, oss1, oss2, oss3;
//
//  int nbCompo = getIntProperty(MEDPresentation::PROP_NB_COMPONENTS);
//  if (nbCompo < 2 || nbCompo > 3)
//    {
//      oss << "The field '" << _fieldName << "' must have 2 or 3 components for this presentation!";
//      STDLOG(oss.str());
//      throw KERNEL::createSalomeException(oss.str().c_str());
//    }
//  if (nbCompo == 3)
//    return;
//
//  // Apply calculator:
//  oss2 << "__srcObj" << GeneratePythonId();
//  oss << oss2.str() << " = pvs.Calculator(Input=" << _srcObjVar << ");";
//  pushAndExecPyLine(oss.str()); oss.str("");
//  // Now the source becomes the result of the CellDatatoPointData:
//  _srcObjVar = oss2.str();
//  std::string typ;
//  if(_pvFieldType == "CELLS")
//    typ = "Cell Data";
//  else if(_pvFieldType == "POINTS")
//    typ = "Point Data";
//  else
//    {
//      oss3 << "Field '" << _fieldName << "' has invalid field type";
//      STDLOG(oss3.str());
//      throw KERNEL::createSalomeException(oss3.str().c_str());
//    }
//  oss << _srcObjVar << ".AttributeMode = '" <<  typ << "';";
//  pushAndExecPyLine(oss.str()); oss.str("");
//  oss << _srcObjVar << ".ResultArrayName = '" <<  _fieldName << "_CALC';";  // will never be needed I think
//  pushAndExecPyLine(oss.str()); oss.str("");
//  oss << _srcObjVar << ".Function = '" <<  _fieldName << "_0*iHat + " << _fieldName << "_1*jHat + 0.0*zHat';";
//  pushAndExecPyLine(oss.str()); oss.str("");
//}
