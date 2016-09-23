#!/bin/sh

if [ "$SUITE" = "coverage" ]; then
	cmake -DENABLE_TESTING=ON -DENABLE_TESTING_COVERAGE=ON -DBUILD_MYSQLPP=ON .. && make
	make coverage
	coveralls-lcov coverage.info.cleaned
elif [ "$SUITE" = "tests" ]; then
  cmake -DENABLE_TESTING=ON -DENABLE_TESTING_COVERAGE=OFF -DBUILD_MYSQLPP=ON .. && make
  ctest --output-on-failure
else
	cmake .. && make
fi
