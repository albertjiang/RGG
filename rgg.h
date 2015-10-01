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
		
		int numPlayers;
		int numResourceNodes;
		vector<vector<vector<int>>> a's;
		vector<vector<int>> b's;
		vector<vector<int> neighbors;
		
		rgg(int numPlayers, int numResourceNodes,
			vector<vector<int>>& a,
			vector<int>& b,
			vector<vector<int>>& neighbors,
			vector<trie_map<double>> utilityFunctions); 	
		
		int getNumPlayers() {return numPlayers;}
		int getNumResourceNodes() {return numResourceNodes;}
		
		tuple<bool, int> isFeasible(int playerID, pureStrategy p);
		double utility(int playerID, pureStrategyProfile &p);
}
