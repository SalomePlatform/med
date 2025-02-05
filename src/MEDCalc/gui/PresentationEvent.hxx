// Copyright (C) 2016-2025  CEA, EDF
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

#ifndef SRC_MEDCALC_GUI_PRESENTATIONEVENT_HXX_
#define SRC_MEDCALC_GUI_PRESENTATIONEVENT_HXX_

#include <string>

typedef struct {
  enum EventType {
    EVENT_VIEW_OBJECT_MESH_VIEW,
    EVENT_VIEW_OBJECT_CONTOUR,
    EVENT_VIEW_OBJECT_DEFLECTION_SHAPE,
    EVENT_VIEW_OBJECT_POINT_SPRITE,
    EVENT_VIEW_OBJECT_PLOT3D,
    EVENT_VIEW_OBJECT_STREAM_LINES,
    EVENT_VIEW_OBJECT_CUT_SEGMENT,
    EVENT_VIEW_OBJECT_SCALAR_MAP,
    EVENT_VIEW_OBJECT_SLICES,
    EVENT_VIEW_OBJECT_VECTOR_FIELD,

    EVENT_DELETE_PRESENTATION,

    EVENT_CHANGE_COMPONENT,
    EVENT_CHANGE_COLORMAP,
    EVENT_CHANGE_TIME_RANGE,

    EVENT_CHANGE_MESH_MODE,

    EVENT_CHANGE_NB_CONTOUR,

    EVENT_CHANGE_SLICE_ORIENTATION,
    EVENT_CHANGE_NB_SLICES,

    EVENT_CHANGE_NORMAL,
    EVENT_CHANGE_PLANE_POS,

    EVENT_CHANGE_INTEGR_DIR,

    EVENT_CHANGE_CUT_POINT1,
    EVENT_CHANGE_CUT_POINT2,

    EVENT_CHANGE_CONTOUR_COMPONENT,

    EVENT_CHANGE_SCALE_FACTOR,
    EVENT_CHANGE_CUSTOM_SCALE_FACTOR,

    EVENT_DISPLAY_PRESENTATION,
    EVENT_ERASE_PRESENTATION,

    EVENT_SCALAR_BAR_VISIBILITY_CHANGED,

    EVENT_CUSTOM_RANGE_CHANGED,

    EVENT_HIDE_DATA_OUTSIDE_CUSTOM_RANGE_CHANGED
  };

  EventType eventtype;
  MEDCALC::FieldHandler * fieldHandler;
  MEDCALC::MeshHandler * meshHandler;
  int presentationId;
  std::string presentationType; // presentation type (can be empty)
  int anInteger;                // multi-purpose variable
  //int anInteger2;             // multi-purpose variable
  std::string aString;          // multi-purpose variable
  double aDouble1;             // multi-purpose variable 
  double aDouble2;             // multi-purpose variable
  double aDouble3;
  double aDoubleN[3];          // multi-purpose variable
  double aDoubleP1[3];         // multi-purpose variable
  double aDoubleP2[3];         // multi-purpose variable
} PresentationEvent;

#endif
