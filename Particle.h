#pragma once
#include <list>
#include <memory>

class Particle
{
	virtual ~Particle() {}

	virtual void Update() {};
	virtual void Draw() {};
};

class ParticleManager
{
public:
	static ParticleManager* GetInstance();

	static void Register(Particle* particle);

private:
	std::list<std::unique_ptr<Particle>> manageList;
};