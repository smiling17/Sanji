#!/usr/bin/env bash

AGENT_VENDER_NAME="Geni"

AGENT_NAME="Genian"

INSTALL_PATH_PREFIX="/usr/local"

INSTALL_PATH="${INSTALL_PATH_PREFIX}/${AGENT_VENDER_NAME}"

VERSION_SCRIPT_NAME="Genian"

VERSION_SCRIPT_PATH="/usr/bin"


# /usr/local/Geni/Genian

HOME_DIR="${INSTALL_PATH}/${AGENT_NAME}"


# /usr/local/Geni/Genian/Agent

AGENT_PATH="${HOME_DIR}/Agent"


VERSION_INFO_DAEMON="gnversion.py"


function main() {
    INSTALL_PATH="${HOME_DIR}"

    # /usr/local/Geni/Genian
    cd ${INSTALL_PATH}

    case $1 in
        -v)
            # /usr/local/Geni/Genian/Agent/gnversion.py
            MSG=`python ${AGENT_PATH}/${VERSION_INFO_DAEMON} -v`
        ;;

        *)
            echo "$0 [options]"
            echo "  -v  Display the product version and exit."
            exit 0
        ;;
    esac

    echo "${MSG}"
}


case $1 in

    *)
        main $1
    ;;

esac

exit 0