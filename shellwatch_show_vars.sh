#!/usr/bin/env bash

export shellwatch_tmp_file="$(mktemp)"
export shellwatch_vars_file="$(mktemp)"
export shellwatch_callback_file="$(mktemp)"

function shellwatch_clean_up() {
	rm -f "$shellwatch_tmp_file"
}

function shellwatch_show_vars() {
	local initial_run="$1" latest_line="$2"
	shellwatch_vars="$(set | grep -P -e "^[a-zA-Z0-9_]+=")"

	shellwatch_var_names="$(awk -F= '{print $1}' <<< "$shellwatch_vars")"
	shellwatch_var_values="$(awk -F= '{for (i=2; i<=NF; i++) printf "%s%s", $i, (i<NF ? "=" : ORS)}' <<< "$shellwatch_vars")"

	if [[ "$initial_run" -eq 1 ]]; then
		# Sort the initial environment variable names, store them to our temp file
		# So that we can ignore them on subsequent runs
		awk -F= '{print $1}' <<< "$shellwatch_vars" > "$shellwatch_tmp_file"
		echo "ping 1" > "$shellwatch_callback_file"
	else
		( for i in $(seq 1 $(wc -l <<< "$shellwatch_var_names")); do
			shellwatch_var_name="$(awk -v i="$i" 'NR==i {print $1}' <<< "$shellwatch_var_names")"

			if grep -q "^$shellwatch_var_name$" "$shellwatch_tmp_file"; then
				continue
			fi

			shellwatch_var_value="$(awk -v i="$i" 'NR==i' <<< "$shellwatch_var_values")"
			echo "$shellwatch_var_name=$shellwatch_var_value"
		done ) > "$shellwatch_vars_file"
		echo "ping $latest_line" > "$shellwatch_callback_file"
		unset i shellwatch_var_name shellwatch_var_value shellwatch_vars shellwatch_var_names shellwatch_var_values
		# Watch the callback file until it says "pong"
		while ! grep -q "pong" "$shellwatch_callback_file"; do
			sleep 1
		done
		# If it says "pong", we can carry on
		# However, if it says "pong done", we should exit early
		if grep -q "done" "$shellwatch_callback_file"; then
			shellwatch_clean_up
			rm -f "$shellwatch_callback_file"
			exit 0
		fi
	fi
}