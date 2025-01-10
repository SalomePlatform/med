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

#include "MEDCommandsHistoryManager_i.hxx"

MEDCommandsHistoryManager_i* MEDCommandsHistoryManager_i::_instance = NULL;

MEDCommandsHistoryManager_i*
MEDCommandsHistoryManager_i::getInstance()
{
  if (_instance == NULL)
    _instance = new MEDCommandsHistoryManager_i();
  return _instance;
}

MEDCommandsHistoryManager_i::MEDCommandsHistoryManager_i()
  : _history(0)
{
  _history = new MEDCALC::CommandsList;
}

MEDCommandsHistoryManager_i::~MEDCommandsHistoryManager_i()
{
  for (CORBA::ULong i = 0; i < _history->length(); ++i) {
    CORBA::string_free((*_history)[i]);
  }
  delete _history;
}

void
MEDCommandsHistoryManager_i::addCommand(const char* command)
{
  CORBA::ULong size = _history->length();
  _history->length(size+1);
  (*_history)[size] = CORBA::string_dup(command);
}

MEDCALC::CommandsList*
MEDCommandsHistoryManager_i::getCommandsHistory()
{
  return _history;
}

void
MEDCommandsHistoryManager_i::clearHistory()
{
  _history->length(0);
}
