// Copyright (C) 2007-2021  CEA/DEN, EDF R&D
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

#include "SPythonInterpreter.hxx"

#include <Python.h>

#include <iostream>
#include <fstream>
#include <float.h>
#include <limits>

static const int MAX_LINE=1024;

void runInFileAndQuit(PyObject *glob, PyObject *loc, const char *fileName)
{
  std::ifstream ifs(fileName);
  ifs.exceptions( std::ifstream::badbit );
  char *line=new char[MAX_LINE+1];
  MEDCoupling::SPythonInterpreter interp(glob,loc);
  bool isspython;
  while(!ifs.eof())
    {
      ifs.getline(line,MAX_LINE);
      interp.run(line,isspython);
    }
  interp.finishSession();
  delete [] line;
}

void runInInteractiveMode(PyObject *glob, PyObject *loc)
{
  std::string s;
  char *line=new char[MAX_LINE+1];
  MEDCoupling::SPythonInterpreter interp(glob,loc);
  bool isInterp=true;
  while(true)
    {
      if(isInterp)
        std::cout << ">>> ";
      else
        std::cout << "... ";
      std::cin.getline(line,MAX_LINE);
      bool isspython;
      isInterp=interp.run(line,isspython);
      if(isspython)
        std::cout << "Spython line detected !" << std::endl;
    }
    delete [] line;
}

int main(int argc, char *argv[])
{
  if(!Py_IsInitialized())
    {
      Py_InitializeEx(0); // do not install signal handlers
      PyEval_InitThreads(); /* Create (and acquire) the interpreter lock (for threads)*/
      PyEval_SaveThread(); /* Release the thread state */
      //here we do not have the Global Interpreter Lock
    }
  PyObject *mainmod,*res ;
  PyObject *globals;
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure(); // acquire the Global Interpreter Lock
  
  mainmod = PyImport_AddModule("__main__");
  globals = PyModule_GetDict(mainmod);
  if (PyDict_GetItemString(globals, "__builtins__") == NULL) 
    {
      PyObject *bimod = PyImport_ImportModule("__builtin__");
      if (bimod == NULL || PyDict_SetItemString(globals, "__builtins__", bimod) != 0)
        Py_FatalError("can't add __builtins__ to __main__");
      Py_DECREF(bimod);
    }
  res=PyRun_String("import sys",Py_single_input,globals,globals);
  res=PyRun_String("from MEDCalculator import *",Py_single_input,globals,globals);
  if(!res)
    PyErr_Print();
  res=PyRun_String("sys.argv=['']",Py_single_input,globals,globals);
  switch(argc)
    {
    case 2:
      runInFileAndQuit(globals,globals,argv[1]);
      break;
    case 1:
      runInInteractiveMode(globals,globals);
      break;
    default:
      std::cerr << "Invalid usage of spython !" << std::endl;
      return 1;
    }
  /*PyObject *_bltins = PyEval_GetBuiltins();
    res=PyRun_String("def tony(a):\n"
    "\tprint \"coucou\"\n"
    "\treturn (7*a,)"
    ,
    Py_single_input,globals,globals );
    PyObject* pyfunc=PyDict_GetItemString(globals,"Augustin");
    PyObject* args = PyTuple_New(1) ;
    PyObject* arg0=PyFloat_FromDouble(6.7);
    PyTuple_SetItem(args,0,arg0);
    //PyObject_Print(args,stderr,Py_PRINT_RAW);
    PyObject* result = PyObject_CallObject( pyfunc , args ) ;
    int nres=PyTuple_Size(result);
    PyObject* resRk0=PyTuple_GetItem(result,0);
    double resCpp=PyFloat_AS_DOUBLE(resRk0);*/
  Py_DECREF(globals);
  return 0;
}
