/*
*  File:        ptree-private.h
*  Description: Private functions for the PTree class
*  Date:        2022-03-06 03:30
*
*               DECLARE YOUR PTREE PRIVATE MEMBER FUNCTIONS HERE
*/

#ifndef _PTREE_PRIVATE_H_
#define _PTREE_PRIVATE_H_

/////////////////////////////////////////////////
// DEFINE YOUR PRIVATE MEMBER FUNCTIONS HERE
//
// Just write the function signatures.
//
// Example:
//
// Node* MyHelperFunction(int arg_a, bool arg_b);
//
/////////////////////////////////////////////////


int NumLeaves(Node* node) const;

int Size(Node* node) const;

void Clear(Node* node);

Node* Copy(Node* oldNode);

void FlipHorizontal(Node* node);

void FlipVertical(Node* node);

void Prune(Node* node, double tolerance);

bool Prunable(Node* node, double tolerance, HSLAPixel benchmark);

void Render(PNG& png, Node* node) const;

void ReconileNodesChildren(Node* node);

#endif