FROM debian:bookworm

USER root

# Install needed packages. Use a separate RUN statement to add your own dependencies.
RUN apt update && export DEBIAN_FRONTEND=noninteractive \
    && apt -y install \
        build-essential \
        git \
        curl \
        && apt autoremove -y \
        && apt clean -y \
        && rm -rf /var/lib/apt/lists/*

RUN apt update

RUN useradd -ms /bin/bash developer

USER developer

RUN curl https://sh.rustup.rs -sSf | bash -s -- -y

RUN echo 'source $HOME/.cargo/env' >> $HOME/.bashrc

RUN rustup toolchain install stable --component rust-src

RUN rustup target add riscv32imc-unknown-none-elf
