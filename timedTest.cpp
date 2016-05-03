#include<iostream>
#include<vector>
#include<utility>
#include "proj_func.h"
#include "trie_map.h"
#include "rgg.h"
#include <chrono>
#include "ubcsat-time.h"
#include <tuple>

using std::get;
using std::vector;
using std::cout;
using std::endl;
using std::cin;

typedef std::chrono::high_resolution_clock Clock;

int main() {
  //cout << "Testing Game with 10 Resource Nodes and 10 Players: " << endl << endl;
  int numPlayers;
  cout << "Enter in the number of players playing this game: ";
  cin >> numPlayers;
  int numNodes;
  cout << "Enter in the number of resource nodes in this game: ";
  cin >> numNodes;
  cout << endl << "Testing Game with " << numPlayers << " Players and " << numNodes << " Resource Nodes: " << endl << endl;
  
  auto t9 = Clock::now();
  vector<vector<vector<int>>> neweqMatrices(numPlayers);
  vector<vector<vector<int>>> newLtMatrices{};
  vector<vector<int>> neweqVectors(numPlayers);
  vector<vector<int>> newltVectors{};
  vector<vector<int>> neighbors = rgg::createSelfLoopGraph(numNodes);
  rgg* r = rgg::makeRandomRGG(numPlayers,numNodes, neweqMatrices, neweqVectors, newLtMatrices, newltVectors, neighbors);
  
  r->addDefaultLT();
  auto t10 = Clock::now();
  auto makeRGGTime = std::chrono::duration_cast<std::chrono::nanoseconds>(t10 - t9).count();
  std::cout << "Time Taken to create Resource Graph version of Game: " 
              << makeRGGTime
              << " nanoseconds" << std::endl << endl;
  //r->multiLinearSolve();
  
  //Normal Form Best Response
  auto t11 = Clock::now();
  int playerNumber = 0;
  Gambit::GameTableRep* nfg = r->toNormalForm();
  auto t12 = Clock::now();
  auto makeNFGTime = std::chrono::duration_cast<std::chrono::nanoseconds>(t12 - t11).count();
  std::cout << "Time Taken to create Normal Form version of Game: " 
              << makeNFGTime
              << " nanoseconds" << std::endl << endl;
  cout << "Creating RGG is " << (double)makeNFGTime/(double)makeRGGTime << " times faster than creating NFG." << endl << endl;
  //r->multiLinearSolve();
  //r->printNormalFormGame(nfg);
  Gambit::StrategyProfileIterator iter{Gambit::StrategySupportProfile(nfg)};
  Gambit::PureStrategyProfile p = *iter;
  
  auto t1 = Clock::now();
  Gambit::List<Gambit::GameStrategy> bestResponseList = r->normalFormBestResponseList(playerNumber, p, nfg);
  auto t2 = Clock::now();
  auto nfgBestResponseTime = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
  std::cout << "Time Taken For NFG Best Response(): " 
              << nfgBestResponseTime
              << " nanoseconds" << std::endl;
  
  for(int i = 1; i<=bestResponseList.size(); i++) {
    cout << endl;
    cout << "NFG Best Response Number " << i <<  endl;
    Gambit::GameStrategy gs = bestResponseList[i];
    for(auto c: r->convertNFGStrategyToRGGStrategy(playerNumber, gs)) {
      cout << c << " ";
    }
    cout << endl;
  }
  //RGG Best Response
  rgg::pureStrategyProfile rggPsp =  r->convertNFGPureStrategyProfileToRGGFormat(p);
  /*for(auto a:rggPsp) {
    for(auto b: a){
      cout << b << " ";
    }
    cout << endl;
  }*/
  cout << endl;
  auto t3 = Clock::now();
  rgg::pureStrategy rggBR = r->rggBestResponse(playerNumber, rggPsp);
  auto t4 = Clock::now();
  cout << endl;
  auto rggBestResponseTime = std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();
  std::cout << "Time Taken For RGG Best Response(): " 
              << rggBestResponseTime
              << " nanoseconds" << std::endl << endl;
 
  cout << "RGG Best Response" << endl;
  for(auto p : rggBR) {
    cout << p << " ";
  }
  cout << endl << endl;
  cout << "RGG Best Response Algorithm is " << (double)nfgBestResponseTime/(double)rggBestResponseTime << " times as fast as NFG Best Response. " << endl << endl;
  
  int rggSize = 0;
  for(auto t: r->utilityFunctions) {
   rggSize += t.size(); 
  }
  cout << "Size of the RGG Representation: " << rggSize << endl;
  int nfgSize = 1;
  for(auto f : r->feasiblePureStrategyProfiles) {
    nfgSize *= f.size();
  }
  cout << "Size of the NFG Representation: " << nfgSize << endl;
}
