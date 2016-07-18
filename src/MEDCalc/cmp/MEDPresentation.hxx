// Copyright (C) 2011-2016  CEA/DEN, EDF R&D
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

#ifndef SRC_MEDCALC_CMP_MEDPRESENTATION_HXX_
#define SRC_MEDCALC_CMP_MEDPRESENTATION_HXX_

#include "MEDCouplingRefCountObject.hxx"
#include <Python.h>
#include "MEDCALC.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDDataManager)
#include CORBA_SERVER_HEADER(MEDPresentationManager)

#include <vector>
#include <map>
#include <string>

class MEDCALC_EXPORT MEDPresentation
{
  friend class MEDPresentationManager_i;

public:

  typedef ::CORBA::Long TypeID;

  virtual ~MEDPresentation();

  void setProperty(const std::string& propName, const std::string& propValue);
  const std::string getProperty(const std::string& propName) const;

  void activateView() const;

protected:
  typedef std::pair<int, PyObject *> PyObjectId;

  MEDPresentation(MEDPresentation::TypeID fieldHandlerId, const std::string& name);
  std::string getRenderViewCommand(MEDCALC::MEDPresentationViewMode viewMode) const;
  std::string getResetCameraCommand() const;
  std::string getColorMapCommand(MEDCALC::MEDPresentationColorMap colorMap) const;

  virtual void internalGeneratePipeline() = 0;
  PyObject* getPythonObjectFromMain(const char* var) const;
  void pushPyObjects(PyObjectId obj, PyObjectId disp);
  void pushAndExecPyLine(const std::string & lin);

  MEDPresentation::TypeID getID() const { return _fieldHandlerId; }
  long getPyViewID() const { return _renderViewPyId; }

  static int GeneratePythonId();

  virtual MEDCALC::MEDPresentationViewMode getViewMode() = 0;

private:

  std::string getFieldTypeString(MEDCoupling::TypeOfField fieldType) const;

  // The following functions are reserved to friend class MEDPresentationManager
  void generatePipeline();

  template<typename PresentationType, typename PresentationParameters>
  void updatePipeline(PresentationParameters params);

protected:
  std::string _fileName;
  std::string _fieldName;
  std::string _fieldType;

  ///! Identifier (in the Python dump) of the render view
  int _renderViewPyId;

private:
  MEDPresentation::TypeID _fieldHandlerId;

  ///! Pipeline elements
  std::vector<PyObjectId> _pipeline;

  ///! Corresponding display object, if any:
  std::vector<PyObjectId> _display;

  ///! Presentation properties <key,value>
  std::map<std::string, std::string> _properties;
};

#include "MEDPresentation.txx"

#endif /* SRC_MEDCALC_CMP_MEDPRESENTATION_HXX_ */
