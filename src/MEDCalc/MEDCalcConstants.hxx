// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
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

// This constant strings are used to specify an attribute name for
// AttributeParameter attributes associated to a SObject
#define IS_IN_WORKSPACE  "isInWorkspace"
#define SOURCE_ID        "sourceId"
#define MESH_ID          "meshId"
#define FIELD_SERIES_ID  "fieldSeriesId"
#define FIELD_ID         "fieldId"
#define PRESENTATION_ID  "presentationId"
#define IS_PRESENTATION  "isPresentation"
#define PRESENTATION_TYPE  "presentationType"

static const int NB_TYPE_OF_FIELDS = 4;
static const char* mapTypeOfFieldLabel[NB_TYPE_OF_FIELDS] =
  {"ON_CELLS", "ON_NODES", "ON_GAUSS_PT", "ON_GAUSS_NE" };

