FROM ubuntu:artful
MAINTAINER Vincent Beffara <vbeffara@gmail.com>

RUN apt-get update
RUN apt-get install -y python-pip cmake gcc g++ ccache lcov git ninja-build xvfb libboost1.63-all-dev ruby
RUN apt-get install -y libcairo2-dev libfltk1.3-dev libfftw3-dev libfplll-dev libgmp-dev libmpfr-dev libpng++-dev
RUN apt-get install -y libyaml-cpp-dev libcln-dev libgsl-dev libeigen3-dev libspdlog-dev libfmt3-dev

RUN pip install codecov
RUN gem install coveralls-lcov
