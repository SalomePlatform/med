// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_DecimationFilter.cxx
 *
 * \brief   see MULTIPR_DecimationFilter.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_DecimationFilter.hxx"
#include "MULTIPR_Field.hxx"
#include "MULTIPR_Mesh.hxx"
#include "MULTIPR_PointOfField.hxx"
#include "MULTIPR_DecimationAccel.hxx"
#include "MULTIPR_Exceptions.hxx"

#include <cmath>
#include <iostream>

using namespace std;


namespace multipr
{


//*****************************************************************************
// Class DecimationFilter implementation
//*****************************************************************************

// Factory used to build all filters from their name.
DecimationFilter* DecimationFilter::create(const char* pFilterName)
{
    if (pFilterName == NULL) throw NullArgumentException("filter name should not be NULL", __FILE__, __LINE__);
    
    if (strcmp(pFilterName, "Filtre_GradientMoyen") == 0)
    {
        return new DecimationFilterGradAvg();
    }
    else
    {
        throw IllegalArgumentException("unknown filter", __FILE__, __LINE__);
    }
}


//*****************************************************************************
// Class DecimationFilterGradAvg
//*****************************************************************************

DecimationFilterGradAvg::DecimationFilterGradAvg() 
{
    // do nothing
}


DecimationFilterGradAvg::~DecimationFilterGradAvg()  
{ 
    // do nothing
}


Mesh* DecimationFilterGradAvg::apply(Mesh* pMesh, const char* pArgv, const char* pNameNewMesh)
{
    //---------------------------------------------------------------------
    // Retrieve and check parameters
    //---------------------------------------------------------------------
    if (pMesh == NULL) throw NullArgumentException("pMesh should not be NULL", __FILE__, __LINE__);
    if (pArgv == NULL) throw NullArgumentException("pArgv should not be NULL", __FILE__, __LINE__);
    if (pNameNewMesh == NULL) throw NullArgumentException("pNameNewMesh should not be NULL", __FILE__, __LINE__);
    
    char   fieldName[MED_TAILLE_NOM + 1];
    int    fieldIt;
    double threshold;
    double radius;
    int    boxing; // number of cells along axis (if 100 then grid will have 100*100*100 = 10**6 cells)
    
    int ret = sscanf(pArgv, "%s %d %lf %lf %d",
        fieldName,
        &fieldIt,
        &threshold,
        &radius,
        &boxing);
    
    if (ret != 5) throw IllegalArgumentException("wrong number of arguments for filter GradAvg; expected 5 parameters", __FILE__, __LINE__);
    
    //---------------------------------------------------------------------
    // Retrieve field = for each point: get its coordinate and the value of the field
    //---------------------------------------------------------------------
    Field* field = pMesh->getFieldByName(fieldName);
    
    if (field == NULL) throw IllegalArgumentException("field not found", __FILE__, __LINE__);
    if ((fieldIt < 1) || (fieldIt > field->getNumberOfTimeSteps())) throw IllegalArgumentException("invalid field iteration", __FILE__, __LINE__);
    
    vector<PointOfField> points;
    pMesh->getAllPointsOfField(field, fieldIt, points);

    //---------------------------------------------------------------------
    // Creates acceleration structure used to compute gradient
    //---------------------------------------------------------------------
    DecimationAccel* accel = new DecimationAccelGrid();
    char strCfg[256]; // a string is used for genericity
    sprintf(strCfg, "%d %d %d", boxing, boxing, boxing);
    accel->configure(strCfg);
    accel->create(points);
    
    //---------------------------------------------------------------------
    // Collects elements of the mesh to be kept
    //---------------------------------------------------------------------
    set<med_int> elementsToKeep;
    
    int numElements = pMesh->getNumberOfElements();
    int numGaussPointsByElt = points.size() / numElements; // for a TETRA10, should be 5 for a field of elements and 10 for a field of nodes
    
    // for each element
    for (int itElt = 0 ; itElt < numElements ; itElt++)
    {
        bool keepElement = false;
        
        // for each Gauss point of the current element
        for (int itPtGauss = 0 ; itPtGauss < numGaussPointsByElt ; itPtGauss++)
        {
            const PointOfField& currentPt = points[itElt * numGaussPointsByElt + itPtGauss];
            
            vector<PointOfField> neighbours = accel->findNeighbours(
                currentPt.mXYZ[0], 
                currentPt.mXYZ[1], 
                currentPt.mXYZ[2], 
                radius);
            
            // if no neighbours => keep element
            if (neighbours.size() == 0)
            {
                keepElement = true;
                break;
            }
            
            // otherwise compute gradient...
            med_float normGrad = computeNormGrad(currentPt, neighbours);
            
            // debug
            //cout << (itElt * numGaussPointsByElt + j) << ": " << normGrad << endl;
            
            if ((normGrad >= threshold) || isnan(normGrad))
            {
                keepElement = true;
                break;
            }
        }
        
        if (keepElement)
        {
            // add index of the element to keep (index must start at 1)
            elementsToKeep.insert(med_int(itElt + 1));
        }
    }

    //---------------------------------------------------------------------
    // Cleans
    //---------------------------------------------------------------------
    delete accel;
    
    //---------------------------------------------------------------------
    // Create the final mesh by extracting elements to keep from the current mesh
    //---------------------------------------------------------------------
    Mesh* newMesh = pMesh->createFromSetOfElements(elementsToKeep, pNameNewMesh);
    
    return newMesh;
}


void DecimationFilterGradAvg::getGradientInfo(
        Mesh*       pMesh, 
        const char* pFieldName, 
        int         pFieldIt, 
        double      pRadius,
        int         pBoxing,
        double*     pOutGradMin,
        double*     pOutGradAvg,
        double*     pOutGradMax)
{
    if (pMesh == NULL) throw NullArgumentException("pMesh should not be NULL", __FILE__, __LINE__);
    if (pFieldName == NULL) throw NullArgumentException("pFieldName should not be NULL", __FILE__, __LINE__);
    
    Field* field = pMesh->getFieldByName(pFieldName);
    
    if (field == NULL) throw IllegalArgumentException("field not found", __FILE__, __LINE__);
    if ((pFieldIt < 1) || (pFieldIt > field->getNumberOfTimeSteps())) throw IllegalArgumentException("invalid field iteration", __FILE__, __LINE__);
    
    vector<PointOfField> points;
    pMesh->getAllPointsOfField(field, pFieldIt, points);

    //---------------------------------------------------------------------
    // Creates acceleration structure used to compute gradient
    //---------------------------------------------------------------------
    DecimationAccel* accel = new DecimationAccelGrid();
    char strCfg[256]; // a string is used for genericity
    sprintf(strCfg, "%d %d %d", pBoxing, pBoxing, pBoxing);
    accel->configure(strCfg);
    accel->create(points);
    
    //---------------------------------------------------------------------
    // Collects elements of the mesh to be kept
    //---------------------------------------------------------------------
    
    int numElements = pMesh->getNumberOfElements();
    int numGaussPointsByElt = points.size() / numElements; // for a TETRA10, should be 5 for a field of elements and 10 for a field of nodes
    
    *pOutGradMax = -1e300;
    *pOutGradMin = 1e300;
    *pOutGradAvg = 0.0;
    int count = 0;
    
    //cout << "numElements=" << numElements << endl;
    //cout << "num gauss pt by elt=" << numGaussPointsByElt << endl;
    
    // for each element
    for (int itElt = 0 ; itElt < numElements ; itElt++)
    {
        // for each Gauss point of the current element
        for (int itPtGauss = 0 ; itPtGauss < numGaussPointsByElt ; itPtGauss++)
        {
            const PointOfField& currentPt = points[itElt * numGaussPointsByElt + itPtGauss];
            
            vector<PointOfField> neighbours = accel->findNeighbours(
                currentPt.mXYZ[0], 
                currentPt.mXYZ[1], 
                currentPt.mXYZ[2], 
                pRadius);
            
            // if no neighbours => keep element
            if (neighbours.size() == 0)
            {
                continue;
            }
            
            // otherwise compute gradient...
            med_float normGrad = computeNormGrad(currentPt, neighbours);
            
            // debug
            //cout << (itElt * numGaussPointsByElt + j) << ": " << normGrad << endl;
            
            if (!isnan(normGrad))
            {
                if (normGrad > *pOutGradMax) *pOutGradMax = normGrad;
                if (normGrad < *pOutGradMin) *pOutGradMin = normGrad;
                *pOutGradAvg += normGrad;
                count++;
            }
        }
    }
    
    if (count != 0) *pOutGradAvg /= double(count);

    //---------------------------------------------------------------------
    // Cleans
    //---------------------------------------------------------------------
    delete accel;
}


med_float DecimationFilterGradAvg::computeNormGrad(const PointOfField& pPt, const std::vector<PointOfField>& pNeighbours) const
{
    med_float gradX = 0.0;
    med_float gradY = 0.0;
    med_float gradZ = 0.0;
 
    // for each neighbour
    for (unsigned i = 0 ; i < pNeighbours.size() ; i++)
    {
        const PointOfField& neighbourPt = pNeighbours[i];
        
        med_float vecX = neighbourPt.mXYZ[0] - pPt.mXYZ[0];
        med_float vecY = neighbourPt.mXYZ[1] - pPt.mXYZ[1];
        med_float vecZ = neighbourPt.mXYZ[2] - pPt.mXYZ[2];
        
        med_float norm = med_float( sqrt( vecX*vecX + vecY*vecY + vecZ*vecZ ) );
        med_float val =  neighbourPt.mVal - pPt.mVal;
        
        val /= norm;
        
        gradX += vecX * val;
        gradY += vecY * val;
        gradZ += vecZ * val;
    }
    
    med_float invSize = 1.0 / med_float(pNeighbours.size());
    
    gradX *= invSize;
    gradY *= invSize;
    gradZ *= invSize;
    
    med_float norm = med_float( sqrt( gradX*gradX + gradY*gradY + gradZ*gradZ ) );
    
    return norm;
    
}


} // namespace multipr

// EOF
