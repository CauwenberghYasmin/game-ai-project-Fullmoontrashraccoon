#pragma once

// Toggle this define to enable/disable spatial partitioning
// #define GAMEAI_USE_SPACE_PARTITIONING

#include "FlockingSteeringBehaviors.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"
#include "Movement/SteeringBehaviors/SteeringHelpers.h"
#include "Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
#include <memory>
#include "imgui.h"

class CellSpace;
class Flock final
{
public:
	Flock(
	UWorld* pworld, 
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize = 10, 
	float WorldSize = 100.f, 
	ASteeringAgent* const pAgentToEvade = nullptr, 
	bool bTrimWorld = false);

	~Flock();
	
	int currentAmountInsidePool{0};
	std::vector<ASteeringAgent*> m_pNeighbors{}; //added a function to baseclass: get/set inUse

	void Tick(float DeltaTime);
	void RenderDebug();
	void ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize);
	void WorldTrimming(ASteeringAgent* Agent);

//#ifdef GAMEAI_USE_SPACE_PARTITIONING
	//const TArray<ASteeringAgent*>& GetNeighborsPartitioning() const { return pPartitionedSpace->GetNeighbors(); }
	//int GetNrOfNeighborsPartitioning() const { return pPartitionedSpace->GetNrOfNeighbors(); }
	//-> move to cpp, give right amswer back depending in enabled
	
//#else // No space partitioning
	void RegisterNeighbors(ASteeringAgent* const Agent);
	
	//will have double version
	int GetNrOfNeighbors() const;
	std::vector<ASteeringAgent*> GetNeighbors() const;
//#endif // USE_SPACE_PARTITIONING

	FVector2D GetAverageNeighborPos() const;
	FVector2D GetAverageNeighborVelocity() const;

	void SetTarget_Seek(FSteeringParams const & Target);

	
private:
	// For debug rendering purposes
	UWorld* pWorld{nullptr};
	
	int FlockSize{0};
	TArray<ASteeringAgent*> Agents{};
//#ifdef GAMEAI_USE_SPACE_PARTITIONING
	std::unique_ptr<CellSpace> pPartitionedSpace{};
	int NrOfCellsX{ 10 };
	TArray<FVector2D> OldPositions{};
	
//#else // No space partitioning
	TArray<ASteeringAgent*> Neighbors{};	//partitioning class has own neighbours vector
//#endif // USE_SPACE_PARTITIONING
	
	float NeighborhoodRadius{200.f};
	int NrOfNeighbors{0};
	float m_WorldSize{};
	bool isUsingSpacialPartitions{true};
	bool canStartTicking{false};
	std::vector<FVector2D> oldPosAgents;
	
	std::unique_ptr<CellSpace> pCellSpace{}; 
	ASteeringAgent* m_pAgentToEvade{nullptr};
	BlendedSteering* m_pBlendedSteering{ nullptr };
	PrioritySteering* m_pPrioritySteering{ nullptr };
	
	//initialize in beginPLay
	ISteeringBehavior* m_pCohesionBehavior{};
	ISteeringBehavior* m_pSeperationBehavior{};
	ISteeringBehavior* m_pAlignmentBehavior{};
	ISteeringBehavior* m_pSeekBehavior{ new Seek() };
	ISteeringBehavior* m_pWanderBehavior{ new Wander() };
	ISteeringBehavior* m_pEvadeBehavior{ new Evade() };
	//make sure to delete!!!

	// UI and rendering
	bool DebugRenderSteering{false};
	bool DebugRenderNeighborhood{true};
	bool DebugRenderPartitions{true};
	
	
	void RenderNeighborhood();
};
