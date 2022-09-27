#include "stdafx.h"
#include <chrono>

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Network community detection. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "graph.txt", "Input graph (undirected graph)");
  const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "communities.txt", "Output file");
  const int CmtyAlg = Env.GetIfArgPrefixInt("-a:", 2, "Algorithm: 1:Girvan-Newman, 2:Clauset-Newman-Moore, 3:Infomap");

  PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>(InFNm, false);
  //PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>("../as20graph.txt", false);
  //PUNGraph Graph = TSnap::GenRndGnm<PUNGraph>(5000, 10000); // generate a random graph

  TSnap::DelSelfEdges(Graph);

  auto start = std::chrono::high_resolution_clock::now();
  TCnComV CmtyV;
  double Q = 0.0;
  TStr CmtyAlgStr;
  if (CmtyAlg == 1) {
    CmtyAlgStr = "Girvan-Newman";
    Q = TSnap::CommunityGirvanNewman(Graph, CmtyV); }
  else if (CmtyAlg == 2) {
    CmtyAlgStr = "Clauset-Newman-Moore";
    Q = TSnap::CommunityCNM(Graph, CmtyV); }
  else if (CmtyAlg == 3) {
    CmtyAlgStr = "Infomap";
    Q = TSnap::Infomap(Graph, CmtyV); }
  else { Fail; }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

  printf("# Input: %s\n", InFNm.CStr());
  printf("# Nodes: %d    Edges: %d\n", Graph->GetNodes(), Graph->GetEdges());
  printf("# Algorithm: %s\n", CmtyAlgStr.CStr());
  if (CmtyAlg!=3) {
    printf("# Modularity: %f\n", Q);
  } else {
    printf("# Average code length: %f\n", Q);
  }
  printf("# Communities: %d\n", CmtyV.Len());

  //fprintf(F, "# NId\tCommunityId\n");
  //for (int c = 0; c < CmtyV.Len(); c++) {
  //  for (int i = 0; i < CmtyV[c].Len(); i++) {
  //    fprintf(F, "%d\t%d\n", CmtyV[c][i].Val, c);
  //  }
  //}

  FILE *F = fopen(OutFNm.CStr(), "wt");
  for (int c = 0; c < CmtyV.Len(); c++) {
    for (int i = 0; i < CmtyV[c].Len(); i++) {
      fprintf(F, "%d\t", CmtyV[c][i].Val);
    }
    fprintf(F, "\n");
  }
  fclose(F);

  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  printf("\nCluster Time: %f\n", duration.count());
  return 0;
}
