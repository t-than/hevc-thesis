/*
 * SearchPattern.h
 *
 *  Created on: Aug 23, 2016
 *      Author: Thanos Tsakiris
 */

#ifndef SOURCE_LIB_TLIBENCODER_SEARCHPATTERN_H_
#define SOURCE_LIB_TLIBENCODER_SEARCHPATTERN_H_

#include <vector>

class SearchPattern
{
public:
  SearchPattern();
  virtual ~SearchPattern();
  int getCurrentX() const;
  int getCurrentY() const;
  unsigned int getNumOfPoints() const;
  void setNumOfPoints( unsigned int num );
  unsigned int getCurrIdx() const;
  void setCurrIdx( unsigned int idx );
  void incNumOfPoints();
  void setSearchPoint( unsigned int pos, int x, int y );
  void pushSearchPoint( int x, int y );
  void setWindow( int t, int r, int b, int l );
  int getBottom() const;
  int getLeft() const;
  int getRight() const;
  int getTop() const;
  void resize( unsigned int size );
  void clear();
  void next(); // Without bound-checking
  virtual void producePoints() = 0;

private:
  std::vector<int> searchPointsX;
  std::vector<int> searchPointsY;
  unsigned int currIdx;
  unsigned int numOfPoints;
  int top, right, bottom, left;
};

class RoodPattern : public SearchPattern
{
public:
  RoodPattern( int x, int y );
  ~RoodPattern();
  void setCenter( int x, int y );
  void producePoints();
  int getCenterX() const;
  int getCenterY() const;
  void produceVer();

private:
  int centerX, centerY;
};

class RasterPattern : public SearchPattern
{
public:
  RasterPattern( unsigned int str, int t, int r, int b, int l );
  ~RasterPattern();
  void producePoints();
  unsigned int getStride() const;
  void setStride( unsigned int str );
private:
  unsigned int stride;
};

class HexagonPattern : public SearchPattern
{
public:
  HexagonPattern( unsigned int str, int x, int y );
  HexagonPattern( int x, int y );
  ~HexagonPattern();
  void producePoints();
  void setCenter( int x, int y );
  int getCenterX() const;
  int getCenterY() const;
  unsigned int getStride() const;
  void setStride( unsigned int str );

private:
  unsigned int stride; // Stride, in this context, means the log2
                          // of the distance of a corner
  int centerX, centerY;
};

#endif /* SOURCE_LIB_TLIBENCODER_SEARCHPATTERN_H_ */
