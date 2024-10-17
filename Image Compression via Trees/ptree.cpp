/*
*  File:        ptree.cpp
*  Description: Implementation of a partitioning tree class for CPSC 221 PA3
*  Date:        2022-03-03 01:53
*
*               ADD YOUR PRIVATE FUNCTION IMPLEMENTATIONS TO THE BOTTOM OF THIS FILE
*/

#include "ptree.h"
#include "hue_utils.h" // useful functions for calculating hue averages

using namespace cs221util;
using namespace std;

// The following definition may be convenient, but is not necessary to use
typedef pair<unsigned int, unsigned int> pairUI;

/////////////////////////////////
// PTree private member functions
/////////////////////////////////

/*
*  Destroys all dynamically allocated memory associated with the current PTree object.
*  You may want to add a recursive helper function for this!
*  POST: all nodes allocated into the heap have been released.
*/
void PTree::Clear() {
  Clear(root);
  delete root;
  root = nullptr;
}

void PTree::Clear(Node* node) {

  if (!node){
    return;
  }

  Clear(node->A);
  Clear(node->B);

  if (node->A)
  {
    delete node->A;
    node->A = nullptr;
  }

  if (node->B)
  {
    delete node->B;
    node->B = nullptr;
  }
}

/*
*  Copies the parameter other PTree into the current PTree.
*  Does not free any memory. Should be called by copy constructor and operator=.
*  You may want a recursive helper function for this!
*  PARAM: other - the PTree which will be copied
*  PRE:   There is no dynamic memory associated with this PTree.
*  POST:  This PTree is a physically separate copy of the other PTree.
*/
void PTree::Copy(const PTree& other) {
  root = Copy(other.root);
}

Node* PTree::Copy(Node* otherNode) {

  if (!otherNode)
  {
    return nullptr;
  }

  Node *newNodeA = Copy(otherNode->A);
  Node *newNodeB = Copy(otherNode->B);

  return new Node(otherNode->upperleft, otherNode->width, otherNode->height, otherNode->avg, newNodeA, newNodeB);
}

/*
*  Private helper function for the constructor. Recursively builds the tree
*  according to the specification of the constructor.
*  You *may* change this if you like, but we have provided here what we
*  believe will be sufficient to use as-is.
*  PARAM:  im - full reference image used for construction
*  PARAM:  ul - upper-left image coordinate of the currently building Node's image region
*  PARAM:  w - width of the currently building Node's image region
*  PARAM:  h - height of the currently building Node's image region
*  RETURN: pointer to the fully constructed Node
*/
Node* PTree::BuildNode(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {

  // base case
  if (w == 1 && h == 1)
  {
    // build leaf node for a single pixel
    Node* leaf = new Node(ul, 1, 1, *im.getPixel(ul.first, ul.second), nullptr, nullptr);
    return leaf;
  }

  Node *regionA = nullptr, *regionB = nullptr;

  // recursive step
  if (h > w)
  {
    // divide vertically into two regions and build them
    regionA = BuildNode(im, ul, w, h/2);
    regionB = BuildNode(im, pair<unsigned int, unsigned int>(ul.first, ul.second + h/2), w, h - (h/2));
  }
  else
  {
    // divide horizontally into two regions and build them
    regionA = BuildNode(im, ul, w/2, h);
    regionB = BuildNode(im, pair<unsigned int, unsigned int>(ul.first + w/2, ul.second), w - (w/2), h);
  }

  // post-order traversal action: build the node for the argument region

  // traverse im section and sum all pixel contributions to average
  double sumHX = 0, sumHY = 0, sumS = 0, sumL = 0, sumA = 0;
  for (int x = ul.first; x < ul.first + w; x++)
  {
    for (int y = ul.second; y < ul.second + h; y++)
    {
      // add pixel contributions to running sums
      HSLAPixel p = *im.getPixel(x, y);
      sumHX += Deg2X(p.h);
      sumHY += Deg2Y(p.h);
      sumS += p.s;
      sumL += p.l;
      sumA += p.a;
    }
  }

  // compute channel averages
  int n = w*h;
  double avgHX = sumHX / n;
  double avgHY = sumHY / n;
  double avgH = XY2Deg(avgHX, avgHY);
  double avgS = sumS / n;
  double avgL = sumL / n;
  double avgA = sumA / n;

  HSLAPixel regionAvg = HSLAPixel(avgH, avgS, avgL, avgA);
  return new Node(ul, w, h, regionAvg, regionA, regionB);
}

////////////////////////////////
// PTree public member functions
////////////////////////////////

/*
*  Constructor that builds the PTree using the provided PNG.
*
*  The PTree represents the sub-image (actually the entire image) from (0,0) to (w-1, h-1) where
*  w-1 and h-1 are the largest valid image coordinates of the original PNG.
*  Each node corresponds to a rectangle of pixels in the original PNG, represented by
*  an (x,y) pair for the upper-left corner of the rectangle, and two unsigned integers for the
*  number of pixels on the width and height dimensions of the rectangular sub-image region the
*  node defines.
*
*  A node's two children correspond to a partition of the node's rectangular region into two
*  equal (or approximately equal) size regions which are either tiled horizontally or vertically.
*
*  If the rectangular region of a node is taller than it is wide, then its two children will divide
*  the region into vertically-tiled sub-regions of equal height:
*  +-------+
*  |   A   |
*  |       |
*  +-------+
*  |   B   |
*  |       |
*  +-------+
*
*  If the rectangular region of a node is wider than it is tall, OR if the region is exactly square,
*  then its two children will divide the region into horizontally-tiled sub-regions of equal width:
*  +-------+-------+
*  |   A   |   B   |
*  |       |       |
*  +-------+-------+
*
*  If any region cannot be divided exactly evenly (e.g. a horizontal division of odd width), then
*  child B will receive the larger half of the two subregions.
*
*  When the tree is fully constructed, each leaf corresponds to a single pixel in the PNG image.
* 
*  For the average colour, this MUST be computed separately over the node's rectangular region.
*  Do NOT simply compute this as a weighted average of the children's averages.
*  The functions defined in hue_utils.h and implemented in hue_utils.cpp will be very useful.
*  Computing the average over many overlapping rectangular regions sounds like it will be
*  inefficient, but as an exercise in theory, think about the asymptotic upper bound on the
*  number of times any given pixel is included in an average calculation.
* 
*  PARAM: im - reference image which will provide pixel data for the constructed tree's leaves
*  POST:  The newly constructed tree contains the PNG's pixel data in each leaf node.
*/
PTree::PTree(PNG& im) {
  root = BuildNode(im, pair<unsigned int, unsigned int>(0,0), im.width(), im.height());
}

/*
*  Copy constructor
*  Builds a new tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  This tree is constructed as a physically separate copy of other tree.
*/
PTree::PTree(const PTree& other) {
  this->Copy(other);
}

/*
*  Assignment operator
*  Rebuilds this tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  If other is a physically different tree in memory, all pre-existing dynamic
*           memory in this tree is deallocated and this tree is reconstructed as a
*           physically separate copy of other tree.
*         Otherwise, there is no change to this tree.
*/
PTree& PTree::operator=(const PTree& other) {

  if (this != &other)
  {
    this->Clear();
    this->Copy(other);
  }
  return *this;
}

/*
*  Destructor
*  Deallocates all dynamic memory associated with the tree and destroys this PTree object.
*/
PTree::~PTree() {
  this->Clear();
}

/*
*  Traverses the tree and puts the leaf nodes' color data into the nodes'
*  defined image regions on the output PNG.
*  For non-pruned trees, each leaf node corresponds to a single pixel that will be coloured.
*  For pruned trees, each leaf node may cover a larger rectangular region that will be
*  entirely coloured using the node's average colour attribute.
*
*  You may want to add a recursive helper function for this!
*
*  RETURN: A PNG image of appropriate dimensions and coloured using the tree's leaf node colour data
*/
PNG PTree::Render() const {
  // replace the line below with your implementation
  PNG renderedPNG = PNG(root->width, root->height);
  Render(renderedPNG, root);
  return PNG(renderedPNG);
}

void PTree::Render(PNG& png, Node* node) const {
    if(node==NULL) {
    return;
  }

  Render(png, node->A);
  Render(png, node->B);

  if(node->B == NULL) { // node is leaf (full tree so only have to check one child)
    for(int i=0; i<node->width; i++) { 
      for(int j=0; j<node->height; j++) {
        HSLAPixel* curr = png.getPixel(node->upperleft.first+i, node->upperleft.second+j);
        curr->h = node->avg.h;
        curr->s = node->avg.s;
        curr->l = node->avg.l;
        curr->a = node->avg.a;
      }
    }
  }
}

/*
*  Trims subtrees as high as possible in the tree. A subtree is pruned
*  (its children are cleared/deallocated) if ALL of its leaves have colour
*  within tolerance of the subtree root's average colour.
*  Pruning criteria should be evaluated on the original tree, and never on a pruned
*  tree (i.e. we expect that Prune would be called on any tree at most once).
*  When processing a subtree, you should determine if the subtree should be pruned,
*  and prune it if possible before determining if it has subtrees that can be pruned.
* 
*  You may want to add (a) recursive helper function(s) for this!
*
*  PRE:  This tree has not been previously pruned (and is not copied/assigned from a tree that has been pruned)
*  POST: Any subtrees (as close to the root as possible) whose leaves all have colour
*        within tolerance from the subtree's root colour will have their children deallocated;
*        Each pruned subtree's root becomes a leaf node.
*/
void PTree::Prune(double tolerance) {
  Prune(root, tolerance);
}

void PTree::Prune(Node* node, double tolerance) {

  // base case 1: null node
  if (!node)
  {
    return;
  }

  // base case 2: node is a pixel
  if (node->width == 1 && node->height == 1)
  {
    return;
  }

  // base case 3: the subtree rooted at node can be pruned
  if (Prunable(node, tolerance, node->avg))
  {
    Clear(node);
    return;
  }

  // recursive step: if subtree rooted at node cannot be pruned, check children
  else
  {
    Prune(node->A, tolerance);
    Prune(node->B, tolerance);
  }

}

bool PTree::Prunable(Node* node, double tolerance, HSLAPixel benchmark) {

  // base case 1: a null node is prunable
  if (!node)
  {
    return true;
  }

  // base case 2: 1-pixel leaf node is checked for tolerance
  if (node->height == 1 && node->width == 1)
  {
    return node->avg.dist(benchmark) <= tolerance;
  }

  // recursive step: an internal node is prunable if both its subtrees are prunable
  return Prunable(node->A, tolerance, benchmark) && Prunable(node->B, tolerance, benchmark);
}

/*
*  Returns the total number of nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::Size() const {
  
  return Size(root);
}

int PTree::Size(Node* node) const {

  if (!node)
  {
    return 0;
  }

  return 1 + Size(node->A) + Size(node->B);
}

/*
*  Returns the total number of leaf nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::NumLeaves() const {
  return NumLeaves(root);
}

int PTree::NumLeaves(Node* node) const {

  if (!node)
  {
    return 0;
  }

  if (!node->A && !node->B)
  {
    // this node is a leaf
    return 1;
  }

  return NumLeaves(node->A) + NumLeaves(node->B);
}

/*
*  Rearranges the nodes in the tree, such that a rendered PNG will be flipped horizontally
*  (i.e. mirrored over a vertical axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped horizontally.
*/
void PTree::FlipHorizontal() {
  
  FlipHorizontal(root);

}

void PTree::FlipHorizontal(Node* node) {

  if(node == NULL || node->A == NULL){
    return;
  }

  /**
   * POST:
   * 
   * this node is not null
   * this node is not a leaf node <=> both its children are non-null
   */

  if(node->A->upperleft.second == node->B->upperleft.second) {

    /**
     * POST:
     * 
     * Both children share the same y-value <=> they are arranged horizontally from one another
     */

    node->B->upperleft = node->A->upperleft;
    node->A->upperleft.first += node->B->width;
    Node* temp = node->A;
    node->A = node->B;
    node->B = temp;

    /**
     * POST:
     * 
     * Node's children have been swapped
     * A and B's ul's have been updated
     */
  }

  ReconileNodesChildren(node->A);
  ReconileNodesChildren(node->B);

  FlipHorizontal(node->A);
  FlipHorizontal(node->B);
}

void PTree::ReconileNodesChildren(Node* node) {

  if (!node || !node->A)
  {
    return;
  }

  /**
   * POST:
   * 
   * Node is not null
   * Parent node is not leaf <=> A and B are both non-null
   */

  if (node->A->upperleft.first == node->B->upperleft.first)
  {
  /**
   * POST:
   * 
   * A and B are vertically arranged
   * A is above B
   * 
   */

    node->A->upperleft = node->upperleft;
    node->B->upperleft = node->upperleft;
    node->B->upperleft.second += node->A->height;
    /**
     * POST:
     * 
     * A and B agree with Node
     */

  }
  else
  {
    /**
     * POST:
     * 
     * A and B are horizontally arranged
     * A is to the left of B
     * 
     */

    node->A->upperleft = node->upperleft;
    node->B->upperleft = node->upperleft;
    node->B->upperleft.first += node->A->width;

    /**
     * POST:
     * 
     * A and B agree with Node
     */
  }

  /**
   * POST:
   * 
   * A and B are reconciled with parent -> exit
   */
}

/*
*  Like the function above, rearranges the nodes in the tree, such that a rendered PNG
*  will be flipped vertically (i.e. mirrored over a horizontal axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped vertically.
*/
void PTree::FlipVertical() {
  FlipVertical(root);
}

void PTree::FlipVertical(Node* node) {

  if(node == NULL || node->A == NULL){
    return;
  }

  /**
   * POST:
   * 
   * this node is not null
   * this node is not a leaf node <=> both its children are non-null
   */

  if(node->A->upperleft.first == node->B->upperleft.first) {

    /**
     * POST:
     * 
     * Both children share the same x-value <=> they are arranged vertically from one another
     */

    node->B->upperleft = node->A->upperleft;
    node->A->upperleft.second += node->B->height;
    Node* temp = node->A;
    node->A = node->B;
    node->B = temp;

    /**
     * POST:
     * 
     * Node's children have been swapped
     * A and B's ul's have been updated
     */
  }

  ReconileNodesChildren(node->A);
  ReconileNodesChildren(node->B);

  FlipVertical(node->A);
  FlipVertical(node->B);
  

}

/*
    *  Provides access to the root of the tree.
    *  Dangerous in practice! This is only used for testing.
    */
Node* PTree::GetRoot() {
  return root;
}

//////////////////////////////////////////////
// PERSONALLY DEFINED PRIVATE MEMBER FUNCTIONS
//////////////////////////////////////////////

