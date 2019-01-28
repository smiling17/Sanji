#1/bin/bash

USER_ENV_LIST=".bash_profile .bashrc .profile .vimrc"
USER_PATH="/root/"

for ENV in ${USER_ENV_LIST}
do
	cp $ENV ${USER_PATH}${ENV}
done

