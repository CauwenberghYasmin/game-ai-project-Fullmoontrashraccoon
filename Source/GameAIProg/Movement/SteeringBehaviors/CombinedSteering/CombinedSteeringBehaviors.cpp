
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"

BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors)
	:WeightedBehaviors(WeightedBehaviors)
{};


void drawDebug(const SteeringOutput& steering, const ASteeringAgent& Agent)
{
	const FVector pos = FVector(Agent.GetPosition().X, Agent.GetPosition().Y, 5.f);
	FVector dir = FVector(steering.LinearVelocity.X, steering.LinearVelocity.Y, 0.f);
	FVector scaledDir = dir.GetSafeNormal() * 100.f;
	const FVector endPoint = pos + scaledDir;
	
	//DrawDebugLine(Agent.GetWorld(), pos, pos + Agent.GetActorForwardVector() * 100.f , FColor::Red);
	//DrawDebugLine(Agent.GetWorld(), pos, endPoint , FColor::Black);
}


//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput BlendedSteering = {};
	// TODO: Calculate the weighted average steeringbehavior
	//have vector weightedBehaviours -> vector with diff steering + weight!
	
	for (auto& behaviour : WeightedBehaviors)
	{
		SteeringOutput steering = behaviour.pBehavior->CalculateSteering(DeltaT, Agent);
		
		//adding and multiplying (see graphics programming for weighted points!)
		BlendedSteering.LinearVelocity += steering.LinearVelocity * behaviour.Weight;
		BlendedSteering.AngularVelocity += steering.AngularVelocity * behaviour.Weight;
	}
	BlendedSteering.LinearVelocity.Normalize();
	
	// TODO: Add debug drawing
	drawDebug(BlendedSteering, Agent);
	
	return BlendedSteering;
}

float* BlendedSteering::GetWeight(ISteeringBehavior* const SteeringBehavior)
{
	auto it = find_if(WeightedBehaviors.begin(),
		WeightedBehaviors.end(),
		[SteeringBehavior](const WeightedBehavior& Elem)
		{
			return Elem.pBehavior == SteeringBehavior;
		}
	);

	if(it!= WeightedBehaviors.end())
		return &it->Weight;
	
	return nullptr;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};

	for (ISteeringBehavior* const pBehavior : m_PriorityBehaviors)
	{
		Steering = pBehavior->CalculateSteering(DeltaT, Agent);
		
		//changed evade steering inside of class!! -> target out of radius: isValid = false!		
		if (Steering.IsValid)
			break;
	}

	//If none of the behavior return a valid output, last behavior is returned
	return Steering;
}