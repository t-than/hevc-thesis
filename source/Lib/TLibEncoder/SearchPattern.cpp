/*
 * SearchPattern.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: Thanos Tsakiris
 */

#include "SearchPattern.h"
#include <assert.h>
#include <iostream>

/* Class SearchPattern
 * Abstract class
 * Implements a combination of an Iterator & Strategy
 * Software Pattern to iterate over the motion vector
 * search points of an inter-picture search pattern.
 */

// Constructons / Destructors

SearchPattern::SearchPattern()
{
  setCurrIdx( 0 );
  setNumOfPoints( 0 );
  setWindow( 0, 0, 0, 0 );
}

SearchPattern::~SearchPattern()     // std::vector-s get deleted when they get out of scope
{
}

// SearchPattern getters and setters

int SearchPattern::getCurrentX() const
{
  return searchPointsX[currIdx];
}

int SearchPattern::getCurrentY() const
{
  return searchPointsY[currIdx];
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
  setCurrIdx( 0 );
}

/* Class RasterPattern
 * Inherits from abstract class SearchPattern
 * Implements the raster search on the set window
 * "stride" points apart.
 */

// RasterPattern constructors / destructors

RasterPattern::RasterPattern( unsigned int str, int t, int r, int b, int l )
{
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
  setCenter( x, y );
  setStride( str );
}

HexagonPattern::HexagonPattern( int x, int y )
{
  setCenter( x, y );
  setStride( 2 );
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
  clear();
  setNumOfPoints( 0 );

  unsigned int str = getStride();
  const int   topBound   = getTop();
  const int   rightBound  = getRight();
  const int   bottomBound    = getBottom();
  const int   leftBound = getLeft();

  assert( str >= 2 );
  //setNumOfPoints( 6 );
  //resize( 6 );
  unsigned int cornerDistance = 1 << (str - 1);
  unsigned int halfCornerDistance = cornerDistance >> 1;
  if ( (str & 1) == 1 ) {
    // In Type-1 hexagon search pattern, odd log2 of
    // corner distance delegates to a horizontal hexagon
    if ( (getCenterY() - cornerDistance) >= topBound ) {
      if ( (getCenterX() - halfCornerDistance) >= leftBound ) {
        pushSearchPoint( getCenterX() - halfCornerDistance, getCenterY() - cornerDistance );
      }
      if ( (getCenterX() + halfCornerDistance) <= rightBound ) {
        pushSearchPoint( getCenterX() + halfCornerDistance, getCenterY() - cornerDistance );
      }
    }
    if ( (getCenterX() - cornerDistance) >= leftBound ) {
      pushSearchPoint( getCenterX() - cornerDistance, getCenterY() );
    }
    if ( (getCenterX() + cornerDistance) <= rightBound ) {
      pushSearchPoint( getCenterX() + cornerDistance, getCenterY() );
    }
    if ( (getCenterY() + cornerDistance) <= bottomBound ) {
      if ( (getCenterX() - halfCornerDistance) >= leftBound ) {
        pushSearchPoint( getCenterX() - halfCornerDistance, getCenterY() + cornerDistance );
      }
      if ( (getCenterX() + halfCornerDistance) <= rightBound ) {
        pushSearchPoint( getCenterX() + halfCornerDistance, getCenterY() + cornerDistance );
      }
    }
  }
  else {
    // In Type-1 hexagon search pattern, even log2 of
    // corner distance delegates to a vertical hexagon
    if ( (getCenterY() - cornerDistance) >= topBound ) {
      pushSearchPoint( getCenterX(), getCenterY() - cornerDistance );
      if ( (getCenterX() - cornerDistance) >= leftBound ) {
        pushSearchPoint( getCenterX() - cornerDistance, getCenterY() - halfCornerDistance );
      }
      if ( (getCenterX() + cornerDistance) <= rightBound ) {
        pushSearchPoint( getCenterX() + cornerDistance, getCenterY() - halfCornerDistance );
      }
      else if ( (getCenterY() - halfCornerDistance) >= topBound ) {
        if ( (getCenterX() - cornerDistance) >= leftBound ) {
          pushSearchPoint( getCenterX() - cornerDistance, getCenterY() - halfCornerDistance );
        }
        if ( (getCenterX() + cornerDistance) <= rightBound ) {
          pushSearchPoint( getCenterX() + cornerDistance, getCenterY() - halfCornerDistance );
        }
      }
    }
    if ( (getCenterY() + halfCornerDistance) <= bottomBound ) {
      if( (getCenterX() - cornerDistance) >= leftBound ) {
        pushSearchPoint( getCenterX() - cornerDistance, getCenterY() + halfCornerDistance );
      }
      if ( (getCenterX() + cornerDistance) <= rightBound ) {
        pushSearchPoint( getCenterX() + cornerDistance, getCenterY() + halfCornerDistance );
      }
      if ( (getCenterY() + cornerDistance) <= bottomBound ) {
        pushSearchPoint( getCenterX(), getCenterY() + cornerDistance );
      }
    }
  }

  setCurrIdx( 0 );
/*
  // 8 point search,                   //   1 2 3
  // search around the start point     //   4 0 5
  // with the required  distance       //   6 7 8
  const Int iTop        = iStartY - iDist;
  const Int iBottom     = iStartY + iDist;
  const Int iLeft       = iStartX - iDist;
  const Int iRight      = iStartX + iDist;

  if ( iDist == 1 )
  {
    if ( iTop >= iSrchRngVerTop ) // check top
    {
      if (bCheckCornersAtDist1)
      {
        if ( iLeft >= iSrchRngHorLeft) // check top-left
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iLeft, iTop, 1, iDist );
        }
        xTZSearchHelp( pcPatternKey, rcStruct, iStartX, iTop, 2, iDist );
        if ( iRight <= iSrchRngHorRight ) // check middle right
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iRight, iTop, 3, iDist );
        }
      }
      else
      {
        xTZSearchHelp( pcPatternKey, rcStruct, iStartX, iTop, 2, iDist );
      }
    }
    if ( iLeft >= iSrchRngHorLeft ) // check middle left
    {
      xTZSearchHelp( pcPatternKey, rcStruct, iLeft, iStartY, 4, iDist );
    }
    if ( iRight <= iSrchRngHorRight ) // check middle right
    {
      xTZSearchHelp( pcPatternKey, rcStruct, iRight, iStartY, 5, iDist );
    }
    if ( iBottom <= iSrchRngVerBottom ) // check bottom
    {
      if (bCheckCornersAtDist1)
      {
        if ( iLeft >= iSrchRngHorLeft) // check top-left
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iLeft, iBottom, 6, iDist );
        }
        xTZSearchHelp( pcPatternKey, rcStruct, iStartX, iBottom, 7, iDist );
        if ( iRight <= iSrchRngHorRight ) // check middle right
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iRight, iBottom, 8, iDist );
        }
      }
      else
      {
        xTZSearchHelp( pcPatternKey, rcStruct, iStartX, iBottom, 7, iDist );
      }
    }
  }
  else
  {
    if ( iDist <= 8 )
    {
      const Int iTop_2      = iStartY - (iDist>>1);
      const Int iBottom_2   = iStartY + (iDist>>1);
      const Int iLeft_2     = iStartX - (iDist>>1);
      const Int iRight_2    = iStartX + (iDist>>1);

      if (  iTop >= iSrchRngVerTop && iLeft >= iSrchRngHorLeft &&
          iRight <= iSrchRngHorRight && iBottom <= iSrchRngVerBottom ) // check border
      {
        xTZSearchHelp( pcPatternKey, rcStruct, iStartX,  iTop,      2, iDist    );
        xTZSearchHelp( pcPatternKey, rcStruct, iLeft_2,  iTop_2,    1, iDist>>1 );
        xTZSearchHelp( pcPatternKey, rcStruct, iRight_2, iTop_2,    3, iDist>>1 );
        xTZSearchHelp( pcPatternKey, rcStruct, iLeft,    iStartY,   4, iDist    );
        xTZSearchHelp( pcPatternKey, rcStruct, iRight,   iStartY,   5, iDist    );
        xTZSearchHelp( pcPatternKey, rcStruct, iLeft_2,  iBottom_2, 6, iDist>>1 );
        xTZSearchHelp( pcPatternKey, rcStruct, iRight_2, iBottom_2, 8, iDist>>1 );
        xTZSearchHelp( pcPatternKey, rcStruct, iStartX,  iBottom,   7, iDist    );
      }
      else // check border
      {
        if ( iTop >= iSrchRngVerTop ) // check top
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iStartX, iTop, 2, iDist );
        }
        if ( iTop_2 >= iSrchRngVerTop ) // check half top
        {
          if ( iLeft_2 >= iSrchRngHorLeft ) // check half left
          {
            xTZSearchHelp( pcPatternKey, rcStruct, iLeft_2, iTop_2, 1, (iDist>>1) );
          }
          if ( iRight_2 <= iSrchRngHorRight ) // check half right
          {
            xTZSearchHelp( pcPatternKey, rcStruct, iRight_2, iTop_2, 3, (iDist>>1) );
          }
        } // check half top
        if ( iLeft >= iSrchRngHorLeft ) // check left
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iLeft, iStartY, 4, iDist );
        }
        if ( iRight <= iSrchRngHorRight ) // check right
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iRight, iStartY, 5, iDist );
        }
        if ( iBottom_2 <= iSrchRngVerBottom ) // check half bottom
        {
          if ( iLeft_2 >= iSrchRngHorLeft ) // check half left
          {
            xTZSearchHelp( pcPatternKey, rcStruct, iLeft_2, iBottom_2, 6, (iDist>>1) );
          }
          if ( iRight_2 <= iSrchRngHorRight ) // check half right
          {
            xTZSearchHelp( pcPatternKey, rcStruct, iRight_2, iBottom_2, 8, (iDist>>1) );
          }
        } // check half bottom
        if ( iBottom <= iSrchRngVerBottom ) // check bottom
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iStartX, iBottom, 7, iDist );
        }
      } // check border
    }
    else // iDist > 8
    {
      if ( iTop >= iSrchRngVerTop && iLeft >= iSrchRngHorLeft &&
          iRight <= iSrchRngHorRight && iBottom <= iSrchRngVerBottom ) // check border
      {
        xTZSearchHelp( pcPatternKey, rcStruct, iStartX, iTop,    0, iDist );
        xTZSearchHelp( pcPatternKey, rcStruct, iLeft,   iStartY, 0, iDist );
        xTZSearchHelp( pcPatternKey, rcStruct, iRight,  iStartY, 0, iDist );
        xTZSearchHelp( pcPatternKey, rcStruct, iStartX, iBottom, 0, iDist );
        for ( Int index = 1; index < 4; index++ )
        {
          const Int iPosYT = iTop    + ((iDist>>2) * index);
          const Int iPosYB = iBottom - ((iDist>>2) * index);
          const Int iPosXL = iStartX - ((iDist>>2) * index);
          const Int iPosXR = iStartX + ((iDist>>2) * index);
          xTZSearchHelp( pcPatternKey, rcStruct, iPosXL, iPosYT, 0, iDist );
          xTZSearchHelp( pcPatternKey, rcStruct, iPosXR, iPosYT, 0, iDist );
          xTZSearchHelp( pcPatternKey, rcStruct, iPosXL, iPosYB, 0, iDist );
          xTZSearchHelp( pcPatternKey, rcStruct, iPosXR, iPosYB, 0, iDist );
        }
      }
      else // check border
      {
        if ( iTop >= iSrchRngVerTop ) // check top
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iStartX, iTop, 0, iDist );
        }
        if ( iLeft >= iSrchRngHorLeft ) // check left
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iLeft, iStartY, 0, iDist );
        }
        if ( iRight <= iSrchRngHorRight ) // check right
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iRight, iStartY, 0, iDist );
        }
        if ( iBottom <= iSrchRngVerBottom ) // check bottom
        {
          xTZSearchHelp( pcPatternKey, rcStruct, iStartX, iBottom, 0, iDist );
        }
        for ( Int index = 1; index < 4; index++ )
        {
          const Int iPosYT = iTop    + ((iDist>>2) * index);
          const Int iPosYB = iBottom - ((iDist>>2) * index);
          const Int iPosXL = iStartX - ((iDist>>2) * index);
          const Int iPosXR = iStartX + ((iDist>>2) * index);

          if ( iPosYT >= iSrchRngVerTop ) // check top
          {
            if ( iPosXL >= iSrchRngHorLeft ) // check left
            {
              xTZSearchHelp( pcPatternKey, rcStruct, iPosXL, iPosYT, 0, iDist );
            }
            if ( iPosXR <= iSrchRngHorRight ) // check right
            {
              xTZSearchHelp( pcPatternKey, rcStruct, iPosXR, iPosYT, 0, iDist );
            }
          } // check top
          if ( iPosYB <= iSrchRngVerBottom ) // check bottom
          {
            if ( iPosXL >= iSrchRngHorLeft ) // check left
            {
              xTZSearchHelp( pcPatternKey, rcStruct, iPosXL, iPosYB, 0, iDist );
            }
            if ( iPosXR <= iSrchRngHorRight ) // check right
            {
              xTZSearchHelp( pcPatternKey, rcStruct, iPosXR, iPosYB, 0, iDist );
            }
          } // check bottom
        } // for ...
      } // check border
    } // iDist <= 8
  } // iDist == 1
*/
}
