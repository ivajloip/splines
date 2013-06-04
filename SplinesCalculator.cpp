#include "SplinesCalculator.h"
#include <iostream>

SplinesCalculator::SplinesCalculator(PointsType* points, int pointsCount) {
  std::cout << "Larido2\n";
  this->upperDiag = new rational[pointsCount];
  this->mainDiag = new rational[pointsCount + 1];
  this->lowerDiag = new rational[pointsCount];
  this->matrixRightSides = new rational[pointsCount + 1];
  this->derivativeParameters = new rational[pointsCount + 1];
  this->_pointsCount = pointsCount + 1;
  
  this->_points = new PointsType[pointsCount + 1];
  this->_points[0].first = 0;
  this->_points[0].second = rational(0);
  
  for (int k = 1; k < this->_pointsCount; k++) {
    this->_points[k].first = points[k - 1].first;
    this->_points[k].second = points[k - 1].second;
  }
  
  for (int i = 0; i < this->_pointsCount; i++)
    this->derivativeParameters[i] = rational(0);

  rational divDiffFirst = this->dividedDifference(this->_points, 1, 0); 
  this->upperDiag[0] = rational(1);
  this->mainDiag[0] = rational(2);
  this->matrixRightSides[0] = 3 * divDiffFirst;

  for (int i = 0; i < this->_pointsCount - 2; i++) {
    rational delta, deltaPlus, divDiff, divDiffPlus;
    divDiff = this->dividedDifference(this->_points, i + 1, i);
    divDiffPlus = this->dividedDifference(this->_points, i + 2, i + 1);
    delta = this->_points[i + 1].first - this->_points[i].first;
    deltaPlus = this->_points[i + 2].first - this->_points[i + 1].first;
    this->upperDiag[i + 1] = delta;
    this->mainDiag[i + 1] = 2 * (delta + deltaPlus);
    this->lowerDiag[i] = deltaPlus;
    this->matrixRightSides[i + 1] = 3 * (divDiffPlus * delta + divDiff * deltaPlus);
  }

  rational divDiffLast = this->dividedDifference(_points, this->_pointsCount - 1, this->_pointsCount - 2); 
  this->mainDiag[this->_pointsCount - 1] = 2;
  this->lowerDiag[this->_pointsCount - 2] = 1;
  this->matrixRightSides[this->_pointsCount - 1] = 3 * divDiffLast;
  std::cout << _points[this->_pointsCount - 1].second << " " << _points[this->_pointsCount - 2].second << std::endl;
  std::cout << divDiffLast << std::endl;
  std::cout << divDiffLast << std::endl;
}

SplinesCalculator::~SplinesCalculator() {
  delete [] this->_points;
  delete [] this->mainDiag;
  delete [] this->upperDiag;
  delete [] this->lowerDiag;
  delete [] this->matrixRightSides;
  delete [] this->derivativeParameters;
}

void SplinesCalculator::tridiagMatrixAlgorithm(rational* lowerDiag, 
    rational* mainDiag, 
    rational* upperDiag,
    rational* matrixRightSides) {
  int matrixLen = this->_pointsCount - 1;
  rational alfa[matrixLen], beta[matrixLen + 1];

  alfa[0] = -(upperDiag[0] / mainDiag[0]);
  beta[0] = (matrixRightSides[0] / mainDiag[0]);

  for (int k = 1; k <= matrixLen; k++) {
    rational denominator = lowerDiag[k - 1] * alfa[k - 1] + mainDiag[k];
    beta[k] = ((matrixRightSides[k] - (lowerDiag[k - 1] * beta[k - 1])) / denominator);
    if (k == matrixLen)
      continue;
    alfa[k] = -(upperDiag[k] / denominator);
  }
  
  this->derivativeParameters[matrixLen] = beta[matrixLen];

  for (int k = matrixLen - 1; k >= 0; k--)
    this->derivativeParameters[k] = alfa[k] * this->derivativeParameters[k + 1] + beta[k];

  std::cout << "Larido1\n";
}

rational SplinesCalculator::dividedDifference(PointsType* points, int first_point, int second_point) {
  rational divDiff = (points[first_point].second - points[second_point].second) /
    (points[first_point].first - points[second_point].first);
  
  return divDiff;
}

rational SplinesCalculator::spline(int variable, int intervalStart) {
  std::cout << "Larodi8\n";
  rational divDiff = this->dividedDifference(this->_points, intervalStart + 1, intervalStart);
  int intervalLength = this->_points[intervalStart + 1].first - this->_points[intervalStart].first;
  rational constantA = (divDiff - this->derivativeParameters[intervalStart]) / intervalLength;
  std::cout << "Larodi8.5\n";
  std::cout << intervalStart << std::endl;
  std::cout << this->derivativeParameters[intervalStart] << std::endl;
  rational constantB = (this->derivativeParameters[intervalStart] - 2 * divDiff +
      this->derivativeParameters[intervalStart + 1]) / (intervalLength * intervalLength);
  std::cout << "Larodi9\n";
  rational distanceToStart = variable - this->_points[intervalStart].first;
  rational distanceToEnd = variable - this->_points[intervalStart + 1].first; 
  rational splineFormula = this->_points[intervalStart].second +
      this->derivativeParameters[intervalStart] * distanceToStart +
      constantA * distanceToStart * distanceToStart +
      constantB * distanceToStart * distanceToStart * distanceToEnd;
  
  std::cout << "Larodi10\n";
  return splineFormula;
}

PointsType* SplinesCalculator::getResultPoints() {
  std::cout << "Larodi4\n";
  PointsType* splineValues = new PointsType[this->getResultPointsCount()];
  int number = 0;

  this->tridiagMatrixAlgorithm(this->lowerDiag, this->mainDiag, this->upperDiag, this->matrixRightSides);


  std::cout << "Larodi5\n";
  while (number <= this->getResultPointsCount() - 1) {
    for (int i = 0; i < this->_pointsCount - 1; i++)
      if ((number >= this->_points[i].first) && (number <= this->_points[i + 1].first)) {
        splineValues[number].first = number;

        std::cout << "Larodi6\n";
        splineValues[number].second = this->spline(number, i);
      }
    number++;
  }
  std::cout << "Larodi7\n";
     
  return splineValues;
}

int SplinesCalculator::getResultPointsCount() {
  return this->_points[this->_pointsCount - 1].first + 1;
}
