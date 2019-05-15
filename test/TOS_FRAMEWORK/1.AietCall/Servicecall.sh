#!/bin/bash

NAME="$1"
METHOD="$2"
INPUTJSON="$3"
ADDRESSOPT="$4"  # optional

if [ -z "$NAME" ] || [ -z "$METHOD" ] || [ -z "$INPUTJSON" ]; then
  echo -e "Usage: servicecall NAME METHOD INPUTJSON [ADDRESSOPT]"
  exit 1
fi
if [ -z "$ADDRESSOPT" ]; then
  ADDRESSOPT="--system"
fi

OBJECT=""

TOKEN_PREV=""
IFS='.'
read -ra ARRAY <<< "$NAME"
for TOKEN in "${ARRAY[@]}"; do # access each element of array
  OBJECT="$OBJECT/$TOKEN"
done

echo "[NAME]   $NAME"
echo "[OBJECT] $OBJECT"
echo "[METHOD] $METHOD"
echo "[INPUT]  $INPUTJSON"
echo "[ADDRESSOPT]  $ADDRESSOPT"

dbus-send $ADDRESSOPT --print-reply --type=method_call --dest="$NAME" \
  "$OBJECT" "$NAME.invoke" string:"$METHOD" string:"$INPUTJSON"
