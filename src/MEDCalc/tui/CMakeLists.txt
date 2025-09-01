# Copyright (C) 2012-2025  CEA, EDF
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

SET(PYFILES_TO_INSTALL
  __init__.py
  fieldproxy.py
  medconsole.py
  medcorba.py
  medevents.py
  medimages.py
  medio.py
  medpresentation.py
  medprocessing.py
  medtest.py
  )

SALOME_INSTALL_SCRIPTS("${PYFILES_TO_INSTALL}" ${SALOME_FIELDS_INSTALL_PYTHON}/medcalc)

SET(BINFILES_TO_INSTALL
  fields_test.py
)

SALOME_INSTALL_SCRIPTS("${BINFILES_TO_INSTALL}" ${SALOME_INSTALL_BINS})
