#include<iostream>
#include<vector>
#include<utility>
#include "proj_func.h"
#include "trie_map.h"

using std::vector;
using std::cout;

int main() {
  
  trie_map<double> m;
  vector<int> t {1,2,3};
  m.insert(std::make_pair(t, 2.4));
  cout << (*m.find(t)).second;
  
  return 0;
}
