#ifndef _AGG_H
#define _AGG_H

#include <iostream>
#include <vector>
#include <iterator>
#include <tuple>

using std::vector;
using std::tuple;

class rgg {
	
	public:		
		//Si intersection with Pi; I don't know what Pi is
		typedef vector<int> pureStrategySet;
		typedef vector<pureStrategySet> pureStrategyProfile;
		
		int numPlayers;
		int numResourceNodes;
		//need to represent Ax <= b
		vector<vector<int>> a;
		vector<int> x;
		vector<int> b;
		//the graph
		vector<vector<int> neighbors;
		
		//constructor -- Why all references?
		rgg(int numPlayers, int numResourceNodes,
			vector<vector<int>>& pureStrategies,
			vector<vector<int>>& a,
			vector<int>& x,
			vector<int>& b,
			vector<vector<int>>& neighbors);
		
		//public methods or getting data
		int getNumPlayers() {return numPlayers;}
		int getNumResourceNodes() {return numResourceNodes;}
		
		tuple<bool, int> isFeasible(pureStrategySet p);
		double	utility(pureStrategyProfile &p);
}
