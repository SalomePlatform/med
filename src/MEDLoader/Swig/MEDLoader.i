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

%module MEDLoader

#define MEDCOUPLING_EXPORT
#define MEDLOADER_EXPORT

%include "MEDCoupling.i"

%{
#include "MEDLoader.hxx"
#include "MEDFileMesh.hxx"
#include "MEDLoaderTypemaps.i"

using namespace ParaMEDMEM;
%}

#if SWIG_VERSION >= 0x010329
%template()  std::vector<std::string>;
#endif

%typemap(out) ParaMEDMEM::MEDFileMesh*
{
  $result=convertMEDFileMesh($1,$owner);
}

%newobject MEDLoader::ReadUMeshFromFamilies;
%newobject MEDLoader::ReadUMeshFromGroups;
%newobject MEDLoader::ReadUMeshFromFile;
%newobject MEDLoader::ReadField;
%newobject MEDLoader::ReadFieldCell;
%newobject MEDLoader::ReadFieldNode;
%newobject MEDLoader::ReadFieldGauss;
%newobject MEDLoader::ReadFieldGaussNE;
%newobject ParaMEDMEM::MEDFileUMesh::New;
%newobject ParaMEDMEM::MEDFileUMesh::getCoords;
%newobject ParaMEDMEM::MEDFileUMesh::getGroup;
%newobject ParaMEDMEM::MEDFileUMesh::getGroups;
%newobject ParaMEDMEM::MEDFileUMesh::getFamily;
%newobject ParaMEDMEM::MEDFileUMesh::getFamilies;
%newobject ParaMEDMEM::MEDFileUMesh::getGroupArr;
%newobject ParaMEDMEM::MEDFileUMesh::getGroupsArr;
%newobject ParaMEDMEM::MEDFileUMesh::getFamilyArr;
%newobject ParaMEDMEM::MEDFileUMesh::getFamiliesArr;
%newobject ParaMEDMEM::MEDFileUMesh::getNodeGroupArr;
%newobject ParaMEDMEM::MEDFileUMesh::getNodeGroupsArr;
%newobject ParaMEDMEM::MEDFileUMesh::getNodeFamilyArr;
%newobject ParaMEDMEM::MEDFileUMesh::getNodeFamiliesArr;
%newobject ParaMEDMEM::MEDFileUMesh::getMeshAtRank;
%newobject ParaMEDMEM::MEDFileUMesh::getRank0Mesh;
%newobject ParaMEDMEM::MEDFileUMesh::getRankM1Mesh;
%newobject ParaMEDMEM::MEDFileUMesh::getRankM2Mesh;
%newobject ParaMEDMEM::MEDFileUMesh::getRankM3Mesh;

class MEDLoader
{
public:
  static void setEpsilonForNodeComp(double val) throw(INTERP_KERNEL::Exception);
  static void setCompPolicyForCell(int val) throw(INTERP_KERNEL::Exception);
  static void setTooLongStrPolicy(int val) throw(INTERP_KERNEL::Exception);
  static void CheckFileForRead(const char *fileName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshNames(const char *fileName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshNamesOnField(const char *fileName, const char *fieldName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshGroupsNames(const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshFamiliesNames(const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshFamiliesNamesOnGroup(const char *fileName, const char *meshName, const char *grpName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshGroupsNamesOnFamily(const char *fileName, const char *meshName, const char *famName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetAllFieldNamesOnMesh(const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetAllFieldNames(const char *fileName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetFieldNamesOnMesh(ParaMEDMEM::TypeOfField type, const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetCellFieldNamesOnMesh(const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetNodeFieldNamesOnMesh(const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static double GetTimeAttachedOnFieldIteration(const char *fileName, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  %extend
     {
       static PyObject *GetFieldIterations(ParaMEDMEM::TypeOfField type, const char *fileName, const char *meshName, const char *fieldName) throw(INTERP_KERNEL::Exception)
       {
         std::vector< std::pair<int,int> > res=MEDLoader::GetFieldIterations(type,fileName,meshName,fieldName);
         PyObject *ret=PyList_New(res.size());
         int rk=0;
         for(std::vector< std::pair<int,int> >::const_iterator iter=res.begin();iter!=res.end();iter++,rk++)
           {
             PyObject *elt=PyTuple_New(2);
             PyTuple_SetItem(elt,0,SWIG_From_int((*iter).first));
             PyTuple_SetItem(elt,1,SWIG_From_int((*iter).second));
             PyList_SetItem(ret,rk,elt);
           }
         return ret;
       }

       static PyObject *GetAllFieldIterations(const char *fileName, const char *meshName, const char *fieldName) throw(INTERP_KERNEL::Exception)
       {
         std::vector< std::pair< std::pair<int,int>, double> > res=MEDLoader::GetAllFieldIterations(fileName,meshName,fieldName);
         PyObject *ret=PyList_New(res.size());
         int rk=0;
         for(std::vector< std::pair< std::pair<int,int>, double> >::const_iterator iter=res.begin();iter!=res.end();iter++,rk++)
           {
             PyObject *elt=PyTuple_New(3);
             PyTuple_SetItem(elt,0,SWIG_From_int((*iter).first.first));
             PyTuple_SetItem(elt,1,SWIG_From_int((*iter).first.second));
             PyTuple_SetItem(elt,2,SWIG_From_double((*iter).second));
             PyList_SetItem(ret,rk,elt);
           }
         return ret;
       }

       static PyObject *GetCellFieldIterations(const char *fileName, const char *meshName, const char *fieldName) throw(INTERP_KERNEL::Exception)
       {
         std::vector< std::pair<int,int> > res=MEDLoader::GetCellFieldIterations(fileName,meshName,fieldName);
         PyObject *ret=PyList_New(res.size());
         int rk=0;
         for(std::vector< std::pair<int,int> >::const_iterator iter=res.begin();iter!=res.end();iter++,rk++)
           {
             PyObject *elt=PyTuple_New(2);
             PyTuple_SetItem(elt,0,SWIG_From_int((*iter).first));
             PyTuple_SetItem(elt,1,SWIG_From_int((*iter).second));
             PyList_SetItem(ret,rk,elt);
           }
         return ret;
       }
       static PyObject *GetNodeFieldIterations(const char *fileName, const char *meshName, const char *fieldName) throw(INTERP_KERNEL::Exception)
       {
         std::vector< std::pair<int,int> > res=MEDLoader::GetNodeFieldIterations(fileName,meshName,fieldName);
         PyObject *ret=PyList_New(res.size());
         int rk=0;
         for(std::vector< std::pair<int,int> >::const_iterator iter=res.begin();iter!=res.end();iter++,rk++)
           {
             PyObject *elt=PyTuple_New(2);
             PyTuple_SetItem(elt,0,SWIG_From_int((*iter).first));
             PyTuple_SetItem(elt,1,SWIG_From_int((*iter).second));
             PyList_SetItem(ret,rk,elt);
           }
         return ret;
       }
       static PyObject *ReadFieldsOnSameMesh(ParaMEDMEM::TypeOfField type, const char *fileName, const char *meshName, int meshDimRelToMax,
                                             const char *fieldName, PyObject *liIts) throw(INTERP_KERNEL::Exception)
       {
         std::vector<std::pair<int,int> > its=convertTimePairIdsFromPy(liIts);
         std::vector<ParaMEDMEM::MEDCouplingFieldDouble *> res=MEDLoader::ReadFieldsOnSameMesh(type,fileName,meshName,meshDimRelToMax,fieldName,its);
         return convertFieldDoubleVecToPy(res);
       }
       static void WriteUMeshesPartition(const char *fileName, const char *meshName, PyObject *li, bool writeFromScratch) throw(INTERP_KERNEL::Exception)
       {
         std::vector<ParaMEDMEM::MEDCouplingUMesh *> v=convertFieldDoubleVecFromPy(li);
         MEDLoader::WriteUMeshesPartition(fileName,meshName,v,writeFromScratch);
       }
       static void WriteUMeshesPartitionDep(const char *fileName, const char *meshName, PyObject *li, bool writeFromScratch) throw(INTERP_KERNEL::Exception)
       {
         std::vector<ParaMEDMEM::MEDCouplingUMesh *> v=convertFieldDoubleVecFromPy(li);
         MEDLoader::WriteUMeshesPartitionDep(fileName,meshName,v,writeFromScratch);
       }
       static void WriteUMeshes(const char *fileName, PyObject *li, bool writeFromScratch) throw(INTERP_KERNEL::Exception)
       {
         std::vector<ParaMEDMEM::MEDCouplingUMesh *> v=convertFieldDoubleVecFromPy(li);
         std::vector<const ParaMEDMEM::MEDCouplingUMesh *> v2(v.begin(),v.end());
         MEDLoader::WriteUMeshes(fileName,v2,writeFromScratch);
       }
       static PyObject *GetTypesOfField(const char *fileName, const char *fieldName, const char *meshName) throw(INTERP_KERNEL::Exception)
       {
         std::vector< ParaMEDMEM::TypeOfField > v=MEDLoader::GetTypesOfField(fileName,fieldName,meshName);
         int size=v.size();
         PyObject *ret=PyList_New(size);
         for(int i=0;i<size;i++)
           PyList_SetItem(ret,i,PyInt_FromLong((int)v[i]));
         return ret;
       }
       static ParaMEDMEM::MEDCouplingUMesh *ReadUMeshFromGroups(const char *fileName, const char *meshName, int meshDimRelToMax, PyObject *li) throw(INTERP_KERNEL::Exception)
       {
         std::vector<std::string> grps;
         converPyListToVecString(li,grps);
         return MEDLoader::ReadUMeshFromGroups(fileName,meshName,meshDimRelToMax,grps);
       }
       static ParaMEDMEM::MEDCouplingUMesh *ReadUMeshFromFamilies(const char *fileName, const char *meshName, int meshDimRelToMax, PyObject *li) throw(INTERP_KERNEL::Exception)
       {
         std::vector<std::string> fams;
         converPyListToVecString(li,fams);
         return MEDLoader::ReadUMeshFromFamilies(fileName,meshName,meshDimRelToMax,fams);
       }
     }
  static ParaMEDMEM::MEDCouplingUMesh *ReadUMeshFromFile(const char *fileName, const char *meshName, int meshDimRelToMax=0) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingUMesh *ReadUMeshFromFile(const char *fileName, int meshDimRelToMax=0) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingFieldDouble *ReadField(ParaMEDMEM::TypeOfField type, const char *fileName, const char *meshName, int meshDimRelToMax, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingFieldDouble *ReadFieldCell(const char *fileName, const char *meshName, int meshDimRelToMax, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingFieldDouble *ReadFieldNode(const char *fileName, const char *meshName, int meshDimRelToMax, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingFieldDouble *ReadFieldGauss(const char *fileName, const char *meshName, int meshDimRelToMax, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingFieldDouble *ReadFieldGaussNE(const char *fileName, const char *meshName, int meshDimRelToMax, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  static void WriteUMesh(const char *fileName, const ParaMEDMEM::MEDCouplingUMesh *mesh, bool writeFromScratch) throw(INTERP_KERNEL::Exception);
  static void WriteUMeshDep(const char *fileName, const ParaMEDMEM::MEDCouplingUMesh *mesh, bool writeFromScratch) throw(INTERP_KERNEL::Exception);
  static void WriteField(const char *fileName, const ParaMEDMEM::MEDCouplingFieldDouble *f, bool writeFromScratch) throw(INTERP_KERNEL::Exception);
  static void WriteFieldDep(const char *fileName, const ParaMEDMEM::MEDCouplingFieldDouble *f, bool writeFromScratch) throw(INTERP_KERNEL::Exception);
  static void WriteFieldUsingAlreadyWrittenMesh(const char *fileName, const ParaMEDMEM::MEDCouplingFieldDouble *f) throw(INTERP_KERNEL::Exception);
};

%include "MEDFileMesh.hxx"
