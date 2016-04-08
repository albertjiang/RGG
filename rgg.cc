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

void rgg::createFeasiblePureStrategyProfiles() {
  if(feasiblePureStrategyProfiles.size() == 0) {
    for(int i=0; i<numPlayers; ++i) {
      vector<vector<int>> possiblePureStrategies = configurations(1,numResourceNodes);
      vector<vector<int>>  playerPureStrategyProfile;
      for(auto p:possiblePureStrategies){
        if(std::get<0>(isFeasible(i, p))) {
         playerPureStrategyProfile.push_back(p);
        }
      }
      feasiblePureStrategyProfiles.push_back(playerPureStrategyProfile);
    }
  }
}

Gambit::GameTableRep* rgg::toNormalForm() {
  /*vector<vector<vector<int>>> setOfPureStrategyProfiles;
  for(int i=0; i<numPlayers; ++i) {
    vector<vector<int>> possiblePureStrategies = configurations(1,numResourceNodes);
    vector<vector<int>>  playerPureStrategyProfile;
    for(auto p:possiblePureStrategies){
      if(std::get<0>(isFeasible(i, p))) {
        playerPureStrategyProfile.push_back(p);
      } 
    }
    setOfPureStrategyProfiles.push_back(playerPureStrategyProfile);
  }*/
  createFeasiblePureStrategyProfiles();
  Gambit::Array<int> dimensions(numPlayers);
  for(int n=1; n<=numPlayers;n++) {
    //dimensions[n] = setOfPureStrategyProfiles[n-1].size();
    dimensions[n] = feasiblePureStrategyProfiles[n-1].size();
  }
  Gambit::GameTableRep *g = new Gambit::GameTableRep(dimensions,false);
  for(Gambit::StrategyProfileIterator iter{Gambit::StrategySupportProfile(g)}; !iter.AtEnd(); iter++) {
    Gambit::PureStrategyProfile p = *iter;
    Gambit::GameOutcome o = p->GetOutcome();
    pureStrategyProfile currP;
    for(int pl = 0; pl<numPlayers; pl++) {
      //cout << endl << p->GetStrategy(pl+1)->GetId() << endl;
      //cout << Gambit::lexical_cast<std::string>((p)->GetStrategy(pl+1)->GetLabel()) << endl;
      //currP.push_back(setOfPureStrategyProfiles[pl][(p->GetStrategy(pl+1)->GetNumber())-1]);
      currP.push_back(feasiblePureStrategyProfiles[pl][(p->GetStrategy(pl+1)->GetNumber())-1]);
    }
    for(int pl = 0; pl <numPlayers; pl++) {
      double u = getPureStrategyUtility(pl,currP);
      o->SetPayoff(pl+1,std::to_string(u));
    }
  }
  return g;
}

void rgg::printNormalFormGame(Gambit::GameTableRep *nfg) {
  std::ostringstream f;
  Gambit::StrategySupportProfile(nfg).WriteNfgFile(f);
  cout << f.str();  
}

Gambit::List<Gambit::GameStrategy> rgg::normalFormBestResponseList(int playerNumber, Gambit::PureStrategyProfile p, Gambit::GameTableRep *nfg) {
 // Gambit::StrategyProfileIterator iter{Gambit::StrategySupportProfile(nfg)};
 // Gambit::PureStrategyProfile p = *iter;
  Gambit::List<Gambit::GameStrategy> bestResponses = p->GetBestResponse(nfg->GetPlayer(playerNumber+1));
  return bestResponses;
}

rgg::pureStrategy rgg::convertNFGStrategyToRGGStrategy(int playerNumber, Gambit::GameStrategy nfgStrategy) {
  createFeasiblePureStrategyProfiles();
  int strategyNumber = (*nfgStrategy).GetNumber();
  /*vector<vector<vector<int>>> setOfPureStrategyProfiles;
  for(int i=0; i<numPlayers; ++i) {
    vector<vector<int>> possiblePureStrategies = configurations(1,numResourceNodes);
    vector<vector<int>>  playerPureStrategyProfile;
    for(auto p:possiblePureStrategies){
      if(std::get<0>(isFeasible(i, p))) {
        playerPureStrategyProfile.push_back(p);
      }
    }
    setOfPureStrategyProfiles.push_back(playerPureStrategyProfile);
  }*/
  return feasiblePureStrategyProfiles[playerNumber][strategyNumber-1];
}

rgg::pureStrategy rgg::nfBestResponseListContainsRGGBestResponse(int playerNumber, Gambit::List<Gambit::GameStrategy> bestResponseList, pureStrategy bestResponse) {
  for(int i = 1; i<=bestResponseList.size(); i++) {
    pureStrategy nfgBestResponse = convertNFGStrategyToRGGStrategy(playerNumber, bestResponseList[i]);
    if(bestResponse == nfgBestResponse)
      return nfgBestResponse;
  }
  cout << endl << endl <<"ERROR: Best Response List Does Not Contain RGG Best Response " << endl << endl;
  return vector<int> {};
}


rgg::pureStrategy rgg::rggBestResponse(int playerID, pureStrategyProfile psp) {
  vector<double> uGradient = this->getUtilityGradient(playerID, psp);
  int ltMatrixRowCount, ltMatrixColCount, ltMatrixSize;
  int eqMatrixRowCount, eqMatrixColCount, eqMatrixSize;
  int problemMatrixCount;
  if(ltMatrices[playerID].size() == 0) {
    ltMatrixRowCount = 0;
    ltMatrixColCount = 0;
    ltMatrixSize = 0;
  } else {
    ltMatrixRowCount = ltMatrices[playerID].size();
    ltMatrixColCount = ltMatrices[playerID][0].size(); 
    ltMatrixSize = ltMatrixRowCount * ltMatrixColCount;
  } 
  if(eqMatrices[playerID].size() == 0) {
    eqMatrixRowCount = 0;
    eqMatrixColCount = 0;
    eqMatrixSize = 0;
  } else { 
    eqMatrixRowCount = eqMatrices[playerID].size();
    eqMatrixColCount = eqMatrices[playerID][0].size(); 
    eqMatrixSize = eqMatrixRowCount * eqMatrixColCount;
  }
  int totalRowCount = ltMatrixRowCount + eqMatrixRowCount;
  problemMatrixCount = ltMatrixColCount * totalRowCount;
  glp_prob *lp;
  int ia[1+problemMatrixCount], ja[1+problemMatrixCount];
  double ar[1+problemMatrixCount];
  lp = glp_create_prob();
  glp_set_obj_dir(lp, GLP_MAX);
  glp_add_rows(lp, totalRowCount);
  if(ltMatrixRowCount > 0) {
    for(int i = 1; i <= ltMatrixRowCount; ++i) {
      glp_set_row_bnds(lp, i, GLP_UP, 0, double(ltVectors[playerID][i-1]));
    }
  }
  if(eqMatrixRowCount > 0) {
    for(int i = 1; i <= eqMatrixRowCount; ++i) {
      glp_set_row_bnds(lp, ltMatrixRowCount + i, GLP_UP, 0, double(eqVectors[playerID][i-1]));
    }
  }
  glp_add_cols(lp, ltMatrixColCount);
  for(int i = 1; i <= ltMatrixColCount; ++i) {
    glp_set_col_bnds(lp, i, GLP_FR, 0, 0);
    glp_set_obj_coef(lp, i, uGradient[i-1]);
  }
  int currRow = 1;
  int currCol = 1;
  if(ltMatrixSize > 0) {
    for(int i = 1; i <= ltMatrixSize; ++i) {
      ia[i] = currRow , ja[i] = currCol , ar[i] = ltMatrices[playerID][currRow-1][currCol-1];
      if(currCol == ltMatrixColCount) {
        currRow += 1;
        currCol = 1;
      }
      else {
        currCol += 1;
      }
    }
  }
  currRow = 1;
  currCol = 1;
  if(eqMatrixSize > 0) {
    for(int i = ltMatrixSize+1; i <= problemMatrixCount; ++i) {
      ia[i] = (currRow + ltMatrixRowCount) , ja[i] = currCol , ar[i] = eqMatrices[playerID][currRow-1][currCol-1];
      if(currCol == ltMatrixColCount) {
        currRow += 1;
        currCol = 1;
      }
      else {
        currCol += 1;
      }
    }
  }
  glp_load_matrix(lp, problemMatrixCount, ia, ja, ar); 
  glp_simplex(lp, NULL);
  double z = glp_get_obj_val(lp);
  //cout << "z = " << z << endl;
  //int cprim[ltMatrixCount];
  pureStrategy ps(ltMatrixColCount);
  for(int i = 1; i <=ltMatrixColCount; i++) {
    ps[i-1] = glp_get_col_prim(lp,i);
    //pureStrategyProfile[playerID][i-1] = glp_get_col_prim(lp, i);
    //cout << "x" << i << " = " << glp_get_col_prim(lp,i) << " ";
  }
  /*cout << endl;
  cout << "i array: ";
  for(int i = 1; i <= problemMatrixCount; ++i)
    cout << ia[i] << " ";
  cout << endl;
  cout << "j array: ";
  for(int i = 1; i <= problemMatrixCount; ++i)
    cout << ja[i] << " ";
  cout << endl;
  cout << "a array: ";
  for(int i = 1; i <= problemMatrixCount; ++i)
    cout << ar[i] << " ";
  cout << endl;
  glp_print_sol(lp, "hello.txt");
  cout << endl << endl;
  cout << "Checking Matrix" << endl;
  for(int i = 1; i <= ltMatrixRowCount; i++) {
    int ind[ltMatrixColCount];
    double val[ltMatrixColCount];
    int len = glp_get_mat_row(lp, i, ind, val);
    cout << "Length " << len << endl;
    cout << "Row " << i << endl;
    cout << "Index and Value pairs: ";
    for(int j = 1; j <= len; j++) {
      cout << ind[j] << " " << val[j] << endl;
    }
    cout << endl;
  }*/
  glp_delete_prob(lp);
  glp_free_env();
  return ps;
} 

void rgg::multiLinearSolve() {
  int playerID;
  vector<vector<int>> pureStrategyProfile(numPlayers, vector<int>(numResourceNodes, 0));
  vector<vector<int>> oldPureStrategyProfile = pureStrategyProfile;
  bool done = false;
  while(done == false) { 
    for(playerID = 0; playerID < numPlayers; playerID++) {   
      pureStrategy ps = rggBestResponse(playerID, pureStrategyProfile);
      pureStrategyProfile[playerID] = ps;
    }
    if(pureStrategyProfile == oldPureStrategyProfile) {
      done = true;
    } else {
      oldPureStrategyProfile = pureStrategyProfile;
    }
  }
  cout << "Printing New Strategy Profile: " << endl;
  for(auto m : pureStrategyProfile) {
    for(auto n: m) {
      cout << n << " ";
    } 
    cout << endl;
  }
  cout << endl << endl;
}

