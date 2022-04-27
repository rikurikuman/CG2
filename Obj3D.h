#pragma once
#include "Transform.h"
#include "ViewProjection.h"

class Obj3D
{
protected:
	Obj3D* parent = nullptr;

public:
	Transform transform;

	void SetParent(Obj3D* parent) {
		this->parent = parent;
		this->transform.parent = &parent->transform;
	}

	const Obj3D* GetParent() {
		return parent;
	}

	//各データのバッファへの転送
	virtual void TransferBuffer(ViewProjection viewprojection) {}

	//描画用のコマンドをまとめてコマンドリストに積む
	virtual void DrawCommands() {}
};

