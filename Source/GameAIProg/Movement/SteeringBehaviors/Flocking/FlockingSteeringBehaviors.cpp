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
SteeringOutput Seperation::CalculateSteering(float deltaT, ASteeringAgent& pAgent) //Todo:: change code (see teacher feedback on onenote!!)
{
	SteeringOutput steering{};
	for (const auto& neighbor : pFlock->m_pNeighbors)
	{
		FVector2D separationVector = pAgent.GetPosition() - neighbor->GetPosition();
		double distance = abs(separationVector.SquaredLength()); //not sure if with abs, it returns a pos number
		
		if (distance > 0.01f)
		{
			steering.LinearVelocity += separationVector / distance;
		}
	}
	
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
