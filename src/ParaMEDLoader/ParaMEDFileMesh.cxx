// Copyright (C) 2007-2014  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (EDF R&D)

#include "ParaMEDFileMesh.hxx"
#include "MEDCouplingAutoRefCountObjectPtr.hxx"
#include "MEDFileMesh.hxx"
#include "MEDFileMeshLL.hxx"
#include "MEDLoader.hxx"

using namespace ParaMEDMEM;

MEDFileMesh *ParaMEDFileMesh::New(int iPart, int nbOfParts, const std::string& fileName, const std::string& mName, int dt, int it, MEDFileMeshReadSelector *mrs)
{
  MEDFileUtilities::CheckFileForRead(fileName);
  ParaMEDMEM::MEDCouplingMeshType meshType;
  MEDFileUtilities::AutoFid fid(MEDfileOpen(fileName.c_str(),MED_ACC_RDONLY));
  int dummy0,dummy1;
  std::string dummy2;
  MEDFileMeshL2::GetMeshIdFromName(fid,mName,meshType,dummy0,dummy1,dummy2);
  switch(meshType)
  {
    case UNSTRUCTURED:
      {
        return ParaMEDFileUMesh::New(iPart,nbOfParts,fileName,mName,dt,it,mrs);
      }
    default:
      throw INTERP_KERNEL::Exception("ParaMEDFileMesh::New : only unstructured mesh supported for the moment !");
  }
}

MEDFileMesh *ParaMEDFileMesh::ParaNew(int iPart, int nbOfParts, const MPI_Comm com, const MPI_Info nfo, const std::string& fileName, const std::string& mName, int dt, int it, MEDFileMeshReadSelector *mrs)
{
  MEDFileUtilities::CheckFileForRead(fileName);
  ParaMEDMEM::MEDCouplingMeshType meshType;
  MEDFileUtilities::AutoFid fid(MEDfileOpen(fileName.c_str(),MED_ACC_RDONLY));
  int dummy0,dummy1;
  std::string dummy2;
  MEDFileMeshL2::GetMeshIdFromName(fid,mName,meshType,dummy0,dummy1,dummy2);
  switch(meshType)
  {
    case UNSTRUCTURED:
      {
        return ParaMEDFileUMesh::ParaNew(iPart,nbOfParts,com,nfo,fileName,mName,dt,it,mrs);
      }
    default:
      throw INTERP_KERNEL::Exception("ParaMEDFileMesh::ParaNew : only unstructured mesh supported for the moment !");
  }
}

MEDFileUMesh *ParaMEDFileUMesh::New(int iPart, int nbOfParts, const std::string& fileName, const std::string& mName, int dt, int it, MEDFileMeshReadSelector *mrs)
{
  MEDCouplingAutoRefCountObjectPtr<MEDFileUMesh> ret;
  MEDFileUtilities::AutoFid fid(MEDfileOpen(fileName.c_str(),MED_ACC_RDONLY));
  //
  int meshDim, spaceDim, numberOfNodes;
  std::vector< std::vector< std::pair<INTERP_KERNEL::NormalizedCellType,int> > > typesDistrib(MEDLoader::GetUMeshGlobalInfo(fileName,mName,meshDim,spaceDim,numberOfNodes));
  std::vector<INTERP_KERNEL::NormalizedCellType> types;
  std::vector<int> distrib;
  for(std::vector< std::vector< std::pair<INTERP_KERNEL::NormalizedCellType,int> > >::const_iterator it0=typesDistrib.begin();it0!=typesDistrib.end();it0++)
    for(std::vector< std::pair<INTERP_KERNEL::NormalizedCellType,int> >::const_iterator it1=(*it0).begin();it1!=(*it0).end();it1++)
      {
        types.push_back((*it1).first);
        int tmp[3];
        DataArray::GetSlice(0,(*it1).second,1,iPart,nbOfParts,tmp[0],tmp[1]);
        tmp[2]=1;
        distrib.insert(distrib.end(),tmp,tmp+3);
      }
  ret=MEDFileUMesh::LoadPartOf(fileName,mName,types,distrib,dt,it,mrs);
  return ret.retn();
}

MEDFileUMesh *ParaMEDFileUMesh::ParaNew(int iPart, int nbOfParts, const MPI_Comm com, const MPI_Info nfo, const std::string& fileName, const std::string& mName, int dt, int it, MEDFileMeshReadSelector *mrs)
{
  MEDCouplingAutoRefCountObjectPtr<MEDFileUMesh> ret(MEDFileUMesh::New());
  MEDFileUtilities::AutoFid fid(MEDparFileOpen(fileName.c_str(),MED_ACC_RDONLY,com,nfo));
  //
  int meshDim, spaceDim, numberOfNodes;
  std::vector< std::vector< std::pair<INTERP_KERNEL::NormalizedCellType,int> > > typesDistrib(MEDLoader::GetUMeshGlobalInfo(fileName,mName,meshDim,spaceDim,numberOfNodes));
  return ret.retn();
}

MEDFileMeshes *ParaMEDFileMeshes::New(int iPart, int nbOfParts, const std::string& fileName)
{
  std::vector<std::string> ms(MEDLoader::GetMeshNames(fileName));
  MEDCouplingAutoRefCountObjectPtr<MEDFileMeshes> ret(MEDFileMeshes::New());
  for(std::vector<std::string>::const_iterator it=ms.begin();it!=ms.end();it++)
    {
      MEDCouplingAutoRefCountObjectPtr<MEDFileMesh> mesh(ParaMEDFileMesh::New(iPart,nbOfParts,fileName,(*it)));
      ret->pushMesh(mesh);
    }
  return ret.retn();
}

MEDFileMeshes *ParaMEDFileMeshes::ParaNew(int iPart, int nbOfParts, const MPI_Comm com, const MPI_Info nfo, const std::string& fileName)
{
  std::vector<std::string> ms(MEDLoader::GetMeshNames(fileName));
  MEDCouplingAutoRefCountObjectPtr<MEDFileMeshes> ret(MEDFileMeshes::New());
  for(std::vector<std::string>::const_iterator it=ms.begin();it!=ms.end();it++)
    {
      MEDCouplingAutoRefCountObjectPtr<MEDFileMesh> mesh(ParaMEDFileMesh::ParaNew(iPart,nbOfParts,com,nfo,fileName,(*it)));
      ret->pushMesh(mesh);
    }
  return ret.retn();
}