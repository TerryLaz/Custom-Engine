// ELEFTERIOS LAZARIDIS
#pragma once
#include <DirectXCollision.h>
#include <vector>
#include "RenderableObject.h"

struct Node
{
	bool isLeaf = true;
	DirectX::BoundingBox bounds;
	std::vector<RenderableObject*> objects;
	std::unique_ptr<Node> children[4];
};

class QuadTree
{
private:
	std::unique_ptr<Node> root;
	int maxDepth = 4;
	DirectX::BoundingBox worldBounds = { DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(3000.0f, 3000.0f, 3000.0f) };// Define the world bounds

	void subdivide(Node* node, int depth);
	void insertObject(Node* node, RenderableObject* object);
	void cullWithFrustum(Node* node, const DirectX::BoundingFrustum& frustum, std::vector<RenderableObject*>& visibleObjects);
public:
	QuadTree();
	~QuadTree();

	void insertObject(RenderableObject* object);
	std::vector<RenderableObject*> cullWithFrustum(const DirectX::BoundingFrustum& frustum);
};