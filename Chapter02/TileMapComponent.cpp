#include "TileMapComponent.h"
#include "Actor.h"

TileMapComponent::TileMapComponent(Actor * owner, const TileMapTexture& tilemapTexture, int drawOrder) 
: SpriteComponent(owner, drawOrder), tilemapTexture(tilemapTexture) {
}

void TileMapComponent::Draw(SDL_Renderer * renderer) {
	int tw = GetTexWidth();
	int th = GetTexHeight();
	int cw = tw / tilemapTexture.GetColumnCount();
	int ch = th / tilemapTexture.GetRowCount();
	int x = 0;
	int y = 0;
	for (int i = 0; i < tilemapTexture.GetLayerCount(); i++) {
		const CSV<int>& csv = tilemapTexture.GetLayer(i);
		x = 0;
		y = 0;
		for (int j = 0; j < csv.Count(); j++) {
			const CSVRow<int> row = csv.GetRowAt(j);
			for (int k = 0; k < row.Count(); k++) {
				int index = row.GetElementAt(k);
				drawCell(renderer, index, x, y, cw, ch);
				x += cw;
			}
			y += ch;
			x = 0;
		}
	}
}

void TileMapComponent::SetTileMapTexture(const TileMapTexture & tilemapTexture) {
	this->tilemapTexture = tilemapTexture;
}

void TileMapComponent::drawCell(SDL_Renderer* renderer, int index, int x, int y, int cw, int ch) {
	if (index == -1) {
		return;
	}
	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = cw;
	r.h = ch;

	SDL_Rect srcrect;
	srcrect.w = cw;
	srcrect.h = ch;
	srcrect.x = (index % tilemapTexture.GetColumnCount()) * cw;
	srcrect.y = (index / tilemapTexture.GetColumnCount()) * ch;

	// Draw (have to convert angle from radians to degrees, and clockwise to counter)
	SDL_RenderCopyEx(renderer,
		mTexture,
		&srcrect,
		&r,
		-Math::ToDegrees(mOwner->GetRotation()),
		nullptr,
		SDL_FLIP_NONE);
}
