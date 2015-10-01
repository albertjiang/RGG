#ifndef _AGG_H
#define _AGG_H

#include <iostream>
#include <vector>
#include <iterator>
#include <tuple>
#include "trie_map.h"

using std::vector;
using std::tuple;

class rgg {
	
	public:		
		typedef vector<int> pureStrategy;
		typedef vector<pureStrategySet> pureStrategyProfile;
		typedef vector<vector<int>>	intMatrix;
	
		int numPlayers;
		int numResourceNodes;
		vector<intMatrix> eqMatrices;
		vector<vector<int>> eqVectors;
		vector<intMatrix> leMatrices;
		vector<vector<int>> leVectors;
		vector<vector<int>> neighbors;
		
		rgg(int numPlayers, int numResourceNodes,
			vector<intMatrix>& eqMatrices,
			vector<int>& eqVectors,
			vector<intMatrix>& leMatrices,
			vector<int>& leVectors,
			vector<vector<int>>& neighbors,
			vector<trie_map<double>> utilityFunctions); 	
		
		int getNumPlayers() {return numPlayers;}
		int getNumResourceNodes() {return numResourceNodes;}
		
		tuple<bool, int> isFeasible(int playerID, pureStrategy p);
		double utility(int playerID, pureStrategyProfile &p);
}
