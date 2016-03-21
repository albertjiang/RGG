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
    
    void addDefaultLT();

    static vector<vector<int>> createCompleteGraph(int numResourceNodes);

    Gambit::GameTableRep* toNormalForm();
};

#endif
