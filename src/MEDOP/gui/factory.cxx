// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
 * configuration below is MEDOPModule.
 */

#include "MEDOPModule.hxx"
extern "C" {
  CAM_Module* createModule()
  {
    return new MEDOPModule();
  }
}
