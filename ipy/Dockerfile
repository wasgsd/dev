FROM lballabio/quantlib-python
MAINTAINER ts williams <tstwasg@gmail.com>
LABEL Description="An IPython notebook server with the QuantLib Python as other modules available"

RUN apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get install -y ipython-notebook python-matplotlib \
                                                      python-pandas python-seaborn \
                                                      python-setuptools\
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/* \
 && easy_install pip \
 && pip install pyzmq
 && pip install pyspark_elastic
 && pip install quandl

EXPOSE 8888

RUN mkdir /notebooks
VOLUME /notebooks
# COPY *.ipynb /notebooks/

CMD ipython notebook --no-browser --ip=0.0.0.0 --port=8888 --notebook-dir=/notebooks
