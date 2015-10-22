#include <iostream>
#include "rgg.h"
#include <valarray>

using std::valarray;

valarray<bool> isFeasible(int playerID, pureStrategy p) {
  intmatrix a = eqMatrices[playerID];
  for(int i=0; i<a.size(); i++){
    vector<int> curr;
    for(int j=0; j<a[i].size(); j++) {
      curr.push_back(a[i][j] * p[j]);
    }
    valarray<int> leftside(curr.data(), curr.size());
    valarray<int> rightside(eqVector[playerID].data(), eqVector[playerID].size());
    return leftside <= rightside;  
   // valarray<bool> results = (a[i] * p < eqVector[playerId]);
  }


double getPureStrategyUtility(int playerID, pureStrategyProfile &p) {
  double u=0;
  vector<int> totalConfig(numResourceNodes,0);
  for(int a=0; a<p.size(); a++){
    for(int b=0; b<p[a].size(); b++) {
      totalConfig[b] += pureStrategyProfile[a][b];
    } 
  }
  for(int i=0; i<numResourceNodes; i++) {
    if(p[playerId][i] == 1) {
      vector<int> localConfig(neighbors[i].size);
      for(int j=0; j<neighbors[i].size(); j++) {
        localConfig[j] = totalConfig[neighbors[i][j]];
      }
      trie_map<double>::iterator curr = utilityFunctions[i].findExact(localConfig);
      if(curr == data.end()) {
        cout << "Error! Local Configuration Not Found";
        exit(1);
      } else {
          u += curr->second;
      }
    }
  }
  return u;
}




//we will possibly test this with a makeRandomRGG function
//random utilities
//you can have different kinds of graphs. self loop graph would give us congestion games, the other thing we can have is a complete graph.
//don't need a complex generator, just add to gambits' generator.
//generating random utilities. don't need to generate different graphs.
//for now just have two graphs complete graph and self loop graph.
