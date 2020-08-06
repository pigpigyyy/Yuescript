FROM ubuntu:18.04

RUN apt-get update && apt-get install -y lua5.1 liblua5.1-0-dev cmake git unzip wget

RUN apt-get install fuse libfuse-dev

RUN cd /tmp && wget https://luarocks.org/releases/luarocks-3.3.1.tar.gz \
    && tar zxpf luarocks-3.3.1.tar.gz \
    && cd luarocks-3.3.1 \
    && ./configure \
    && make && make install \
    && cd /tmp ; rm -rf luarocks-3.3.1 luarocks-3.3.1.tar.gz

RUN luarocks install moonplus

RUN cd /tmp && git clone https://github.com/pigpigyyy/MoonPlus \
    && cd MoonPlus && make install \
    && cd /tmp ; rm -rf MoonPlus/