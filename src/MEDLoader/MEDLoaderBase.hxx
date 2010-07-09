//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef __MEDLOADERBASE_HXX__
#define __MEDLOADERBASE_HXX__

class MEDLoaderBase
{
public:
  static int getStatusOfFile(const char *fileName);
  static char *buildEmptyString(int lgth);
public:
  static const int EXIST_RW=0;
  static const int NOT_EXIST=1;
  static const int EXIST_RDONLY=2;
  static const int EXIST_WRONLY=3;
  static const int DIR_LOCKED=4;
};

#endif
