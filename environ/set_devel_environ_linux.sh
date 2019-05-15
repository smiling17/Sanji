#1/bin/bash

USER_ENV_LIST=".bash_profile .bashrc .profile .vimrc"
USER_PATH="/root/"

cd ./files

for ENV in ${USER_ENV_LIST}
do
	cp $ENV ${USER_PATH}${ENV}
done

source ${USER_PATH}".bashrc"
