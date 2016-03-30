#include<iostream>
#include<vector>
#include<utility>
#include "proj_func.h"
#include "trie_map.h"
#include "rgg.h"

using std::vector;
using std::cout;
using std::endl;

int main() {
  
  /*trie_map<double> m;
  vector<int> t {1,2,3};
  m.insert(std::make_pair(t, 2.4));
  cout << (*m.find(t)).second;*/
  vector<vector<vector<int>>> neweqMatrices(2);
  vector<vector<vector<int>>> newLtMatrices{};
  vector<vector<int>> neweqVectors(2);
  vector<vector<int>> newltVectors{};
  vector<vector<int>> neighbors = rgg::createSelfLoopGraph(3); 
  rgg* r = rgg::makeRandomRGG(2, 3, neweqMatrices, neweqVectors, newLtMatrices, newltVectors, neighbors);
  cout << "Testing Create Complete Graph With 3 Resource Nodes" << endl; 
  for(auto a: rgg::createCompleteGraph(3)) {
    for(auto b: a) {
      cout << b << " ";
    }
    cout << endl;
  }
  cout << "Testing Self Loop Graph With 3 Resource Nodes" << endl; 
  for(auto a: rgg::createSelfLoopGraph(3)) {
    for(auto b: a) {
      cout << b << " ";
      cout << endl;
    }
  }
 
  cout << "Created a game with 2 players, 3 resource nodes and a complete graph" << endl;
  cout << "Testing getNumPlayers():" << endl;
  cout << r->getNumPlayers() << endl;
  cout << endl;
  cout << "Testing Utility Functions Creation From makeRandomRGG" << endl;
  for(auto a: r->utilityFunctions) {
    for(auto b: a) {
      for(auto c: b.first) {
        cout << c << " ";
      }
      cout << b.second;
      cout << endl;
    }
    cout << endl;
  } 
  cout << endl;
  cout << "Testing addDefaultLT() Function:" << endl;
  cout << "Making Sure LT Matrices are correct:" << endl;
  r->addDefaultLT();
  for(int i=0;i<2; i++) {
    cout << "Checking Player " << i << endl;
    for(auto a:(r->ltMatrices)[i]){ 
      for(auto b: a){
        cout << b << " ";
      }
      cout << endl;
    }   
    cout << endl;
  }
  //Need to check LT Vectors as well
  cout << "Making sure LT Vectors are correct:" << endl;
  for(int i = 0; i<2; i++) {
    cout << "Checking Player " << i << endl;
    for(auto a: (r->ltVectors)[i]) {
      cout << a << " ";
    }
    cout << endl << endl;
  }
  cout << endl;
  cout << "Testing isFeasible Function:" << endl;
  vector<int> vec{1,1,1};
  std::tuple<bool, valarray<bool>, valarray<bool>> feas = r->isFeasible(0, vec);
  cout << std::get<0>(feas) << endl;
  for(auto a: std::get<1>(feas)) {
    cout << a << " ";
  }
  cout << endl;
  for(auto a: std::get<2>(feas)) {
    cout << a << " ";
  }
  cout << endl << endl;
  cout << "Testing getPureStrategyUtility:" << endl;
  vector<vector<int>> pureStrategyProfile;
  pureStrategyProfile.push_back(vector<int>{1,0,1});
  pureStrategyProfile.push_back(vector<int>{1,0,1});
  cout << r->getPureStrategyUtility(0, pureStrategyProfile) << endl << endl; 
  cout << "Testing getUtilityGradient:" << endl;
  for(auto a: r->getUtilityGradient(0, pureStrategyProfile)) {
    cout << a << " ";
  }
  cout << endl << endl;
  cout << "Testing solveMultiLinear()" << endl; 
  r->solveMultiLinear();
  cout << endl;
  cout << "Creating Normal Form Rep " << endl; 
  r->toNormalForm();
  //Need to test normal form representation here
  return 0;
} 
