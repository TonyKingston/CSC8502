#pragma once
#include "Mesh.h"
#include "Vector2.h"
#include <string>
#include <vector>

class HeightMap : public Mesh
{
public:
	HeightMap(const std::string& name);
	~HeightMap(void) {};
	
	Vector3 GetHeightmapSize() const { return heightmapSize; }
	float GetHeightForPosition(Vector2 pos) const;
protected:
	Vector3 heightmapSize;
	vector<float> heights;
	int width;
};

