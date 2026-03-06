#include "Level_CombinedSteering.h"
#include "imgui.h"

// Sets default values
ALevel_CombinedSteering::ALevel_CombinedSteering()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_CombinedSteering::BeginPlay()
{
	Super::BeginPlay();
	//initialize everything
	//agent -> get world, spawn
	// set steering behaviour
	
	m_pBlendedSteering = new BlendedSteering({
		BlendedSteering::WeightedBehavior(m_pSeekBehavior, 0.5f),
		BlendedSteering::WeightedBehavior(m_pWanderBehavior, 0.5f)});
    
	m_pPrioritySteering = new PrioritySteering({m_pEvadeBehavior, m_pWanderBehavior});
    
	m_pBlendedAgent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, FVector{0,0,90}, FRotator::ZeroRotator);
	m_pBlendedAgent->SetSteeringBehavior(m_pBlendedSteering);
	
	m_pPriorityAgent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, FVector{0,0,90}, FRotator::ZeroRotator);
	m_pPriorityAgent->SetSteeringBehavior(m_pPrioritySteering);;
}

void ALevel_CombinedSteering::BeginDestroy()
{
	Super::BeginDestroy();
	
	// delete m_pBlendedSteering;
	// delete m_pPrioritySteering;
	// delete m_pBlendedAgent;
	// delete m_pPriorityAgent;
	// delete m_pSeekBehavior;
	// delete m_pWanderBehavior;
	// delete m_pEvadeBehavior;
}

// Called every frame
void ALevel_CombinedSteering::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//set target somewhere


#pragma region UI
	//UI
	{
		//Setup
		bool windowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Game AI", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	
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
		ImGui::Spacing();
	
		ImGui::Text("Flocking");
		ImGui::Spacing();
		ImGui::Spacing();
	
		if (ImGui::Checkbox("Debug Rendering", &CanDebugRender))
		{
			// Handle the debug rendering of your agents here :)
			//-> already done in combined steeringBehaviours, was also mentioned there!
		}
		ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
		if (TrimWorld->bShouldTrimWorld)
		{
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
				TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
				[this](float InVal) { TrimWorld->SetTrimWorldSize(InVal); });
		}
		
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
	
		ImGui::Text("Behavior Weights");
		ImGui::Spacing();


		ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
			m_pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
			[this](float InVal) { m_pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
		m_pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
		[this](float InVal) { m_pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
	
		//End
		ImGui::End();
	}
#pragma endregion
	
	// Combined Steering Update
 // TODO: implement handling mouse click input for seek
 // TODO: implement Make sure to also evade the wanderer
	
	m_pSeekBehavior->SetTarget(MouseTarget);
    
	FTargetData Target;
	Target.Position = m_pBlendedAgent->GetPosition();
	Target.Orientation = m_pBlendedAgent->GetRotation();
	Target.AngularVelocity = m_pBlendedAgent->GetAngularVelocity();
	Target.LinearVelocity = m_pBlendedAgent->GetLinearVelocity();

	m_pEvadeBehavior->SetTarget(Target);
}
