#include <iostream>
#include <utility>
#include <math.h>
#include "rgg.h"
#include<string>
#include<cstdlib>
#include<ctime>
#include<glpk.h>

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

vector<double> rgg::getUtilityGradient(int playerID, pureStrategyProfile &p) {
  /*vector<pureStrategy> pureStrats(p);
  for(int a = 0; a < pureStrats[playerID].size(); a++) {
    pureStrats[playerID][a] = 0;
  }*/
  vector<double> utilities(numResourceNodes,0);
  vector<int> totalConfig(numResourceNodes,0);
  for(int a=0; a<p.size(); a++){
    if(a!=playerID) {
      for(int b=0; b<p[a].size(); b++) {
       totalConfig[b] += p[a][b];
      }
    }
  }
  for(int i=0; i<numResourceNodes; i++) {
    totalConfig[i] += 1;
    vector<int> localConfig(neighbors[i].size());
    for(int j=0; j<neighbors[i].size(); j++) {
      localConfig[j] = totalConfig[neighbors[i][j]];
    }
    trie_map<double>::iterator curr = utilityFunctions[i].findExact(localConfig);
    if(curr == utilityFunctions[i].end()) {
      cout << "Error! Local Configuration  ";
      for(auto a: localConfig)
        cout << a << " ";
      cout << "Was Not Found";
      cout << endl << endl;
      exit(1);
    } else {
        utilities[i] = curr->second;
    }
    totalConfig[i] -= 1;
  }
  return utilities;
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
  srand(time(NULL));
  vector<trie_map<double>> utilityFunctions(newNumResourceNodes);
  for(auto j=0; j<newNumResourceNodes; j++){
    vector<vector<int>> configs = configurations(newNumPlayers, newNeighbors[j].size());
    for(auto i=0; i<configs.size(); i++){
      double randNum =  (100) * ( (double)rand() / (double)RAND_MAX );
      utilityFunctions[j].insert(std::make_pair(configs[i], randNum));
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

vector<vector<int>> rgg::createSelfLoopGraph(int numResourceNodes) {
  vector<vector<int>> neighbors(numResourceNodes, vector<int>(1));
  for(int i=0; i<numResourceNodes; ++i) {
    neighbors[i][0] = i;
  }
  return neighbors;
}

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
      double u = getPureStrategyUtility(p,currP);
      o->SetPayoff(p+1,std::to_string(u));
    }
  } 
  return g;
}

void rgg::solveMultiLinear() {
  vector<vector<int>> pureStrategyProfile(numPlayers, vector<int>(numResourceNodes, 0));
  vector<double> uGradient = this->getUtilityGradient(0, pureStrategyProfile);
  int ltMatrixRowCount = ltMatrices[0].size();
  int ltMatrixColCount = ltMatrices[0][0].size(); 
  int ltMatrixSize = ltMatrixRowCount * ltMatrixColCount;
  cout << endl << endl << ltMatrixSize << endl << endl;
  glp_prob *lp;
  int ia[1+ltMatrixSize], ja[1+ltMatrixSize];
  double ar[1+ltMatrixSize], z, x1, x2;
  lp = glp_create_prob();
  glp_set_obj_dir(lp, GLP_MAX);
  glp_add_rows(lp, ltMatrixRowCount);
  for(int i = 1; i <= ltMatrixRowCount; ++i) {
    glp_set_row_bnds(lp, i, GLP_UP, 0, double(ltVectors[0][i]));
  }
  //I don't think I need to worry about col bounds right now
  glp_add_cols(lp, ltMatrixColCount);
  for(int i = 1; i <= ltMatrixColCount; ++i) {
    glp_set_obj_coef(lp, i, uGradient[i]);
  }
  int currRow = 1;
  int currCol = 1;
  for(int i = 1; i <= ltMatrixSize; ++i) {
    ia[i] = currRow , ja[i] = currCol , ar[i] = ltMatrices[0][currRow][currCol];
    if( (currCol % ltMatrixColCount) == 0) {
      currRow += 1;
      currCol = 1;
    }
    else {
      currCol += 1;
    }
  }
  glp_load_matrix(lp, ltMatrixSize, ia, ja, ar); 
  glp_simplex(lp, NULL);
  z = glp_get_obj_val(lp);
  x1 = glp_get_col_prim(lp, 1);
  x2 = glp_get_col_prim(lp, 2);
  printf("z = %g; x1 = %g; x2 = %g\n", z, x1, x2);
  glp_delete_prob(lp);
  glp_free_env();
  /*vector<vector<int>> pureStrategyProfile(numPlayers, vector<int>(numResourceNodes, 0));
  for(int p = 0; p < numPlayers; ++p) {
    vector<double> uGradient = this->getUtilityGradient(p, pureStrategyProfile);
    int usize = uGradient.size();
  }*/
}
