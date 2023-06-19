// Copyright (C) 2011-2023  CEA, EDF
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

#ifndef SRC_MEDCALC_CMP_MEDPYLOCKWRAPPER_HXX_
#define SRC_MEDCALC_CMP_MEDPYLOCKWRAPPER_HXX_

#include <Python.h>

#ifdef _DEBUG_
  #include <iostream>
#endif

/**
 * \class MEDPyLockWrapper
 * \brief Python GIL wrapper. Copy of GUI's PyLockWrapper.
 *
 * Utility class wrapping the Python GIL acquisition. This makes use of the high level
 * API (PyGILState_Ensure and PyGILState_Release), and is hence compatible with only
 * one running Python interpreter (no call to Py_NewInterpreter()).
 * When the class is instantiated the lock is acquired. It is released at destruction time.
 * Copy construction (and hence assignation) is forbidden.
 */
class MEDPyLockWrapper
{
public:
  /**
   * \brief Constructor. Automatically acquires GIL.
   */
  MEDPyLockWrapper()
  {
    _gil_state = PyGILState_Ensure();
    // Save current thread state for later comparison
    _state = PyGILState_GetThisThreadState();
  }

  /**
   * \brief Destructor. Automatically releases GIL.
   */
  ~MEDPyLockWrapper()
  {
#ifdef _DEBUG_
    if (PyGILState_GetThisThreadState() != _state)
    {
      std::cout << "!!!!!!!!! MEDPyLockWrapper inconsistency - now entering infinite loop for debugging\n";
      while(1);
    }
#endif
    PyGILState_Release(_gil_state);
  }

private:
  PyGILState_STATE _gil_state;
  PyThreadState* _state;

  // "Rule of 3" - Forbid usage of copy operator and copy-constructor
  MEDPyLockWrapper(const MEDPyLockWrapper & another);
  const MEDPyLockWrapper & operator=(const MEDPyLockWrapper & another);
};
#endif /* SRC_MEDCALC_CMP_MEDPYLOCKWRAPPER_HXX_ */
