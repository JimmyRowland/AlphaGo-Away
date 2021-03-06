#include "kd-tree.hpp"
#include <iostream>
#include <utility>
#include <vector>

KD_Tree::KD_Tree(int isEnemy) {
	for (auto entity : ECS::view<Motion, Property>()) {
		Property& property = ECS::registry<Property>.get(entity);
		if (property.isEnemy == isEnemy) {
			this->root = insert(this->root, entity);
		}
	}
}

void KD_Tree::print(KD_Node* root) {
	if (root) {
		Motion& motionEnt = ECS::registry<Motion>.get(root->e);
		std::cout << "(" << motionEnt.position.x << ", " << motionEnt.position.y << ") " << std::endl;
		std::cout << "left" << std::endl;
		print(root->left);
		std::cout << "right" << std::endl;
		print(root->right);
	}
}


KD_Tree::KD_Node* KD_Tree::insert(KD_Node*& root, ECS::Entity e, int depth) {
	if (root != nullptr) {
		Motion& motionEnt = ECS::registry<Motion>.get(e);
		Motion& motionRoot = ECS::registry<Motion>.get(root->e);
		if (depth % 2) {
			if (motionEnt.position.x < motionRoot.position.x) {
				root->left = insert(root->left, e, depth++);
			}
			else {
				root->right = insert(root->right, e, depth++);
			}
		}
		else {
			if (motionEnt.position.y < motionRoot.position.y) {
				root->left = insert(root->left, e, depth++);
			}
			else {
				root->right = insert(root->right, e, depth++);
			}
		}
	}
	else {
		root = new KD_Node(e);
	}
	return root;
}

void KD_Tree::nearest(KD_Node*& root, ECS::Entity e, ECS::Entity& res, float min, int depth) {
	if (root == nullptr) {
		return;
	}
	float d = dist(root->e, e);
	if (d >= min) {
		vec2 pose = ECS::registry<Motion>.get(e).position;
		vec2 posr = ECS::registry<Motion>.get(root->e).position;
		if (depth % 2) {
			if (pose.x < posr.x) {
				nearest(root->left, e, res);
			}
			else {
				nearest(root->right, e, res);
			}
		}
		else {
			if (pose.y < posr.y) {
				nearest(root->left, e, res);
			}
			else {
				nearest(root->right, e, res);
			}
		}
		return;
	}
	else {
		res = root->e;
		min = d;
		nearest(root->left, e, res, min, depth++);
		nearest(root->right, e, res, min, depth++);
	}

}