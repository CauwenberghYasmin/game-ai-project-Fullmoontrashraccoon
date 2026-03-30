#pragma once
#include <stack>
#include "Shared/Graph/Graph.h"
#include <utility>

#include "AsyncTreeDifferences.h"
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

	inline Eulerianity EulerianPath::IsEulerian() const //where is this function called???
	{
		// TODO If the graph is not connected, there can be no Eulerian Trail
		if (!IsConnected())
			return Eulerianity::notEulerian;

		// TODO Count nodes with odd degree 
		uint8_t amountNodesWithOddDegrees{0}; // uint8_t is 0-255, for low counting try using it instead!!
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		
		for (const auto& node : Nodes)
		{
			auto connections = m_pGraph->FindConnectionsFrom(node->GetId());
			if (connections.size() % 2 == 1) 
				++amountNodesWithOddDegrees;
		}
		
		// TODO A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (amountNodesWithOddDegrees > 2)
		{
			return Eulerianity::notEulerian;
		}
		else if (amountNodesWithOddDegrees == 2 && Nodes.size() != 2)
		{
			return Eulerianity::semiEulerian;
		}
		else
		{
			return Eulerianity::eulerian;
		}

		
		// TODO A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// TODO An Euler trail can be made, but only starting and ending in these 2 nodes

		// TODO A connected graph with no odd nodes is Eulerian
	}

	inline std::vector<Node*> EulerianPath::FindPath(Eulerianity& eulerianity) const //IsEulerian function  not needed, because it's passed in the parameter
	{
		// Get a copy of the graph because this algorithm involves removing edges
		Graph graphCopy = m_pGraph->Clone();
		std::vector<Node*> Path = {};
		std::vector<Node*> Nodes = graphCopy.GetActiveNodes();
		int currentNodeId{ Graphs::InvalidNodeId };
		
		
		// TODO Check if there can be an Euler path
		// TODO If this graph is not eulerian, return the empty path
		if (eulerianity == Eulerianity::notEulerian)
		{
			return Path; //is empty
		}
		else if ( eulerianity == Eulerianity::eulerian )
		{
			//pick any starting node
			//set id
			currentNodeId = Nodes[0]->GetId(); //for random just pick first
		}
		else if ( eulerianity == Eulerianity::semiEulerian )
		{
			//find uneven for starting node
			for (const auto& node : Nodes)
			{
				auto connections = graphCopy.FindConnectionsFrom(node->GetId());
				if (connections.size() % 2 == 1)
				{
					currentNodeId = node->GetId();
					break;
				}
			}
		}
		std::stack<int> nodeStack{};
		
		while (!nodeStack.empty() || graphCopy.FindConnectionsFrom(currentNodeId).size() != 0) //check if conditions are correct!
		{
			nodeStack.push(currentNodeId);
			
			auto connections = graphCopy.FindConnectionsFrom(currentNodeId);
			if (connections.size() != 0)
			{
				size_t neighborID = connections[0]->GetToId();
				graphCopy.RemoveConnection(currentNodeId,neighborID );
				currentNodeId = neighborID;
			}
			else //no more neighbours!!
			{
				const int lastElement {nodeStack.top()};
				Path.push_back(m_pGraph->GetNode(lastElement).get());
				nodeStack.pop();
				if (!nodeStack.empty())
				{
					currentNodeId = nodeStack.top();
				}
			}
		}
		Path.push_back(m_pGraph->GetNode(currentNodeId).get());
		
		std::reverse(Path.begin(), Path.end());
		return Path;
	}

	inline void EulerianPath::VisitAllNodesDFS(const std::vector<Node*>& Nodes, std::vector<bool>& visited, int startIndex ) const
	{
		//TO THE TEACHER:
		//I started making the exercise before checking the next slide, (due to my carelessness)
		//so you can find a working version below with other functions + the sources I used to create the algorithm.
		//I added an isVisited variable the node class to make it work
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
