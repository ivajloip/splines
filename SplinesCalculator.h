#ifndef __splines_calculator_h__

#define __splines_calculator_h__

#include <utility>

typedef std::pair<int, float> PointsType;

class SplinesCalculator {
  private:
    int _pointsCount;

    PointsType* _points;

  public: 
    SplinesCalculator(PointsType* points, int pointsCount); 

    ~SplinesCalculator();

    PointsType* getResultPoints();

    int getResultPointsCount();
};

#endif
