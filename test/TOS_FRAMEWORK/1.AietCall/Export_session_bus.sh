#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 UID"
  exit 1;
fi

OUTPUT="$(./Servicecall.sh \
  tos.service.ServiceManager.Session getSessionBusAddress \{\"\":\""$1"\"\})"

SUBSTRING="\"return\""
if echo "$OUTPUT" | grep -q "$SUBSTRING"; then
  INDEX="$(awk -v a="$OUTPUT" -v b="$SUBSTRING" 'BEGIN{print index(a,b)}')"
  ENDSTRING="\"}\""
  OUTPUT1=${OUTPUT:$((INDEX+9))}
  OUTPUT1=${OUTPUT1%"$ENDSTRING"}
  export DBUS_SESSION_BUS_ADDRESS="${OUTPUT1}"
  echo "$OUTPUT1"
else
  echo "ERROR"
fi
