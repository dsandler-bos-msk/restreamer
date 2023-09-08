# syntax=docker/dockerfile:1.3
FROM ubuntu:20.04 as dev

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential cmake && \
    rm -rf /var/lib/apt/lists/* && \
    apt-get clean -y --no-install-recommends && \
    apt-get autoclean -y --no-install-recommends

RUN --mount=type=bind,rw,target=/home,source=. \
    cd /home && mkdir build && cd build && \
    cmake .. && make -j && mv restreamer /


FROM ubuntu:20.04

COPY --from=dev restreamer restreamer

WORKDIR /workspace
ENTRYPOINT ["/restreamer"]
