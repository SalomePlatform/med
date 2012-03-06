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

#include "MEDPARTITIONER_ParaDomainSelector.hxx"
#include "MEDPARTITIONER_UserGraph.hxx"
#include "MEDPARTITIONER_Utils.hxx"

#include "MEDCouplingUMesh.hxx"

#include <iostream>
#include <numeric>

#ifdef HAVE_MPI2
#include <mpi.h>
#endif

#ifndef WIN32
#include <sys/sysinfo.h>
#endif

using namespace std;
using namespace MEDPARTITIONER;

/*!
 * \brief Constructor. Find out my rank and world size
 */
ParaDomainSelector::ParaDomainSelector(bool mesure_memory)
  :_rank(0),_world_size(1), _nb_result_domains(-1), _mesure_memory(mesure_memory),
   _init_time(0.0), _init_memory(0), _max_memory(0)
{
#ifdef HAVE_MPI2
  MPI_Comm_size(MPI_COMM_WORLD,&_world_size) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&_rank) ;
  _init_time = MPI_Wtime();
#endif
  evaluateMemory();
}

ParaDomainSelector::~ParaDomainSelector()
{
}

/*!
 * \brief Return true if is running on different hosts
 */
bool ParaDomainSelector::isOnDifferentHosts() const
{
  evaluateMemory();
  if ( _world_size < 2 ) return false;

#ifdef HAVE_MPI2
  char name_here[ MPI_MAX_PROCESSOR_NAME+1 ], name_there[ MPI_MAX_PROCESSOR_NAME+1 ];
  int size;
  MPI_Get_processor_name( name_here, &size);

  int next_proc = (rank() + 1) % nbProcs();
  int prev_proc = (rank() - 1 + nbProcs()) % nbProcs();
  int tag  = 1111111;

  MPI_Status status;
  MPI_Sendrecv((void*)&name_here[0],  MPI_MAX_PROCESSOR_NAME, MPI_CHAR, next_proc, tag,
               (void*)&name_there[0], MPI_MAX_PROCESSOR_NAME, MPI_CHAR, prev_proc, tag,
               MPI_COMM_WORLD, &status);
               
  //cout<<"proc "<<rank()<<" : names "<<name_here<<" "<<name_there<<endl;
  //bug: (isOnDifferentHosts here and there) is not (isOnDifferentHosts somewhere)
  //return string(name_here) != string(name_there);
  
  int sum_same = -1;
  int same = 1;
  if (string(name_here) != string(name_there)) same=0;
  MPI_Allreduce( &same, &sum_same, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD );
  //cout<<"proc "<<rank()<<" : sum_same "<<sum_same<<endl;
  
  return (sum_same != nbProcs());
#endif
}

/*!
 * \brief Return true if the domain with domainIndex is to be loaded on this proc
 *  \param domainIndex - index of mesh domain
 *  \retval bool - to load or not
 */
bool ParaDomainSelector::isMyDomain(int domainIndex) const
{
  evaluateMemory();
  return (_rank == getProcessorID( domainIndex ));
}

/*!
 * \brief Return processor id where the domain with domainIndex resides
 *  \param domainIndex - index of mesh domain
 *  \retval int - processor id
 */
int ParaDomainSelector::getProcessorID(int domainIndex) const
{
  evaluateMemory();
  return ( domainIndex % _world_size );
}

/*!
 * \brief Gather info on nb of cell entities on each processor and return total nb.
 *
 * Is called
 * 1) for MED_CELL to know global id shift for domains at graph construction;
 * 2) for sub-entity to know total nb of sub-entities before creating those of joints
 */
void ParaDomainSelector::gatherNbOf(const vector<ParaMEDMEM::MEDCouplingUMesh*>& domain_meshes)
{
  evaluateMemory();
  // get nb of elems of each domain mesh
  int nb_domains=domain_meshes.size();
  //cout<<"proc "<<MyGlobals::_Rank<<" : gatherNbOf "<<nb_domains<<endl;
  vector<int> nb_elems(nb_domains*2, 0); //NumberOfCells & NumberOfNodes
  for (int i=0; i<nb_domains; ++i)
    if ( domain_meshes[i] )
      {
        nb_elems[i*2] = domain_meshes[i]->getNumberOfCells();
        nb_elems[i*2+1] = domain_meshes[i]->getNumberOfNodes();
      }
  // receive nb of elems from other procs
  vector<int> all_nb_elems( nb_domains*2 );
#ifdef HAVE_MPI2
  MPI_Allreduce((void*)&nb_elems[0], (void*)&all_nb_elems[0], nb_domains*2,
                MPI_INT, MPI_SUM, MPI_COMM_WORLD);
#endif
  int total_nb_cells=0, total_nb_nodes=0;
  for (int i=0; i<nb_domains; ++i)
    {
      total_nb_cells+=all_nb_elems[i*2];
      total_nb_nodes+=all_nb_elems[i*2+1];
    }
  
  if (MyGlobals::_Is0verbose>10)
    cout<<"totalNbCells "<<total_nb_cells<<" totalNbNodes "<<total_nb_nodes<<endl;
  
  vector<int>& cell_shift_by_domain=_cell_shift_by_domain;
  vector<int>& node_shift_by_domain=_node_shift_by_domain;
  vector<int>& face_shift_by_domain=_face_shift_by_domain;
 
  vector< int > ordered_nbs_cell, ordered_nbs_node, domain_order(nb_domains);
  ordered_nbs_cell.push_back(0);
  ordered_nbs_node.push_back(0);
  for (int iproc=0; iproc<nbProcs(); ++iproc)
    for (int idomain=0; idomain<nb_domains; ++idomain)
      if (getProcessorID( idomain )==iproc)
        {
          domain_order[idomain] = ordered_nbs_cell.size() - 1;
          ordered_nbs_cell.push_back( ordered_nbs_cell.back() + all_nb_elems[idomain*2] );
          ordered_nbs_node.push_back( ordered_nbs_node.back() + all_nb_elems[idomain*2+1] );
        }
  cell_shift_by_domain.resize( nb_domains+1, 0 );
  node_shift_by_domain.resize( nb_domains+1, 0 );
  face_shift_by_domain.resize( nb_domains+1, 0 );
  for (int idomain=0; idomain<nb_domains; ++idomain)
    {
      cell_shift_by_domain[ idomain ] = ordered_nbs_cell[ domain_order[ idomain ]];
      node_shift_by_domain[ idomain ] = ordered_nbs_node[ domain_order[ idomain ]];
    }
  cell_shift_by_domain.back() = ordered_nbs_cell.back(); // to know total nb of elements
  node_shift_by_domain.back() = ordered_nbs_node.back(); // to know total nb of elements
  
  if (MyGlobals::_Is0verbose>300)
    {
      cout<<"proc "<<MyGlobals::_Rank<<" : cellShiftByDomain ";
      for (int i=0; i<=nb_domains; ++i) cout<<cell_shift_by_domain[i]<<"|";
      cout<<endl;
      cout<<"proc "<<MyGlobals::_Rank<<" : nodeShiftBy_domain ";
      for (int i=0; i<=nb_domains; ++i) cout<<node_shift_by_domain[i]<<"|";
      cout<<endl;
    }
  // fill _nb_vert_of_procs (is Vtxdist)
  _nb_vert_of_procs.resize(_world_size+1);
  _nb_vert_of_procs[0] = 0; // base = 0
  for (int i=0; i<nb_domains; ++i)
    {
      int rank = getProcessorID(i);
      _nb_vert_of_procs[rank+1] += all_nb_elems[i*2];
    }
  for (int i=1; i<_nb_vert_of_procs.size(); ++i)
    _nb_vert_of_procs[i] += _nb_vert_of_procs[i-1]; // to CSR format : cumulated
  
  if (MyGlobals::_Is0verbose>200)
    {
      cout<<"proc "<<MyGlobals::_Rank<<" : gatherNbOf : vtxdist is ";
      for (int i = 0; i <= _world_size; ++i) cout<<_nb_vert_of_procs[i]<<" ";
      cout<<endl;
    }
  
  evaluateMemory();
  return;
}

/*!
 * \brief Return distribution of the graph vertices among the processors
 *  \retval int* - array containing nb of vertices (=cells) on all processors
 *
 * gatherNbOf() must be called before.
 * The result array is to be used as the first arg of ParMETIS_V3_PartKway() and
 * is freed by ParaDomainSelector.
 */
int* ParaDomainSelector::getProcVtxdist() const
{
  evaluateMemory();
  if (_nb_vert_of_procs.empty()) throw INTERP_KERNEL::Exception(LOCALIZED("_nb_vert_of_procs not set"));
  return (int*) & _nb_vert_of_procs[0];
}

/*!
 * \brief Return nb of cells in domains with lower index.
 *
 * gatherNbOf() must be called before.
 * Result added to local id on given domain gives id in the whole distributed mesh
 */
int ParaDomainSelector::getDomainCellShift(int domainIndex) const
{
  evaluateMemory();
  if (_cell_shift_by_domain.empty()) throw INTERP_KERNEL::Exception(LOCALIZED("_cell_shift_by_domain not set"));
  return _cell_shift_by_domain[domainIndex];
}

int ParaDomainSelector::getDomainNodeShift(int domainIndex) const
{
  evaluateMemory();
  if (_node_shift_by_domain.empty()) throw INTERP_KERNEL::Exception(LOCALIZED("_node_shift_by_domain not set"));
  return _node_shift_by_domain[domainIndex];
}

/*!
 * \brief Return nb of nodes on processors with lower rank.
 *
 * gatherNbOf() must be called before.
 * Result added to global id on this processor gives id in the whole distributed mesh
 */
int ParaDomainSelector::getProcNodeShift() const
{
  evaluateMemory();
  if (_nb_vert_of_procs.empty()) throw INTERP_KERNEL::Exception(LOCALIZED("_nb_vert_of_procs not set"));
  //cout<<"_nb_vert_of_procs "<<_nb_vert_of_procs[0]<<" "<<_nb_vert_of_procs[1]<<endl;
  return _nb_vert_of_procs[_rank];
}

/*!
 * \brief Gather graphs from all processors into one
 */
auto_ptr<Graph> ParaDomainSelector::gatherGraph(const Graph* graph) const
{
  Graph* glob_graph = 0;

  evaluateMemory();
#ifdef HAVE_MPI2

  // ---------------
  // Gather indices
  // ---------------

  vector<int> index_size_of_proc( nbProcs() ); // index sizes - 1
  for ( int i = 1; i < _nb_vert_of_procs.size(); ++i )
    index_size_of_proc[i-1] = _nb_vert_of_procs[ i ] - _nb_vert_of_procs[ i-1 ];

  int index_size = 1 + _cell_shift_by_domain.back();
  int* graph_index = new int[ index_size ];
  const int* index = graph->getGraph()->getIndex();
  int* proc_index_displacement = (int*) & _nb_vert_of_procs[0];

  MPI_Allgatherv((void*) (index+1),         // send local index except first 0 (or 1)
                 index_size_of_proc[_rank], // index size on this proc
                 MPI_INT,
                 (void*) graph_index,       // receive indices
                 & index_size_of_proc[0],   // index size on each proc
                 proc_index_displacement,   // displacement of each proc index
                 MPI_INT,
                 MPI_COMM_WORLD);
  graph_index[0] = index[0]; // it is not overwritten thanks to proc_index_displacement[0]==1

  // get sizes of graph values on each proc by the got indices of graphs
  vector< int > value_size_of_proc( nbProcs() ), proc_value_displacement(1,0);
  for ( int i = 0; i < nbProcs(); ++i )
    {
      if ( index_size_of_proc[i] > 0 )
        value_size_of_proc[i] = graph_index[ proc_index_displacement[ i+1 ]-1 ] - graph_index[0];
      else
        value_size_of_proc[i] = 0;
      proc_value_displacement.push_back( proc_value_displacement.back() + value_size_of_proc[i] );
    }
  
  // update graph_index
  for ( int i = 1; i < nbProcs(); ++i )
    {
      int shift = graph_index[ proc_index_displacement[i]-1 ]-graph_index[0];
      for ( int j = proc_index_displacement[i]; j < proc_index_displacement[i+1]; ++j )
        graph_index[ j ] += shift;
    }
  
  // --------------
  // Gather values
  // --------------

  int value_size = graph_index[ index_size-1 ] - graph_index[ 0 ];
  int* graph_value = new int[ value_size ];
  const int* value = graph->getGraph()->getValue();

  MPI_Allgatherv((void*) value,                // send local value
                 value_size_of_proc[_rank],    // value size on this proc
                 MPI_INT,
                 (void*) graph_value,          // receive values
                 & value_size_of_proc[0],      // value size on each proc
                 & proc_value_displacement[0], // displacement of each proc value
                 MPI_INT,
                 MPI_COMM_WORLD);

  // -----------------
  // Gather partition
  // -----------------

  int * partition = new int[ _cell_shift_by_domain.back() ];
  const int* part = graph->getPart();
  
  MPI_Allgatherv((void*) part,              // send local partition
                 index_size_of_proc[_rank], // index size on this proc
                 MPI_INT,
                 (void*)(partition-1),      // -1 compensates proc_index_displacement[0]==1
                 & index_size_of_proc[0],   // index size on each proc
                 proc_index_displacement,   // displacement of each proc index
                 MPI_INT,
                 MPI_COMM_WORLD);

  // -----------
  // Make graph
  // -----------

  //   MEDPARTITIONER::SkyLineArray* array =
  //     new MEDPARTITIONER::SkyLineArray( index_size-1, value_size, graph_index, graph_value, true );

  //   glob_graph = new UserGraph( array, partition, index_size-1 );

  evaluateMemory();

  delete [] partition;

#endif // HAVE_MPI2

  return auto_ptr<Graph>( glob_graph );
}


/*!
 * \brief Set nb of cell/cell pairs in a joint between domains
 */
void ParaDomainSelector::setNbCellPairs( int nb_cell_pairs, int dist_domain, int loc_domain )
{
  // This method is needed for further computing global numbers of faces in joint.
  // Store if both domains are on this proc else on one of procs only
  if ( isMyDomain( dist_domain ) || dist_domain < loc_domain )
    {
      if ( _nb_cell_pairs_by_joint.empty() )
        _nb_cell_pairs_by_joint.resize( _nb_result_domains*(_nb_result_domains+1), 0);

      int joint_id = jointId( loc_domain, dist_domain );
      _nb_cell_pairs_by_joint[ joint_id ] = nb_cell_pairs;
    }
  evaluateMemory();
}

//================================================================================
/*!
 * \brief Return nb of cell/cell pairs in a joint between domains on different procs
 */
//================================================================================

int ParaDomainSelector::getNbCellPairs( int dist_domain, int loc_domain ) const
{
  evaluateMemory();

  int joint_id = jointId( loc_domain, dist_domain );
  return _nb_cell_pairs_by_joint[ joint_id ];
}

//================================================================================
/*!
 * \brief Gather size of each joint
 */
//================================================================================

void ParaDomainSelector::gatherNbCellPairs()
{
  if ( _nb_cell_pairs_by_joint.empty() )
    _nb_cell_pairs_by_joint.resize( _nb_result_domains*(_nb_result_domains+1), 0);
  evaluateMemory();

  vector< int > send_buf = _nb_cell_pairs_by_joint;
#ifdef HAVE_MPI2
  MPI_Allreduce((void*)&send_buf[0],
                (void*)&_nb_cell_pairs_by_joint[0],
                _nb_cell_pairs_by_joint.size(),
                MPI_INT, MPI_SUM, MPI_COMM_WORLD);
#endif
  // check that the set nbs of cell pairs are correct,
  // namely that each joint is treated on one proc only
  for ( int j = 0; j < _nb_cell_pairs_by_joint.size(); ++j )
    if ( _nb_cell_pairs_by_joint[j] != send_buf[j] && send_buf[j]>0 )
      throw INTERP_KERNEL::Exception(LOCALIZED("invalid nb of cell pairs"));
}

//================================================================================
/*!
 * \brief Return the first global id of sub-entity for the joint
 */
//================================================================================

int ParaDomainSelector::getFisrtGlobalIdOfSubentity( int loc_domain, int dist_domain ) const
{
  // total_nb_faces includes faces existing before creation of joint faces
  // (got in gatherNbOf( MED_FACE )).
  evaluateMemory();

  int total_nb_faces = _face_shift_by_domain.empty() ? 0 : _face_shift_by_domain.back();
  int id = total_nb_faces + 1;

  if ( _nb_cell_pairs_by_joint.empty() )
    throw INTERP_KERNEL::Exception(LOCALIZED("gatherNbCellPairs() must be called before"));
  int joint_id = jointId( loc_domain, dist_domain );
  for ( int j = 0; j < joint_id; ++j )
    id += _nb_cell_pairs_by_joint[ j ];

  return id;
}

//================================================================================
/*!
 * \brief Send-receive local ids of joint faces
 */
//================================================================================

int* ParaDomainSelector::exchangeSubentityIds( int loc_domain, int dist_domain,
                                               const vector<int>& loc_ids_here ) const
{
  int* loc_ids_dist = new int[ loc_ids_here.size()];
  int dest = getProcessorID( dist_domain );
  int tag  = 2002 + jointId( loc_domain, dist_domain );
#ifdef HAVE_MPI2
  MPI_Status status;
  MPI_Sendrecv((void*)&loc_ids_here[0], loc_ids_here.size(), MPI_INT, dest, tag,
               (void*) loc_ids_dist,    loc_ids_here.size(), MPI_INT, dest, tag,
               MPI_COMM_WORLD, &status);  
#endif
  evaluateMemory();

  return loc_ids_dist;
}

//================================================================================
/*!
 * \brief Return identifier for a joint
 */
//================================================================================

int ParaDomainSelector::jointId( int local_domain, int distant_domain ) const
{
  evaluateMemory();
  if (_nb_result_domains < 0)
    throw INTERP_KERNEL::Exception(LOCALIZED("setNbDomains() must be called before"));

  if ( local_domain < distant_domain )
    swap( local_domain, distant_domain );
  return local_domain * _nb_result_domains + distant_domain;
}


//================================================================================
/*!
 * \brief Return time passed from construction in seconds
 */
//================================================================================

double ParaDomainSelector::getPassedTime() const
{
#ifdef HAVE_MPI2
  return MPI_Wtime() - _init_time;
#else
  return 0.0;
#endif
}

/*!
 * \brief Evaluate current memory usage and return the maximal one in KB
 */
int ParaDomainSelector::evaluateMemory() const
{
  if ( _mesure_memory )
    {
      int used_memory = 0;
#ifndef WIN32
      struct sysinfo si;
      int err = sysinfo( &si );
      if ( !err )
        used_memory =
          (( si.totalram - si.freeram + si.totalswap - si.freeswap ) * si.mem_unit ) / 1024;
#endif
      if ( used_memory > _max_memory )
        ((ParaDomainSelector*) this)->_max_memory = used_memory;

      if ( !_init_memory )
        ((ParaDomainSelector*) this)->_init_memory = used_memory;
    }
  return _max_memory - _init_memory;
}

/*!
  Sends content of \a mesh to processor \a target. To be used with \a recvMesh method.
  \param mesh mesh to be sent
  \param target processor id of the target
*/

void ParaDomainSelector::sendMesh(const ParaMEDMEM::MEDCouplingUMesh& mesh, int target) const
{
  if (MyGlobals::_Verbose>600) cout<<"proc "<<_rank<<" : sendMesh '"<<mesh.getName()<<"' size "<<mesh.getNumberOfCells()<<" to "<<target<<endl;
  // First stage : sending sizes
  // ------------------------------
  vector<int> tinyInfoLocal;
  vector<string> tinyInfoLocalS;
  vector<double> tinyInfoLocalD;
  //Getting tiny info of local mesh to allow the distant proc to initialize and allocate
  //the transmitted mesh.
  mesh.getTinySerializationInformation(tinyInfoLocalD,tinyInfoLocal,tinyInfoLocalS);
  //cout<<"sendMesh getTinySerializationInformation "<<mesh.getName()<<endl;
  tinyInfoLocal.push_back(mesh.getNumberOfCells());
#ifdef        HAVE_MPI2
  int tinySize=tinyInfoLocal.size();
  //cout<<"MPI_Send cvw11 "<<tinySize<<endl;
  MPI_Send(&tinySize, 1, MPI_INT, target, 1113, MPI_COMM_WORLD);
  //cout<<"MPI_Send cvw22 "<<tinyInfoLocal.size()<<endl;
  MPI_Send(&tinyInfoLocal[0], tinyInfoLocal.size(), MPI_INT, target, 1112, MPI_COMM_WORLD);
#endif

  if (mesh.getNumberOfCells()>0) //no sends if empty
    {
      ParaMEDMEM::DataArrayInt *v1Local=0;
      ParaMEDMEM::DataArrayDouble *v2Local=0;
      //serialization of local mesh to send data to distant proc.
      mesh.serialize(v1Local,v2Local);
      int nbLocalElems=0;
      int* ptLocal=0;
      if(v1Local) //cvw if empty getNbOfElems() is 1!
        {
          nbLocalElems=v1Local->getNbOfElems(); //cvw if empty be 1!
          ptLocal=v1Local->getPointer();
        }
#ifdef        HAVE_MPI2
      MPI_Send(ptLocal, nbLocalElems, MPI_INT, target, 1111, MPI_COMM_WORLD);
#endif
      int nbLocalElems2=0;
      double *ptLocal2=0;
      if(v2Local) //cvw if empty be 0!
        {
          nbLocalElems2=v2Local->getNbOfElems();
          ptLocal2=v2Local->getPointer();
        }
#ifdef HAVE_MPI2
      MPI_Send(ptLocal2, nbLocalElems2, MPI_DOUBLE, target, 1110, MPI_COMM_WORLD);
#endif
      if(v1Local) v1Local->decrRef();
      if(v2Local) v2Local->decrRef();
    }
  else
    {
      //cout<<"sendMesh empty Mesh cvw3344 "<<endl;
    }
  //cout<<"end sendMesh "<<mesh.getName()<<endl;
}

/*! Receives messages from proc \a source to fill mesh \a mesh.
  To be used with \a sendMesh method.
  \param mesh  pointer to mesh that is filled
  \param source processor id of the incoming messages
*/
void ParaDomainSelector::recvMesh(ParaMEDMEM::MEDCouplingUMesh*& mesh, int source)const
{
  // First stage : exchanging sizes
  // ------------------------------
  vector<int> tinyInfoDistant;
  vector<string> tinyInfoLocalS;
  vector<double> tinyInfoDistantD(1);
  //Getting tiny info of local mesh to allow the distant proc to initialize and allocate
  //the transmitted mesh.
#ifdef HAVE_MPI2
  MPI_Status status; 
  int tinyVecSize;
  MPI_Recv(&tinyVecSize, 1, MPI_INT, source, 1113, MPI_COMM_WORLD, &status);
  tinyInfoDistant.resize(tinyVecSize);
#endif
  std::fill(tinyInfoDistant.begin(),tinyInfoDistant.end(),0);

#ifdef HAVE_MPI2
  MPI_Recv(&tinyInfoDistant[0], tinyVecSize, MPI_INT,source,1112,MPI_COMM_WORLD, &status);
#endif
  //there was tinyInfoLocal.push_back(mesh.getNumberOfCells());
  int NumberOfCells=tinyInfoDistant[tinyVecSize-1];
  //cout<<"recvMesh NumberOfCells "<<NumberOfCells<<endl;
  if (NumberOfCells>0)
    {
      ParaMEDMEM::DataArrayInt *v1Distant=ParaMEDMEM::DataArrayInt::New();
      ParaMEDMEM::DataArrayDouble *v2Distant=ParaMEDMEM::DataArrayDouble::New();
      //Building the right instance of copy of distant mesh.
      ParaMEDMEM::MEDCouplingPointSet *distant_mesh_tmp=
        ParaMEDMEM::MEDCouplingPointSet::BuildInstanceFromMeshType(
                                                                   (ParaMEDMEM::MEDCouplingMeshType) tinyInfoDistant[0]);
      std::vector<std::string> unusedTinyDistantSts;
      mesh=dynamic_cast<ParaMEDMEM::MEDCouplingUMesh*> (distant_mesh_tmp);
 
      mesh->resizeForUnserialization(tinyInfoDistant,v1Distant,v2Distant,unusedTinyDistantSts);
      int nbDistElem=0;
      int *ptDist=0;
      if(v1Distant)
        {
          nbDistElem=v1Distant->getNbOfElems();
          ptDist=v1Distant->getPointer();
        }
#ifdef HAVE_MPI2
      MPI_Recv(ptDist, nbDistElem, MPI_INT, source,1111, MPI_COMM_WORLD, &status);
#endif
      double *ptDist2=0;
      nbDistElem=0;
      if(v2Distant)
        {
          nbDistElem=v2Distant->getNbOfElems();
          ptDist2=v2Distant->getPointer();
        }
#ifdef HAVE_MPI2
      MPI_Recv(ptDist2, nbDistElem, MPI_DOUBLE,source, 1110, MPI_COMM_WORLD, &status);
#endif
      //finish unserialization
      mesh->unserialization(tinyInfoDistantD,tinyInfoDistant,v1Distant,v2Distant,unusedTinyDistantSts);
      if(v1Distant) v1Distant->decrRef();
      if(v2Distant) v2Distant->decrRef();
    }
  else
    {
      mesh=CreateEmptyMEDCouplingUMesh();
    }
  if (MyGlobals::_Verbose>600) cout<<"proc "<<_rank<<" : recvMesh '"<<mesh->getName()<<"' size "<<mesh->getNumberOfCells()<<" from "<<source<<endl;
}

