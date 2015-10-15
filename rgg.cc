#include <iostream>
#include "rgg.h"

tuple<bool, int> isFeasible(int playerID, pureStrategy p) {
  if(linearSolver(p, eqMatrices, eqVectors, ltMatrices, ltVectors)) {
    return {true, playerID};
  } else {
    return {false, -1};
  }
}

double getPureStrategyUtility(int playerID, pureStrategyProfile &p) {
	double u=0;
	vector<int> totalConfig(numResourceNodes,0);
	//Going through the pure strategy profile, and adding 1 to
	//total config value at index whenever a resource node is
	//found in the profile.
	for(int a=0; a<p.size(); a++){
    for(int b=0; b<p[a].size(); b++) {
      totalConfig[pureStrategyProfile[a][b]] += 1;
    }
	}
	for(int i=0; i<numResourceNodes; i++) {
			if(p[playerId][i]) {
				vector<int> localConfig(neighbors[i].size);
				for(int j=0; j<neighbors[i].size(); j++) {
					localConfig[j] = totalConfig[neighbors[i][j]];
				}
        trie_map<double> p = findExact(localConfig);
				if(currMap == data.end()) {
          cout << "Error! Local Configuration Not Found";
          exit(1);
        } else {
          u += p->second;
				}
			}
	}
	return u;
}




//we will possibly test this with a makeRandomRGG function
//random utilities
//you can have different kinds of graphs. self loop graph would give us congestion games, the other thing we can have is a complete graph.
//don't need a complex generator, just add to gambits' generator.
//generating random utilities. don't need to generate different graphs.
//for now just have two graphs complete graph and self loop graph.
