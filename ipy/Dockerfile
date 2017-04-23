FROM lballabio/quantlib
MAINTAINER ts williams <tstwasg@gmail.com>
LABEL Description="An IPython notebook server with the QuantLib Python as other modules available"

RUN apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get install -y git bzip2 build-essential libssl-dev libffi-dev

ENV LANG C.UTF-8

# install Python + NodeJS with conda
RUN wget -q https://repo.continuum.io/miniconda/Miniconda3-4.2.12-Linux-x86_64.sh -O /tmp/miniconda.sh  && \
    echo 'd0c7c71cc5659e54ab51f2005a8d96f3 */tmp/miniconda.sh' | md5sum -c - && \
    bash /tmp/miniconda.sh -f -b -p /opt/conda && \
    /opt/conda/bin/conda install --yes -c conda-forge python=3.6 sqlalchemy tornado jinja2 traitlets requests pip nodejs configurable-http-proxy && \
    /opt/conda/bin/pip install --upgrade pip && \
    rm /tmp/miniconda.sh

ENV PATH=/opt/conda/bin:$PATH

RUN git clone https://github.com/jupyterhub/jupyterhub.git /src/jupyterhub
#ADD . /src/jupyterhub
WORKDIR /src/jupyterhub
RUN python setup.py js && pip install . && \
    rm -rf $PWD ~/.cache ~/.npm

RUN mkdir -p /srv/jupyterhub/
WORKDIR /srv/jupyterhub/
EXPOSE 8000

LABEL org.jupyter.service="jupyterhub"
# missing in 3.6: pyspark_elastic
RUN pip install pandas_datareader \
 && pip install pyzmq \
 && pip install cryptography \
 && pip install quandl \
 && pip install oauthenticator \
 && pip install redis

ARG quantlib_swig_version=1.9
ENV quantlib_swig_version ${quantlib_swig_version}

RUN wget http://downloads.sourceforge.net/project/quantlib/QuantLib/${quantlib_swig_version}/other\ languages/QuantLib-SWIG-${quantlib_swig_version}.tar.gz \
    && tar xfz QuantLib-SWIG-${quantlib_swig_version}.tar.gz \
    && rm QuantLib-SWIG-${quantlib_swig_version}.tar.gz \
    && cd QuantLib-SWIG-${quantlib_swig_version} \
    && ./configure --disable-perl --disable-ruby --disable-mzscheme --disable-guile --disable-csharp --disable-ocaml --disable-r --disable-java CXXFLAGS=-O3 \
    && make && make -C Python check && make install \
    && cd .. && rm -rf QuantLib-SWIG-${quantlib_swig_version}

CMD ["jupyterhub"]
