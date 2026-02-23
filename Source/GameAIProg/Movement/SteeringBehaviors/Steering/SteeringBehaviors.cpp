#include "SteeringBehaviors.h"

#include "CircleTypes.h"
#include "CollisionDebugDrawingPublic.h"
#include "BaseGizmos/GizmoMath.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "GeometryCollection/GeometryCollectionDebugDrawActor.h"
#include "Intersection/IntersectionUtil.h"
#include "WorldPartition/WorldPartitionHelpers.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)


//draw debug stuff!!!
void drawDebugLines(const SteeringOutput& steering, const ASteeringAgent& Agent)
{
	const FVector pos = FVector(Agent.GetPosition().X, Agent.GetPosition().Y, 5.f);
	FVector dir = FVector(steering.LinearVelocity.X, steering.LinearVelocity.Y, 0.f);
	FVector scaledDir = dir.GetSafeNormal() * 100.f;
	const FVector endPoint = pos + scaledDir;
	
	DrawDebugLine(Agent.GetWorld(), pos, pos + Agent.GetActorForwardVector() * 100.f , FColor::Red);
	DrawDebugLine(Agent.GetWorld(), pos, endPoint , FColor::Black);
}


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
	
	//path draw
	drawDebugLines(steering, Agent);
	
	return steering;
 }



SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steering{};
	steering.LinearVelocity =  Agent.GetPosition() - Target.Position;

	const FVector pos = FVector(Agent.GetPosition().X, Agent.GetPosition().Y, 5.f);
	const FVector desiredDIr = pos + (FVector(steering.LinearVelocity.X, steering.LinearVelocity.Y, 1));
	//DrawDebugLine(Agent.GetWorld(), pos, desiredDIr , FColor::Black);
	
	drawDebugLines(steering, Agent);
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
	
	
	drawDebugLines(steering, Agent);
	
	return steering;
}

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)		//fix this one!!
{
	SteeringOutput steering{};
	
	FVector2d LinearVelocity =  (Target.Position - Agent.GetPosition()).GetSafeNormal(); //goes towards target
	float AgentAngle{static_cast<float>(Agent.GetActorRotation().Yaw)/180.f * PI};
	float targetAngle{static_cast<float>(atan2(LinearVelocity.Y, LinearVelocity.X)) };
	float epsilon {PI/180.f}; //buffer
	
	if (targetAngle > AgentAngle - epsilon &&  targetAngle< AgentAngle + epsilon)
	{
		steering.AngularVelocity = 0.f; //facing target
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.F, FColor{255, 0,0,255}, "no angle diff");
	}
	else if ((targetAngle - AgentAngle < PI && targetAngle - AgentAngle >0) || (targetAngle - AgentAngle < -PI && targetAngle - AgentAngle < 0))
	{
		steering.AngularVelocity = 1.f;
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.F, FColor{255, 0,0,255}, "going 1");
	}
	else
	{
		steering.AngularVelocity = -1.f;
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.F, FColor{255, 0,0,255}, "going -1");
	}
	
	
	
	return steering;
}


SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)	//can't test yet, NO ONE TO PURSUIT 
{
	SteeringOutput steering{};
	FVector2d vectorToTarget =  Target.Position - Agent.GetPosition(); //vector towards target
	
	float timeFlight =  vectorToTarget.Size() / Agent.GetMaxLinearSpeed(); //time to reach target
	
	FVector2D distanceMade =Target.Position + Target.LinearVelocity * timeFlight; //pos where they will be
	
	steering.LinearVelocity = distanceMade - Agent.GetPosition(); //direction to that point
	FVector pos {Agent.GetPosition().X, Agent.GetPosition().Y, 1};
	FVector dir {steering.LinearVelocity.X, steering.LinearVelocity.Y, 1};
	
	//DrawDebugLine(Agent.GetWorld(), pos, dir , FColor::Black);
	//DrawDebugPoint(Agent.GetWorld(), FVector{distanceMade.X, distanceMade.Y, 2}, 7, FColor::Green);
	
	drawDebugLines(steering, Agent);
	
	return steering;
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)	
{
	SteeringOutput steering{};
	FVector2d vectorToTarget =  Target.Position - Agent.GetPosition(); //vector towards target
	
	float timeFlight =  vectorToTarget.Size() / Agent.GetMaxLinearSpeed(); //time to reach target
	
	FVector2D distanceMade =Target.Position + Target.LinearVelocity * timeFlight; //pos where they will be
	
	steering.LinearVelocity = Agent.GetPosition() - distanceMade; //direction to that point
	FVector pos {Agent.GetPosition().X, Agent.GetPosition().Y, 1};
	FVector dir {steering.LinearVelocity.X, steering.LinearVelocity.Y, 1};
	
	//DrawDebugLine(Agent.GetWorld(), pos, dir , FColor::Black);
	//DrawDebugPoint(Agent.GetWorld(), FVector{distanceMade.X, distanceMade.Y, 2}, 7, FColor::Green);
	
	drawDebugLines(steering, Agent);
	
	return steering;
}


SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)	
{
	SteeringOutput steering{};
	float random = rand() % 90 + m_WanderAngle - m_MaxAngleChange; 
	if (random < 0)
		random = 360 + random;
	else if (random > 360)
		random = random - 360;
	
	m_WanderAngle = random;
	random = random / 180 * PI; //now in radian
	
	FVector2D posCirlce = FVector2D{Agent.GetPosition().X + (Agent.GetActorForwardVector().X * m_OffsetDistance), Agent.GetPosition().Y + (Agent.GetActorForwardVector().Y * m_OffsetDistance)};
	UE::Geometry::FCircle2d WanderCIrlce (posCirlce, 100.f);
	
	//DrawLineTraces()
	DrawDebugCircle(Agent.GetWorld(), FVector{posCirlce.X, posCirlce.Y, 5}, 100.f , 32, FColor::Blue, false, -1, 0, 0, FVector(0,1,0), FVector(1,0,0), true);
	FVector2D targetPos {posCirlce.X + (WanderCIrlce.Radius * cos(random)), posCirlce.Y + (WanderCIrlce.Radius * sin(random)) };
	
	DrawDebugPoint(Agent.GetWorld(), FVector{targetPos.X, targetPos.Y, 1}, 7, FColor::Green);
	steering.LinearVelocity = targetPos - Agent.GetPosition();
	
	drawDebugLines(steering, Agent);
	
	return steering;
}