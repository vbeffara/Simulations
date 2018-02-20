FROM ubuntu:artful
MAINTAINER Vincent Beffara <vbeffara@gmail.com>

RUN apt-get update
RUN apt-get install -y python-pip cmake gcc g++ ccache lcov git ninja-build xvfb ruby
RUN apt-get install -y libcairo2-dev libfltk1.3-dev libfftw3-dev libfplll-dev libgmp-dev libmpfr-dev
RUN apt-get install -y libcln-dev libgsl-dev libeigen3-dev

RUN pip install codecov conan
RUN gem install coveralls-lcov
