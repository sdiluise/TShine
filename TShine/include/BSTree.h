/*
 * BSTree.h
 *
 *  Created on: Jul 27, 2014
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch
 *
 *      Binary Search Tree Templated Class
 *
 *
 */



#ifndef BSTREE_H_
#define BSTREE_H_

#include<iostream>
#include<cassert>
using namespace std;

template<class T,class LessClass>
class TreeClass {

private:
	struct Node{Node *left, *right, *parent; T key; T key2;
	            int data; int state; int order;};
	Node *root;

public:
	TreeClass(): root(0),Nentries(0){};
	//virtual ~TreeClass();
	void insert(T);
	void insert(T,int);
	void print(){p_print(root);cout<<endl;}
	bool search(T user_key){return p_serach(root,user_key);}
	T minimum();
	T maximum();
	T maximum_value();
    int Nentries;
private:
	LessClass fLess;
	void p_print(Node *);
	bool p_search(Node *x, T user_key);

	//ClassDef(TreeClass,1)
};

#include "../src/BSTree.cct" //!

#endif /* BSTREE_H_ */
