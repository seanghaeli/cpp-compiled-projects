// File:        imglist.cpp
// Date:        2022-01-27 10:21
// Description: Contains partial implementation of ImgList class
//              for CPSC 221 2021W2 PA1
//              Function bodies to be completed by yourselves
//
// ADD YOUR FUNCTION IMPLEMENTATIONS IN THIS FILE
//

#include "imglist.h"

#include <math.h> // provides fmin and fabs functions

/**************************
 * MISCELLANEOUS FUNCTIONS *
 **************************/

/*
 * This function is NOT part of the ImgList class,
 * but will be useful for one of the ImgList functions.
 * Returns the "difference" between two hue values.
 * PRE: hue1 is a double between [0,360).
 * PRE: hue2 is a double between [0,360).
 * 
 * The hue difference is the absolute difference between two hues,
 * but takes into account differences spanning the 360 value.
 * e.g. Two pixels with hues 90 and 110 differ by 20, but
 *      two pixels with hues 5 and 355 differ by 10.
 */
double HueDiff(double hue1, double hue2) {
  return fmin(fabs(hue1 - hue2), fabs(360 + fmin(hue1, hue2) - fmax(hue1, hue2)));
}

double mod360(double x) {
  double result = fmod(x, 360);
  return result < 0 ? result + 360 : result;
}

/*********************
 * CONSTRUCTORS, ETC. *
 *********************/

/*
 * Default constructor. Makes an empty list
 */
ImgList::ImgList(): northwest(nullptr), southeast(nullptr) {}

/*
 * Creates a list from image data
 * PRE: img has dimensions of at least 1x1
 */
ImgList::ImgList(PNG & img) {
  const int imgWidth = img.width();
  const int imgHeight = img.height();

  // pntr to the previous node added to this ImgList
  ImgNode * lastNode = nullptr;

  // pntr to a vector containing pntrs to the last row of pixel nodes created
  vector < ImgNode * > * lastRow = new vector < ImgNode * > (imgWidth);

  // pntr to a vector containing pntrs to to the current row of pixel nodes being created during iteration
  vector < ImgNode * > * currentRow = new vector < ImgNode * > (imgWidth);

  for (int y = 0; y < imgHeight; y++) {
    for (int x = 0; x < imgWidth; x++) {
      // create new ImgNode for pixel
      ImgNode * node = new ImgNode();
      node -> colour = * (img.getPixel(x, y));

      if (x != 0) // if pixel is not in leftmost column
      {
        // link left
        node -> west = lastNode;
        lastNode -> east = node;
      } else if (y == 0) // if pixel is the upper, leftmost one
      {
        this -> northwest = node;
      }

      if (y != 0) // if pixel is not in uppermost row
      {
        // link up
        node -> north = lastRow -> at(x);
        lastRow -> at(x) -> south = node;
      }
      currentRow -> at(x) = node;
      lastNode = node;
    }
    vector < ImgNode * > * tempPntr = lastRow;
    lastRow = currentRow;
    currentRow = tempPntr;
  }
  // last pixel is the lowest, rightmost one
  this -> southeast = lastNode;

  delete lastRow;
  delete currentRow;
}

/*
 * Copy constructor.
 * Creates this this to become a separate copy of the data in otherlist
 */
ImgList::ImgList(const ImgList & otherlist) {
  // build the linked node structure using otherlist as a template
  Copy(otherlist);
}

/*
 * Assignment operator. Enables statements such as list1 = list2;
 *   where list1 and list2 are both variables of ImgList type.
 * POST: the contents of this list will be a physically separate copy of rhs
 */
ImgList & ImgList::operator = (const ImgList & rhs) {
  // Re-build any existing structure using rhs as a template

  if (this != & rhs) { // if this list and rhs are different lists in memory
    // release all existing heap memory of this list
    Clear();

    // and then rebuild this list using rhs as a template
    Copy(rhs);
  }

  return *this;
}

/*
 * Destructor.
 * Releases any heap memory associated with this list.
 */
ImgList::~ImgList() {
  Clear();
}

/************
 * ACCESSORS *
 ************/

/*
 * Returns the horizontal dimension of this list (counted in nodes)
 * Note that every row will contain the same number of nodes, whether or not
 *   the list has been carved.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionX() const {
  return this -> GetDimensionRow(this -> northwest);
}

/*
 * Returns the vertical dimension of the list (counted in nodes)
 * It is useful to know/assume that the grid will never have nodes removed
 *   from the first or last columns. The returned value will thus correspond
 *   to the height of the PNG image from which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   y dimension.
 */
unsigned int ImgList::GetDimensionY() const {
  ImgNode * node = this -> northwest;

  if (!node) {
    return 0;
  }
  unsigned int counter = 1;

  while (node -> south) {

    counter++;
    node = node -> south;

  }

  return counter;
}

/*
 * Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
 * The returned value will thus correspond to the width of the PNG image from
 *   which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionFullX() const {
  ImgNode * node = this -> northwest;

  if (!node) {
    return 0;
  }
  unsigned int counter = 1;

  while (node -> east) {

    counter += 1 + node -> skipright;
    node = node -> east;

  }

  return counter;
}

/*
 * Returns a pointer to the node which best satisfies the specified selection criteria.
 * The first and last nodes in the row cannot be returned.
 * PRE: rowstart points to a row with at least 3 physical nodes
 * PRE: selectionmode is an integer in the range [0,1]
 * PARAM: rowstart - pointer to the first node in a row
 * PARAM: selectionmode - criterion used for choosing the node to return
 *          0: minimum luminance across row, not including extreme left or right nodes
 *          1: node with minimum total of "hue difference" with its left neighbour and with its right neighbour.
 *        In the (likely) case of multiple candidates that best match the criterion,
 *        the left-most node satisfying the criterion (excluding the row's starting node)
 *        will be returned.
 * A note about "hue difference": For PA1, consider the hue value to be a double in the range [0, 360).
 * That is, a hue value of exactly 360 should be converted to 0.
 * The hue difference is the absolute difference between two hues,
 * but be careful about differences spanning the 360 value.
 * e.g. Two pixels with hues 90 and 110 differ by 20, but
 *      two pixels with hues 5 and 355 differ by 10.
 */
ImgNode * ImgList::SelectNode(ImgNode * rowstart, int selectionmode) {

  ImgNode * node = rowstart -> east;
  ImgNode * resultPntr = nullptr;
  double minTarget = __DBL_MAX__;
  while (node -> east) {
    switch (selectionmode % 2) {
    case 0: {
      if (node -> colour.l < minTarget) {
        minTarget = node -> colour.l;
        resultPntr = node;
      }

      break;
    }
    case 1: {
      double hueDiffEast = HueDiff(node -> colour.h, node -> east -> colour.h);
      double hueDiffWest = HueDiff(node -> colour.h, node -> west -> colour.h);
      double totalHueDiff = hueDiffEast + hueDiffWest;
      if (totalHueDiff < minTarget) {
        minTarget = totalHueDiff;
        resultPntr = node;
      }
      break;
    }
    default: {
      // unreachable
      return nullptr;
    }
    }
    node = node -> east;
  }
  return resultPntr;
}

/*
 * Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
 * PRE: fillmode is an integer in the range of [0,2]
 * PARAM: fillgaps - whether or not to fill gaps caused by carving
 *          false: render one pixel per node, ignores fillmode
 *          true: render the full width of the original image,
 *                filling in missing nodes using fillmode
 * PARAM: fillmode - specifies how to fill gaps
 *          0: solid, uses the same colour as the node at the left of the gap
 *          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
 *             Note that "average" for hue will use the closer of the angular distances,
 *             e.g. average of 10 and 350 will be 0, instead of 180.
 *             Average of diametric hue values will use the smaller of the two averages
 *             e.g. average of 30 and 210 will be 120, and not 300
 *                  average of 170 and 350 will be 80, and not 260
 *          2: *** OPTIONAL - FOR BONUS ***
 *             linear gradient between the colour (all channels) of the nodes at the left and right of the gap
 *             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
 *             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
 *             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
 *             Like fillmode 1, use the smaller difference interval for hue,
 *             and the smaller-valued average for diametric hues
 */
PNG ImgList::Render(bool fillgaps, int fillmode) const {

  unsigned int width = this -> GetDimensionX();
  unsigned int widthFull = this -> GetDimensionFullX();
  unsigned int height = this -> GetDimensionY();

  int x = 0;
  int y = 0;

  ImgNode * node = this -> northwest;
  PNG outpng;

  if (fillgaps) { //fillgaps == "True"
    outpng = PNG(widthFull, height);

    while (node) {

      for (int i = 0; i <= node -> skipright; i++) {

        HSLAPixel * pixel = (outpng.getPixel(x + i, y));
        HSLAPixel * left = & (node -> colour);
        HSLAPixel * right = nullptr;

        switch (fillmode) {
        case 0: // solid, same colour as left node
        {
          pixel -> h = left -> h;
          pixel -> s = left -> s;
          pixel -> l = left -> l;
          pixel -> a = left -> a;
          break;
        }
        case 1: // solid, average of left and right neighbours
        {
          if (i == 0) {
            // no interpolation for left node
            pixel -> h = left -> h;
            pixel -> s = left -> s;
            pixel -> l = left -> l;
            pixel -> a = left -> a;
          } else {
            // interpolation for carved nodes
            right = & (node -> east -> colour);
            double hueDiff = HueDiff(left -> h, right -> h) / 2.0;
            pixel -> h = fmin(mod360(left -> h + hueDiff), mod360(right -> h + hueDiff));
            pixel -> s = (left -> s + right -> s) / 2.0;
            pixel -> l = (left -> l + right -> l) / 2.0;
            pixel -> a = (left -> a + right -> a) / 2.0;
          }
          break;
        }
        case 2: // linear gradient
        {
          if (i == 0) {
            // no interpolation
            pixel -> h = left -> h;
            pixel -> s = left -> s;
            pixel -> l = left -> l;
            pixel -> a = left -> a;
          } else {
            // interpolation for carved nodes
            right = & (node -> east -> colour);
            double hueDiff = HueDiff(left -> h, right -> h);
            double hueStep = hueDiff / (node -> skipright + 1);
            if (hueDiff < 180.0) {
              if (fabs(mod360(left -> h + hueDiff) - right -> h) < 0.001) {
                pixel -> h = mod360(left -> h + hueStep * i);
              } else {
                pixel -> h = mod360(left -> h - hueStep * i);
              }
            } else {
              if (left -> h >= right -> h) {
                pixel -> h = mod360(left -> h - hueStep * i);
              } else {
                pixel -> h = mod360(left -> h + hueStep * i);
              }
            }
            double lumStep = fabs(left -> l - right -> l) / (node -> skipright + 1);
            if (left -> l > right -> l) {
              pixel -> l = left -> l - lumStep * i;
            } else {
              pixel -> l = left -> l + lumStep * i;
            }
            double satStep = fabs(left -> s - right -> s) / (node -> skipright + 1);
            if (left -> s > right -> s) {
              pixel -> s = left -> s - satStep * i;
            } else {
              pixel -> s = left -> s + satStep * i;
            }
            double alphaStep = fabs(left -> a - right -> a) / (node -> skipright + 1);
            if (left -> a > right -> a) {
              pixel -> a = left -> a - alphaStep * i;
            } else {
              pixel -> a = left -> a + alphaStep * i;
            }
          }
          break;
        }
        default:
          // unreachable
          break;
        }
      }
      if (y % 2 == 0) {
        if (node -> east) {
          x += 1 + node -> skipright;
          node = node -> east;
        } else {
          node = node -> south;
          y++;
        }
      } else {
        if (node -> west) {
          x -= 1 + node -> skipleft;
          node = node -> west;
        } else {
          node = node -> south;
          y++;
        }
      }
    }
  } else { //fillgaps == "False"
    outpng = PNG(width, height);
    while (node) {
      HSLAPixel * currPixel = (outpng.getPixel(x, y));
      currPixel -> h = node -> colour.h;
      currPixel -> s = node -> colour.s;
      currPixel -> l = node -> colour.l;
      currPixel -> a = node -> colour.a;
      if (y % 2 == 0) {
        if (node -> east) {
          node = node -> east;
          x++;
        } else {
          node = node -> south;
          y++;
        }
      } else {
        if (node -> west) {
          node = node -> west;
          x--;
        } else {
          node = node -> south;
          y++;
        }
      }
    }
  }
  return outpng;
}

/************
 * MODIFIERS *
 ************/

/*
 * Removes exactly one node from each row in this list, according to specified criteria.
 * The first and last nodes in any row cannot be carved.
 * PRE: this list has at least 3 nodes in each row
 * PRE: selectionmode is an integer in the range [0,1]
 * PARAM: selectionmode - see the documentation for the SelectNode function.
 * POST: this list has had one node removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 */
void ImgList::Carve(int selectionmode) {
  ImgNode * rowHead = this -> northwest;

  while (rowHead) {
    ImgNode * nodeToRemove = this -> SelectNode(rowHead, selectionmode);
    if (nodeToRemove -> north) {
      if (nodeToRemove -> south) {
        nodeToRemove -> north -> south = nodeToRemove -> south;
      } else {
        nodeToRemove -> north -> south = nullptr;
      }
      nodeToRemove -> north -> skipdown += 1 + nodeToRemove -> skipdown;

    }
    if (nodeToRemove -> south) {
      if (nodeToRemove -> north) {
        nodeToRemove -> south -> north = nodeToRemove -> north;
      } else {
        nodeToRemove -> south -> north = nullptr;
      }
      nodeToRemove -> south -> skipup += 1 + nodeToRemove -> skipup;
    }
    nodeToRemove -> west -> east = nodeToRemove -> east;
    nodeToRemove -> west -> skipright += 1 + nodeToRemove -> skipright;
    nodeToRemove -> east -> west = nodeToRemove -> west;
    nodeToRemove -> east -> skipleft += 1 + nodeToRemove -> skipleft;
    delete nodeToRemove;
    rowHead = rowHead -> south;
  }
}

// note that a node on the boundary will never be selected for removal
/*
 * Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
 * based on specific selection criteria.
 * Note that this should remove one node from every row, repeated "rounds" times,
 * and NOT remove "rounds" nodes from one row before processing the next row.
 * PRE: selectionmode is an integer in the range [0,1]
 * PARAM: rounds - number of nodes to remove from each row
 *        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
 *        i.e. Ensure that the final list has at least two nodes in each row.
 * POST: this list has had "rounds" nodes removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 */
void ImgList::Carve(unsigned int rounds, int selectionmode) {
  for (int i = 0; i < rounds; i++) {
    if (this -> GetDimensionRow(this->northwest) >= 3) {
      this -> Carve(selectionmode);
    }
    else {
      return;
    }
  }
}

/*
 * Helper function deallocates all heap memory associated with this list,
 * puts this list into an "empty" state. Don't forget to set your member attributes!
 * POST: this list has no currently allocated nor leaking heap memory,
 *       member attributes have values consistent with an empty list.
 */
void ImgList::Clear() {
  int numTurns = 0;
  ImgNode * node = nullptr;
  ImgNode * nextNode = this -> northwest;

  while (nextNode) {
    node = nextNode;
    nextNode = nullptr;
    switch (numTurns % 2) {

    case 0:
      if (node -> east) {
        nextNode = node -> east;
      } else {
        nextNode = node -> south;
        numTurns++;
      }
      delete node;
      break;

    case 1:
      if (node -> west) {
        nextNode = node -> west;
      } else {
        nextNode = node -> south;
        numTurns++;
      }
      delete node;
      break;

    default:
      // unreachable
      break;

    }
  }
  this -> northwest = nullptr;
  this -> southeast = nullptr;
}

/* ************************
 *  * OPTIONAL - FOR BONUS *
 ** ************************
 * Helper function copies the contents of otherlist and sets this list's attributes appropriately
 * PRE: this list is empty
 * PARAM: otherlist - list whose contents will be copied
 * POST: this list has contents copied from by physically separate from otherlist
 */
void ImgList::Copy(const ImgList & otherlist) {

  ImgNode * rowHead = otherlist.northwest;
  const int imgWidth = otherlist.GetDimensionX();
  vector < ImgNode * > * lastRow = new vector < ImgNode * > (imgWidth);
  vector < ImgNode * > * currentRow = new vector < ImgNode * > (imgWidth);
  int y = 0;
  while (rowHead) {
    ImgNode * lastNode = nullptr;
    ImgNode * node = rowHead;
    rowHead = rowHead -> south;
    int x = 0;
    while (node) {
      ImgNode * newNode = new ImgNode( * node);
      if (x != 0) {
        // link left
        newNode -> west = lastNode;
        lastNode -> east = newNode;
      } else if (y == 0) {
        this -> northwest = newNode;
      }

      if (y != 0) {
        // link up
        newNode -> north = lastRow -> at(x);
        lastRow -> at(x) -> south = newNode;
      }
      currentRow -> at(x) = newNode;
      lastNode = newNode;
      node = node -> east;
      x++;
    }
    vector < ImgNode * > * tempPntr = lastRow;
    lastRow = currentRow;
    currentRow = tempPntr;
    this -> southeast = lastNode;
    y++;
  }

  delete lastRow;
  delete currentRow;
}

/*************************************************************************************************
 * IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
 *************************************************************************************************/

unsigned int ImgList::GetDimensionRow(ImgNode * rowHead) const {
  ImgNode * node = rowHead;

  if (!node) {
    return 0;
  }
  unsigned int counter = 1;

  while (node -> east) {

    counter++;
    node = node -> east;

  }

  return counter;
}