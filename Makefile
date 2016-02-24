FLAGS= -std=c++11 -I.

GameTableTest: libagg/*.cc libgambit/*.cc testingGameTable.cpp;\
  g++ $(FLAGS) -o GameTableTest libagg/*.cc libgambit/*.cc testingGameTable.cpp;
