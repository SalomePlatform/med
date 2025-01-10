// Copyright (C) 2015-2025  CEA, EDF
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

#include "DlgChangeUnderlyingMesh.hxx"
#include <MEDCalcConstants.hxx>

#include <SALOMEDS_SObject.hxx>
#include <SALOMEDS_Study.hxx>
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)

#include <QString>
#include <QMessageBox>

DlgChangeUnderlyingMesh::DlgChangeUnderlyingMesh(SALOME_AppStudyEditor * studyEditor,
             QDialog * parent)
  : GenericDialog(parent)
{
  ui.setupUi(this->getPanel());
  _meshId=-1;
  _studyEditor = studyEditor;
  connect(this->ui.btnSelectMesh, SIGNAL(clicked()), this, SLOT(OnSelectMesh()));
  this->setWindowTitle("Remplacement du maillage support");
}

/** This reset the dialog for a new selection */
void DlgChangeUnderlyingMesh::setFieldId(int fieldId) {
  _fieldId = fieldId;
  _meshId=-1;
  this->ui.txtMesh->setText(QString(""));
}
int DlgChangeUnderlyingMesh::getFieldId() {
  return _fieldId;
}

int DlgChangeUnderlyingMesh::getMeshId() {
  return _meshId;
}

void DlgChangeUnderlyingMesh::accept() {
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

void DlgChangeUnderlyingMesh::OnSelectMesh() {
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
