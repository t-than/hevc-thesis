/*
 * SearchPattern.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: Thanos Tsakiris
 */

#include "SearchPattern.h"

/* Class SearchPattern
 * Abstract class
 * Implements a combination of an Iterator & Strategy
 * Software Pattern to iterate over the motion vector
 * search points of an inter-picture search pattern.
 */

// Constructons / Destructors

SearchPattern::SearchPattern()
{
  currentX = 0;
  currentY = 0;
  currIdx = 0;
  numOfPoints = 0;
  setWindow( 0, 0, 0, 0 );
}

SearchPattern::~SearchPattern()     // std::vector-s get deleted when they get out of scope
{
}

// SearchPattern getters and setters

int SearchPattern::getCurrentX() const
{
  return currentX;
}

int SearchPattern::getCurrentY() const
{
  return currentY;
}

void SearchPattern::setSearchPoint( unsigned int pos, int x, int y )
{
  searchPointsX[pos] = x;
  searchPointsY[pos] = y;
}

unsigned int SearchPattern::getNumOfPoints() const
{
  return numOfPoints;
}

void SearchPattern::setNumOfPoints( unsigned int num )
{
  this->numOfPoints = num;
}

unsigned int SearchPattern::getCurrIdx() const
{
  return currIdx;
}

void SearchPattern::setCurrIdx( unsigned int idx )
{
  this->currIdx = idx;
}

void SearchPattern::setWindow( int t, int r, int b, int l )
{
  this->top = t;
  this->right = r;
  this->bottom = b;
  this->left = l;
}

int SearchPattern::getBottom() const
{
  return bottom;
}

int SearchPattern::getLeft() const
{
  return left;
}

int SearchPattern::getRight() const
{
  return right;
}

int SearchPattern::getTop() const
{
  return top;
}

// SearchPattern Methods

void SearchPattern::next()
{
  currIdx++;
}

void SearchPattern::incNumOfPoints()
{
  numOfPoints++;
}

void SearchPattern::pushSearchPoint( int x, int y )
{
  incNumOfPoints();
  searchPointsX.push_back( x );
  searchPointsY.push_back( y );
}

void SearchPattern::resize( unsigned int size )
{
  searchPointsX.resize( size );
  searchPointsY.resize( size );
}

void SearchPattern::clear()
{
  searchPointsX.clear();
  searchPointsY.clear();
}
/* Class RoodPattern
 * Inherits from abstract class SearchPattern
 * Implements the center and rood search pattern:
 *                          (1)
 *                     (2) (*) (3)
 *                          (4)
 */

// RoodPattern constructors/destructors

RoodPattern::RoodPattern( int x, int y )
{
  setCurrIdx( 0 );
  setNumOfPoints( 0 );
  setCenter( x, y );
}

RoodPattern::~RoodPattern()
{
}

// RoodPattern getters and setters

void RoodPattern::setCenter( int x, int y )
{
  this->centerX = x;
  this->centerY = y;
}

int RoodPattern::getCenterX() const
{
  return centerX;
}

int RoodPattern::getCenterY() const
{
  return centerY;
}

// RoodPattern methods
void RoodPattern::producePoints()
{
  clear();

  if ( (getCenterY() - 1) >= getTop() )
    pushSearchPoint( 0, -1 );
  if ( (getCenterX() - 1) >= getLeft() )
    pushSearchPoint( -1, 0 );
  if ( (getCenterX() + 1) <= getRight() )
    pushSearchPoint( 1, 0 );
  if ( (getCenterY() + 1) <= getBottom() )
    pushSearchPoint( 0, 1 );
}

/* Class RasterPattern
 * Inherits from abstract class SearchPattern
 * Implements the raster search on the set window
 * "stride" points apart.
 */

// RasterPattern constructors / destructors

RasterPattern::RasterPattern( unsigned int str, int t, int r, int b, int l )
{
  setCurrIdx( 0 );
  this->stride = str;
  setWindow( t, r, b, l );
}

RasterPattern::~RasterPattern()
{
}

// RasterPattern getters and setters

unsigned int RasterPattern::getStride() const
{
  return stride;
}

void RasterPattern::setStride( unsigned int str )
{
  this->stride = str;
}

// RasterPattern methods

void RasterPattern::producePoints()
{
  unsigned int numOfHorPoints = (getRight() - getLeft()) / stride + 1 ;
  unsigned int numOfVerPoints = (getTop() - getBottom()) / stride + 1 ;
  setNumOfPoints( numOfHorPoints * numOfVerPoints );
  resize( numOfHorPoints * numOfVerPoints );

  unsigned int i, j;
  int l = getLeft();
  int t = getTop();
  unsigned int str = getStride();
  for ( i = 0; i < numOfVerPoints; i++ )
    for ( j = 0; j < numOfHorPoints; j++ )
      setSearchPoint( i*numOfVerPoints + j, l + j*str, t + i*str );
}

/**
 * Class HexagonPattern
 * Inherits from abstract class SearchPattern
 * Implements the rotating hexagon search pattern
 * If stride >= 2:
 *      pattern if log2(stride) is odd                  pattern if log2(stride) is even
 *              *           *                                               *
 *
 *                                                              *                       *
 *
 *        *                       *
 *
 *                                                              *                       *
 *
 *              *            *                                               *
 */

// Hexagon Pattern constructors / destructors

HexagonPattern::HexagonPattern( unsigned int str, int x, int y )
{
  setCurrIdx( 0 );
  setCenter( x, y );
  setStride( str );
}

HexagonPattern::HexagonPattern( int x, int y )
{
  setCurrIdx( 0 );
  setCenter( x, y );
  resize( 1 );
  setNumOfPoints( 0 );
}

HexagonPattern::~HexagonPattern()
{
}

// HexagonPattern getters and setters

void HexagonPattern::setCenter( int x, int y )
{
  this->centerX = x;
  this->centerY = y;
}

int HexagonPattern::getCenterX() const
{
  return centerX;
}

int HexagonPattern::getCenterY() const
{
  return centerY;
}

unsigned int HexagonPattern::getStride() const
{
  return stride;
}

void HexagonPattern::setStride( unsigned int str )
{
  this->stride = str;
}

// HexagonPatters methods

void HexagonPattern::producePoints()
{
  // Str, in this context, means the log2
  // of the distance of a corner
  unsigned int str = getStride();
  if ( str == 0 ) {
    setNumOfPoints( 1 );
    resize( 1 );
    setSearchPoint( 0, 0, 0 );
  }
  else if ( str == 1 ) {
    setNumOfPoints( 4 );
    resize( 4 );

    setSearchPoint( 0, 0, 1 );
    setSearchPoint( 1, -1, 0 );
    setSearchPoint( 2, 1, 0 );
    setSearchPoint( 3, 0, -1 );
  }
  else {
    setNumOfPoints( 6 );
    resize( 6 );
    unsigned int cornerDistance = 1 << (str - 1);
    unsigned int halfCornerDistance = cornerDistance >> 1;
    if ( (str & 1) == 1 ) {
      // In Type-1 hexagon search pattern, odd log2 of
      // corner distance delegates to a horizontal hexagon
      setSearchPoint( 0, -halfCornerDistance, cornerDistance );
      setSearchPoint( 1, halfCornerDistance, cornerDistance);
      setSearchPoint( 2, -cornerDistance, 0 );
      setSearchPoint( 3, cornerDistance, 0 );
      setSearchPoint( 4, -halfCornerDistance, -cornerDistance );
      setSearchPoint( 5, halfCornerDistance, -cornerDistance );
    }
    else {
      // In Type-1 hexagon search pattern, even log2 of
      // corner distance delegates to a vertical hexagon
      setSearchPoint( 0, 0, cornerDistance );
      setSearchPoint( 1, -cornerDistance, halfCornerDistance);
      setSearchPoint( 2, cornerDistance, halfCornerDistance );
      setSearchPoint( 3, -cornerDistance, -halfCornerDistance );
      setSearchPoint( 4, cornerDistance, -halfCornerDistance );
      setSearchPoint( 5, 0, -cornerDistance );
    }
  }
}
