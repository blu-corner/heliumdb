FROM centos:centos7
MAINTAINER Colin Pattison <colin.pattison@neueda.com>

RUN yum -y install epel-release

RUN yum -y install git wget \
    python-devel.x86_64 python-pip \
    python36.x86_64 python36-devel.x86_64 python36-pip \
    clang cmake

RUN yum groupinstall -y "Development Tools" "Development Libraries"

RUN pip install --upgrade pip

# ADD install-helium-linux-3.3.1.sh /install-hel.sh
# RUN /install-hel.sh
