#!/bin/bash
set -e
if [ -z "$TRAVIS_OS_NAME" ]; then
    TRAVIS_OS_NAME=linux
fi

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
  if [ ! -f "$TRAVIS_BUILD_DIR/ninja/ninja" ]; then
	  git clone https://github.com/ninja-build/ninja.git && cd ninja
	  git checkout release
	  ./configure.py --bootstrap
	  cd ..
	else
		echo 'Using cached ninja directory.';
	fi
else
    echo "Unknown OS ($TRAVIS_OS_NAME). Stopping build ..."
    exit 1
fi
