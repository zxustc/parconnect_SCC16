/**
 * @file    time_ccl_coloring_undirected_graph500.cpp
 * @ingroup group
 * @author  Chirag Jain <cjain7@gatech.edu>
 * @brief   Computes connected components in the synthetic undirected kronecker graph
 *
 * Copyright (c) 2015 Georgia Institute of Technology. All Rights Reserved.
 */

//Includes
#include <mpi.h>
#include <iostream>

//Own includes
#include "graphGen/graph500/graph500Gen.hpp"
#include "coloring/labelProp.hpp"
#include "utils/logging.hpp"
#include "utils/argvparser.hpp"

#include "mxx/reduction.hpp"
#include "mxx/utils.hpp"

INITIALIZE_EASYLOGGINGPP
using namespace std;
using namespace CommandLineProcessing;

int main(int argc, char** argv)
{
  // Initialize the MPI library:
  MPI_Init(&argc, &argv);

  //Initialize the communicator
  mxx::comm comm;


  //Print mpi rank distribution
  mxx::print_node_distribution();

  LOG_IF(!comm.rank(), INFO) << "Code computes connected components using coloring in the undirected synthetic graph";

  //Parse command line arguments
  ArgvParser cmd;

  cmd.setIntroductoryDescription("Computes connected components using coloring in the undirected synthetic graph");
  cmd.setHelpOption("h", "help", "Print this help page");

  cmd.defineOption("scale", "scale of the graph", ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);
  cmd.defineOption("edgefactor", "edgefactor of the graph", ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);

  int result = cmd.parse(argc, argv);

  //Make sure we get the right command line args
  if (result != ArgvParser::NoParserError)
  {
    if (!comm.rank()) cout << cmd.parseErrorDescription(result) << "\n";
    exit(1);
  }

  //Graph params
  uint8_t scale = std::stoi(cmd.optionValue("scale"));
  uint8_t edgefactor = std::stoi(cmd.optionValue("edgefactor"));

  //Object of the graph500 generator class
  conn::graphGen::graph500Gen g;

  //graph500 generator only uses int64_t
  using nodeIdType = int64_t;

  //Declare a edgeList vector to save edges
  std::vector< std::pair<nodeIdType, nodeIdType> > edgeList;

  //Populate the edgeList
  g.populateEdgeList(edgeList, scale, edgefactor, conn::graphGen::graph500Gen::UNDIRECTED, comm); 

  //Sum up the edge count across ranks
  auto totalEdgeCount = mxx::reduce(edgeList.size(), 0, comm);
  LOG_IF(!comm.rank(), INFO) << "Total edge count is " << totalEdgeCount;

  //Compute connected components
  conn::coloring::ccl<nodeIdType> cclInstance(edgeList, comm);
  cclInstance.compute();

  MPI_Finalize();
  return(0);
}
