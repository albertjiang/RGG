FLAGS= -std=c++11 -I.

#main: libagg/*.o libgambit/*.o rgg.cc testFunctions.cpp;\
  g++ $(FLAGS) -g -o main libagg/*.o libgambit/*.o rgg.cc testFunctions.cpp -lglpk;

main: libagg/*.o libgambit/*.o rgg.cc timedTest.cpp;\
  g++ $(FLAGS) -g -o main libagg/*.o libgambit/*.o rgg.cc ubcsat-time.cc timedTest.cpp -lglpk;
