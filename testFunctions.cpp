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
  vector<vector<vector<int>>> neweqMatrices{};
  vector<vector<vector<int>>> newLtMatrices{};
  vector<vector<int>> neweqVectors{};
  vector<vector<int>> newltVectors{};
  vector<vector<int>> neighbors = rgg::createCompleteGraph(3); 
  cout << "Testing Create Complete Graph With 3 Resource Nodes" << endl; 
  for(auto a: neighbors) {
    for(auto b: a) {
      cout << b << " ";
    }
    cout << endl;
  }
  cout << endl;
  rgg* r = rgg::makeRandomRGG(2, 3, neweqMatrices, neweqVectors, newLtMatrices, newltVectors, neighbors);
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
      cout << endl;
    }
    cout << endl;
  } 
  cout << endl;
  cout << "Testing addDefaultLT() Function:" << endl;
  cout << "Making Sure LT Matrices are correct:" << endl;
  r->addDefaultLT();
  for(auto a: (r->ltMatrices)[0]) { 
    for(auto b: a){
      cout << b << " ";
    }   
    cout << endl;
  }
  cout << "Making sure LT Vectors are correct:" << endl;
  for(auto a: (r->ltVectors)[0]) {
    cout << a << endl;
  }
  cout << endl;
  cout << "Testing isFeasible Function:" << endl;
  r->isFeasible(0, vector<int>{0,1,2});
  //cout << std::get<0>(r->isFeasible(0, vector<int>{0,1,2})); 
  return 0;
} 
