FROM ubuntu:artful
MAINTAINER Vincent Beffara <vbeffara@gmail.com>

RUN apt-get update
RUN apt-get install -y python-pip cmake gcc g++ ccache lcov git ninja-build xvfb ruby
RUN apt-get install -y libcairo2-dev libfplll-dev libgmp-dev libmpfr-dev libgl1-mesa-dev
RUN apt-get install -y libcln-dev libgsl-dev libeigen3-dev libpng++-dev mesa-common-dev

RUN pip install codecov conan
RUN gem install coveralls-lcov
