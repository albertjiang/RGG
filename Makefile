CXXFLAGS= -std=c++11 -I. -I/users/xjiang/include -L/users/xjiang/lib 
LIBAGG_OBJS= libagg/agg.o libagg/bagg.o
LIBGAMBIT_OBJS= libgambit/game.o libgambit/gametable.o libgambit/rational.o libgambit/integer.o libgambit/vector.o libgambit/mixed.o \
    libgambit/gameagg.o libgambit/gamebagg.o libgambit/gametree.o libgambit/stratspt.o libgambit/stratitr.o \
    libgambit/behav.o libgambit/behavitr.o libgambit/behavspt.o libgambit/pvector.o libgambit/dvector.o \
    libgambit/matrix.o libgambit/file.o libgambit/tinyxml.o libgambit/tinyxmlparser.o libgambit/tinyxmlerror.o

default: main

rgg.o:rgg.h rgg.cc

clean:
	rm *.o libgambit/*.o libagg/*.o main


#main: libagg/*.o libgambit/*.o rgg.cc testFunctions.cpp;\
  g++ $(CXXFLAGS) -g -o main libagg/*.o libgambit/*.o rgg.cc testFunctions.cpp -lglpk;

main: $(LIBAGG_OBJS) $(LIBGAMBIT_OBJS) rgg.o ubcsat-time.o timedTest.o;\
  g++ $(CXXFLAGS) -g -o main $(LIBAGG_OBJS) $(LIBGAMBIT_OBJS) rgg.o ubcsat-time.o timedTest.o -lglpk;

#main: libagg/*.o libgambit/*.o rgg.cc timedRGGSolverTest.cpp;\
  g++ $(CXXFLAGS) -g -o main libagg/*.o libgambit/*.o rgg.cc ubcsat-time.cc timedRGGSolverTest.cpp -lglpk;

#main: libagg/*.o libgambit/*.o rgg.cc extendedTimedTest.cpp;\
  g++ $(CXXFLAGS) -g -o main libagg/*.o libgambit/*.o rgg.cc ubcsat-time.cc extendedTimedTest.cpp -lglpk;
