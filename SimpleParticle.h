#pragma once
#include "Particle.h"
#include "Texture.h"
#include "Vector3.h"

class SimpleParticle : public Particle
{
public:
	enum class RenderType {
		Normal,
		Add
	};

	TextureHandle texture;
	Vector3 pos;
	Vector3 speed;
	Vector3 rot;
	Vector3 scale;

	virtual void Update() override;
	virtual void Draw() override;


};