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

#ifndef _DlgInterpolateField_HXX
#define _DlgInterpolateField_HXX

#include "ui_DlgInterpolateField.h"
#include "GenericDialog.hxx"

#include <SALOME_AppStudyEditor.hxx>
#include "MEDCALCGUIdialogs.hxx"

class MEDCALCGUI_DIALOGS_EXPORT DlgInterpolateField : public GenericDialog
{
  Q_OBJECT

public:
  DlgInterpolateField(SALOME_AppStudyEditor * studyEditor, QDialog *parent = 0);

  void setFieldId(int fieldId);
  int getFieldId();
  int getMeshId();

  double getPrecision();
  double getDefaultValue();
  bool getReverse();
  std::string getIntersectionType();
  std::string getMethod();
  std::string getFieldNature();

signals:
  void inputValidated();

protected slots:
  void OnSelectMesh();

 private:
  void accept();
  int _meshId;

  SALOME_AppStudyEditor * _studyEditor;
  int _fieldId;

  Ui_DlgInterpolateField ui; // instance of the class defined in ui_dlgInterpolateField.hxx
};


#endif // _DlgInterpolateField_HXX
