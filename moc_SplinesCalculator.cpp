#include <SplinesCalculator.h>
#include <iostream>

#define POINST_COUNT 10

SplinesCalculator::SplinesCalculator(PointsType* points, int _pointsCount) {
}

SplinesCalculator::~SplinesCalculator() {
}

int SplinesCalculator::getResultPointsCount() {
  return POINST_COUNT;
}

PointsType* SplinesCalculator::getResultPoints() {
  PointsType* points = new PointsType[POINST_COUNT];

  for(int i = 0; i < POINST_COUNT; i++) {
    points[i].first = i;
    points[i].second = i * i / 0.7f;
    std::cout << points[i].second << std::endl;
  }

  return points;
}


