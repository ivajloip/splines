#include <SplinesCalculator.h>
#include <iostream>

SplinesCalculator::SplinesCalculator(PointsType* points, int pointsCount) {
  this->upperDiag = new double[pointsCount];
  this->mainDiag = new double[pointsCount + 1];
  this->lowerDiag = new double[pointsCount];
  this->matrixRightSides = new double[pointsCount + 1];
  this->derivativeParameters = new double[pointsCount + 1];
  this->_pointsCount = pointsCount + 1;
  
  this->_points = new PointsType[pointsCount + 1];
  this->_points[0].first = 0;
  this->_points[0].second = 0;
  
  for (int k = 1; k < this->_pointsCount; k++) {
    this->_points[k].first = points[k - 1].first;
    this->_points[k].second = points[k - 1].second;
  }
  
  for (int i = 0; i < this->_pointsCount; i++)
    this->derivativeParameters[i] = 0;

  double divDiffFirst = this->dividedDifference(this->_points, 1, 0); 
  this->upperDiag[0] = 1;
  this->mainDiag[0] = 2;
  this->matrixRightSides[0] = 3 * divDiffFirst;

  for (int i = 0; i < this->_pointsCount - 2; i++) {
    double delta, deltaPlus, divDiff, divDiffPlus;
    divDiff = this->dividedDifference(this->_points, i + 1, i);
    divDiffPlus = this->dividedDifference(this->_points, i + 2, i + 1);
    delta = this->_points[i + 1].first - this->_points[i].first;
    deltaPlus = this->_points[i + 2].first - this->_points[i + 1].first;
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

  for (int k = 1; k <= matrixLen; k++) {
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

double SplinesCalculator::spline(int variable, int intervalStart) {
  double divDiff = this->dividedDifference(this->_points, intervalStart + 1, intervalStart);
  int intervalLength = this->_points[intervalStart].first - this->_points[intervalStart + 1].first;
  double constantA = (divDiff - this->derivativeParameters[intervalStart]) / intervalLength;
  double constantB = (this->derivativeParameters[intervalStart] - 2 * divDiff +
      this->derivativeParameters[intervalStart + 1]) / (intervalLength * intervalLength);
  double distanceToStart = variable - this->_points[intervalStart].first;
  double splineFormula = this->_points[intervalStart].second +
      this->derivativeParameters[intervalStart] * distanceToStart +
      constantA * distanceToStart * distanceToStart +
      constantB * distanceToStart * distanceToStart * (variable - this->_points[intervalStart + 1].first);
  
  return splineFormula;
}

PointsType* SplinesCalculator::getResultPoints() {
  PointsType* splineValues = new PointsType[this->getResultPointsCount()];
  int number = 0;
  while (number <= this->getResultPointsCount() - 1) {
    for (int i = 0; i < this->_pointsCount - 1; i++)
      if ((number >= this->_points[i].first) && (number <= this->_points[i + 1].first) || number == this->getResultPointsCount() - 1) {
        this->tridiagMatrixAlgorithm(this->lowerDiag, this->mainDiag, this->upperDiag, this->matrixRightSides);
        splineValues[number].first = number;
        splineValues[number].second = this->spline(number, i);
      }
    number++;
  }
    

  return splineValues;
}

int SplinesCalculator::getResultPointsCount() {
  return this->_points[this->_pointsCount - 1].first + 1;
}
