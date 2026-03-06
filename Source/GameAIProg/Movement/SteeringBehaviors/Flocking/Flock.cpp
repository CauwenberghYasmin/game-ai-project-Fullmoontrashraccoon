#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Movement/SteeringBehaviors/CombinedSteering/Level_CombinedSteering.h"
#include "Shared/ImGuiHelpers.h"
#include "Movement/SteeringBehaviors/SpacePartitioning/SpacePartitioning.h"

Flock::Flock(
	UWorld* pworld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize,
	float WorldSize,
	ASteeringAgent* const pAgentToEvade,
	bool bTrimWorld)
	: pWorld{pworld}
	, FlockSize{ FlockSize }
	, m_WorldSize{ WorldSize }
	, m_pAgentToEvade{pAgentToEvade}
{
	float activeWorldSize { m_WorldSize / 3.0f * 2 };
	pCellSpace = std::make_unique<CellSpace>(pWorld, activeWorldSize, activeWorldSize, 10, 10, FlockSize);
	
	Agents.SetNum(FlockSize);
	
 // TODO: initialize the flock and the memory pool
	const int MaxAmount {FlockSize - 1}; //don't include yourself
	m_pNeighbors.reserve(MaxAmount);
	Agents.Reserve(FlockSize);
	
	m_pCohesionBehavior = new Cohesion(this) ;
	m_pSeperationBehavior = new Seperation(this) ;
	m_pAlignmentBehavior = new Alignment(this);
	
	m_pBlendedSteering = new BlendedSteering({
		BlendedSteering::WeightedBehavior(m_pCohesionBehavior, 0.1f),
			BlendedSteering::WeightedBehavior(m_pSeperationBehavior, 0.5f),
			BlendedSteering::WeightedBehavior(m_pAlignmentBehavior, 0.1f),
			BlendedSteering::WeightedBehavior(m_pSeekBehavior, 0.1f),
			BlendedSteering::WeightedBehavior(m_pWanderBehavior, 0.2f)});
	m_pPrioritySteering = new PrioritySteering({m_pEvadeBehavior, m_pBlendedSteering});
	
	for (int i = 0; i < FlockSize; ++i) 
	{
		
		while (Agents[i] == nullptr)
		{
			FVector SpawnLocation = FVector(FMath::RandRange(-WorldSize/2, WorldSize/2), FMath::RandRange(-WorldSize/2, WorldSize/2), 90.f);
			Agents[i] = pWorld->SpawnActor<ASteeringAgent>(AgentClass, SpawnLocation, FRotator::ZeroRotator);
		}
		
		Agents[i]->SetSteeringBehavior(m_pPrioritySteering);
		pCellSpace->AddAgent(*Agents[i]);
	
	}
	
	FVector SpawnLocation = FVector(FMath::RandRange(-WorldSize/2, WorldSize/2), FMath::RandRange(-WorldSize/2, WorldSize/2), 90.f);
	m_pAgentToEvade = pWorld->SpawnActor<ASteeringAgent>(AgentClass, SpawnLocation, FRotator::ZeroRotator);
	m_pAgentToEvade->SetSteeringBehavior(m_pWanderBehavior); //just wanders around, the rest has to avoid it
	
	canStartTicking = true; //only once done
}

Flock::~Flock()
{
	for ( auto& Agent : Agents )
	{ 
		Agent->Destroy(); //check if this causes double delete or not
	}
	delete m_pSeperationBehavior;
	delete m_pCohesionBehavior;
	delete m_pAlignmentBehavior;
}

void Flock::Tick(float DeltaTime)
{
	if (!canStartTicking) return; //only tick when done constructor
	
	for (auto& agent : Agents )
	{
		RegisterNeighbors(agent);
		WorldTrimming(agent);
		
		
		m_pSeperationBehavior->CalculateSteering(DeltaTime, *agent);
		m_pCohesionBehavior->CalculateSteering(DeltaTime, *agent);
		m_pAlignmentBehavior->CalculateSteering(DeltaTime, *agent);
		
	}
	
	WorldTrimming(m_pAgentToEvade);
	
	//update the evade target aswell (priority steering)
	FTargetData evadeTarget;
	evadeTarget.Position = m_pAgentToEvade->GetPosition();
	evadeTarget.Orientation = m_pAgentToEvade->GetRotation();
	evadeTarget.AngularVelocity = m_pAgentToEvade->GetAngularVelocity();
	evadeTarget.LinearVelocity = m_pAgentToEvade->GetLinearVelocity();
	
	m_pEvadeBehavior->SetTarget(evadeTarget);
}

void Flock::WorldTrimming(ASteeringAgent* Agent)
{
	// so the active is m_WorldSize / 3.0f * 2 -> need for spactial partitioning
	float HalfSize = m_WorldSize / 3.0f;
	FVector CurrentPos = Agent->GetActorLocation();
	bool bNeedsTeleport = false;
	
	//world trimming
	if (CurrentPos.X > HalfSize)
	{
		CurrentPos.X = -HalfSize;
		bNeedsTeleport = true;
	}
	else if (CurrentPos.X < -HalfSize)
	{
		CurrentPos.X = HalfSize;
		bNeedsTeleport = true;
	}
		
	if (CurrentPos.Y > HalfSize)
	{
		CurrentPos.Y = -HalfSize;
		bNeedsTeleport = true;
	}
	else if (CurrentPos.Y < -HalfSize)
	{
		CurrentPos.Y = HalfSize;
		bNeedsTeleport = true;
	}
		
	if (bNeedsTeleport)
	{
		Agent->SetActorLocation(CurrentPos);
	}
}
void Flock::RenderDebug()
{
	//render to highlight evage agent
	
	FVector2D posCirlce = FVector2D{m_pAgentToEvade->GetPosition().X + (m_pAgentToEvade->GetActorForwardVector().X * 100.f), m_pAgentToEvade->GetPosition().Y + (m_pAgentToEvade->GetActorForwardVector().Y * 100.f)};
	DrawDebugCircle(pWorld, FVector{posCirlce.X, posCirlce.Y, 5}, 100.f , 32, FColor::Red, false, -1, 0, 0, FVector(0,1,0), FVector(1,0,0), true);
	DrawDebugCircle(pWorld, FVector{posCirlce.X, posCirlce.Y, 5}, 90.f , 32, FColor::Blue, false, -1, 0, 0, FVector(0,1,0), FVector(1,0,0), true);
	DrawDebugCircle(pWorld, FVector{posCirlce.X, posCirlce.Y, 5}, 80.f , 32, FColor::Red, false, -1, 0, 0, FVector(0,1,0), FVector(1,0,0), true);
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		
		ImGui::Checkbox("spatial partitioning", &isUsingSpacialPartitions);
		{
			//isUsingSpacialPartitions = !isUsingSpacialPartitions;
		}
		
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		
		
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Cohesion",
			m_pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
			[this](float InVal) { m_pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Seperation",
		m_pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
		[this](float InVal) { m_pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Alignment",
		m_pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight, 0.f, 1.f,
		[this](float InVal) { m_pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
		m_pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight, 0.f, 1.f,
		[this](float InVal) { m_pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
		m_pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight, 0.f, 1.f,
		[this](float InVal) { m_pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight = InVal; }, "%.2f");

		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
	if (isUsingSpacialPartitions)
		pCellSpace->RenderCells();
}


void Flock::RegisterNeighbors(ASteeringAgent* const pAgent) //pAgent is the main rn
{
	currentAmountInsidePool = 0; //reset
	
	for (auto& Agent : Agents )
	{
		if (Agent != pAgent) //not including itself
		{
			Agent->SetInUse(false); //TODO:: check these again with the theory!!
			
			FVector2D length = Agent->GetPosition() - pAgent->GetPosition();
			if (length.Length() < NeighborhoodRadius)
			{
				m_pNeighbors[currentAmountInsidePool] = Agent; //puts it in the first empty space
				++currentAmountInsidePool;
				
				Agent->SetInUse(true); //TODO:: check these again with memory pool 
			}
		}
	}
}



FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D avgPosition = FVector2D::ZeroVector;

	if (currentAmountInsidePool == 0) return avgPosition; // Prevent divide by zero!
	
	for (int i = 0; i < currentAmountInsidePool; ++i)
	{
		avgPosition += m_pNeighbors[i]->GetPosition();
	}
    
	avgPosition /= currentAmountInsidePool;
	return avgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;

	for (int i = 0; i < currentAmountInsidePool; ++i)
	{
		avgVelocity += FVector2D {m_pNeighbors[i]->GetVelocity().X, m_pNeighbors[i]->GetVelocity().Y};
	}

	return avgVelocity;
}


void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
 // TODO: Implement
	
	 m_pSeekBehavior->SetTarget(Target);
}


int Flock::GetNrOfNeighbors() const
{
	if (!isUsingSpacialPartitions)
		return NrOfNeighbors;
	else 
	{
		return pPartitionedSpace->GetNrOfNeighbors();
	}
}

const TArray<ASteeringAgent*>& Flock::GetNeighbors() const
{
	if (!isUsingSpacialPartitions)
		return Neighbors;
	else
	{
		return pPartitionedSpace->GetNeighbors();
	}
}

//const TArray<ASteeringAgent*>& GetNeighborsPartitioning() const { return pPartitionedSpace->GetNeighbors(); }
//int GetNrOfNeighborsPartitioning() const { return pPartitionedSpace->GetNrOfNeighbors(); }
