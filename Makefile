FLAGS= -std=c++11 -I.

main: libagg/*.o libgambit/*.o rgg.cc testFunctions.cpp;\
  g++ $(FLAGS) -g -o main libagg/*.o libgambit/*.o rgg.cc testFunctions.cpp;
