#pragma once
#include "Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
class Flock;

//COHESION - FLOCKING
//*******************
class Cohesion final : public Seek
{
public:
	Cohesion(Flock* const pFlock) :pFlock(pFlock) {};

	//Cohesion Behavior
	virtual SteeringOutput CalculateSteering(float deltaT, ASteeringAgent& pAgent) override; //overrides from seek

private:
	Flock* pFlock = nullptr;
};


//SEPARATION - FLOCKING
//*********************
class Seperation final : public Flee
{
public:
	Seperation(Flock* const pFlock) :pFlock(pFlock) {};

	//Separation Behavior
	SteeringOutput CalculateSteering(float deltaT, ASteeringAgent& pAgent) override;

private:
	Flock* pFlock = nullptr;
};


//VELOCITY MATCH - FLOCKING
//************************
class Alignment final : public Wander
{
public:
	Alignment(Flock* const pFlock) :pFlock(pFlock) {};

	//Separation Behavior
	SteeringOutput CalculateSteering(float deltaT, ASteeringAgent& pAgent) override;

private:
	Flock* pFlock = nullptr;
};

//Add seek and wander -> is for in blended!