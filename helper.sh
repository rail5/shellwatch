#!/usr/bin/env bash

set -T
shopt -s extdebug

# Socket descriptors should be set by the GUI before sourcing this script
export SHELLWATCH_SOCKET_TO_GUI
export SHELLWATCH_SOCKET_FROM_GUI
if [[ -z "$SHELLWATCH_SOCKET_TO_GUI" || -z "$SHELLWATCH_SOCKET_FROM_GUI" ]]; then
	>&2 echo "Error: SHELLWATCH_SOCKET_TO_GUI and SHELLWATCH_SOCKET_FROM_GUI must be set before sourcing this script."
	exit 1
fi

# Send all initial shell variables to the GUI, so it can filter them out
echo "__SHELLWATCH_INIT_BEGIN" >&"$SHELLWATCH_SOCKET_TO_GUI"
compgen -v >&"$SHELLWATCH_SOCKET_TO_GUI" # Lists all variable names
echo "__SHELLWATCH_INIT_END" >&"$SHELLWATCH_SOCKET_TO_GUI"

_shellwatch_debug() {
	# write the following to the socket:
	# 1. the current line number
	# 2. all environment variables (and their values) not in the initial set

	echo "__SHELLWATCH_VARS_BEGIN ${BASH_LINENO[0]}" >&"$SHELLWATCH_SOCKET_TO_GUI"
	declare -p >&"$SHELLWATCH_SOCKET_TO_GUI" # Lists all variables and their values, in the format: declare -OPTIONS varname[="value"]
	# Note: value may be given as $'...' rather than "...",
	# or as (...) for arrays
	echo "__SHELLWATCH_VARS_END" >&"$SHELLWATCH_SOCKET_TO_GUI"

	# do a blocking read from the socket to wait for a response from the GUI
	# the GUI will send 'C' to continue, or 'D' to die
	read -r _shellwatch_response <&"$SHELLWATCH_SOCKET_FROM_GUI"
	case "$_shellwatch_response" in
		C)
			# continue execution
			;;
		D)
			# exit the script
			exit 0
			;;
		*)
			>&2 echo "Unknown response from GUI: $_shellwatch_response"
			exit 1
			;;
	esac
}

trap '_shellwatch_debug' DEBUG
