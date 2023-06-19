// Copyright (C) 2007-2023  CEA, EDF
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

#ifndef _XMEDDATAMODEL_H_
#define _XMEDDATAMODEL_H_

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDDataManager)

#include "MEDCALCGUI.hxx"

//
// =================================================================
// Definition of an atom in the data model as an implementation of
// the virtual class DataObject
// =================================================================
//
#include "DataObject.hxx"
class MEDCALCGUI_EXPORT XmedDataObject: public DataObject {
public:
  XmedDataObject();
  virtual std::string getPath();

  void setFieldHandler(MEDCALC::FieldHandler fieldHandler);
  MEDCALC::FieldHandler * getFieldHandler();

  void setPresentationId(int presId);
  int getPresentationId() const;

private:
  MEDCALC::FieldHandler _fieldHandler;
  int _presId;
  std::string _getTypedFieldLabel();
};

//
// =================================================================
// Definition of the data model as an implementation of the virtual
// class DataModel. It implements the DataObject factory.
// =================================================================
//
#include "DataModel.hxx"
class MEDCALCGUI_EXPORT XmedDataModel: public DataModel {
public:
  XmedDataModel():DataModel() {};
  virtual DataObject * newDataObject();
};

//
// =================================================================
// Base class for data processing
// =================================================================
// Howto use it?
//
// 1) Create an instance, call the "process" function,
// then get the result you are interested in, using the corresponding
// "result*" function.
//
// 2) Alternatively, you can just call directly the suitable master
// "get*" function to process and get the result you are interested
// in.
//
#include "DataProcessor.hxx"

class MEDCALCGUI_EXPORT XmedDataProcessor : public DataProcessor {
public:
  XmedDataProcessor(DataModel * dataModel):DataProcessor(dataModel) {};
  virtual ~XmedDataProcessor() {};

  MEDCALC::FieldHandlerList * getResultingFieldHandlerList();
  MEDCALC::FieldIdList * getResultingFieldIdList();

protected:
  virtual void preprocess(QStringList itemNameIdList);
  virtual void processDataObject(DataObject * dataObject);

private:
  MEDCALC::FieldHandlerList_var _resultingFieldHandlerList;
  MEDCALC::FieldIdList_var _resultingFieldIdList;
  int _objectIndex;

};


#endif // _XMEDDATAMODEL_H_
