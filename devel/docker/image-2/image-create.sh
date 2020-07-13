#!/bin/bash

IMAGE_NAME="sanji-img"
IMAGE_TAG="1.1"
IMAGE_DIR="./" # Dockerfile 이 위치한 디렉토리


# docker 기반 image 생성
[ -x /usr/bin/docker ] || exit 0

docker build --tag ${IMAGE_NAME}:${IMAGE_TAG} ${IMAGE_DIR}

# Image transport (docker-daemon -> cri-0)
[ -x /usr/bin/skopeo ] || exit 0

skopeo copy docker-daemon:${IMAGE_NAME}:${IMAGE_TAG} containers-storage:${IMAGE_NAME}:${IMAGE_TAG}
