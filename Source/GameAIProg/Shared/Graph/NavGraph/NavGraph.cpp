#include "NavGraph.h"

#include "NavGraphNode.h"

GameAI::NavGraph::NavGraph(std::unique_ptr<TriPolygon> && NavPoly)
	: Graph{false}
	, pNavPoly{std::move(NavPoly)}
{
	CreateNavigationGraph();
}

GameAI::NavGraph::NavGraph(const NavGraph& Other)
	: Graph(false)
{
	Nodes.reserve(Other.Nodes.size());
	for (std::unique_ptr<Node> const & OtherNode : Other.Nodes)
	{
		Nodes.push_back(std::make_unique<NavGraphNode>(*dynamic_cast<NavGraphNode*>(OtherNode.get())));
	}
        
	Connections.reserve(Other.Connections.size());
	for (std::unique_ptr<Connection> const & OtherConnection : Other.Connections)
	{
		Connections.push_back(std::make_unique<Connection>(*OtherConnection.get()));
	}
}

std::unique_ptr<GameAI::NavGraph> GameAI::NavGraph::Clone() const
{
	return std::make_unique<NavGraph>(*this);
}

int GameAI::NavGraph::GetNodeIdFromEdgeIndex(int EdgeIdx) const
{
	if (EdgeIdx >= 0)
	{
		for (auto const & pNode : Nodes)
		{
			if (reinterpret_cast<NavGraphNode*>(pNode.get())->GetEdgeIdx() == EdgeIdx)
			{
				return pNode->GetId();
			}
		}
	}
	
	return Graphs::InvalidNodeId;
}

void GameAI::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigation mesh and create nodes
			// Create node here
	auto lines = pNavPoly->GetEdges();
	auto triangles = pNavPoly->GetTriangles();
	size_t lineIndex{0};
	
	
	for (const auto& line : lines) //can't fins getTriangleFromLineIndex function so I wrote this instead (optimization is LACKING)
	{
		bool sharedEdgesFound{false};
		size_t amountEdgesShared{0};
		
		for (auto const & triangle : triangles)
		{
			if (triangle.HasEdge(line))
			{
				++amountEdgesShared;
				if (amountEdgesShared >= 2)
				{
					sharedEdgesFound = true;		
					break;
				}
			}
		}
		
		if (sharedEdgesFound)
		{
			FVector p1 = line.GetP1(*pNavPoly.get());
			FVector p2 = line.GetP2(*pNavPoly.get());
			FVector2D position{ (p1.X + p2.X) / 2.0f, (p1.Y + p2.Y) / 2.0f };
			this->AddNode(std::make_unique<NavGraphNode>(position, lineIndex));
		}
		++lineIndex;
	}
	
	
	//2. Create connections now that every node is created	
		//2 valid nodes -> 1 connection
		//3 valid nodes -> 3 connections
	for (auto const & triangle : triangles)
	{
		std::vector<int> NodeIds;
		NodeIds.reserve(3);
		auto triangleEdges = triangle.GetEdges();
		
		for (auto const& edge : triangleEdges)
		{
			auto edgeIndex = pNavPoly->FindEdgeIndex(edge);
			if (edgeIndex.has_value()) //is std::optional!
			{
				int nodeId = GetNodeIdFromEdgeIndex(edgeIndex.value());
				if (nodeId != Graphs::InvalidNodeId) 
				{
					NodeIds.push_back(nodeId);
				}
			}
		}
		
		size_t amountValidNodes {NodeIds.size()};
		//pre initialize otherwise swtich case gives warnings
		int id{};
		int id2{};
		switch (amountValidNodes)
		{
		case 2:
			id = NodeIds[0];
			id2 = NodeIds[1];
			//float distance = FVector2D::Distance(GetNode(id)->GetPosition(), GetNode(id2)->GetPosition());)
			//-> don't need to calc weight each sep, just call big funciton at the end!!
			AddConnection(std::make_unique<Connection>(id, id2));
			AddConnection(std::make_unique<Connection>(id2, id));
			break;
		case 3:
			for (int i = 0; i < 3; ++i)
			{
				id = NodeIds[i];
				id2 = NodeIds[(i + 1) % 3];
				AddConnection(std::make_unique<Connection>(id, id2));
				AddConnection(std::make_unique<Connection>(id2, id));
			}
			break;
		}
		
		//3. Set the connections cost to the actual distance
		SetConnectionCostsToDistances(); //bless this funciton
	}

		
}
