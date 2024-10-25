FROM ubuntu:latest
RUN apt update && \
    apt install -y sudo git gcc gdb make \
    binutils libc6-dev