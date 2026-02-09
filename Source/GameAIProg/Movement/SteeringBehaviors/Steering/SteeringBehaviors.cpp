#include "SteeringBehaviors.h"

#include "CircleTypes.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "WorldPartition/WorldPartitionHelpers.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steering{};
	steering.LinearVelocity = Target.Position - Agent.GetPosition();
	//no need to normalize, happens later in code
	
	if (abs(steering.LinearVelocity.Y) < 3.7 && abs(steering.LinearVelocity.X) < 3.7) //no wiggling
	{
		steering.LinearVelocity = FVector2D{0,0};
	}
	
	return steering;
 }


SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steering{};
	steering.LinearVelocity =  Agent.GetPosition() - Target.Position;

	return steering;
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steering{};
	steering.LinearVelocity =  Target.Position - Agent.GetPosition(); //goes towards target
	
	UE::Geometry::FCircle2d slowRadius (Target.Position, 20.f);
	UE::Geometry::FCircle2d targetRadius (Target.Position, 5.f);
	
	DrawDebugCircle(Agent.GetWorld(), FVector{Agent.GetPosition().X, Agent.GetPosition().Y, 5}, 300.f , 32, FColor::Blue, false, -1, 0, 0, FVector(0,1,0), FVector(1,0,0), true);
	DrawDebugCircle(Agent.GetWorld(), FVector{Agent.GetPosition().X, Agent.GetPosition().Y, 5}, 150.f , 32, FColor::Red, false, -1, 0, 0, FVector(0,1,0), FVector(1,0,0), true);
	
	//finish arrive

	
	return steering;
}