#ifndef __splines_calculator_h__

#define __splines_calculator_h__

#include <utility>

typedef std::pair<int, float> PointsType;

class SplinesCalculator {
  private:
    int _pointsCount;

    PointsType* _points;

    double* derivativeParameters;

    double* mainDiag, *lowerDiag, *upperDiag, *matrixRightSides;
 
    void tridiagMatrixAlgorithm(double* lowerDiag,
        double* mainDiag,
        double* upperDiag,
        double* matrixRightSides);

    double dividedDifference(PointsType* points,
        int first_point,
        int second_point);

    double spline(int variable,
        int intervalStart,
        int intervalEnd);

  public: 
    SplinesCalculator(PointsType* points, int pointsCount); 

    ~SplinesCalculator();

    PointsType* getResultPoints();

    int getResultPointsCount();
};

#endif
