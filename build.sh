#!/bin/sh

apk update && \
    apk add --no-cache gcc g++ make python3-dev py3-pip && \
    apk add --no-cache git python3-dev

git clone https://github.com/ThrowTheSwitch/Unity.git /app/unity

cd /app/unity && \
    gcc -c src/unity.c -o unity.o -Isrc && \
    ar rcs libunity.a unity.o && \
    cp libunity.a /usr/local/lib && \
    cp src/unity.h /usr/local/include && \
    cd /app && \
    apk del git

python3 -m venv /app/.venv
source /app/.venv/bin/activate
pip install -r requirements.txt

make buildso
make buildtests
