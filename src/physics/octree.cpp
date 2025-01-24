// Includes from project
#include "octree.hpp"

// Includes from STL


/*
* Check if the node is a leaf
* 
* @return bool True if the node is a leaf, false otherwise
*/
bool OctreeNode::isLeaf() const
{
	return m_pChildren.empty();
}


/*
* Add a child to the node
* 
* @param min The minimum point of the AABB of the child
* @param max The maximum point of the AABB of the child
* @return void
*/
void OctreeNode::addChildren(const Vec3& min, const Vec3& max)
{
	if (m_pChildren.size() < 8)
	{
		std::shared_ptr<OctreeNode> child = std::make_shared<OctreeNode>(min, max);
		child->m_pParent = this;
		m_pChildren.push_back(child);

	}
}


/*
* Detect if the input AABB has collided with some AABB of the octree
* 
* @param other The other AABB
* @return std::vector<OctreeNode*> The list of collided nodes
*/
std::vector<OctreeNode*> OctreeNode::detectCollision(const AABB& other) const
{
	auto collisionLambda = [&](const OctreeNode* pNode) -> bool {
		return pNode->hasCollided(other);
	};

	return detectCollisionInternal(collisionLambda);
}


/*
* Detect if the input ray has collided with some AABB of the octree
* 
* @param p0 The origin of the ray
* @param p1 The end of the ray
* @return std::vector<OctreeNode*> The list of collided nodes
*/
std::vector<OctreeNode*> OctreeNode::detectCollision(const Vec3& p0, const Vec3& p1) const
{
	auto collisionLambda = [&](const OctreeNode* pNode) -> bool {
		return pNode->hasCollided(p0, p1);
	};

	return detectCollisionInternal(collisionLambda);
}


/*
* Detect if the input collider has collided with some AABB of the octree
* The input collider is a lambda function that handle the collision detection according to the collider type
* 
* @param collisionLambda The lambda function that handle the collision detection
* @return std::vector<OctreeNode*> The list of collided nodes
*/
template <typename CollisionLambda>
std::vector<OctreeNode*> OctreeNode::detectCollisionInternal(CollisionLambda collisionLambda) const
{
	std::vector<OctreeNode*> collidedNodes;

	// Check if the AABB has collided with the other AABB
	if (collisionLambda(this))
	{
		// If the node is a leaf, return itself
		if (isLeaf())
		{
			collidedNodes.push_back(const_cast<OctreeNode*>(this));
		}
		else
		{
			// Check the children
			for (const auto& pChild : m_pChildren)
			{
				if (!pChild) // Shoudn't happen
				{
					continue;
				}

				// Recursively check the collisions in the children
				std::vector<OctreeNode*> collidedChildren = pChild->detectCollisionInternal(collisionLambda);

				// Insert the collided children into the list
				collidedNodes.insert(collidedNodes.end(), collidedChildren.begin(), collidedChildren.end());
			}
		}
	}

	return collidedNodes;
}