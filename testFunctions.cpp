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
  vector<vector<vector<int>>> eqMatrices{};
  vector<vector<vector<int>>> ltMatrices{};
  vector<vector<int>> eqVectors{};
  vector<vector<int>> ltVectors{};
  vector<vector<int>> neighbors{};
  rgg* r = rgg::makeRandomRGG(2, 4, eqMatrices, eqVectors, ltMatrices, ltVectors, neighbors);
  cout << r->getNumPlayers() << endl;
  for(auto a: r->utilityFunctions) {
    cout << a.second << endl;
  } 
  return 0;
} 
