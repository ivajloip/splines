#include <SplinesCalculator.h>
#include <iostream>

SplinesCalculator::SplinesCalculator(PointsType* points, int pointsCount) {
  this->upperDiag = new double[pointsCount];
  this->mainDiag = new double[pointsCount + 1];
  this->lowerDiag = new double[pointsCount];
  this->matrixRightSides = new double[pointsCount + 1];

  double divDiffFirst = this->dividedDifference(points, 1, 0); 
  this->_pointsCount = pointsCount + 1;
  this->upperDiag[0] = 1;
  this->mainDiag[0] = 2;
  this->matrixRightSides[0] = 3 * divDiffFirst;

  this->_points = new PointsType[pointsCount + 1];
  this->_points[0].first = 0;
  this->_points[0].second = 0;
  for (int k = 1; k < pointsCount + 1; k++) {
    this->_points[k].first = points[k - 1].first;
    this->_points[k].second = points[k - 1].second;
  }

  for (int i = 0; i < this->_pointsCount - 2; i++) {
    double delta, deltaPlus, divDiff, divDiffPlus;
    divDiff = this->dividedDifference(points, i + 1, i);
    divDiffPlus = this->dividedDifference(points, i + 2, i + 1);
    delta = points[i + 1].first - points[i].first;
    deltaPlus = points[i + 2].first - points[i + 1].first;
    this->upperDiag[i + 1] = delta;
    this->mainDiag[i + 1] = 2 * (delta + deltaPlus);
    this->lowerDiag[i] = deltaPlus;
    this->matrixRightSides[i + 1] = 3 * (divDiffPlus * delta + divDiff * deltaPlus);
  }

  double divDiffLast = this->dividedDifference(points, this->_pointsCount - 1, this->_pointsCount - 2); 
  this->mainDiag[this->_pointsCount - 1] = 2;
  this->lowerDiag[this->_pointsCount - 2] = 1;
  this->matrixRightSides[this->_pointsCount - 1] = 3 * divDiffLast;
}

SplinesCalculator::~SplinesCalculator() {
  delete [] this->_points;
  delete [] this->mainDiag;
  delete [] this->upperDiag;
  delete [] this->lowerDiag;
  delete [] this->matrixRightSides;
}

void SplinesCalculator::tridiagMatrixAlgorithm(double* lowerDiag, 
    double* mainDiag, 
    double* upperDiag,
    double* matrixRightSides) {
  int matrixLen = this->_pointsCount - 1;
  double alfa[matrixLen], beta[matrixLen];

  alfa[0] = -(upperDiag[0] / mainDiag[0]);
  beta[0] = (matrixRightSides[0] / mainDiag[0]);

  for (int k = 1; k <= matrixLen; k++){
    double denominator = lowerDiag[k - 1] * alfa[k - 1] + mainDiag[k];
    beta[k] = ((matrixRightSides[k] - (lowerDiag[k - 1] * beta[k - 1])) / denominator);
    if (k == matrixLen)
      continue;
    alfa[k] = -(upperDiag[k] / denominator);
  }

  this->derivativeParameters[matrixLen] = beta[matrixLen];

  for (int k = matrixLen - 1; k >= 0; k--)
    this->derivativeParameters[k] = alfa[k] * this->derivativeParameters[k + 1] + beta[k];
}

double SplinesCalculator::dividedDifference(PointsType* points, int first_point, int second_point) {
  double divDiff = (points[first_point].second - points[second_point].second) /
    (points[first_point].first - points[second_point].first);
  
  return divDiff;
}

double SplinesCalculator::spline(int variable, int intervalStart, int intervalEnd) {
  double divDiff = this->dividedDifference(this->_points, intervalEnd, intervalStart);
  int intervalLength = this->_points[intervalStart].first - this->_points[intervalEnd].first;
  double constantA = (divDiff - this->derivativeParameters[intervalStart]) / intervalLength;
  double constantB = (this->derivativeParameters[intervalStart] - 2 * divDiff +
      this->derivativeParameters[intervalEnd]) / intervalLength;
  double distanceToStart = variable - this->_points[intervalStart].first;
  double splineFormula = this->_points[intervalStart].second +
      this->derivativeParameters[intervalStart] * distanceToStart +
      constantA * distanceToStart * distanceToStart +
      constantB * distanceToStart * distanceToStart * (variable - this->_points[intervalEnd].first);
  
  return splineFormula;
}

PointsType* SplinesCalculator::getResultPoints() {
  PointsType* splineValues = new PointsType[this->_points[this->_pointsCount].first];
  int number = 0;
  while (number <= this->_points[this->_pointsCount - 1].first) {
    for (int i = 0; i < this->_pointsCount - 1; i++)
      if ((number >= this->_points[i].first) && (number <= this->_points[i + 1].first)) {
        this->tridiagMatrixAlgorithm(this->lowerDiag, this->mainDiag, this->upperDiag, this->matrixRightSides);
        splineValues[number].first = number;
        splineValues[number].second = this->spline(number, i, i + 1);
      }
    number++;
  }
    

  return splineValues;
}
