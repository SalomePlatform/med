// Copyright (C) 2016-2022  CEA/DEN, EDF R&D
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

#ifdef WIN32
#define NOMINMAX
#endif

#include "WidgetPresentationParameters.hxx"
#include <Basics_Utils.hxx>

#include <limits>

using namespace std;

WidgetPresentationParameters::WidgetPresentationParameters(QWidget* parent)
  : QWidget(parent), _blockSig(false)
{
  _ui.setupUi(this); // To be done first

  toggleWidget(false);
  QObject::connect(_ui.comboBoxCompo,          SIGNAL(activated(int)),
                   this,                       SLOT(onComboCompoIndexChanged(int)) );
  QObject::connect(_ui.comboBoxMesh,           SIGNAL(activated(int)),
                     this,                     SLOT(onComboMeshIndexChanged(int)) );
  QObject::connect(_ui.comboBoxScalarBarRange, SIGNAL(activated(int)),
                   this,                       SLOT(onComboScalarBarRangeIndexChanged(int)) );
  QObject::connect(_ui.comboBoxColorMap,       SIGNAL(activated(int)),
                   this,                       SLOT(onComboColorMapIndexChanged(int)) );
  QObject::connect(_ui.comboBoxSliceOrient,    SIGNAL(activated(int)),
                   this,                       SLOT(onComboOrientIndexChanged(int)) );
  QObject::connect(_ui.comboBoxIntegrDir,      SIGNAL(activated(int)),
                   this,                       SLOT(onComboIntegrDirIndexChanged(int)) );
  QObject::connect(_ui.comboBoxContComp,       SIGNAL(activated(int)),
                   this,                       SLOT(onComboContCompIndexChanged(int)) );
  QObject::connect(_ui.spinBox,                SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinBoxEditingFinished()) );
  QObject::connect(_ui.checkBoxShowScalarBar,  SIGNAL(stateChanged(int)),
                     this,                     SLOT(onCheckboxScalarBarVisibilityChanged(int)));
  QObject::connect(_ui.checkBoxCustomRange,    SIGNAL(stateChanged(int)),
                     this,                     SLOT(onCheckboxCustomRangeChanged(int)));
  QObject::connect(_ui.checkBoxScaleFactor,    SIGNAL(stateChanged(int)),
                     this,                     SLOT(onCheckboxScaleFactorChanged(int)));
  QObject::connect(_ui.checkBoxScaleFactor,    SIGNAL(stateChanged(int)),
                     this,                     SLOT(onCheckboxCustomScaleFactorChanged(int)));
  QObject::connect(_ui.spinCustomRangeMin,     SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinboxCustomRangeChanged()));
  QObject::connect(_ui.spinCustomRangeMax,     SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinboxCustomRangeChanged()));
  QObject::connect(_ui.spinScaleFactor,        SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinboxScaleFactorChanged()));
  QObject::connect(_ui.checkBoxHideDataOutsideCR, SIGNAL(stateChanged(int)),
                     this,                        SLOT(onCheckboxHideDataOutsideCustomRangeChanged(int)));
  // sphinx doc: begin of normal and plane connections
  QObject::connect(_ui.spinNormalX,            SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinNormalEditingFinished()) );
  QObject::connect(_ui.spinNormalY,            SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinNormalEditingFinished()) );
  QObject::connect(_ui.spinNormalZ,            SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinNormalEditingFinished()) );
  QObject::connect(_ui.spinPlanePos,           SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinPlanePosEditingFinished()) );
  // sphinx doc: end of normal and plane connections
  QObject::connect(_ui.spinPoint1_X,           SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinCutPoint1EditingFinished()) );
  QObject::connect(_ui.spinPoint1_Y,           SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinCutPoint1EditingFinished()) );
  QObject::connect(_ui.spinPoint1_Z,           SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinCutPoint1EditingFinished()) );
  QObject::connect(_ui.spinPoint2_X,           SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinCutPoint2EditingFinished()) );
  QObject::connect(_ui.spinPoint2_Y,           SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinCutPoint2EditingFinished()) );
  QObject::connect(_ui.spinPoint2_Z,           SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinCutPoint2EditingFinished()) );

   // Disable Custom Range Spin boxes
  _ui.spinCustomRangeMax->setEnabled(false);
  _ui.spinCustomRangeMin->setEnabled(false);
  _ui.spinScaleFactor->setEnabled(false);
  _ui.checkBoxHideDataOutsideCR->setEnabled(false);

  // Min and max values
  _ui.spinCustomRangeMin->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
  _ui.spinCustomRangeMax->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
  
  _ui.spinCustomRangeMin->setDecimals(6);
  _ui.spinCustomRangeMax->setDecimals(6);
  
  _ui.spinNormalX->setRange(0.0, 1.0);
  _ui.spinNormalY->setRange(0.0, 1.0);
  _ui.spinNormalZ->setRange(0.0, 1.0);
  _ui.spinPlanePos->setRange(0.0, 1.0);
}

// for sphinx doc: start of onComboCompoIndexChanged
void
WidgetPresentationParameters::onComboCompoIndexChanged(int idx)
{
  if (!_blockSig) emit comboCompoIndexChanged(idx);
}
// for sphinx doc: end of onComboCompoIndexChanged

void
WidgetPresentationParameters::onComboOrientIndexChanged(int idx)
{
  if (!_blockSig) emit comboOrientIndexChanged(idx);
}


void
WidgetPresentationParameters::onComboMeshIndexChanged(int idx)
{
  if (!_blockSig) emit comboMeshIndexChanged(idx);
}

void
WidgetPresentationParameters::onComboColorMapIndexChanged(int idx)
{
  if (!_blockSig) emit comboColorMapIndexChanged(idx);
}

void
WidgetPresentationParameters::onComboScalarBarRangeIndexChanged(int idx)
{
  if (!_blockSig) emit comboScalarBarRangeIndexChanged(idx);
}

void
WidgetPresentationParameters::onComboIntegrDirIndexChanged(int idx)
{
  if (!_blockSig) emit comboIntegrDirIndexChanged(idx);
}

void
WidgetPresentationParameters::onComboContCompIndexChanged(int idx)
{
  if (!_blockSig) emit comboContCompIndexChanged(idx);
}

void
WidgetPresentationParameters::onCheckboxScalarBarVisibilityChanged(int flag)
{
  if (!_blockSig) emit checkboxScalarBarVisibilityChanged(flag);
}

void
WidgetPresentationParameters::onCheckboxCustomRangeChanged(int flag)
{
  _ui.spinCustomRangeMin->setEnabled(flag);
  _ui.spinCustomRangeMax->setEnabled(flag);
  _ui.checkBoxHideDataOutsideCR->setEnabled(flag);
  _ui.comboBoxScalarBarRange->setEnabled(!flag);
  if (!_blockSig) emit checkboxCustomRangeChanged(flag);
}

void
WidgetPresentationParameters::onCheckboxScaleFactorChanged(int flag)
{
  _ui.spinScaleFactor->setEnabled(flag);
  if (!_blockSig) emit checkboxScaleFactorChanged(flag);
}

void
WidgetPresentationParameters::onSpinboxCustomRangeChanged()
{
  if (!_blockSig) emit spinboxCustomRangeChanged(_ui.spinCustomRangeMin->value(),
                                                 _ui.spinCustomRangeMax->value());
}

void
WidgetPresentationParameters::onSpinboxScaleFactorChanged()
{
  if (!_blockSig) emit spinboxScaleFactorChaged(_ui.spinScaleFactor->value());
}

void
WidgetPresentationParameters::
onCheckboxHideDataOutsideCustomRangeChanged(int flag)
{
  if (!_blockSig) emit checkboxHideDataOutsideCustomRangeChanged(flag);
}

void
WidgetPresentationParameters::
onCheckboxCustomScaleFactorChanged(int flag) {
  if (!_blockSig) emit checkboxCustomScaleFactorChanged(flag);
}

void
WidgetPresentationParameters::onSpinBoxEditingFinished()
{
  if (!_blockSig) emit spinBoxValueChanged(_ui.spinBox->value());
}

void
WidgetPresentationParameters::onSpinNormalEditingFinished()
{
  if (!_blockSig) emit spinNormalValuesChanged(_ui.spinNormalX->value(),
                                               _ui.spinNormalY->value(),
                                               _ui.spinNormalZ->value());
}

void
WidgetPresentationParameters::onSpinCutPoint1EditingFinished()
{
  if (!_blockSig) emit spinCutPoint1ValuesChanged(_ui.spinPoint1_X->value(),
                                                  _ui.spinPoint1_Y->value(),
                                                  _ui.spinPoint1_Z->value());
}

void
WidgetPresentationParameters::onSpinCutPoint2EditingFinished()
{
  if (!_blockSig) emit spinCutPoint2ValuesChanged(_ui.spinPoint2_X->value(),
                                                  _ui.spinPoint2_Y->value(),
                                                  _ui.spinPoint2_Z->value());
}

void
WidgetPresentationParameters::onSpinPlanePosEditingFinished()
{
  if (!_blockSig) emit spinPlanePosValueChanged(_ui.spinPlanePos->value());
}

void
WidgetPresentationParameters::hidePlot3D()
{
  _ui.labelCutPlanePosition->hide();
  _ui.spinPlanePos->hide();
}

void
WidgetPresentationParameters::hideContourComponent()
{
  _ui.labelContourComp->hide();
  _ui.comboBoxContComp->hide();
}

void
WidgetPresentationParameters::toggleCommonFieldWidget(bool show)
{
  _blockSig = true;
  _ui.commonWidget->setEnabled(show);
  _blockSig = false;
}

void
WidgetPresentationParameters::toggleWidget(bool show)
{
  toggleCommonFieldWidget(true);
  if (!show)
    {
      _blockSig = true;
      _ui.widgetDynamic->hide();
      setPresName(tr("LAB_DEFAULT_DYN_TITLE").toStdString());
      // reset colorMap and scalarBarRange:
      setColorMap(MEDCALC::COLOR_MAP_DEFAULT);
      setScalarBarRange(MEDCALC::SCALAR_BAR_RANGE_DEFAULT);
    }
  else
    {
      _ui.widgetDynamic->show();
      // It is the WidgetHelper responsibility to re-show the widgets it needs
      _ui.labelCompo->hide();
      _ui.comboBoxCompo->hide();
      _ui.labelMeshMode->hide();
      _ui.comboBoxMesh->hide();
      _ui.labelSpinBox->hide();
      _ui.spinBox->hide();
      _ui.labelSliceOrient->hide();
      _ui.comboBoxSliceOrient->hide();
      _ui.labelCutPlaneNormal->hide();
      _ui.labelCutPlanePosition->hide();
      _ui.spinNormalX->hide();
      _ui.spinNormalY->hide();
      _ui.spinNormalZ->hide();
      _ui.spinPlanePos->hide();
      _ui.labelIntegrDir->hide();
      _ui.comboBoxIntegrDir->hide();
      _ui.labelPoint1->hide();
      _ui.spinPoint1_X->hide();
      _ui.spinPoint1_Y->hide();
      _ui.spinPoint1_Z->hide();
      _ui.labelPoint2->hide();
      _ui.spinPoint2_X->hide();
      _ui.spinPoint2_Y->hide();
      _ui.spinPoint2_Z->hide();
      _ui.labelContourComp->hide();
      _ui.comboBoxContComp->hide();
      _ui.checkBoxScaleFactor->hide();
      _ui.spinScaleFactor->hide();
      _blockSig = false;
    }
}

bool
WidgetPresentationParameters::isShown() const
{
  return _ui.widgetDynamic->isVisible();
}

string
WidgetPresentationParameters::getComponent() const
{
  if (_ui.comboBoxCompo->count() > 1 && _ui.comboBoxCompo->count() <= 4)
    if (_ui.comboBoxCompo->currentIndex() == 0) // Euclidean norm
      return "";

  return _ui.comboBoxCompo->currentText().toStdString();
}

void
WidgetPresentationParameters::setComponents(vector<string> compos, int selecIndex)
{
  _blockSig = true;

  // Show the widget:
  _ui.labelCompo->show();
  _ui.comboBoxCompo->show();

  _ui.comboBoxCompo->clear();
  bool vectorField = (compos.size() > 1 && compos.size() <= 3);
  if (vectorField)
    _ui.comboBoxCompo->addItem(tr("LAB_EUCLIDEAN_NORM"));
  for(vector<string>::const_iterator it = compos.begin(); it != compos.end(); ++it)
    _ui.comboBoxCompo->addItem(QString::fromStdString(*it));
  if (!vectorField)
    _ui.comboBoxCompo->setCurrentIndex(std::max(0, selecIndex-1));
  else
    _ui.comboBoxCompo->setCurrentIndex(selecIndex);

  _blockSig = false;
}

void
WidgetPresentationParameters::setNbContour(int nbContour)
{
  _blockSig = true;

  if (nbContour <= 0)
    {
      //TODO throw?
      STDLOG("WidgetPresentationParameters::setNbContour(): invalid number of contours!");
    }

  // Show the widget:
  _ui.labelSpinBox->setText(tr("LAB_NB_CONTOURS").arg(MEDCALC::NB_CONTOURS_MAX));

  _ui.labelSpinBox->show();
  _ui.spinBox->show();
  _ui.spinBox->setRange(1, MEDCALC::NB_CONTOURS_MAX);
  _ui.spinBox->setValue(nbContour);

  _blockSig = false;
}

void
WidgetPresentationParameters::setNbSlices(int nbSlices)
{
  _blockSig = true;

  if (nbSlices <= 0)
    {
      //TODO throw?
      STDLOG("WidgetPresentationParameters::setNbSlices(): invalid number of slices!");
    }

  // Show the widget:
  _ui.labelSpinBox->setText(tr("LAB_NB_SLICES").arg(MEDCALC::NB_SLICES_MAX));
  _ui.labelSpinBox->show();
  _ui.spinBox->show();
  _ui.spinBox->setRange(1, MEDCALC::NB_SLICES_MAX);
  _ui.spinBox->setValue(nbSlices);

  _blockSig = false;
}

int WidgetPresentationParameters::getNbContour() const
{
  return _ui.spinBox->value();
}

int WidgetPresentationParameters::getNbSlices() const
{
  return _ui.spinBox->value();
}


void
WidgetPresentationParameters::setScalarBarRange(MEDCALC::ScalarBarRangeType sbrange)
{
  _blockSig = true;

  int idx = -1;
  if (sbrange == MEDCALC::SCALAR_BAR_ALL_TIMESTEPS)
    idx = _ui.comboBoxScalarBarRange->findText(tr("LAB_ALL_TIMESTEPS"));
  else if (sbrange == MEDCALC::SCALAR_BAR_CURRENT_TIMESTEP)
    idx = _ui.comboBoxScalarBarRange->findText(tr("LAB_CURRENT_TIMESTEP"));

  if (idx >= 0) {
      _ui.comboBoxScalarBarRange->setCurrentIndex(idx);
      _ui.checkBoxCustomRange->setChecked(false);
  }
  else {
    if (sbrange == MEDCALC::SCALAR_BAR_CUSTOM_RANGE) {
      _ui.checkBoxCustomRange->setChecked(true);
    } else {
      STDLOG("Strange!! No matching found - unable to set scalar bar range in GUI.");
    }
  }
  _blockSig = false;
}

void
WidgetPresentationParameters::setColorMap(MEDCALC::ColorMapType colorMap)
{
  _blockSig = true;

  int idx = -1;
  if (colorMap == MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW)
    idx = _ui.comboBoxColorMap->findText(tr("LAB_BLUE_TO_RED"));
  else if (colorMap == MEDCALC::COLOR_MAP_COOL_TO_WARM)
    idx = _ui.comboBoxColorMap->findText(tr("LAB_COOL_TO_WARM"));

  if (idx >= 0)
  {
    _ui.comboBoxColorMap->setCurrentIndex(idx);
  }
  else
  {
    STDLOG("Strange!! No matching found - unable to set color map in GUI.");
  }

  _blockSig = false;
}

void
WidgetPresentationParameters::setMeshMode(MEDCALC::MeshModeType mode)
{
  _blockSig = true;

  // Show the widget:
  _ui.labelMeshMode->show();
  _ui.comboBoxMesh->show();

  int idx;
  switch(mode)
  {
    case MEDCALC::MESH_MODE_WIREFRAME:
      idx = _ui.comboBoxMesh->findText(tr("LAB_MESH_WIREFRAME"));
      break;
    case MEDCALC::MESH_MODE_SURFACE:
      idx = _ui.comboBoxMesh->findText(tr("LAB_MESH_SURFACE"));
      break;
    case MEDCALC::MESH_MODE_SURFACE_EDGES:
      idx = _ui.comboBoxMesh->findText(tr("LAB_MESH_SURF_EDGES"));
      break;
    default:
      idx = -1;
  }
  if (idx >= 0)
  {
    _ui.comboBoxMesh->setCurrentIndex(idx);
  }
  else
  {
    STDLOG("Strange!! No matching found - unable to set mesh mode in GUI.");
  }

  _blockSig = false;
}

void
WidgetPresentationParameters::setIntegrationDir(MEDCALC::IntegrationDirType iDir)
{
  _blockSig = true;

  // Show the widget;
  _ui.labelIntegrDir->show();
  _ui.comboBoxIntegrDir->show();

  int idx;
  switch(iDir)
  {
    case MEDCALC::INTEGRATION_DIR_BOTH:
      idx = _ui.comboBoxIntegrDir->findText(tr("LAB_INTEGR_DIR_BOTH"));
      break;
    case MEDCALC::INTEGRATION_DIR_FORWARD:
      idx = _ui.comboBoxIntegrDir->findText(tr("LAB_INTEGR_DIR_FORWARD"));
      break;
    case MEDCALC::INTEGRATION_DIR_BACKWARD:
      idx = _ui.comboBoxIntegrDir->findText(tr("LAB_INTEGR_DIR_BACKWARD"));
      break;
    default:
      idx = -1;
  }
  if (idx >= 0)
  {
    _ui.comboBoxIntegrDir->setCurrentIndex(idx);
  }
  else
  {
    STDLOG("Strange!! No matching found - unable to set integration direction in GUI.");
  }

  _blockSig = false;
}

void
WidgetPresentationParameters::setContourComponents(std::vector<std::string> compos, int selecIndex)
{
  _blockSig = true;

  // Show the widget;
  _ui.labelContourComp->show();
  _ui.comboBoxContComp->show();

  _ui.comboBoxContComp->clear();
  bool vectorField = (compos.size() > 1 && compos.size() <= 3);
  if (vectorField)
    _ui.comboBoxContComp->addItem(tr("LAB_EUCLIDEAN_NORM"));
  for (vector<string>::const_iterator it = compos.begin(); it != compos.end(); ++it)
    _ui.comboBoxContComp->addItem(QString::fromStdString(*it));
  if (!vectorField)
    _ui.comboBoxContComp->setCurrentIndex(std::max(0, selecIndex - 1));
  else
    _ui.comboBoxContComp->setCurrentIndex(selecIndex);

  _blockSig = false;
}

void
WidgetPresentationParameters::setSliceOrientation(MEDCALC::SliceOrientationType orient)
{
  _blockSig = true;

  // Show the widget:
  _ui.labelSliceOrient->show();
  _ui.comboBoxSliceOrient->show();

  int idx;
  switch(orient)
  {
    case MEDCALC::SLICE_NORMAL_TO_X:
      idx = _ui.comboBoxSliceOrient->findText(tr("LAB_SLICE_NORMAL_TO_X"));
      break;
    case MEDCALC::SLICE_NORMAL_TO_Y:
      idx = _ui.comboBoxSliceOrient->findText(tr("LAB_SLICE_NORMAL_TO_Y"));
      break;
    case MEDCALC::SLICE_NORMAL_TO_Z:
      idx = _ui.comboBoxSliceOrient->findText(tr("LAB_SLICE_NORMAL_TO_Z"));
      break;
    case MEDCALC::SLICE_NORMAL_TO_XY:
      idx = _ui.comboBoxSliceOrient->findText(tr("LAB_SLICE_NORMAL_TO_XY"));
      break;
    case MEDCALC::SLICE_NORMAL_TO_XZ:
      idx = _ui.comboBoxSliceOrient->findText(tr("LAB_SLICE_NORMAL_TO_XZ"));
      break;
    case MEDCALC::SLICE_NORMAL_TO_YZ:
      idx = _ui.comboBoxSliceOrient->findText(tr("LAB_SLICE_NORMAL_TO_YZ"));
      break;
    case MEDCALC::SLICE_NORMAL_TO_XYZ:
      idx = _ui.comboBoxSliceOrient->findText(tr("LAB_SLICE_NORMAL_TO_XYZ"));
      break;
    default:
      idx = -1;
  }
  if (idx >= 0)
  {
    _ui.comboBoxSliceOrient->setCurrentIndex(idx);
  }
  else
  {
    STDLOG("Strange!! No matching found - unable to set slice orientation in GUI.");
  }

  _blockSig = false;
}


MEDCALC::SliceOrientationType
WidgetPresentationParameters::getSliceOrientation() const
{
  QString sbrange = _ui.comboBoxSliceOrient->currentText();
  if (sbrange == tr("LAB_SLICE_NORMAL_TO_X")) {
      return MEDCALC::SLICE_NORMAL_TO_X;
  }
  else if (sbrange == tr("LAB_SLICE_NORMAL_TO_Y")) {
      return MEDCALC::SLICE_NORMAL_TO_Y;
  }
  else if (sbrange == tr("LAB_SLICE_NORMAL_TO_Z")) {
      return MEDCALC::SLICE_NORMAL_TO_Z;
  }
  else if (sbrange == tr("LAB_SLICE_NORMAL_TO_XY")) {
      return MEDCALC::SLICE_NORMAL_TO_XY;
  }
  else if (sbrange == tr("LAB_SLICE_NORMAL_TO_XZ")) {
      return MEDCALC::SLICE_NORMAL_TO_XZ;
  }
  else if (sbrange == tr("LAB_SLICE_NORMAL_TO_YZ")) {
      return MEDCALC::SLICE_NORMAL_TO_YZ;
  }
  else if (sbrange == tr("LAB_SLICE_NORMAL_TO_XYZ")) {
      return MEDCALC::SLICE_NORMAL_TO_XYZ;
  }
  // Should not happen
  STDLOG("Strange!! No matching found - returning SLICE_NORMAL_TO_X.");
  return MEDCALC::SLICE_NORMAL_TO_X;
}

MEDCALC::MeshModeType
WidgetPresentationParameters::getMeshMode() const
{
  QString mesm = _ui.comboBoxMesh->currentText();
  if (mesm == tr("LAB_MESH_WIREFRAME")) {
      return MEDCALC::MESH_MODE_WIREFRAME;
  }
  else if (mesm == tr("LAB_MESH_SURFACE")) {
      return MEDCALC::MESH_MODE_SURFACE;
  }
  else if (mesm == tr("LAB_MESH_SURF_EDGES")) {
      return MEDCALC::MESH_MODE_SURFACE_EDGES;
  }
  // Should not happen
  STDLOG("Strange!! No matching found - returning MESH_MODE_WIREFRAME.");
  return MEDCALC::MESH_MODE_WIREFRAME;
}

MEDCALC::IntegrationDirType 
WidgetPresentationParameters::getIntegrationDir() const
{
  QString iDir = _ui.comboBoxIntegrDir->currentText();
  if (iDir == tr("LAB_INTEGR_DIR_BOTH")) {
      return MEDCALC::INTEGRATION_DIR_BOTH;
  }
  else if (iDir == tr("LAB_INTEGR_DIR_FORWARD")) {
      return MEDCALC::INTEGRATION_DIR_FORWARD;
  }
  else if (iDir == tr("LAB_INTEGR_DIR_BACKWARD")) {
      return MEDCALC::INTEGRATION_DIR_BACKWARD;
  }
  // Should not happen
  STDLOG("Strange!! No matching found - returning LAB_INTEGR_DIR_BOTH.");
  return MEDCALC::INTEGRATION_DIR_BOTH;
}

std::string 
WidgetPresentationParameters::getContourComponent() const
{
  if (_ui.comboBoxContComp->count() > 1 && _ui.comboBoxContComp->count() <= 4)
    if (_ui.comboBoxContComp->currentIndex() == 0) // Euclidean norm
      return "";
  return _ui.comboBoxContComp->currentText().toStdString();
}

MEDCALC::ScalarBarRangeType
WidgetPresentationParameters::getScalarBarRange() const
{
  if (_ui.checkBoxCustomRange->isChecked()) {
    return  MEDCALC::SCALAR_BAR_CUSTOM_RANGE;
  }
  QString sbrange = _ui.comboBoxScalarBarRange->currentText();
  if (sbrange == tr("LAB_ALL_TIMESTEPS")) {
    return MEDCALC::SCALAR_BAR_ALL_TIMESTEPS;
  }
  else if (sbrange == tr("LAB_CURRENT_TIMESTEP")) {
    return MEDCALC::SCALAR_BAR_CURRENT_TIMESTEP;
  }
  // Should not happen
  STDLOG("Strange!! No matching found - returning SCALAR_BAR_ALL_TIMESTEPS.");
  return MEDCALC::SCALAR_BAR_ALL_TIMESTEPS;
}

//double
//WidgetPresentationParameters::getScalarBarTimestep() const
//{
//  return _ui.doubleSpinBoxTimeStep->value();
//}
//
//double
//WidgetPresentationParameters::getScalarBarMinVal() const
//{
//  return _ui.doubleSpinBoxMinVal->value();
//}
//
//double
//WidgetPresentationParameters::getScalarBarMaxVal() const
//{
//  return _ui.doubleSpinBoxMaxVal->value();
//}

MEDCALC::ColorMapType
WidgetPresentationParameters::getColorMap() const
{
  QString colorMap = _ui.comboBoxColorMap->currentText();
  if (colorMap == tr("LAB_BLUE_TO_RED")) {
    return MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW;
  }
  else if (colorMap == tr("LAB_COOL_TO_WARM")) {
    return MEDCALC::COLOR_MAP_COOL_TO_WARM;
  }
  // Should not happen
  STDLOG("Strange!! No matching color map found - returning blue to red.");
  return MEDCALC::COLOR_MAP_BLUE_TO_RED_RAINBOW;
}

void
WidgetPresentationParameters::setPresName(const std::string& name)
{
  _ui.labelPresName->setText(QString::fromStdString(name));
  QFont f(_ui.labelPresName->font());
  f.setItalic(true);
  _ui.labelPresName->setFont(f);
}


void 
WidgetPresentationParameters::getScalarBarRangeValue(double* arr) const 
{
  arr[0] = _ui.spinCustomRangeMin->value();
  arr[1] = _ui.spinCustomRangeMax->value();
}

void WidgetPresentationParameters::setScalarBarRangeValue(double aMin, double aMax) {
  _blockSig = true;
  _ui.spinCustomRangeMin->setValue(aMin);
  _ui.spinCustomRangeMax->setValue(aMax);
  _blockSig = false;
}

void WidgetPresentationParameters::setScaleFactor(double scale) {
  _blockSig = true;

  // Show widget:
  _ui.checkBoxScaleFactor->show();
  _ui.spinScaleFactor->show();

  _ui.spinScaleFactor->setValue(scale);
  _blockSig = false;
}

// sphinx doc: begin of getNormal
void WidgetPresentationParameters::getNormal(double* arr) const
{
  arr[0] = _ui.spinNormalX->value();
  arr[1] = _ui.spinNormalY->value();
  arr[2] = _ui.spinNormalZ->value();
}
// sphinx doc: end of getNormal

// sphinx doc: begin of setNormal
void
WidgetPresentationParameters::setNormal(const double normX, const double normY, const double normZ)
{
  _blockSig = true;
  _ui.labelCutPlaneNormal->show();
  _ui.spinNormalX->show();
  _ui.spinNormalY->show();
  _ui.spinNormalZ->show();
  _ui.spinNormalX->setValue(normX);
  _ui.spinNormalY->setValue(normY);
  _ui.spinNormalZ->setValue(normZ);
  _blockSig = false;
}
// sphinx doc: end of setNormal

void WidgetPresentationParameters::getCutPoint1(double* arr) const
{
  arr[0] = _ui.spinPoint1_X->value();
  arr[1] = _ui.spinPoint1_Y->value();
  arr[2] = _ui.spinPoint1_Z->value();
}

void
WidgetPresentationParameters::setCutPoint1(const double x, const double y, const double z)
{
  _blockSig = true;
  _ui.labelPoint1->show();
  _ui.spinPoint1_X->show();
  _ui.spinPoint1_Y->show();
  _ui.spinPoint1_Z->show();
  _ui.spinPoint1_X->setValue(x);
  _ui.spinPoint1_Y->setValue(y);
  _ui.spinPoint1_Z->setValue(z);
  _blockSig = false;
}

void WidgetPresentationParameters::getCutPoint2(double* arr) const
{
  arr[0] = _ui.spinPoint2_X->value();
  arr[1] = _ui.spinPoint2_Y->value();
  arr[2] = _ui.spinPoint2_Z->value();
}

void
WidgetPresentationParameters::setCutPoint2(const double x, const double y, const double z)
{
  _blockSig = true;
  _ui.labelPoint2->show();
  _ui.spinPoint2_X->show();
  _ui.spinPoint2_Y->show();
  _ui.spinPoint2_Z->show();
  _ui.spinPoint2_X->setValue(x);
  _ui.spinPoint2_Y->setValue(y);
  _ui.spinPoint2_Z->setValue(z);
  _blockSig = false;
}

// sphinx doc: begin of getPlanePosition
double WidgetPresentationParameters::getPlanePosition() const
{
  return _ui.spinPlanePos->value();
}
// sphinx doc: begin of getPlanePosition

// sphinx doc: begin of setPlanePosition
void WidgetPresentationParameters::setPlanePosition(double pos)
{
  _blockSig = true;
  _ui.labelCutPlanePosition->show();
  _ui.spinPlanePos->show();
  _ui.spinPlanePos->setValue(pos);
  _blockSig = false;
}
// sphinx doc: begin of setPlanePosition

bool WidgetPresentationParameters::getScalarBarVisibility() const {
  return _ui.checkBoxShowScalarBar->isChecked();
}

void WidgetPresentationParameters::setScalarBarVisibility(const bool visibility) {
  _blockSig = true;
  _ui.checkBoxShowScalarBar->setChecked(visibility);
  _blockSig = false;
}

bool WidgetPresentationParameters::getHideDataOutsideCustomRange() const {
  return _ui.checkBoxHideDataOutsideCR->isChecked();
}

void WidgetPresentationParameters::setHideDataOutsideCustomRange(const bool flag) {
  _blockSig = true;
  _ui.checkBoxHideDataOutsideCR->setChecked(flag);
  _blockSig = false;
}

void WidgetPresentationParameters::setScaleFactorFlag(const bool flag) {
  _blockSig = true;
  _ui.checkBoxScaleFactor->setChecked(flag);
  _blockSig = false;
}