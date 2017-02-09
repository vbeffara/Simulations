FROM ubuntu:yakkety
MAINTAINER Vincent Beffara <vbeffara@gmail.com>

RUN apt-get update -qq

RUN apt-get install -y -qq python-pip cmake gcc g++ ccache lcov git ninja-build xvfb libboost-all-dev ruby
RUN apt-get install -y -qq libcairo2-dev libfltk1.3-dev libfftw3-dev libfplll-dev libgmp-dev libmpfr-dev libpng++-dev
RUN apt-get install -y -qq libyaml-cpp-dev libcln-dev libgsl-dev libeigen3-dev

RUN pip install codecov
RUN gem install coveralls-lcov
