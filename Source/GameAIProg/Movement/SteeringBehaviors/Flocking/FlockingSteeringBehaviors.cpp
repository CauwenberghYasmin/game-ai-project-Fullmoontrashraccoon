#include "FlockingSteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"
#include "Flock.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent) //need to initialize flock somewhere
{
	SteeringOutput steering{};
	steering.LinearVelocity = pFlock->GetAverageNeighborPos() - pAgent.GetPosition(); //direction
	
	return steering;
}

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Seperation::CalculateSteering(float deltaT, ASteeringAgent& pAgent) //need to initialize flock somewhere
{
	SteeringOutput steering{};
	double  smallestDistance{0.f}; //when comparing take abs !!!
	FVector2D closestPosition {};
	
	for (const auto& neighbor: pFlock->m_pNeighbors)
	{
		double distance {(neighbor->GetPosition() - pAgent.GetPosition()).SquaredLength()}; //don't need exact length :D
		if (std::abs(distance) < smallestDistance)//if distance smaller than smallest distance
		{
			smallestDistance = distance;
			closestPosition = neighbor->GetPosition();
		}	//TODO:: change this: needs to have an endresult which adds all of the previous which are changed by the weight (closest)
	}
	
	steering.LinearVelocity = pAgent.GetPosition() - closestPosition; //opposite!
	return steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput Alignment::CalculateSteering(float deltaT, ASteeringAgent& pAgent) //need to initialize flock somewhere
{
	SteeringOutput steering{};
	steering.LinearVelocity = pFlock->GetAverageNeighborVelocity();//should be speed?
	return steering;
}
