#pragma once
#include "SpriteComponent.h"
#include "TileMapTexture.h"

class TileMapComponent : public SpriteComponent {
public:
	TileMapComponent(class Actor* owner, const TileMapTexture& tilemapTexture, int drawOrder = 100);

	void Draw(SDL_Renderer* renderer) override;
	void SetTileMapTexture(const TileMapTexture& tilemapTexture);
private:
	void drawCell(SDL_Renderer* renderer, int index, int x, int y, int cw, int ch);
	TileMapTexture tilemapTexture;
};