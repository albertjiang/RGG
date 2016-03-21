#include <iostream>
#include <utility>
#include <math.h>
#include "rgg.h"
#include<string>

using std::valarray;
using std::cout;

std::tuple<bool, valarray<bool>, valarray<bool>> rgg::isFeasible(int playerID, rgg::pureStrategy &p) {
  if(ltMatrices[playerID].size() == 0) {
    cout << "Error: No Less Than Matrices" << endl;
    return std::make_tuple(false, valarray<bool>{}, valarray<bool>{});
  }
  else {
    rgg::intMatrix &a = eqMatrices.at(playerID);
    vector<int> sums;
    for(int i=0; i<a.size(); i++){
      int sum = 0;
      for(int j=0; j<a[i].size(); j++) {
        sum += a[i][j] * p[j];
      }
      sums.push_back(sum);
    }
    valarray<int> leftside(sums.data(), sums.size());
    valarray<int> rightside(eqVectors[playerID].data(), eqVectors[playerID].size());
    valarray<bool> eqRet = (leftside == rightside);

    rgg::intMatrix &a2 = ltMatrices.at(playerID);
    vector<int> sums2;
    for(int i=0; i<a2.size(); i++){
      int sum2 = 0;
      for(int j=0; j<a2[i].size(); j++) {
        sum2 += a2[i][j] * p[j];
      }
      sums2.push_back(sum2);
    }
    valarray<int> leftside2(sums2.data(), sums2.size());
    valarray<int> rightside2(ltVectors[playerID].data(), ltVectors[playerID].size());
    valarray<bool> ltRet = (leftside2 <= rightside2);

    int ltretsum = 0;
    for(auto a: ltRet) {
      if(a == true)
        ++ltretsum;
    }
    int eqretsum = 0;
    for(auto a: eqRet) {
      if(a == true)
        ++eqretsum;
    }
    bool feasible = (eqretsum==eqRet.size()) && (ltretsum==ltRet.size());
    return std::make_tuple(feasible, eqRet, ltRet);
  }
}
// valarray<bool> results = (a[i] * p < eqVector[playerId]);
double rgg::getPureStrategyUtility(int playerID, pureStrategyProfile &p) {
  double u=0;
  vector<int> totalConfig(numResourceNodes,0);
  for(int a=0; a<p.size(); a++){
    for(int b=0; b<p[a].size(); b++) {
      totalConfig[b] += p[a][b];
    }
  }
  for(int i=0; i<numResourceNodes; i++) {
    if(p[playerID][i] == 1) {
      vector<int> localConfig(neighbors[i].size());
      for(int j=0; j<neighbors[i].size(); j++) {
        localConfig[j] = totalConfig[neighbors[i][j]];
      }
      trie_map<double>::iterator curr = utilityFunctions[i].findExact(localConfig);
      if(curr == utilityFunctions[i].end()) {
        cout << "Error! Local Configuration Not Found";
        exit(1);
      } else {
          u += curr->second;
      }
    }
  }
  return u;
}

rgg::rgg(int newNumPlayers, int newNumResourceNodes,
      vector<intMatrix> newEqMatrices,
      vector<vector<int>> newEqVectors,
      vector<intMatrix> newLtMatrices,
      vector<vector<int>> newLtVectors,
      vector<vector<int>> newNeighbors,
      vector<trie_map<double>> newUtilFuncs) {
  numPlayers = newNumPlayers;
  numResourceNodes = newNumResourceNodes;
  eqMatrices = newEqMatrices;
  eqVectors = newEqVectors;
  ltMatrices = newLtMatrices;
  ltVectors = newLtVectors;
  neighbors = newNeighbors;
  utilityFunctions = newUtilFuncs;
}

void Rec(vector<int> vec, vector<vector<int>> &retVec, int numPlayers, int numDigits) {
  if(numDigits==0) {
    retVec.push_back(vec);
  }else {
    for(auto i=0; i<=numPlayers; i++) {
      int index = vec.size()-numDigits;
      vec[index] = i;
      Rec(vec, retVec, numPlayers, numDigits-1);
    }
  }
}

vector<vector<int>> configurations(int numPlayers, int numDigits) {
   vector<int> vec(numDigits, 0);
   vector<vector<int>> solution;
   Rec(vec, solution, numPlayers, numDigits);
   return solution;
}

rgg* rgg::makeRandomRGG(int newNumPlayers, int newNumResourceNodes,
			vector<intMatrix> newEqMatrices,
			vector<vector<int>> newEqVectors,
			vector<intMatrix> newLtMatrices,
			vector<vector<int>> newLtVectors,
			vector<vector<int>> newNeighbors){
  vector<trie_map<double>> utilityFunctions(newNumResourceNodes);
  for(auto j=0; j<newNumResourceNodes; j++){
    vector<vector<int>> configs = configurations(newNumPlayers, newNeighbors[j].size());
    for(auto i=0; i<configs.size(); i++){
      utilityFunctions[j].insert(std::make_pair(configs[i], 4.3));
    }
  }
  rgg* r = new rgg(newNumPlayers, newNumResourceNodes, newEqMatrices, newEqVectors, newLtMatrices, newLtVectors, newNeighbors, utilityFunctions);
  return r;
}

void rgg::addDefaultLT() {
  for(int p=0; p<numPlayers; p++) {
      vector<vector<int>> newLTMatrix(2*numResourceNodes, vector<int>(numResourceNodes));
      for(int i=0; i<numResourceNodes; i++) {
        newLTMatrix[i][i] = 1;
        newLTMatrix[i+numResourceNodes][i] = -1;
      }
      ltMatrices.push_back(newLTMatrix);
      vector<int> newLTVector(2*numResourceNodes);
      for(int i=0; i<numResourceNodes; i++) {
        newLTVector[i] = 1;
        newLTVector[i+numResourceNodes] = 0;
      }
      ltVectors.push_back(newLTVector);
  }   
}

vector<vector<int>> rgg::createCompleteGraph(int numResourceNodes) {
  vector<vector<int>> neighbors(numResourceNodes, vector<int>(numResourceNodes));
  for(int i=0; i<numResourceNodes; ++i) {
    for(int j = 0; j<numResourceNodes; ++j) {
      neighbors[i][j] = j;
    }
  }
  return neighbors;
} 

//code for creating normal form RGG
Gambit::GameTableRep* rgg::toNormalForm() {
  vector<vector<vector<int>>> setOfPureStrategyProfiles;
  for(int i=0; i<numPlayers; ++i) {
    vector<vector<int>> possiblePureStrategies = configurations(1,numResourceNodes);
    vector<vector<int>>  playerPureStrategyProfile;
    for(auto p:possiblePureStrategies){
      if(std::get<0>(isFeasible(i, p))) {
        playerPureStrategyProfile.push_back(p);
      } 
    }
    setOfPureStrategyProfiles.push_back(playerPureStrategyProfile);
  }
  Gambit::Array<int> dimensions(numPlayers);
  for(int n=1; n<=numPlayers;n++) {
    dimensions[n] = setOfPureStrategyProfiles[n-1].size();
  }
  Gambit::GameTableRep *g = new Gambit::GameTableRep(dimensions,false);
  for(Gambit::StrategyProfileIterator iter{Gambit::StrategySupportProfile(g)}; !iter.AtEnd(); iter++) {
    Gambit::PureStrategyProfile p = *iter;
    Gambit::GameOutcome o = p->GetOutcome();
    pureStrategyProfile currP;
    for(int pl = 0; pl<numPlayers; pl++) {
      currP.push_back(setOfPureStrategyProfiles[pl][(p->GetStrategy(pl+1)->GetNumber())-1]);
    }
    for(int p = 0; p <numPlayers; p++) {
      //cout << p->GetStrategy(pl+1)->GetNumber() << endl;
      double u = getPureStrategyUtility(p,currP);
      o->SetPayoff(p+1,std::to_string(u));
    }
  } 
  return g;
}
