#include <iostream>
#include <utility>
#include <math.h>
#include "rgg.h"
#include<string>
#include<cstdlib>
#include<ctime>
#include<glpk.h>
#include <cassert>

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
    for(unsigned int i=0; i<a.size(); i++){
      int sum = 0;
      for(unsigned int j=0; j<a[i].size(); j++) {
        sum += a[i][j] * p[j];
      }
      sums.push_back(sum);
    }
    valarray<int> leftside(sums.data(), sums.size());
    valarray<int> rightside(eqVectors[playerID].data(), eqVectors[playerID].size());
    valarray<bool> eqRet = (leftside == rightside);

    rgg::intMatrix &a2 = ltMatrices.at(playerID);
    vector<int> sums2;
    for(unsigned int i=0; i<a2.size(); i++){
      int sum2 = 0;
      for(unsigned int j=0; j<a2[i].size(); j++) {
        sum2 += a2[i][j] * p[j];
      }
      sums2.push_back(sum2);
    }
    valarray<int> leftside2(sums2.data(), sums2.size());
    valarray<int> rightside2(ltVectors[playerID].data(), ltVectors[playerID].size());
    valarray<bool> ltRet = (leftside2 <= rightside2);

    unsigned int ltretsum = 0;
    for(auto a: ltRet) {
      if(a == true)
        ++ltretsum;
    }
    unsigned int eqretsum = 0;
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
  for(unsigned int a=0; a<p.size(); a++){
    for(unsigned int b=0; b<p[a].size(); b++) {
      totalConfig[b] += p[a][b];
    }
  }
  for(int i=0; i<numResourceNodes; i++) {
    if(p[playerID][i] == 1) {
      vector<int> localConfig(neighbors[i].size());
      for(unsigned int j=0; j<neighbors[i].size(); j++) {
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

double rgg::getExpectedUtility(int playerID, const marginalStrategyProfile& p){
  if(!multilinear){
	throw Gambit::UndefinedException();
  }
  auto g=getUtilityGradient(playerID,p);
  valarray<double> gr (g.data(),g.size());
  return (p[playerID] * gr).sum();
}
vector<double> rgg::getUtilityGradient(int playerID, const marginalStrategyProfile &p){
  if(!multilinear){
	throw Gambit::UndefinedException();
  }
  vector<double> utilities(numResourceNodes,0);
  for(int node=0;node<numResourceNodes; node++){
    //init playerID's strat
    distrib Pr;
    vector<int> localConfig(neighbors[node].size(), 0);
    for (unsigned int i=0;i<neighbors[node].size();++i) if (neighbors[node][i] == node) localConfig[i] = 1;
    Pr.insert(make_pair(localConfig,1.0));
    //apply other players strat
    for(int pl=0;pl<numPlayers; ++pl)if (pl!=playerID){
      distrib projStrat;
      for (unsigned int i=0;i<neighbors[node].size();++i){
        vector<int> con (neighbors.size(),0);
        con[i]=1;
	projStrat.insert(make_pair(con, p[pl][neighbors[node][i]]));
      }
      vector<proj_func*> projFunctions (neighbors[node].size(), new proj_func_SUM);
      Pr.multiply(projStrat, neighbors[node].size(), projFunctions);
    }
    //get payoff
    utilities[node] = Pr.inner_prod(utilityFunctions[node]);
  }
  return utilities;
}

vector<double> rgg::getUtilityGradient(int playerID, const pureStrategyProfile &p) {
  if(!multilinear){
	throw Gambit::UndefinedException();
  }
  vector<double> utilities(numResourceNodes,0);
  vector<int> totalConfig(numResourceNodes,0);
  for(int a=0; a<(int)p.size(); a++){
    if(a!=playerID) {
      for(unsigned int b=0; b<p[a].size(); b++) {
       totalConfig[b] += p[a][b];
      }
    }
  }
  for(int i=0; i<numResourceNodes; i++) {
    totalConfig[i] += 1;
    vector<int> localConfig(neighbors[i].size());
    for(unsigned int j=0; j<neighbors[i].size(); j++) {
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
  if (eqMatrices.size()==0) eqMatrices=vector<intMatrix>(numPlayers);
  eqVectors = newEqVectors;
  if (eqVectors.size()==0) eqVectors=vector<vector<int>>(numPlayers);

  ltMatrices = newLtMatrices;
  ltVectors = newLtVectors;
  if(ltMatrices.size()==0) addDefaultLT();

  neighbors = newNeighbors;
  utilityFunctions = newUtilFuncs;
  multilinear = computeIsMultilinear();
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
    for(unsigned int i=0; i<configs.size(); i++){
      double randNum =  (20) * ( (double)rand() / (double)RAND_MAX );
      randNum -= 10;
      utilityFunctions[j].insert(std::make_pair(configs[i], randNum));
    }
  }
  rgg* r = new rgg(newNumPlayers, newNumResourceNodes, newEqMatrices, newEqVectors, newLtMatrices, newLtVectors, newNeighbors, utilityFunctions);
  return r;
}

void rgg::addDefaultLT() {
  if (ltMatrices.size()==0)ltMatrices=vector<intMatrix>(numPlayers);
  if (ltVectors.size()==0)ltVectors=vector<vector<int>>(numPlayers);
  assert((int)ltMatrices.size()==numPlayers && (int)ltVectors.size()==numPlayers);
  for(int p=0; p<numPlayers; p++) {
    assert(ltMatrices[p].size()==ltVectors[p].size());
    if (ltMatrices[p].size()!=0) continue;

    vector<vector<int>> newLTMatrix(2*numResourceNodes, vector<int>(numResourceNodes));
    for(int i=0; i<numResourceNodes; i++) {
      newLTMatrix[i][i] = 1;
      newLTMatrix[i+numResourceNodes][i] = -1;
    }
    ltMatrices[p]=newLTMatrix;
    vector<int> newLTVector(2*numResourceNodes);
    for(int i=0; i<numResourceNodes; i++) {
      newLTVector[i] = 1;
      newLTVector[i+numResourceNodes] = 0;
    }
    ltVectors[p]=newLTVector;
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
  createFeasiblePureStrategyProfiles();
  Gambit::Array<int> dimensions(numPlayers);
  for(int n=1; n<=numPlayers; n++) {
    dimensions[n] = feasiblePureStrategyProfiles[n-1].size();
  }
  Gambit::GameTableRep *g = new Gambit::GameTableRep(dimensions,false);
  for(Gambit::StrategyProfileIterator iter{Gambit::StrategySupportProfile(g)}; !iter.AtEnd(); iter++) {
    Gambit::PureStrategyProfile p = *iter;
    Gambit::GameOutcome o = p->GetOutcome();
    pureStrategyProfile currP;
    for(int pl = 0; pl<numPlayers; pl++) {
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
  Gambit::List<Gambit::GameStrategy> bestResponses = p->GetBestResponse(nfg->GetPlayer(playerNumber+1));
  return bestResponses;
}

rgg::pureStrategy rgg::convertNFGStrategyToRGGStrategy(int playerNumber, Gambit::GameStrategy nfgStrategy) {
  createFeasiblePureStrategyProfiles();
  int strategyNumber = (*nfgStrategy).GetNumber();
  return feasiblePureStrategyProfiles[playerNumber][strategyNumber-1];
}

rgg::pureStrategy rgg::nfBestResponseListContainsRGGBestResponse(int playerNumber, Gambit::List<Gambit::GameStrategy> bestResponseList, pureStrategy bestResponse) {
  for(unsigned int i = 1; i<=bestResponseList.size(); i++) {
    pureStrategy nfgBestResponse = convertNFGStrategyToRGGStrategy(playerNumber, bestResponseList[i]);
    cout << endl;
    if(bestResponse == nfgBestResponse)
      return nfgBestResponse;
  }
  cout << endl << endl <<"ERROR: Best Response List Does Not Contain RGG Best Response " << endl << endl;
  return vector<int> {};
}


bool rgg::computeIsMultilinear() {
  bool res=true;
  for(int node=0;node<numResourceNodes;++node){
    for(int oPlayer=0;oPlayer<numPlayers; ++oPlayer){
      vector<double> coeffs(numResourceNodes, 0.0);
      coeffs[node]=1;
      for(auto nei: neighbors[node]) coeffs[nei]=1;
      auto t=linOpt(oPlayer, coeffs);
      double obj=std::get<1>(t);
      if (round(obj)>=2 ) return false;
    }
  }
  return res;
}
 
rgg::pureStrategy rgg::rggBestResponse(int playerID, const pureStrategyProfile& psp) {
  vector<double> uGradient = this->getUtilityGradient(playerID, psp);
  return std::get<0>(linOpt(playerID, uGradient));
}

rgg::pureStrategy rgg::rggBestResponse(int playerID, const marginalStrategyProfile& msp) {
  vector<double> uGradient = this->getUtilityGradient(playerID, msp);
  return std::get<0>(linOpt(playerID, uGradient));
}
double rgg::checkEpsNash(const marginalStrategyProfile& msp){
  double eps=0;
  for(int i=0;i<numPlayers;++i){
    vector<double> uGradient = this->getUtilityGradient(i, msp);
    double bru=std::get<1>(linOpt(i,uGradient));
    valarray<double> gr (uGradient.data(),uGradient.size());
    double eu= (msp[i] * gr).sum();
    eps=std::max(eps, bru-eu);
  }
  return eps;
}

std::tuple<rgg::pureStrategy,double> rgg::linOpt(int playerID, const vector<double>& objCoeffs) {

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
    glp_set_obj_coef(lp, i, objCoeffs[i-1]);
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
  pureStrategy ps(ltMatrixColCount);
  for(int i = 1; i <=ltMatrixColCount; i++) {
    ps[i-1] = glp_get_col_prim(lp,i);
  }
  glp_delete_prob(lp);
  glp_free_env();
  return std::make_tuple(ps,z);
} 

rgg::marginalStrategyProfile rgg::fictitiousPlay(int numIter){
  vector<vector<int>> psp(numPlayers, vector<int>(numResourceNodes, 0));


  for(int i=0;i<numPlayers;++i)
    if(! std::get<0>(isFeasible(i, psp[i]))){
      cout<<"Player "<<i<<" init strategy infeasible"<<endl;
      vector<double> coeffs(numResourceNodes, 1.0);
      psp[i]=std::get<0>(linOpt(i, coeffs));
      cout<<"Instead using strategy ";
      for(auto x:psp[i]) cout<<x<<" ";
      cout<<endl;
  }

  marginalStrategyProfile msp(numPlayers, marginalStrategy(numResourceNodes));
  for(int i=0;i<numPlayers;++i)for(int j=0;j<numResourceNodes;++j) msp[i][j]=psp[i][j];

  marginalStrategyProfile oldmsp=msp;
  //bool done=false;
  int iter=1;
  for(;iter<=numIter;++iter, oldmsp=msp){
    for(int i=0;i<numPlayers;++i){
      pureStrategy ps=rggBestResponse(i, msp); //non-simultaneous: respond to msp instead of oldmsp
      marginalStrategy ms(numResourceNodes);
      for(int j=0;j<numResourceNodes; ++j)ms[j]=ps[j];
      msp[i] +=  (ms- msp[i]) / iter;

      //auto diff=msp[i]-oldmsp[i];
      //std::max(std::abs(diff.max()), std::abs( diff.min()));
    }
    
  }
  cout << "Printing New Strategy Profile: " << endl;
  for(auto m : msp) {
    for(auto n: m) {
      cout << n << " ";
    } 
    cout << endl;
  }
  cout << endl << endl;
  return msp;
}



rgg::pureStrategyProfile rgg::iterBRSolve() {
  int playerID;
  vector<vector<int>> psp(numPlayers, vector<int>(numResourceNodes, 0));


  for(int i=0;i<numPlayers;++i)
    if(! std::get<0>(isFeasible(i, psp[i]))){
      cout<<"Player "<<i<<" init strategy infeasible"<<endl;
      vector<double> coeffs(numResourceNodes, 1.0);
      psp[i]=std::get<0>(linOpt(i, coeffs));
      cout<<"Instead using strategy ";
      for(auto x:psp[i]) cout<<x<<" ";
      cout<<endl;
  }
  vector<vector<int>> oldPureStrategyProfile = psp;
  bool done = false;
  while(done == false) { 
    for(playerID = 0; playerID < numPlayers; playerID++) {   
      pureStrategy ps = rggBestResponse(playerID, psp);
      psp[playerID] = ps;
    }
    if(psp == oldPureStrategyProfile) {
      done = true;
    } else {
      oldPureStrategyProfile = psp;
    }
  }
  cout << "Printing New Strategy Profile: " << endl;
  for(auto m : psp) {
    for(auto n: m) {
      cout << n << " ";
    } 
    cout << endl;
  }
  cout << endl << endl;
  return psp;
}

rgg::pureStrategyProfile rgg::convertNFGPureStrategyProfileToRGGFormat(Gambit::PureStrategyProfile psp) {
  pureStrategyProfile newpsp;
  for(int n = 0; n < getNumPlayers(); n++) {
    newpsp.push_back(convertNFGStrategyToRGGStrategy(n, psp->GetStrategy(n+1)));
  }
  return newpsp;
}
