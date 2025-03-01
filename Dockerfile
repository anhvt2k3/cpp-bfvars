FROM ubuntu:noble

RUN apt-get update -y
RUN apt-get install -y \
    libssl-dev \
    build-essential

WORKDIR /app
