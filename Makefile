CXXFLAGS= -std=c++11 -I. -I/users/xjiang/include -L/users/xjiang/lib 
LIBAGG_OBJS= libagg/agg.o libagg/bagg.o
default: main


#main: libagg/*.o libgambit/*.o rgg.cc testFunctions.cpp;\
  g++ $(CXXFLAGS) -g -o main libagg/*.o libgambit/*.o rgg.cc testFunctions.cpp -lglpk;

main: $(LIBAGG_OBJS) libgambit/*.o rgg.o ubcsat-time.o timedTest.o;\
  g++ $(CXXFLAGS) -g -o main $(LIBAGG_OBJS) libgambit/*.o rgg.o ubcsat-time.o timedTest.o -lglpk;

#main: libagg/*.o libgambit/*.o rgg.cc timedRGGSolverTest.cpp;\
  g++ $(CXXFLAGS) -g -o main libagg/*.o libgambit/*.o rgg.cc ubcsat-time.cc timedRGGSolverTest.cpp -lglpk;

#main: libagg/*.o libgambit/*.o rgg.cc extendedTimedTest.cpp;\
  g++ $(CXXFLAGS) -g -o main libagg/*.o libgambit/*.o rgg.cc ubcsat-time.cc extendedTimedTest.cpp -lglpk;
