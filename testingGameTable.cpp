#include "./libgambit/libgambit.h"
#include "./libgambit/gametable.h"

#include<iostream>
using Gambit::StrategyProfileIterator;

int main() {
  Gambit::Array<int> numActions(3);
  //for(Gambit::Array<int>::iterator i = numActions.begin(); i!=numActions.end(); ++i) {
  //  *i = 9;
  //}
  for(int i=1; i<=3; i++) {
    numActions[i] = 9;
  }
  cout << "Num actions size " << numActions.size() << endl;
  cout << numActions[1] << endl;
  Gambit::GameTableRep *g = new Gambit::GameTableRep(numActions, false);
  Gambit::StrategyProfileIterator iter{Gambit::StrategySupportProfile(g)};  
  cout << (*iter)->GetStrategy(1) << endl;
  iter.operator++();
  cout << *iter << endl;
}
