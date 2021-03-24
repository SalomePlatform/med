// Copyright (C) 2015-2021  CEA/DEN, EDF R&D
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

#ifndef _DlgChangeUnderlyingMesh_HXX
#define _DlgChangeUnderlyingMesh_HXX

#include "ui_DlgChangeUnderlyingMesh.h"
#include "GenericDialog.hxx"

#include <SALOME_AppStudyEditor.hxx>
#include "MEDCALCGUIdialogs.hxx"

class MEDCALCGUI_DIALOGS_EXPORT DlgChangeUnderlyingMesh : public GenericDialog
{
  Q_OBJECT

public:
  DlgChangeUnderlyingMesh(SALOME_AppStudyEditor * studyEditor, QDialog *parent = 0);

  void setFieldId(int fieldId);
  int getFieldId();
  int getMeshId();

signals:
  void inputValidated();

protected slots:
  void OnSelectMesh();

 private:
  void accept();
  int _meshId;

  SALOME_AppStudyEditor * _studyEditor;
  int _fieldId;

  Ui_DlgChangeUnderlyingMesh ui; // instance of the class defined in ui_DlgChangeUnderlyingMesh.hxx
};


#endif // _DlgChangeUnderlyingMesh_HXX
