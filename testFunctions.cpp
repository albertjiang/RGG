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
  //vector<vector<int>> newneighbors{{1,2,3},{1,2,3},{1,2,3}};
  vector<vector<int>> newneighbors{{1,2},{1,2}};  


  rgg* r = rgg::makeRandomRGG(2, 2, neweqMatrices, neweqVectors, newLtMatrices, newltVectors, newneighbors);
  cout << r->getNumPlayers() << endl;
  /*for(auto a: r->utilityFunctions) {
    for(auto b: a.first) {
      cout << b;
    }
    cout << endl;
  }*/
  cout << (*(r->utilityFunctions[0].find(newneighbors[0]))).second << endl;
  
  r->addDefaultLTMatrix(r->getNumPlayers());
  for(auto a: (r->ltMatrices)[0]) { 
    for(auto b: a){
      cout << b << " ";
    }   
    cout << endl;
  }
  for(auto a: (r->ltVectors)[0]) {
    cout << a << endl;
  }

  r->isFeasible(0, vector<int>{0,1,2});
  //cout << std::get<0>(r->isFeasible(0, vector<int>{0,1,2})); 

  return 0;
} 
