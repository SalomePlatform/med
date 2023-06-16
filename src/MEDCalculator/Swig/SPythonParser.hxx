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

#ifndef __SPYTHONPARSER_HXX__
#define __SPYTHONPARSER_HXX__

// rnv: avoid compilation warning on Linux : "_POSIX_C_SOURCE" and "_XOPEN_SOURCE" are redefined
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif 

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif 

#include "Python.h"

#include "MedCalculatorSPythonDefines.hxx"

#include <string>
#include <vector>

namespace MEDCoupling
{
  typedef enum
    {
      EMPTY_TYPE=0,
      FLOAT_TYPE=2,
      INT_TYPE=3,
      FIELDDB_TYPE=4,
      FUNC_TYPE=5,
      UNKNOWN_TYPE=6,
      IDENTITY_TYPE=7
    } TypeOfEntity;

  class SPythonParser;

  class MEDCALCULATORSPYTHON_EXPORT SPythonPredParser
  {
  public:
    SPythonPredParser();
    TypeOfEntity getType() const { return _type; }
    void assign(const std::string& s, PyObject *glob, PyObject *loc);
    std::string getRepr() const;
    bool empty() const;
    static TypeOfEntity getTypeOfVar(const std::string& var, PyObject *glob, PyObject *loc);
  public:
    static const char FIELD_TYPE_STR[];
  private:
    std::string _var;
    std::string _method;
    TypeOfEntity _type;
  };

  /*!
   * This class works on strings previously stripped.
   */
  class MEDCALCULATORSPYTHON_EXPORT SPythonParser
  {
  public:
    SPythonParser();
    TypeOfEntity getType() const { return _type; }
    bool parseWithoutEqual(const std::string& s, int parLev, PyObject *glob, PyObject *loc, std::vector<SPythonParser>& alreadyParsed);
    TypeOfEntity getTypeOfElt(const std::string& elt, PyObject *glob, PyObject *loc, const std::vector<SPythonParser>& alreadyParsed) const;
    TypeOfEntity combineType(TypeOfEntity t1, TypeOfEntity t2) const;
    std::string getRepr(const std::vector<SPythonParser>& v) const;
    std::string replaceByCompacted(const std::string& s, int parLev, int id) const;
    std::string substitute(const std::vector<SPythonParser>& v) const;
    TypeOfEntity analyzeType(PyObject *glob, PyObject *loc, const std::vector<SPythonParser>& alreadyParsed);
    void keepSelectedLevOfPar(const std::string& s, int parLev, PyObject *glob, PyObject *loc);
    static std::vector<int> levOfParenthesis(const std::string& s);
    //
    static void replaceFromCompacted(std::string& ret,const std::vector<SPythonParser>& v);
    static bool isAlreadyControledParenthesis(const std::string& s);
    static bool isParenthesisMatching(const std::string& s, std::string& res);
    static bool isElementInParenthesisMatching(const std::string& s, std::string& result, bool& isNumber);
  public:
    static const char NUMBERS[];
  private:
    TypeOfEntity _type;
    SPythonPredParser _pred;
    std::string _content;
    std::string _content_py;
  };

  /*!
   * The main aim of this class is to deal with operator = between spython parts and aggregate it.
   * HL means HL because parse method dispatch the core operation on SPythonParse class.
   */
  class MEDCALCULATORSPYTHON_EXPORT SPythonParserHL
  {
  public:
    SPythonParserHL(PyObject *glob, PyObject *loc);
    bool parse(const std::string& s, std::string& result);
    static std::vector<std::string> splitBetweenEqualChar(const std::string &s);
  private:
    bool parseWithoutEqual(const std::string& s, TypeOfEntity& type, std::string& result);
  private:
    PyObject *_glob;
    PyObject *_loc;
  };
}

#endif
