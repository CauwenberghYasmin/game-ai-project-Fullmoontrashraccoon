#include "BFS.h"

#include <map>
#include <queue>

#include "AStar.h"
#include "Heuristics.h"
#include "Shared/Graph/Graph.h"
#include <Utils.h>
#include <algorithm>

using namespace GameAI;

BFS::BFS(Graph* const pGraph)
	: pGraph(pGraph)
{
}

// TODO Breath First Search Algorithm searches for a path from the startNode to the destinationNode
std::vector<Node*> BFS::FindPath(Node* const pStartNode, Node* const pDestinationNode) const //I'm confuseds to why use bfs is we saw the A*, so I decided to use that, since I couldn't find where in the code to implement the A*, and it wasn't mentioned in the ppt
{																							// I also couldn't really understand the AStar class, so I hope this implementation is also okay
	std::vector<Node*> path;
	std::vector<std::pair<Node*,size_t>> openList;
	std::vector<Node*> closedList;
	Node* currentNodeRecord {pStartNode};
	
	size_t currentCost {0};
	
	std::vector<Node*> Nodes = pGraph->GetActiveNodes();
	if (Nodes.size() == 0)
		return path;
	
	openList.push_back(std::move(std::pair(currentNodeRecord, INT_MAX))); //has max because should be furthest away from the end
	
	const FVector2D desinationPosition {pGraph->GetNode(pDestinationNode->GetId())->GetPosition()};
	
	while (openList.size() != 0)
	{
		++currentCost;
		
		auto ConnectedNodes {pGraph->FindConnectionsFrom(currentNodeRecord->GetId())};
		for (const auto& ConnectedNode : ConnectedNodes) //adding all neighbours to list
		{
			int desinationNodeID = ConnectedNode->GetToId();
			auto foundNode = (pGraph->GetNode(desinationNodeID)).get();
			
			//calc cost
			FVector2D toDestination = desinationPosition- pGraph->GetNode(pStartNode->GetId())->GetPosition();
			size_t cost = currentCost + HeuristicFunctions::SqEuclidean(abs(toDestination.X), abs(toDestination.Y));
			
			
			auto node2 = std::find(openList.begin(), openList.end(), foundNode);
			if (node2 != openList.end())
			{
				if (node2->second >= cost) 
				{
					openList.erase(node2);
				}
			}
			
			openList.push_back(std::move(std::pair(foundNode, cost)));
		}
		closedList.push_back(currentNodeRecord);//add original to the closed list
		
		auto it = std::min_element(openList.begin(), openList.end(), 
		[](const auto& a, const auto& b) {
			 return a.second < b.second;});

		currentNodeRecord = it->first;	//picking new current with shortest distance
		openList.erase(it); 
		
		if (currentNodeRecord == pDestinationNode) //go out loop
			break;
	}
	
	std::reverse(closedList.begin(), closedList.end());
	return closedList;
}























