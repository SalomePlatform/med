// Copyright (C) 2016-2023  CEA, EDF
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

#ifndef PROCESSING_CONTROLLER_HXX
#define PROCESSING_CONTROLLER_HXX

#include <Python.h>
#include "MEDCALCGUI.hxx"

#include "MEDEventListener_i.hxx"
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDDataManager)

#include <QObject>
#include <SalomeApp_Module.h>
#include <SALOME_AppStudyEditor.hxx>

#include "XmedDataModel.hxx"
#include "DlgChangeUnderlyingMesh.hxx"
#include "DlgInterpolateField.hxx"

typedef struct {
  enum EventType {
    EVENT_IMPORT_OBJECT,
    EVENT_INTERPOLATE_FIELD,
    EVENT_CHANGE_UNDERLYING_MESH
  };
  int eventtype;
  int fieldId;
  int meshId;
  MEDCALC::InterpolationParameters interpParams;
} ProcessingEvent;

class MEDModule;

class MEDCALCGUI_EXPORT ProcessingController : public QObject {
  Q_OBJECT

public:
  ProcessingController(MEDModule* salomeModule);
  ~ProcessingController();

  void createActions();

public slots:
  // Callback connected to dialog box validation signals
  void OnChangeUnderlyingMeshInputValidated();
  void OnInterpolateFieldInputValidated();

protected slots:
  void OnChangeUnderlyingMesh();
  void OnInterpolateField();

signals:
  void processingSignal(const ProcessingEvent*);

private:
  MEDModule* _salomeModule;
  SALOME_AppStudyEditor* _studyEditor; // borrowed to MEDModule
  DlgChangeUnderlyingMesh* _dlgChangeUnderlyingMesh;
  DlgInterpolateField* _dlgInterpolateField;
};

#endif /* PROCESSING_CONTROLLER_HXX */
