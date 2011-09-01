// Copyright (C) 2007-2011  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
// File      : SauvWriter.hxx
// Created   : Wed Aug 24 11:18:49 2011
// Author    : Edward AGAPOV (eap)

#ifndef __SauvWriter_HXX__
#define __SauvWriter_HXX__

#include "MEDLoaderDefines.hxx"
#include "MEDCouplingRefCountObject.hxx"
#include "NormalizedUnstructuredMesh.hxx"
#include "SauvUtilities.hxx"
#include "MEDCouplingAutoRefCountObjectPtr.hxx"

#include <vector>
#include <string>
#include <map>

namespace ParaMEDMEM
{
  class MEDFileData;
  class MEDFileMesh;
  class MEDFileFieldMultiTS;
  class DataArrayInt;

  /*!
   * \brief Class to write a MEDFileData into a SAUVE format file
   */
  class MEDLOADER_EXPORT SauvWriter : public ParaMEDMEM::RefCountObject
  {
  public:
    static SauvWriter * New();
    void setMEDFileDS(const MEDFileData* medData, unsigned meshIndex = 0);
    void write(const char* fileName);

  private:

    /*!
     * \brief Class representing a GIBI sub-mesh (described in the pile 1 of the SAUVE file).
     * It stands for a named med sub-mesh (family, etc) and contains either cell IDs or other sub-meshes. 
     */
    struct SubMesh
    {
      std::vector<int>      _cellIDsByType[ INTERP_KERNEL::NORM_MAXTYPE+1 ];
      std::vector<SubMesh*> _subs;
      std::string           _name;
      int                   _id;
      int                   _nbSauvObjects;
      int                   _dimRelExt;
      int nbTypes() const;
      static int cellIDsByTypeSize() { return INTERP_KERNEL::NORM_MAXTYPE+1; }
    };
    SubMesh* addSubMesh(const std::string& name, int dimRelExt);

    void fillSubMeshes(int& nbSauvObjects, std::map<std::string,int>& nameNbMap);
    void fillFamilySubMeshes();
    void fillGroupSubMeshes();
    void fillProfileSubMeshes();
    int evaluateNbProfileSubMeshes() const;
    void makeCompNames(const std::string&                  fieldName,
                       const std::vector<std::string>&     compInfo,
                       std::map<std::string, std::string>& compMedToGibi );
    void makeProfileIDs( SubMesh*                          sm,
                         INTERP_KERNEL::NormalizedCellType type,
                         const DataArrayInt*               profile );
    void writeFileHead();
    void writeSubMeshes();
    void writeCompoundSubMesh(int iSub);
    void writeNodes();
    void writeLongNames();
    void writeNodalFields(std::map<std::string,int>& fldNamePrefixMap);
    void writeElemFields(std::map<std::string,int>& fldNamePrefixMap);
    void writeFieldNames(const bool isNodal, std::map<std::string,int>& fldNamePrefixMap);
    void writeElemTimeStamp(int iF, int iter, int order);
    void writeLastRecord();
    void writeNames( const std::map<std::string,int>& nameNbMap );

  private:

    MEDCouplingAutoRefCountObjectPtr< MEDFileMesh >                        _fileMesh;
    std::vector< MEDCouplingAutoRefCountObjectPtr< MEDFileFieldMultiTS > > _nodeFields;
    std::vector< MEDCouplingAutoRefCountObjectPtr< MEDFileFieldMultiTS > > _cellFields;

    std::vector<SubMesh>                      _subs;
    std::map< int, SubMesh* >                 _famIDs2Sub;
    std::map< std::string, SubMesh* >         _profile2Sub;
    enum
      {
        LN_MAIL=0, LN_CHAM, LN_COMP, LN_NB
      };
    std::vector<SauvUtilities::nameGIBItoMED> _longNames[ LN_NB ];

    std::fstream*                             _sauvFile;
  };
}

#endif
