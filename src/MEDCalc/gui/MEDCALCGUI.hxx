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

#ifndef MEDCALCGUI_HXX_
#define MEDCALCGUI_HXX_

#ifdef WIN32
# if defined MEDCALCGUI_EXPORTS
#  define MEDCALCGUI_EXPORT __declspec( dllexport )
# else
#  define MEDCALCGUI_EXPORT __declspec( dllimport )
# endif
#else
# define MEDCALCGUI_EXPORT
#endif

#endif //MEDCALCGUI_HXX_
