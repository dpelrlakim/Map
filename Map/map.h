#ifndef A2Q4MAP_H
#define A2Q4MAP_H

#include <iostream>
#include <string>
#include <initializer_list>
#include <utility>
#include <stdexcept>

namespace cs246e {
#define DEBUGGING 0
	
template <typename K, typename V> class map {
	struct BST {
		std::pair<K,V> nodePair;
		size_t n;
	 	BST *parent, *left, *right, *min, *max;
	 	BST(std::pair<K,V> nodePair, BST *parent);  
	};
 	BST *root;

 	BST *createLeaf(BST *slot, BST *parentNode, const std::pair<K,V> &p);
 	void map_insert(const std::pair<K,V> &p);
 	void copyTree(const BST *other);
 public:
	map();
 	map(const map &other);
 	map(map &&other);
  map(std::initializer_list<std::pair<K,V>> init);
 	map &operator=(map other);
 private:
 	void updateSize(BST *path, const std::string &&op);
 	void updateExtreme(BST *path, BST *newExtreme, const std::string &fieldToUpdate);
 	void promote(BST *target, BST *replacement);
 	void deleteNode(BST *node);
	void cutTree(BST *stem);
 	BST *keySearch(BST *node, const K &targetKey, BST **parent) const;
 public:
 	~map();
 	void clear();
	size_t erase (const K &targetKey);
 	V &at(const K &targetKey);
 	V &operator[](const K &targetKey);
 	const V &at(const K &targetKey) const;
	const V &operator[](const K &targetKey) const;
 	bool empty() const;
 	size_t size() const;
 	size_t count(const K &targetKey) const;
 	
 	class iterator {
 		BST *node;
 		size_t remaining;
 		iterator(BST *node, size_t remaining);
 	 public:
 	 	std::pair<K, V> &operator*() const;
 	 	bool operator!=(iterator &other) const;
 	 	iterator &operator++();
 	 	friend class map;
 	};
 	class const_iterator {
 		BST *node;
 		size_t remaining;
 		const_iterator(BST *node, size_t remaining);
 	 public:
 		const std::pair<K,V> &operator*() const;
 	 	bool operator!=(const_iterator &other) const;
 	 	const_iterator &operator++();
 	 	friend class map;
 	};
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

 #if DEBUGGING
 private:
	void printNode(const BST *node, const std::string &&property) const;
	void printRelation(const BST *node, size_t depth) const;
 public:
	void printStuff();
 #endif
};

	
// ---- Construction Helpers ----

template <typename K, typename V>
typename map<K,V>::BST *map<K,V>::createLeaf(BST *slot, BST *parentNode, 
	const std::pair<K,V> &p) {
	slot = static_cast <BST*> (operator new (sizeof(BST)));
	new (slot) BST(p, parentNode);

	if (parentNode) {
		updateSize(parentNode, "plus");
		if (parentNode->nodePair.first > slot->nodePair.first) {
			parentNode->left = slot;
			updateExtreme(parentNode, slot, "min");
		} else {
			parentNode->right = slot;
			updateExtreme(parentNode, slot, "max");
		}
	}
	return slot;
}

template <typename K, typename V>
void map<K,V>::map_insert(const std::pair<K,V> &p) {
	if (!root) {
		root = createLeaf(root, nullptr, p);
		return;
	}
	const K &targetKey = p.first;
	BST *parentNode = nullptr; 
  BST *node = root;

	while (node) { // Look for "key slot"
		if (node->nodePair.first == targetKey) return;
		parentNode = node;
		if (node->nodePair.first > targetKey) node = node->left;
		else node = node->right;
	}
	createLeaf(node, parentNode, p);
}

template <typename K, typename V>
void map<K,V>::copyTree(const BST *other) {
	if (!other) return;
	map_insert({other->nodePair.first, other->nodePair.second});
	if (other->left) copyTree(other->left);
	if (other->right) copyTree(other->right);
}


// ---- Construction/Assignment ----

template <typename K, typename V> 
map<K,V>::BST::BST(std::pair<K,V> nodePair, BST *parent): 
	nodePair{std::pair<K,V>(nodePair.first, nodePair.second)}, n{1}, 
	parent{parent}, left{nullptr}, right{nullptr}, min{this}, max{this} {}  

template <typename K, typename V>
map<K,V>::map(): root{nullptr} {};

template <typename K, typename V> 
map<K,V>::map(const map &other): root{nullptr} { copyTree(other.root); }

template <typename K, typename V> 
map<K,V>::map(map &&other): root{other.root} { other.root = nullptr; }

template <typename K, typename V> 
map<K,V>::map(std::initializer_list<std::pair<K, V>> init): root{nullptr} { 
	for (auto x:init) map_insert(x); 
}

template <typename K, typename V>
typename map<K,V>::map &map<K, V>::operator=(map other) {
	std::swap(root, other.root);
	return *this;
}


// ---- Field update helpers ----

template <typename K, typename V>
void map<K,V>::updateSize(map<K,V>::BST *path, const std::string &&op) {
	while (path) {
		if (op == "plus") ++(path->n);
		else if (op == "minus") --(path->n);
		path = path->parent;
	}
}

template <typename K, typename V>
void map<K,V>::updateExtreme(map<K,V>::BST *path, map<K,V>::BST *newExtreme, 
	const std::string &fieldToUpdate) {
	if (!path) return;

	if (fieldToUpdate == "min") {
		if (path->min == newExtreme) return;
		path->min = newExtreme;
		if (path->parent && path->parent->left == path) 
			updateExtreme(path->parent, newExtreme, fieldToUpdate);

	} else if (fieldToUpdate == "max") {
		if (path->max == newExtreme) return;
		path->max = newExtreme;
		if (path->parent && path->parent->right == path) 
			updateExtreme(path->parent, newExtreme, fieldToUpdate);
	}
}

template <typename K, typename V>
void map<K,V>::promote(map<K,V>::BST *target, map<K,V>::BST *replacement) {
	if (replacement) replacement->parent = target->parent;
	if (!target->parent) return;

	if (target == target->parent->left) {
		target->parent->left = replacement;
		if (replacement) updateExtreme(target->parent, replacement->min, "min");
		else updateExtreme(target->parent, target->parent, "min");

	} else {
		target->parent->right = replacement;
		if (replacement) updateExtreme(target->parent, replacement->max, "max");
		else updateExtreme(target->parent, target->parent, "max");
	}
}

template <typename K, typename V>
typename map<K,V>::BST *map<K,V>::keySearch(BST *node, const K &targetKey, BST **parent) const {
	if (!node) return nullptr;
	if (targetKey == node->nodePair.first) return node;
	if (parent) *parent = node;
	if (targetKey < node->nodePair.first) return keySearch(node->left, targetKey, parent);
	else return keySearch(node->right, targetKey, parent); 
}


// ---- Destruction ----

template <typename K, typename V>
size_t map<K,V>::erase (const K &targetKey) {
	BST *target = keySearch(root, targetKey, nullptr);
	if (!target) return 0;
	if (size() == 1) {
		deleteNode(root);
		root = nullptr;
		return 1;
	}
  BST *startOfChain, *replacement;

  if (target->left && target->right) {
 	  replacement = target->right->min; // Promote leftmost node in the right subtree
 		startOfChain = replacement;
 		if (replacement != target->right) {
 			startOfChain = replacement->parent;
 			promote(replacement, replacement->right);
 			replacement->max = target->max;
			replacement->right = target->right;
			replacement->right->parent = replacement;
		}
		replacement->n = target->n;
 		replacement->min = target->min;
 		replacement->left = target->left;
 		replacement->left->parent = replacement;
	} else {
 		startOfChain = target->parent;
 		if (!target->left && !target->right) replacement = nullptr;
 		else if (!target->right) replacement = target->left;
 		else replacement = target->right;
 	}

	promote(target, replacement);
	updateSize(startOfChain, "minus");
 	if (target == root) root = replacement;
	deleteNode(target);
	return 1;
} 	

template <typename K, typename V>
void map<K,V>::deleteNode(BST *node) {
	node->nodePair.first.~K();
	node->nodePair.second.~V();
	operator delete (node);
}

template <typename K, typename V>
void map<K,V>::cutTree(BST *stem) {
	if (stem->right) cutTree(stem->right);
	if (stem->left) cutTree(stem->left);
	deleteNode(stem);
}

template <typename K, typename V>
void map<K,V>::clear() { 
	if (root) cutTree(root); 
	root = nullptr;
}

template <typename K, typename V> map<K,V>::~map() { clear(); }


// ---- Accessors ----

template <typename K, typename V>
V &map<K,V>::at(const K &targetKey) {
	BST *target = keySearch(root, targetKey, nullptr);
	if (!target) throw std::out_of_range("No such key\n");
	else return target->nodePair.second;
}

template <typename K, typename V>
V &map<K,V>::operator[](const K &targetKey) {
	BST *parent = nullptr; // Remembering the parent allows us to insert in O(h)
	BST *targetNode = keySearch(root, targetKey, &parent);
	if (!targetNode) targetNode = createLeaf(targetNode, parent, {targetKey, V()});
	if (!root) root = targetNode;
	return targetNode->nodePair.second;
}

template <typename K, typename V>
const V &map<K,V>::at(const K &targetKey) const {
	BST *target = keySearch(root, targetKey, nullptr);
	if (!target) throw std::out_of_range("No such key\n");
	else return target->nodePair.second;
}

template <typename K, typename V>
const V &map<K,V>::operator[](const K &targetKey) const {
	BST *parent = nullptr; 
	BST *targetNode = keySearch(root, targetKey, &parent);
	if (!targetNode) targetNode = createLeaf(targetNode, parent, {targetKey, V()});
	if (!root) root = targetNode;
	return targetNode->nodePair.second;
}

template <typename K, typename V>
size_t map<K,V>::size() const {
	if (!root) return 0;
	return root->n;
}

template <typename K, typename V>
bool map<K,V>::empty() const { return !size(); }

template <typename K, typename V>
size_t map<K,V>::count(const K &targetKey) const { 
	return keySearch(root, targetKey, nullptr) != nullptr; 
}


// ---- Iteration ----

template <typename K, typename V>
map<K,V>::iterator::iterator(BST *node, size_t remaining): 
	node{node}, remaining{remaining} {}

template <typename K, typename V>
std::pair<K, V> &map<K,V>::iterator::operator*() const { return node->nodePair; }

template <typename K, typename V>
bool map<K,V>::iterator::operator!=(iterator &other) const { 
	return (node != other.node) || (remaining != other.remaining); 
}

template <typename K, typename V>
typename map<K,V>::iterator &map<K,V>::iterator::operator++() {
	--remaining;
	if (!remaining) return *this;
	if (node->right) {
		node = node->right;
		while (node->left) node = node->left;
	} else if (node->parent) {
		while (node->nodePair.first > node->parent->nodePair.first) node = node->parent;
		node = node->parent;
	}
	return *this;
}

template <typename K, typename V>
map<K,V>::const_iterator::const_iterator(BST *node, size_t remaining): 
	node{node}, remaining{remaining} {}

template <typename K, typename V>
const std::pair<K, V> &map<K,V>::const_iterator::operator*() const { 
	return node->nodePair; 
}

template <typename K, typename V>
bool map<K,V>::const_iterator::operator!=(const_iterator &other) const { 
	return (node != other.node) || (remaining != other.remaining); 
}

template <typename K, typename V>
typename map<K,V>::const_iterator &map<K,V>::const_iterator::operator++() {
	--remaining;
	if (!remaining) return *this;
	if (node->right) {
		node = node->right;
		while (node->left) node = node->left;
	} else if (node->parent) {
		while (node->nodePair.first > node->parent->nodePair.first) node = node->parent;
		node = node->parent;
	}
	return *this;
}

template <typename K, typename V>
typename map<K,V>::iterator map<K,V>::begin() {
	if (root) return iterator { root->min, size() };
	return iterator { nullptr, 0 };
}

template <typename K, typename V>
typename map<K,V>::iterator map<K,V>::end() { 
	if (root) return iterator { root->max, 0 }; 
	return iterator { nullptr, 0 };
}

template <typename K, typename V>
typename map<K,V>::const_iterator map<K,V>::begin() const {
	if (root) return const_iterator { root->min, size() };
	return const_iterator { nullptr, 0 };
}

template <typename K, typename V>
typename map<K,V>::const_iterator map<K,V>::end() const { 
	if (root) return const_iterator { root->max, 0 }; 
	return const_iterator { nullptr, 0 };
}

//---- Debugging ----

#if DEBUGGING

template <typename K, typename V>
void map<K,V>::printNode(const BST *node, const std::string &&property) const {
	if (!node) {
		std::cout << "None\n";
		return; 
	}
	if (property == "pair") {
		std::cout << "(" << node->nodePair.first
							<< "," << node->nodePair.second << ")";
	} else if (property == "n") {
		std::cout << node->n << std::endl;
	}
}

template <typename K, typename V>
void map<K,V>::printRelation(const BST *node, size_t depth) const {
	if (!node) return;

	std::cout << "depth: " << depth << std::endl;
	if (!node->right && !node->left) {
		printNode(node, "pair");
		std::cout << " is a leaf\n-End of path-\n";
		return;
	}

	if (node->left) {
		printNode(node, "pair");
		std::cout << "'s left: ";
		printNode(node->left, "pair");
		std::cout << "\n";
	} else std::cout << "no left\n";
	printRelation(node->left, depth + 1);

	std::cout << "depth: " << depth << std::endl;
	if (node->right) {
		printNode(node, "pair");
		std::cout << "'s right: ";
		printNode(node->right, "pair");
		std::cout << "\n";
	} else std::cout << "no right\n";
	printRelation(node->right, depth + 1);
}

template <typename K, typename V>
void map<K,V>::printStuff() {
	std::cout << "----printStuff----\n";
	std::cout << "size: " << size() << std::endl;
	if (!root) return;
	
	std::cout << "size of left: ";
	printNode(root->left, "n");

	std::cout << "size of right: ";
	printNode(root->right, "n");

	std::cout << "root: ";
	printNode(root, "pair");
	std::cout << std::endl;

	std::cout << "root->max: ";
	printNode(root->max, "pair");
	std::cout << std::endl;

	std::cout << "root->min: ";
	printNode(root->min, "pair");
	std::cout << std::endl;
	
	std::cout << "------------------\n";
	std::cout << "Relations:\n";
	printRelation(root, 1);
	std::cout << "------------------\n";
}
#endif
}
#endif
