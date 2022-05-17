#pragma once
#include "Obj3D.h"
#include "Image3D.h"
class Cube : public Obj3D
{
public:
	enum Direction {
		Front, Left, Back, Right, Top, Bottom
	};

	Image3D faces[6];
	Vector2 size = { 1, 1 };

	Cube(Vector2 size = {1, 1});
	Cube(Texture* texture, Vector2 size);

	/// <summary>
	/// 指定した面のテクスチャをセットする
	/// </summary>
	/// <param name="texture">テクスチャのポインタ</param>
	/// <param name="direction">面の向き</param>
	void SetTexture(Texture* texture, Direction direction);

	/// <summary>
	/// 全ての面のテクスチャをセットする
	/// </summary>
	/// <param name="texture"></param>
	void SetAllTexture(Texture* texture);

	//面のTransformとかを更新する
	void UpdateFaces();

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画用のコマンドをまとめてコマンドリストに積む
	void DrawCommands() override;
};

