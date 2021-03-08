//#include "tiny_ecs.hpp"
//#include "common.hpp"
//#include <utility>
//#include <vector>
//#ifndef KD_TREE_H
//#define KD_TREE_H
//
//
//
//class KD_Tree {
//public:
//	struct KD_Node {
//		ECS::Entity e;
//		KD_Node* left, * right;
//
//		KD_Node(ECS::Entity e, KD_Node* left = nullptr, KD_Node* right = nullptr) {
//			this->e = e;
//			this->left = left;
//			this->right = right;
//		}
//
//	};
//
//	KD_Node* root = nullptr;
//
//	KD_Tree(int isenemy);
//
//	KD_Node* insert(KD_Node*& root, ECS::Entity e, int depth = 0);
//
//	void nearest(KD_Node*& root, ECS::Entity e, ECS::Entity& res, float min = INFINITY, int depth = 0);
//
//	void print(KD_Node*);
//
//	float dist(ECS::Entity root, ECS::Entity e) {
//		vec2 pose = ECS::registry<Motion>.get(e).position;
//		vec2 posr = ECS::registry<Motion>.get(root).position;
//		return pow(pose.x - posr.x, 2) + pow(pose.y - posr.y, 2);
//	}
//
//
//
//};
//
//#endif