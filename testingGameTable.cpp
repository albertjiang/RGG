#include "./libgambit/libgambit.h"
#include "./libgambit/gametable.h"

#include<iostream>

int main() {
  const Gambit::Array<int> numActions = 3;
  Gambit::GameTableRep *g = new Gambit::GameTableRep(numActions, false);
  Gambit::StrategyProfileIterator iter(Gambit::StrategySupportProfile(g));  
  cout << 1;
}
