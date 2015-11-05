#include <iostream>
#include "rgg.h"
#include <valarray>

using std::valarray;

tuple(bool, valarray<bool>, valarray<bool>) isFeasible(int playerID, pureStrategy p) {

  intmatrix a = eqMatrices[playerID];
  vector<int> sums;
  for(int i=0; i<a.size(); i++){
    vector<int> dotProdVec;
    for(int j=0; j<a[i].size(); j++) {
      dotProdVec.push_back(a[i][j] * p[j]);
    }
    sums.push_back(dotProd.sum());
  }
  valarray<int> leftside(sums.data(), sums.size());
  valarray<int> rightside(eqVectors[playerID].data(), eqVectors[playerID].size());
  valarray<bool> eqRet = (leftside == rightside);

  intmatrix a2 = ltMatrices[playerID];
  vector<int> sums2;
  for(int i=0; i<a2.size(); i++){
    vector<int> dotProdVec2;
    for(int j=0; j<a2[i].size(); j++) {
      dotProdVec2.push_back(a[i][j] * p[j]);
    }
    sums.push_back(dotProdVec2.sum());
  }
  valarray<int> leftside2(sums2.data(), sums2.size());
  valarray<int> rightside2(ltVectors[playerID].data(), ltVectors[playerID].size());
  valarray<bool> ltRet = (leftside2 <= rightside2);

  bool feasible = (eqRet.sum()==eqRet.size()) && (ltRet.sum()==ltRet.size());
  return{feasible, eqRet, ltRet};
}
// valarray<bool> results = (a[i] * p < eqVector[playerId]);

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


static rgg* makeRandomRGG(int numPlayers, int numResourceNodes,
			vector<intMatrix>& eqMatrices,
			vector<int>& eqVectors,
			vector<intMatrix>& ltMatrices,
			vector<int>& ltVectors,
			vector<vector<int>>& neighbors) {





      return new rgg(numPlayers, numResourceNodes, eqMatrices, eqVectors, ltMatrices, letVectors, neighbors, utilityFunctions);
}


//we will possibly test this with a makeRandomRGG function
//random utilities
//you can have different kinds of graphs. self loop graph would give us congestion games, the other thing we can have is a complete graph.
//don't need a complex generator, just add to gambits' generator.
//generating random utilities. don't need to generate different graphs.
//for now just have two graphs complete graph and self loop graph.
