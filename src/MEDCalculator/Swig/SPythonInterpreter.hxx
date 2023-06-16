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
// Author : Anthony Geay (CEA/DEN)

#ifndef __SPYTHONINTERPRETER_HXX__
#define __SPYTHONINTERPRETER_HXX__

// rnv: avoid compilation warning on Linux : "_POSIX_C_SOURCE" and "_XOPEN_SOURCE" are redefined
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif 

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif 

#include <Python.h>

#include "MedCalculatorSPythonDefines.hxx"

#include <string>
#include <vector>
#include <set>

namespace MEDCoupling
{
  class MEDCALCULATORSPYTHON_EXPORT SPythonInterpreter
  {
  public:
    SPythonInterpreter(PyObject *globals, PyObject *locals);
    void initialize();
    bool run(const char *str, bool& isSPython);
    bool finishSession();
  private:
    bool checkIndentCoherency(const std::string& s, std::size_t p);
    void checkPythonInterp(PyObject *r);
    bool isIndenter(const std::string& s, std::size_t p);
    bool isSPythonExpression(const std::string& s);
    bool isSPythonExpressionLev1(const std::string& s, std::string& result);
  public:
    static std::string strip(const std::string& s);
    static bool isSpythonManipulationNeeded(const std::string& s, std::string& pythonLines);
    static bool isCandidateParenthesis(const std::string& s, std::size_t p1, std::size_t& n);
  public:
    static const int NB_OF_INDENT=7;
    static const char *INDENT_TOKEN[];
    static const char NUMBERS[];
  private:
    std::string _cmd;
    std::set<int> _indent_pos;
    bool _indent_must_change;
    PyObject *_glob;
    PyObject *_loc;
  };
}

#endif
