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

#ifdef WIN32
#define NOMINMAX
#endif

#include "WidgetPresentationParameters.hxx"
#include <Basics_Utils.hxx>

using namespace std;

WidgetPresentationParameters::WidgetPresentationParameters(QWidget* parent)
  : QWidget(parent), _blockSig(false)
{
  _ui.setupUi(this); // To be done first

  toggleWidget(false);
  QObject::connect(_ui.comboBoxCompo,          SIGNAL(activated(int)),
                   this,                       SLOT(onComboCompoIndexChanged(int)) );
  QObject::connect(_ui.comboBoxMesh,          SIGNAL(activated(int)),
                     this,                       SLOT(onComboMeshIndexChanged(int)) );
  QObject::connect(_ui.comboBoxScalarBarRange, SIGNAL(activated(int)),
                   this,                       SLOT(onComboScalarBarRangeIndexChanged(int)) );
  QObject::connect(_ui.comboBoxColorMap,       SIGNAL(activated(int)),
                   this,                       SLOT(onComboColorMapIndexChanged(int)) );
  QObject::connect(_ui.comboBoxSliceOrient,       SIGNAL(activated(int)),
                   this,                       SLOT(onComboOrientIndexChanged(int)) );
  QObject::connect(_ui.spinBox,                SIGNAL(editingFinished()),
                     this,                     SLOT(onSpinBoxEditingFinished()) );
}

void
WidgetPresentationParameters::onComboCompoIndexChanged(int idx)
{
  if (!_blockSig) emit comboCompoIndexChanged(idx);
}

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
WidgetPresentationParameters::onSpinBoxEditingFinished()
{
  if (!_blockSig) emit spinBoxValueChanged(_ui.spinBox->value());
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

  if (idx >= 0)
  {
    _ui.comboBoxScalarBarRange->setCurrentIndex(idx);
  }
  else
  {
    STDLOG("Strange!! No matching found - unable to set scalar bar range in GUI.");
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


MEDCALC::ScalarBarRangeType
WidgetPresentationParameters::getScalarBarRange() const
{
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
