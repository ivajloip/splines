#ifndef __splines_calculator_h__

#define __splines_calculator_h__

#include <utility>
#include <boost/rational.hpp>

typedef boost::rational<int> rational;

typedef std::pair<int, rational> PointsType;

class SplinesCalculator {
  private:
    int _pointsCount;

    PointsType* _points;

    rational* derivativeParameters;

    rational* mainDiag, *lowerDiag, *upperDiag, *matrixRightSides;
 
    void tridiagMatrixAlgorithm(rational* lowerDiag,
        rational* mainDiag,
        rational* upperDiag,
        rational* matrixRightSides);

    rational dividedDifference(PointsType* points,
        int first_point,
        int second_point);

    rational spline(int variable,
        int intervalStart);

  public: 
    SplinesCalculator(PointsType* points, int pointsCount); 

    ~SplinesCalculator();

    PointsType* getResultPoints();

    int getResultPointsCount();
};

#endif
