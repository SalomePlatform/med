// Copyright (C) 2011-2021  CEA/DEN, EDF R&D
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
      _globalDict(0)
{
  setStringProperty(MEDPresentation::PROP_NAME, name);

  setIntProperty(MEDPresentation::PROP_NB_COMPONENTS, 0);
  setIntProperty(MEDPresentation::PROP_SELECTED_COMPONENT, 0);

  setIntProperty(MEDPresentation::PROP_COLOR_MAP, static_cast<int>(colorMap));
  setIntProperty(MEDPresentation::PROP_SCALAR_BAR_RANGE, static_cast<int>(sbRange));

  // Python variables:
  int id = GeneratePythonId();
  std::ostringstream oss_o, oss_d, oss_l, oss_s, oss_r;
  oss_o << "__obj" << id;
  oss_s << "__srcObj" << id;
  oss_d << "__disp" << id;
  oss_l << "__lut" << id;
  oss_r << "__range" << id;
  _objVar = oss_o.str();
  _srcObjVar = oss_s.str();
  _dispVar = oss_d.str();
  _lutVar = oss_l.str();
  _rangeVar = oss_r.str();
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
  _colorByType = _pvFieldType;  // by default the same; overridden in DeflectionShape, VectorField, PointSprite and Contour
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
   pushAndExecPyLine( "import pvsimple as pvs;");
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
  oss << _srcObjVar << " = pvs.MEDReader(FileName=r'" << _fileName << "');";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << "medcalc.SelectSourceField(" << _srcObjVar << ", '" << _meshName << "', '"
      << _fieldName << "', '" << typ << "');";
  pushAndExecPyLine(oss.str()); oss.str("");
  // Generate complete vector fields: fields with 2 components will copied into <name>_vector and
  // have a third null component added.
  oss << _srcObjVar << ".GenerateVectors = 1;";
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
      oss2 << view << " = pvs.GetActiveViewOrCreate('RenderView');";
      pushAndExecPyLine(oss2.str()); oss2.str("");
  } else if (_viewMode == MEDCALC::VIEW_MODE_REPLACE) {
      // same as above
      oss2 << view << " = pvs.GetActiveViewOrCreate('RenderView');";
      pushAndExecPyLine(oss2.str()); oss2.str("");
      oss2 << "pvs.active_objects.source and pvs.Hide(view=" << view << ");";
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
  oss << "pvs.GetScalarBar(" << _lutVar << ").ComponentTitle = '" << compoName << "';";
  pushAndExecPyLine(oss.str()); oss.str("");
}

void
MEDPresentation::selectColorMap()
{
  std::ostringstream oss, oss2;

  oss2 << _lutVar << " = pvs.GetColorTransferFunction('" << _fieldName << "');";
  pushAndExecPyLine(oss2.str());

  switch (_colorMap) {
  case MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW:
    oss << _lutVar << ".ApplyPreset('Blue to Red Rainbow',True);";
    break;
  case MEDCALC::COLOR_MAP_COOL_TO_WARM:
    oss << _lutVar << ".ApplyPreset('Cool to Warm',True);";
    break;
  default:
    STDLOG("MEDPresentation::getColorMapCommand(): invalid colormap!");
    throw KERNEL::createSalomeException("MEDPresentation::getColorMapCommand(): invalid colormap!");
  }
  pushAndExecPyLine(oss.str());

  selectFieldComponent(); // somehow PV keeps the LUT parameters of the previous presentation, so better reset this.
}

void
MEDPresentation::showObject()
{
  std::ostringstream oss;
  oss << _dispVar << " = pvs.Show(" << _objVar << ", " << getRenderViewVar() << ");";
  pushAndExecPyLine(oss.str());
}

void
MEDPresentation::showScalarBar()
{
  std::ostringstream oss;
  oss << _dispVar <<  ".SetScalarBarVisibility(" << getRenderViewVar() << ", True);";
  pushAndExecPyLine(oss.str());
}

void
MEDPresentation::colorBy()
{
  std::ostringstream oss;
  oss << "pvs.ColorBy(" << _dispVar << ", ('" << _colorByType << "', '" << _fieldName << "'));";
  pushAndExecPyLine(oss.str());
}

void
MEDPresentation::rescaleTransferFunction()
{
  std::ostringstream oss;
  switch(_sbRange)
  {
    case MEDCALC::SCALAR_BAR_ALL_TIMESTEPS:
      oss << _dispVar << ".RescaleTransferFunctionToDataRangeOverTime();";
      break;
    case MEDCALC::SCALAR_BAR_CURRENT_TIMESTEP:
      oss << _dispVar << ".RescaleTransferFunctionToDataRange(False);";
      break;
    default:
      STDLOG("MEDPresentation::getRescaleCommand(): invalid range!");
      throw KERNEL::createSalomeException("MEDPresentation::getRescaleCommand(): invalid range!");
  }
  pushAndExecPyLine(oss.str()); oss.str("");
  // Get min-max
  oss << _rangeVar << " = [" << _dispVar << ".LookupTable.RGBPoints[0], " << _dispVar << ".LookupTable.RGBPoints[-4]];";
  pushAndExecPyLine(oss.str());

  // Adapt scalar bar title
  scalarBarTitle();
}



int
MEDPresentation::GeneratePythonId()
{
  static int INIT_ID = 0;
  return INIT_ID++;
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
  showObject();
  colorBy();
  showScalarBar();
  selectColorMap();
  rescaleTransferFunction();
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
  std::string typ;

  if(_pvFieldType == "CELLS") {
      typ = "CellData";
  }
  else if (_pvFieldType == "POINTS") {
      typ = "PointData";
  }
  else {
      std::string msg("Unsupported spatial discretisation: " + _pvFieldType);
      STDLOG(msg);
      throw KERNEL::createSalomeException(msg.c_str());
  }

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
        compo = std::string(Py_EncodeLocale(PyUnicode_AS_UNICODE(p_obj), NULL));  // pointing to internal Python memory, so make a copy!!
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
        aCompoMap[compo] = aCompoMap[compo]++;
      }
    }
  for (std::vector<std::string>::size_type i = 0; i != aCompos.size(); i++) {
    std::ostringstream oss_p;
    oss_p << MEDPresentation::PROP_COMPONENT << i;
    setStringProperty(oss_p.str(), aCompos[i]);
  }

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
