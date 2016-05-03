#ifndef __RGG_H
#define __RGG_H
#include<valarray>
#include <vector>
#include <iterator>
#include <tuple>
#include "proj_func.h"
#include "trie_map.h"
#include "./libgambit/libgambit.h"
#include "./libgambit/gametable.h"

using std::vector;
using std::tuple;
using std::valarray;

class rgg {

  public:
    typedef vector<int> pureStrategy;
    typedef vector<pureStrategy> pureStrategyProfile;
    typedef vector<vector<int>>	intMatrix;

    int numPlayers;
    int numResourceNodes;
    vector<intMatrix> eqMatrices;
    vector<vector<int>> eqVectors;
    vector<intMatrix> ltMatrices;
    vector<vector<int>> ltVectors;
    vector<vector<int>> neighbors;
    vector<trie_map<double>> utilityFunctions;
    vector<pureStrategyProfile> feasiblePureStrategyProfiles;

    rgg(int numPlayers, int numResourceNodes,
      vector<intMatrix> eqMatrices,
      vector<vector<int>> eqVectors,
      vector<intMatrix> ltMatrices,
      vector<vector<int>> ltVectors,
      vector<vector<int>> neighbors,
      vector<trie_map<double>> utilityFunctions);

    static rgg* makeRandomRGG(int n, int r,
      vector<intMatrix> eqMats,
      vector<vector<int>> eqVecs,
      vector<intMatrix> ltMats,
      vector<vector<int>> ltVecs,
      vector<vector<int>> neigh);

    int getNumPlayers() {return numPlayers;}
    int getNumResourceNodes() {return numResourceNodes;}

    tuple<bool, valarray<bool>, valarray<bool>> isFeasible(int playerID, pureStrategy& p);
    double getPureStrategyUtility(int playerID, pureStrategyProfile &p);
    vector<double> getUtilityGradient(int playerID, pureStrategyProfile &p);
    
    void addDefaultLT();

    static vector<vector<int>> createCompleteGraph(int numResourceNodes);
    static vector<vector<int>> createSelfLoopGraph(int numResourceNodes);

    Gambit::GameTableRep* toNormalForm();

    void multiLinearSolve(); 
    Gambit::List<Gambit::GameStrategy> normalFormBestResponseList(int playerNumber, Gambit::PureStrategyProfile, Gambit::GameTableRep *nfg);
    pureStrategy convertNFGStrategyToRGGStrategy(int playerNumber, Gambit::GameStrategy);
    pureStrategy nfBestResponseListContainsRGGBestResponse(int playerNumber, Gambit::List<Gambit::GameStrategy> bestResponseList, pureStrategy bestResponse);
    void createFeasiblePureStrategyProfiles();
    void printNormalFormGame(Gambit::GameTableRep *nfg);

    pureStrategy rggBestResponse(int playerNumber, pureStrategyProfile psp);

    pureStrategyProfile convertNFGPureStrategyProfileToRGGFormat(Gambit::PureStrategyProfile);

};

#endif
