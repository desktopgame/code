#pragma once
#include "SDL/SDL.h"
#include <vector>
#include "CSV.h"

class TileMapTexture {
public:
	explicit TileMapTexture(int rowCount, int columnCount);
	void AddLayer(const CSV<int>& layer);
	void RemoveLayer(int index);
	const CSV<int>& GetLayer(int index) const;
	size_t GetLayerCount() const;

	int GetRowCount() const;
	int GetColumnCount() const;
private:
	int rowCount;
	int columnCount;
	std::vector<CSV<int> > layers;
};