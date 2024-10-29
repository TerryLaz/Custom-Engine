// ELEFTERIOS LAZARIDIS
#include "QuadTree.h"

void QuadTree::subdivide(Node* node, int depth)
{
	if (depth <= 0 || node == nullptr)
		return;

	DirectX::XMFLOAT3 extentsHalf;
	extentsHalf.x = node->bounds.Extents.x * 0.5f;
	extentsHalf.y = node->bounds.Extents.y * 0.5f;
	extentsHalf.z = node->bounds.Extents.z * 0.5f;

	for (int i = 0; i < 4; ++i) {
		DirectX::XMFLOAT3 childCenter = node->bounds.Center;
		// Calculate child's center based on quadrant
		childCenter.x += ((i & 1) ? 1 : -1) * extentsHalf.x;
		childCenter.z += ((i & 2) ? 1 : -1) * extentsHalf.z;

		// Create a child node with new center and half the extents
		DirectX::BoundingBox childBounds;
		DirectX::XMStoreFloat3(&childBounds.Center, DirectX::XMLoadFloat3(&childCenter));
		DirectX::XMStoreFloat3(&childBounds.Extents, DirectX::XMLoadFloat3(&extentsHalf));

		node->children[i] = std::make_unique<Node>();
		node->children[i]->isLeaf = true;
		node->children[i]->bounds = childBounds;
		subdivide(node->children[i].get(), depth - 1);
	}
}

void QuadTree::insertObject(Node* node, RenderableObject* object)
{
	if (node == nullptr || object == nullptr) {
		return;
	}

	// If node is a leaf and doesn't exceed maximum object count or depth, insert here
	if (node->isLeaf) {
		node->objects.push_back(object);
		return;
	}

	// Otherwise, try to insert the object into the appropriate child node
	for (int i = 0; i < 4; ++i) {
		if (node->children[i]->bounds.Contains(object->GetBoundingBox()) != DirectX::DISJOINT) {
			insertObject(node->children[i].get(), object);
			return;
		}
	}

	// If the object doesn't fit neatly into a child, keep it at this node level
	node->objects.push_back(object);
}

void QuadTree::cullWithFrustum(Node* node, const DirectX::BoundingFrustum& frustum, std::vector<RenderableObject*>& visibleObjects)
{
	if (node == nullptr) {
		return;
	}

	// Test the node's bounding box against the frustum
	if (frustum.Contains(node->bounds) == DirectX::DISJOINT) {
		return; // Node is completely outside the frustum; cull it and its children
	}

	// If the node is a leaf, test each object in the node
	if (node->isLeaf) {
		for (RenderableObject* object : node->objects) {
			if (frustum.Contains(object->GetBoundingBox()) != DirectX::DISJOINT) {
				visibleObjects.push_back(object); // Object is potentially visible
			}
		}
	}
	else {
		// Recursively test child nodes
		for (int i = 0; i < 4; ++i) {
			cullWithFrustum(node->children[i].get(), frustum, visibleObjects);
		}
	}
}

QuadTree::QuadTree()
{
	root = std::make_unique<Node>();
	root->bounds = worldBounds;
	subdivide(root.get(), maxDepth);
}

QuadTree::~QuadTree()
{
}

void QuadTree::insertObject(RenderableObject* object)
{
	insertObject(root.get(), object);
}

std::vector<RenderableObject*> QuadTree::cullWithFrustum(const DirectX::BoundingFrustum& frustum)
{
	std::vector<RenderableObject*> visibleObjects;
	cullWithFrustum(root.get(), frustum, visibleObjects);
	return visibleObjects;
}
