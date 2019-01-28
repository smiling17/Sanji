#!/bin/bash

USER_ID="sangjun_lee2"
USER_EMAIL="sangjun_lee2@tmax.co.kr"

git config --global user.name "${USER_ID}"
git config --global user.email"${USER_EMAIL}"

ssh-keygen -t rsa -C "${USER_EMAIL}" 

