#include "SteeringBehaviors.h"

#include "CircleTypes.h"
#include "BaseGizmos/GizmoMath.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Intersection/IntersectionUtil.h"
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
		//steering.LinearVelocity = FVector2D{0,0};
		return FVector2D{0,0};
	}
	
	//FRotator rotation = Agent.GetActorRotation();
	const FVector pos = FVector(Agent.GetPosition().X, Agent.GetPosition().Y, 5.f);
	const FVector dir = FVector(steering.LinearVelocity.X, steering.LinearVelocity.Y, 1); //.Normalize();
	const FVector desiredDIr = pos + dir; //find a way so it has a const length 
	
	//desired path
	DrawDebugLine(Agent.GetWorld(), pos, desiredDIr , FColor::Black);
	
	return steering;
 }


SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steering{};
	steering.LinearVelocity =  Agent.GetPosition() - Target.Position;

	const FVector pos = FVector(Agent.GetPosition().X, Agent.GetPosition().Y, 5.f);
	const FVector desiredDIr = pos + (FVector(steering.LinearVelocity.X, steering.LinearVelocity.Y, 1));
	DrawDebugLine(Agent.GetWorld(), pos, desiredDIr , FColor::Black);
	
	return steering;
}

//--------------------------------------------------------------------------------------------------------------------
float Arrive::calcPointInsideCircle (const FVector& pos1,const FVector& pos2, float radius, float speed)
{
	double distance {sqrt( (pos2.X - pos1.X) * (pos2.X - pos1.X) + (pos2.Y - pos1.Y) * (pos2.Y - pos1.Y))};
	
	if (distance < 5)
	{
		return 0;	//complete stop movement
	}
	else if (distance < radius)
	{
		if ( speed < 10.f)	//while slowing down, it should still have a min speed
		{
			return speed;
		}
		else
		{
			return 0.95 * speed; //amount slow down
		}
	}
	else
	{
		return orginalSpeed; //keeps speed
	}
	//if distance smaller -> more slower
	//we multiply so the kommagetal needs to get bigger
}

bool IsPointInCircle (const FVector& pos1, const FVector& pos2, float radius)
{
	double distance {sqrt( (pos2.X - pos1.X) * (pos2.X - pos1.X) + (pos2.Y - pos1.Y) * (pos2.Y - pos1.Y))};
	
	if (distance < radius)
		return true;
	else
		return false;
}
//--------------------------------------------------------------------------------------------------------------------
SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	if (!firstTimeRuning)	//get original speed
	{
		orginalSpeed = Agent.GetMaxLinearSpeed();
		firstTimeRuning = true;
	}
	
	SteeringOutput steering{};
	steering.LinearVelocity =  Target.Position - Agent.GetPosition(); //goes towards target
	
	if (abs(steering.LinearVelocity.Y) < 3.7 && abs(steering.LinearVelocity.X) < 3.7) //no wiggling
	{
		//steering.LinearVelocity = FVector2D{0,0};
		return FVector2D{0,0};
	}
	
	UE::Geometry::FCircle2d slowRadius (Target.Position, 300.f);
	UE::Geometry::FCircle2d targetRadius (Target.Position, 100.f);
	
	DrawDebugCircle(Agent.GetWorld(), FVector{Agent.GetPosition().X, Agent.GetPosition().Y, 5}, 300.f , 32, FColor::Blue, false, -1, 0, 0, FVector(0,1,0), FVector(1,0,0), true);
	DrawDebugCircle(Agent.GetWorld(), FVector{Agent.GetPosition().X, Agent.GetPosition().Y, 5}, 100.f , 32, FColor::Red, false, -1, 0, 0, FVector(0,1,0), FVector(1,0,0), true);
	
	float currentSpeed = Agent.GetMaxLinearSpeed();

	float speed = calcPointInsideCircle(FVector{Agent.GetPosition().X, Agent.GetPosition().Y, 5}, FVector{Target.Position.X, Target.Position.Y, 5}, slowRadius.Radius, currentSpeed);
	Agent.SetMaxLinearSpeed(speed);
	
	bool isInside =IsPointInCircle(FVector{Agent.GetPosition().X, Agent.GetPosition().Y, 5}, FVector{Target.Position.X, Target.Position.Y, 5}, targetRadius.Radius);
	if (isInside)
		Agent.SetMaxLinearSpeed(0); //speed zero when inside target circle
	
	return steering;
}

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)		//fix this one!!
{
	SteeringOutput steering{};
	FVector2d LinearVelocity =  Target.Position - Agent.GetPosition(); //goes towards target
	steering.AngularVelocity =  Agent.GetAngularVelocity() * LinearVelocity.Normalize();
	
	return steering;
}


