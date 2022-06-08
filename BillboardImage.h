#pragma once
#include "Obj3D.h"
#include "Texture.h"
#include "Vector2.h"
#include "Image3D.h"

class BillboardImage : public Obj3D
{
public:
	Image3D image;
	bool billboardY = false;

	BillboardImage() {};
	BillboardImage(Texture* texture, Vector2 size)
	{
		Init(texture, size);
	}

	//初期化処理
	void Init(Texture* texture, Vector2 size);

	void Update(const ViewProjection& vp);

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画用のコマンドをまとめてコマンドリストに積む
	void DrawCommands() override;
};

