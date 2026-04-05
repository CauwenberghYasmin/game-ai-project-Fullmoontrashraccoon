#include "NavGraphPathfinding.h"

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos,
	NavGraph* const pNavGraph, std::vector<FVector2D>& debugNodePositions, std::vector<NavLine>& debugPortals) 
{
	//Create the path to return
	std::vector<FVector2D> finalPath{};

	//Get the start and endTriangle
	const TriPolygon* polyGon = pNavGraph->GetNavPolygon();
	if (polyGon == nullptr) return finalPath;
	
	const TriPolygon::Triangle* startTriangle = polyGon->GetTriangleAtPosition(startPos, true);
	const TriPolygon::Triangle* endTriangle = polyGon->GetTriangleAtPosition(endPos, true);
	
	//We have valid start/end triangles and they are not the same
	if (startTriangle == nullptr || endTriangle == nullptr || startTriangle == endTriangle) return finalPath;	
	
	//=> Start looking for a path
	//Copy the graph
	auto graphClone = pNavGraph->Clone(); //returns a unique pointer, but the ppt mentions shared... ->ask teacher!
	
	//Create Extra node for the Start Node (Agent's position	
	std::unique_ptr<NavGraphNode> startNode = std::make_unique<NavGraphNode>(NavGraphNode( startPos, -1)); //imma assume startnode = agent pos
	graphClone->AddNode(std::move(startNode));
	
	const auto edges = startTriangle->GetEdges();
	for (const auto& edge : edges)
	{
		if (startTriangle->HasEdge(edge))	//logic here doesn't seem to make fully sense...? -> revisit later again!!!
		{
			auto id = startNode->GetEdgeIdx();
			graphClone->AddConnection(startNode->GetId(), graphClone->GetNodeIdFromEdgeIndex(id));
		}
	}
	
	//Create extra node for the endNode
	std::unique_ptr<NavGraphNode> endNode = std::make_unique<NavGraphNode>(NavGraphNode( endPos, -1)); //imma assume startnode = agent pos
	graphClone->AddNode(std::move(endNode));
	graphClone->SetConnectionCostsToDistances();
	
	//Run A star on new graph
	
	//CODE SMTH LIKE THIS
	//auto AStar = AStar(graphClone);
	//finalPath = AStar->FindPath(startNode, endNode);

	return finalPath;
	
	//Debug Visualisation
	// Extra: Run optimiser on new graph (First check if everything works without SSFA!)
	// debugPortals = SSFA::FindPortals(nodes, *pNavGraph->GetNavPolygon());
	// finalPath = SSFA::OptimizePortals(debugPortals, *pNavGraph->GetNavPolygon());
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph)
{
	std::vector<FVector2D> debugNodePositions{};
	std::vector<NavLine> debugPortals{};

	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}