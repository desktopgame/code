#include "TileMapTexture.h"

TileMapTexture::TileMapTexture(int rowCount, int columnCount) 
	: layers(), rowCount(rowCount), columnCount(columnCount) {
}

void TileMapTexture::AddLayer(const CSV<int>& layer) {
	layers.emplace_back(layer);
}

void TileMapTexture::RemoveLayer(int index) {
	layers.erase(layers.begin() + index);
}

const CSV<int>& TileMapTexture::GetLayer(int index) const {
	return layers.at(index);
}

size_t TileMapTexture::GetLayerCount() const {
	return layers.size();
}

int TileMapTexture::GetRowCount() const {
	return rowCount;
}

int TileMapTexture::GetColumnCount() const {
	return columnCount;
}
