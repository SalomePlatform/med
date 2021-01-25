// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// File   : MEDCALCGUI_Displayer.h
// Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//
#ifndef MEDCALCGUI_DISPLAYER_H
#define MEDCALCGUI_DISPLAYER_H
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDPresentationManager)
#include <LightApp_Displayer.h>

#include "MEDCALCGUI.hxx"

class PresentationController;

class MEDCALCGUI_EXPORT MEDCALCGUI_Displayer : public LightApp_Displayer
{

public:
  /* Constructor */
  MEDCALCGUI_Displayer(PresentationController* presentationController);
  /* Destructor */
  virtual ~MEDCALCGUI_Displayer() override;
  virtual void Display(const QStringList&, const bool = true, SALOME_View* = 0) override;
  virtual void Erase(const QStringList&, const bool forced = false, const bool updateViewer = true, SALOME_View* = 0) override;
  virtual bool canBeDisplayed( const QString& /*entry*/, const QString& /*viewer_type*/ ) const override;
  virtual bool IsDisplayed(const QString& /*entry*/, SALOME_View* = 0) const override;

private:
  void changeVisibility(const QStringList& list, const bool visible);
  MEDCALC::PresentationVisibility visibilityState(const QString& entry) const;

private:
  PresentationController* _presentationController;
};

#endif // MEDCALCGUI_DISPLAYER_H

