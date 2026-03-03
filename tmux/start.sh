#!/bin/bash

# Absolute path to this script. /home/user/bin/foo.sh
SCRIPT=$(readlink -f "$0")
# Absolute path this script is in. /home/user/bin
SCRIPT_PATH=$(dirname "$SCRIPT")
cd "$SCRIPT_PATH"

export TMUX_SESSION_NAME=ros2_pkg_template
export TMUX_SOCKET_NAME=f4f

# start tmuxinator
tmuxinator start -p ./session.yaml

# if we are not in tmux attach to the session, otherwise switch to it
if [ -z "${TMUX:-}" ]; then
    tmux -L "$TMUX_SOCKET_NAME" a -t "$TMUX_SESSION_NAME"
else
    tmux detach-client -E "tmux -L \"$TMUX_SOCKET_NAME\" a -t \"$TMUX_SESSION_NAME\""
fi
