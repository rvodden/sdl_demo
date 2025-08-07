ARG GCC_VERSION=14
ARG USERNAME=ubuntu
# Default GCC version

FROM ubuntu:24.04
# Use the official Ubuntu 24.04 image as a base

ARG GCC_VERSION
ARG USERNAME

RUN apt update && \
    apt install -y sudo zsh && \
    rm -rf /var/lib/apt/lists/* && \
    echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${USERNAME} && \
    chmod 0440 /etc/sudoers.d/${USERNAME}

RUN apt update && \
    apt install -y gnome-desktop-testing libasound2-dev libpulse-dev \
    libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
    libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev \
    libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
    libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev fcitx-libs-dev \
    libpipewire-0.3-dev libwayland-dev libdecor-0-dev && \
    rm -rf /var/lib/apt/lists/*

# Add cmake repository and install cmake
RUN apt update && \
    apt install -y ca-certificates gpg wget ninja-build sudo && \
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - >/usr/share/keyrings/kitware-archive-keyring.gpg && \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble main' > /etc/apt/sources.list.d/kitware.list && \
    apt-get update && \
    apt update && \
    apt install -y cmake && \
    rm -rf /var/lib/apt/lists/*

# Add modern GCC
RUN apt update && \
    apt install -y software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt update && \
    apt install -y gcc-${GCC_VERSION} g++-${GCC_VERSION} gdb && \
    rm -rf /var/lib/apt/lists/*
COPY ld.so.conf /etc/ld.so.conf.d/g++.conf

# Add modern git
RUN apt update && \
    add-apt-repository ppa:git-core/ppa && \
    apt update && \
    apt install -y git && \
    rm -rf /var/lib/apt/lists/*

ADD --chmod=0644 CMakeToolchain.txt /CMakeToolchain.txt
ENV CMAKE_TOOLCHAIN_FILE="/CMakeToolchain.txt"

ENV NVM_DIR=/usr/local/nvm
RUN mkdir -p ${NVM_DIR}
SHELL ["/bin/bash", "--login", "-c"]
RUN wget -O- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.1/install.sh | bash
RUN bash -c 'source ${NVM_DIR}/nvm.sh && nvm install --lts'
RUN chown -R ubuntu:root /usr/local/nvm
RUN apt clean
RUN chsh ${USERNAME} -s /usr/bin/zsh

# [Optional] Set the default user. Omit if you want to keep the default as root.
USER ${USERNAME}
RUN sh -c "$(wget https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh -O -)"
RUN curl -fsSL https://bun.sh/install | bash
RUN echo "export NVM_DIR=${NVM_DIR}"                                         >> "$HOME/.bashrc"
RUN echo '[ -s "$NVM_DIR/nvm.sh" ] && . "$NVM_DIR/nvm.sh"  # This loads nvm' >> "$HOME/.bashrc"
RUN echo '[ -s "$NVM_DIR/bash_completion" ] && . "$NVM_DIR/bash_completion"' >> "$HOME/.bashrc"
RUN echo "export NVM_DIR=${NVM_DIR}"                                         >> "$HOME/.zshrc"
RUN echo '[ -s "$NVM_DIR/nvm.sh" ] && . "$NVM_DIR/nvm.sh"  # This loads nvm' >> "$HOME/.zshrc"
RUN bash -c 'source ${NVM_DIR}/nvm.sh && npm install -g @anthropic-ai/claude-code'
