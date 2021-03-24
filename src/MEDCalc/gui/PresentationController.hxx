// Copyright (C) 2016-2021  CEA/DEN, EDF R&D
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

#ifndef PRESENTATION_CONTROLLER_HXX
#define PRESENTATION_CONTROLLER_HXX

#include "MEDEventListener_i.hxx"

#include <QObject>
#include "MEDCALCGUI.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDPresentationManager)
#include CORBA_CLIENT_HEADER(MEDDataManager)

#include "XmedDataModel.hxx"
#include <SALOME_AppStudyEditor.hxx>
#include "WidgetPresentationParameters.hxx"

#include "PresentationEvent.hxx"

class MEDModule;
class QDockWidget;
class MEDWidgetHelper;
class XmedConsoleDriver;

class MEDCALCGUI_EXPORT PresentationController : public QObject {
  Q_OBJECT

public:
  PresentationController(MEDModule* salomeModule);
  ~PresentationController();

  void createActions();

  MEDCALC::ViewModeType getSelectedViewMode() const;
  MEDCALC::ColorMapType getSelectedColorMap() const;
  MEDCALC::ScalarBarRangeType getSelectedScalarBarRange() const;

  void showDockWidgets(bool isVisible);

  std::string getPresTypeFromWidgetHelper(int presId) const;

  void setConsoleDriver(XmedConsoleDriver* driver) { _consoleDriver = driver; };

signals:
  void presentationSignal(const PresentationEvent*);

protected slots:
  void onVisualizeMeshView();
  void onVisualizeScalarMap();
  void onVisualizeContour();
  void onVisualizeVectorField();
  void onVisualizeSlices();
  void onVisualizeDeflectionShape();
  void onVisualizePointSprite();

  void onDeletePresentation();
  void onParavisDump();

  void onPresentationSelected(int presId, const QString& presType, const QString& presName);

  void processWorkspaceEvent(const MEDCALC::MedEvent*);
  void processPresentationEvent(const PresentationEvent*);

private:
  void visualize(PresentationEvent::EventType);
  void updateTreeViewWithNewPresentation(long dataId, long presentationId);
  void updateTreeViewForPresentationRemoval(long presId);
  std::string _getIconName(const std::string&);
  MEDWidgetHelper * findOrCreateWidgetHelper(MEDCALC::MEDPresentationManager_ptr presManager,
                                             int presId, const std::string& type, const std::string& name);

  QString getViewModePython() const;
  QString getColorMapPython() const;
  QString getScalarBarRangePython() const;
  QString getMeshModePython(const int mode) const;
  QString getSliceOrientationPython(const int orient) const;

  void _dealWithReplaceMode();

private:
  MEDModule* _salomeModule;
  XmedConsoleDriver* _consoleDriver;   // the same as in WorkspaceController
  SALOME_AppStudyEditor* _studyEditor; // borrowed to MEDModule

  QDockWidget *_dockWidget;
  WidgetPresentationParameters* _widgetPresentationParameters;

  // GUI needs to talk directly to the pres manager to activate a view, get some params, print low level py dump, etc ...:
  static MEDCALC::MEDPresentationManager_ptr _presManager;

  // Key: presentation ID, value MEDPresentationHelper *
  std::map<int, MEDWidgetHelper *> _presHelperMap;

  MEDWidgetHelper * _currentWidgetHelper;
};

#endif /* PRESENTATION_CONTROLLER_HXX */
