// Copyright (C) 2007-2023  CEA/DEN, EDF R&D
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

// Author : Guillaume Boulant (EDF)

/*!
 * IMPORTANT NOTE: this file implements the factory interface required
 * to specify the SALOME GUI part of a module.
 *
 * The GUI part of a module is an implementation of the generic class
 * CAM_Module and must be returned here by a factory function called
 * createModule().
 *
 * The function createModule can be designed to switch between several
 * configurations, but generally the configuration is statically
 * defined (at build step). The default configuration could be DefaultGui. The
 * configuration below is MEDModule.
 */

#include <FIELDS_version.h>
#include "MEDModule.hxx"
#include "MEDCALCGUI.hxx"

#ifdef __WITH_MEDMEMGUI__
#include "MEDGUI.h"
#endif

extern "C" {

  /*! This function is the factory for the GUI Module */
  MEDCALCGUI_EXPORT CAM_Module* createModule()
  {
    CAM_Module * medModule;

#ifdef __WITH_MEDMEMGUI__
    medModule = new MedGUI();
#else
    medModule = new MEDModule();
#endif

    return medModule;
  }

  /*! This function specifies the version of the module to be displayed in the about dialog box */
  MEDCALCGUI_EXPORT char* getModuleVersion() {
    return (char*)SALOMEFIELDS_VERSION_STR;
  }

}
