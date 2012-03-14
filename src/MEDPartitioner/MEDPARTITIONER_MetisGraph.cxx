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

#include "MEDPARTITIONER_MetisGraph.hxx"
#include "MEDPARTITIONER_ParaDomainSelector.hxx"
#include "MEDPARTITIONER_Utils.hxx"

#include "InterpKernelException.hxx"

#include <iostream>

#ifdef MED_ENABLE_PARMETIS
#include <mpi.h>
#include "parmetis.h"
#endif

#ifdef MED_ENABLE_METIS
extern "C"
{
#include "metis.h"
}
#endif

using namespace ParaMEDMEM;
using namespace MEDPARTITIONER;

METISGraph::METISGraph():Graph()
{
}

METISGraph::METISGraph(MEDPARTITIONER::SkyLineArray* graph, int* edgeweight)
  :Graph(graph,edgeweight)
{
}

METISGraph::~METISGraph()
{
}

void METISGraph::partGraph(int ndomain,
                           const std::string& options_string,
                           ParaDomainSelector *parallelizer)
{
  using std::vector;
  vector<int> ran,vx,va; //for randomize
  
  if (MyGlobals::_Verbose>10)
    std::cout << "proc " << MyGlobals::_Rank << " : METISGraph::partGraph" << std::endl;
  
  // number of graph vertices
  int n=_graph->getNumberOf();
  //graph
  int * xadj=const_cast<int*>(_graph->getIndex());
  int * adjncy=const_cast<int*>(_graph->getValue());
  //constraints
  int * vwgt=_cell_weight;
  int * adjwgt=_edge_weight;
  int wgtflag=(_edge_weight!=0)?1:0+(_cell_weight!=0)?2:0;
  //base 0 or 1
  int base=0;
  //ndomain
  int nparts=ndomain;
  //options
  /*
    (0=default_option,option,random_seed) see defs.h
    #define PMV3_OPTION_DBGLVL 1
    #define PMV3_OPTION_SEED 2
    #define PMV3_OPTION_IPART 3
    #define PMV3_OPTION_PSR 3
    seems no changes int options[4]={1,0,33,0}; //test for a random seed of 33
  */
  int options[4]={0,0,0,0};
  // output parameters
  int edgecut;
  int* partition=new int[n];

  if(nparts >1)
    {
      if(parallelizer)
        {
#ifdef MED_ENABLE_PARMETIS
          // distribution of vertices of the graph among the processors
          if (MyGlobals::_Verbose>100) 
            std::cout << "proc " << MyGlobals::_Rank << " : METISGraph::partGraph ParMETIS_PartKway" << std::endl;
          int * vtxdist=parallelizer->getProcVtxdist();
          MPI_Comm comm=MPI_COMM_WORLD;
          try
            {
              if (MyGlobals::_Verbose>200) 
                {
                  std::cout << "proc " << MyGlobals::_Rank << " : vtxdist :";
                  for (int i=0; i<MyGlobals::_World_Size+1; ++i)
                    std::cout << vtxdist[i] <<" ";
                  std::cout << std::endl;
          
                  int lgxadj=vtxdist[MyGlobals::_Rank+1]-vtxdist[MyGlobals::_Rank];
          
                  if (lgxadj>0)
                    {
                      std::cout<< "\nproc " << MyGlobals::_Rank << " : lgxadj " << lgxadj << " lgadj " << xadj[lgxadj+1] << std::endl;
                      for (int i=0; i<10; ++i)
                        std::cout << xadj[i] << " ";
                      std::cout << "... " << xadj[lgxadj] << std::endl;
                      for (int i=0; i<15; ++i)
                        std::cout << adjncy[i] << " ";
                      int ll=xadj[lgxadj]-1;
                      std::cout << "... [" << ll << "] " << adjncy[ll-1] << " " << adjncy[ll] << std::endl;
                      int imaxx=0;
                      for (int ilgxadj=0; ilgxadj<lgxadj; ilgxadj++)
                        {
                          int ilg=xadj[ilgxadj+1]-xadj[ilgxadj];
                          if(ilg>imaxx)
                            imaxx=ilg;
                        }
                      std::cout<< "\nproc " << MyGlobals::_Rank << " : on " << lgxadj << " cells, max neighbourg number (...for one cell) is " << imaxx << std::endl;
                    }
                }
              if ((MyGlobals::_Randomize!=0 || MyGlobals::_Atomize!=0) && MyGlobals::_World_Size==1)
                {
                  //randomize initially was for test on ParMETIS error (sometimes)
                  //due to : seems no changes int options[4]={1,0,33,0}; //test for a random seed of 33
                  //it was keeped
                  ran=CreateRandomSize(n);
                  RandomizeAdj(&xadj[0],&adjncy[0],ran,vx,va);
                  ParMETIS_PartKway( //cvwat11
                                    vtxdist, &vx[0], &va[0], vwgt, 
                                    adjwgt, &wgtflag, &base, &nparts, options, 
                                    &edgecut, partition, &comm );
                }
              else
                {
                  //MPI_Barrier(MPI_COMM_WORLD);
                  ParMETIS_PartKway( //cvwat11
                                    vtxdist, xadj, adjncy, vwgt, 
                                    adjwgt, &wgtflag, &base, &nparts, options, 
                                    &edgecut, partition, &comm );
                }

              /*doc from parmetis.h
                void __cdecl ParMETIS_PartKway(
                idxtype *vtxdist, idxtype *xadj, idxtype *adjncy, idxtype *vwgt, 
                idxtype *adjwgt, int *wgtflag, int *numflag, int *nparts, int *options, 
                int *edgecut, idxtype *part, MPI_Comm *comm);

                void __cdecl ParMETIS_V3_PartKway(
                idxtype *vtxdist, idxtype *xadj, idxtype *adjncy, idxtype *vwgt, 
                idxtype *adjwgt, int *wgtflag, int *numflag, int *ncon, int *nparts, 
                float *tpwgts, float *ubvec, int *options, int *edgecut, idxtype *part, 
                MPI_Comm *comm);
              */

            }
          catch(...)
            {
              //shit ParMETIS "Error! Key -2 not found!" not catched...
              throw INTERP_KERNEL::Exception(LOCALIZED("Problem in ParMETIS_PartKway"));
            }
          if (n<8 && nparts==3)
            {
              for (int i=0; i<n; i++)
                partition[i]=i%3;
            }
#else
          throw INTERP_KERNEL::Exception(LOCALIZED("ParMETIS is not available. Check your products, please."));
#endif
        }
      else
        {
#ifdef MED_ENABLE_METIS
          if (MyGlobals::_Verbose>10) 
            std::cout << "proc " << MyGlobals::_Rank << " : METISGraph::partGraph METIS_PartGraph Recursive or Kway" << std::endl;
          if (options_string != "k")
            METIS_PartGraphRecursive(&n, xadj, adjncy, vwgt, adjwgt, &wgtflag,
                                     &base, &nparts, options, &edgecut, partition);
          else
            METIS_PartGraphKway(&n, xadj, adjncy, vwgt, adjwgt, &wgtflag,
                                &base, &nparts, options, &edgecut, partition);
#else
          throw INTERP_KERNEL::Exception(LOCALIZED("METIS is not available. Check your products, please."));
#endif
        }
    }
  else
    {
      for (int i=0; i<n; i++)
        partition[i]=0;
    }
  
  vector<int> index(n+1);
  vector<int> value(n);
  index[0]=0;
  if (ran.size()>0 && MyGlobals::_Atomize==0) //there is randomize
    {
      if (MyGlobals::_Is0verbose>100)
        std::cout << "randomize" << std::endl;
      for (int i=0; i<n; i++)
        {
          index[i+1]=index[i]+1;
          value[ran[i]]=partition[i];
        }
    }
  else
    {
      for (int i=0; i<n; i++)
        {
          index[i+1]=index[i]+1;
          value[i]=partition[i];
        }
    }
  delete [] partition;

  //creating a skylinearray with no copy of the index and partition array
  //the fifth argument true specifies that only the pointers are passed 
  //to the object
  
  _partition = new MEDPARTITIONER::SkyLineArray(index,value);
}
