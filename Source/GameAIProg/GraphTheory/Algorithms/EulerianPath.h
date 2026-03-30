#pragma once
#include <stack>
#include "Shared/Graph/Graph.h"
#include <utility>

#include "HLSLTypeAliases.h"


namespace GameAI
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath final
	{
	public:
		EulerianPath(Graph* const pGraph);

		Eulerianity IsEulerian() const;
		std::vector<Node*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<Node*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;
		void FirstDepthSearch( Node*& node, std::vector<Node*>& stack, size_t& amountVisited) const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* const pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		// TODO If the graph is not connected, there can be no Eulerian Trail

		// TODO Count nodes with odd degree 

		// TODO A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian

		// TODO A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// TODO An Euler trail can be made, but only starting and ending in these 2 nodes

		// TODO A connected graph with no odd nodes is Eulerian
		
		return Eulerianity::notEulerian;
	}

	inline std::vector<Node*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		Graph graphCopy = m_pGraph->Clone();
		std::vector<Node*> Path = {};
		std::vector<Node*> Nodes = graphCopy.GetActiveNodes();
		int currentNodeId{ Graphs::InvalidNodeId };
		
		
		// TODO Check if there can be an Euler path
		// TODO If this graph is not eulerian, return the empty path
		if (!IsConnected() || eulerianity == Eulerianity::notEulerian)
		{
			return Path; //is empty
		}
		else if ( eulerianity == Eulerianity::eulerian )
		{
			//pick any starting node
			//set id
		}
		else if ( eulerianity == Eulerianity::semiEulerian )
		{
			//find uneven for starting node
		}
		
		
		// TODO Start algorithm loop
		std::stack<int> nodeStack;

		std::reverse(Path.begin(), Path.end());
		return Path;
	}

	inline void EulerianPath::VisitAllNodesDFS(const std::vector<Node*>& Nodes, std::vector<bool>& visited, int startIndex ) const
	{
		//TO THE TEACHER:
		//I started making the exercise before checking the next slide, (due to my carelessness)
		//so you can find a working version below with other functions + the sources I used to create the algorithm.
		//I added an isVisited variable the the node class to make it work
	}

	inline bool EulerianPath::IsConnected() const
	{
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		if (Nodes.size() == 0)
			return false;
		
		size_t amountVisited{ 0 }; //checks to see if all nodes are visited
		const size_t NeededVisits{ Nodes.size() }; //checks to see if all nodes are visited
		
		std::vector<Node*> stack{}; // using example https://www.codecademy.com/article/depth-first-search-dfs-algorithm
		Node* startNode = Nodes[0]; //pick first in list for simplicity
		stack.push_back(startNode);
		
		while (!stack.empty())
		{
			FirstDepthSearch(stack.back(), stack, amountVisited);
		}
		
		for ( auto& node : Nodes)
		{
			node->isVisited = false;
		}
		
		if (amountVisited == NeededVisits)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	inline void EulerianPath::FirstDepthSearch( Node*& node, std::vector<Node*>& stack, size_t& amountVisited) const
	{
		stack.pop_back(); //removes passed node
		node->isVisited = true;
		++amountVisited;
		
		auto ConnectedNodes {m_pGraph->FindConnectionsFrom(node->GetId())};
		for (const auto& ConnectedNode : ConnectedNodes)
		{
			int desinationNodeID = ConnectedNode->GetToId();
			auto foundNode = (m_pGraph->GetNode(desinationNodeID)).get();
			
			if (foundNode->isVisited == false)
			{
				stack.push_back(foundNode);
			}
		}
	}
}
