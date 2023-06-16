// Copyright (C) 2015-2023  CEA/DEN, EDF R&D
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

#include "DlgInterpolateField.hxx"
#include <MEDCalcConstants.hxx>

#include <SALOMEDS_SObject.hxx>
#include <SALOMEDS_Study.hxx>
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)

#include <QString>
#include <QMessageBox>
#include <QDoubleValidator>

DlgInterpolateField::DlgInterpolateField(SALOME_AppStudyEditor * studyEditor,
             QDialog * parent)
  : GenericDialog(parent)
{
  ui.setupUi(this->getPanel());
  _meshId=-1;
  _studyEditor = studyEditor;

  QDoubleValidator* precisionValidator = new QDoubleValidator(1e-15, 1e-1, 1, this);
  precisionValidator->setNotation(QDoubleValidator::ScientificNotation);
  this->ui.lineEditPrecision->setValidator(precisionValidator);
  this->ui.lineEditPrecision->setText("1e-12");

  QDoubleValidator* defaultValueValidator = new QDoubleValidator(this);
  this->ui.lineEditDefaultValue->setValidator(defaultValueValidator);
  this->ui.lineEditDefaultValue->setText("0");

  QStringList intersectionTypes;
  intersectionTypes << "Triangulation" << "Convex" << "Geometric2D" << "PointLocator" << "Barycentric" << "BarycentricGeo2D";
  this->ui.comboBoxIntersType->addItems(intersectionTypes);

  QStringList methods;
  methods << "P0P0" << "P0P1" << "P1P0" << "P1P1" << "P2P0";
  this->ui.comboBoxMethod->addItems(methods);

  QStringList natures;
  natures << "NoNature" << "IntensiveMaximum" << "ExtensiveMaximum" << "ExtensiveConservation" << "IntensiveConservation";
  this->ui.comboBoxNature->addItems(natures);

  connect(this->ui.btnSelectMesh, SIGNAL(clicked()), this, SLOT(OnSelectMesh()));
  this->setWindowTitle("Field interpolation");
  this->getPanel()->adjustSize();
  this->adjustSize();
}

/** This reset the dialog for a new selection */
void DlgInterpolateField::setFieldId(int fieldId) {
  _fieldId = fieldId;
  _meshId=-1;
  this->ui.txtMesh->setText(QString(""));
}
int DlgInterpolateField::getFieldId() {
  return _fieldId;
}

int DlgInterpolateField::getMeshId() {
  return _meshId;
}

double DlgInterpolateField::getPrecision() {
  return this->ui.lineEditPrecision->text().toDouble();
}

double DlgInterpolateField::getDefaultValue() {
  return this->ui.lineEditDefaultValue->text().toDouble();
}

bool DlgInterpolateField::getReverse() {
  return this->ui.checkBoxReverse->isChecked();
}

std::string DlgInterpolateField::getIntersectionType() {
  return this->ui.comboBoxIntersType->currentText().toStdString();
}

std::string DlgInterpolateField::getMethod() {
  return this->ui.comboBoxMethod->currentText().toStdString();
}

std::string DlgInterpolateField::getFieldNature() {
  return this->ui.comboBoxNature->currentText().toStdString();
}

void DlgInterpolateField::accept() {
  if ( _meshId == -1 ) {
    QMessageBox::warning(this,
       tr("Data verification"),
       tr("You must select a mesh in the explorer and clic the button Mesh"));
  }
  else {
    GenericDialog::accept();
    emit inputValidated();
  }
}

void DlgInterpolateField::OnSelectMesh() {
  SALOME_StudyEditor::SObjectList * listOfSObject = _studyEditor->getSelectedObjects();
  if ( listOfSObject->size() > 0 ) {
    SALOMEDS::SObject_var soMesh = listOfSObject->at(0);
    std::string name(_studyEditor->getName(soMesh));
    if (soMesh->_is_nil() || name == "MEDCalc")
      return;
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeParameter_var aParam;
    if ( soMesh->FindAttribute(anAttr,"AttributeParameter") ) {
      aParam = SALOMEDS::AttributeParameter::_narrow(anAttr);
      if (! aParam->IsSet(MESH_ID, PT_INTEGER))
        return;
    }
    _meshId = aParam->GetInt(MESH_ID);
    const char * meshname = _studyEditor->getName(soMesh);
    this->ui.txtMesh->setText(QString(meshname));
  }

}
