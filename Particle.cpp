#include "Particle.h"

using namespace std;

ParticleManager* ParticleManager::GetInstance()
{
	static ParticleManager manager;
	return &manager;
}

void ParticleManager::Register(Particle* particle)
{
	ParticleManager* manager = GetInstance();
	manager->manageList.push_back(unique_ptr<Particle>(particle));
}
