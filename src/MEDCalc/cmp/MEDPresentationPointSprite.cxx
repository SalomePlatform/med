#include "MEDPresentationPointSprite.hxx"

void
MEDPresentationPointSprite::internalGeneratePipeline()
{
  PyGILState_STATE _gil_state = PyGILState_Ensure();

  std::string cmd = std::string("import pvsimple as pvs;");
  cmd += std::string("__obj1 = pvs.MEDReader(FileName='") + _fileName + std::string("');");
  cmd += std::string("__disp1 = pvs.Show(__obj1);");
  cmd += std::string("pvs.ColorBy(__disp1, ('") + _fieldType + std::string("', '") + _fieldName + std::string("'));");
  cmd += std::string("pvs.GetActiveViewOrCreate('RenderView').ResetCamera();");
  cmd += std::string("__disp1.RescaleTransferFunctionToDataRangeOverTime();");
  cmd += std::string("__disp1.SetRepresentationType('Point Sprite');");

  //std::cerr << "Python command:" << std::endl;
  //std::cerr << cmd << std::endl;
  PyRun_SimpleString(cmd.c_str());
  // Retrieve Python object for internal storage:
  PyObject * obj = getPythonObjectFromMain("__obj1");
  PyObject * disp = getPythonObjectFromMain("__disp1");
  pushInternal(obj, disp);

  PyGILState_Release(_gil_state);
}