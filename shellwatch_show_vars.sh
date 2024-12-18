#!/usr/bin/env bash

export shellwatch_tmp_file="$(mktemp)"

function shellwatch_show_vars() {
	local initial_run="$1" latest_line="$2"
	shellwatch_vars="$(set | grep -P -e "^[a-zA-Z0-9_]+=")"

	shellwatch_var_names="$(awk -F= '{print $1}' <<< "$shellwatch_vars")"
	shellwatch_var_values="$(awk -F= '{print $2}' <<< "$shellwatch_vars")"

	if [[ "$initial_run" -eq 1 ]]; then
		# Sort the initial environment variable names, store them to our temp file
		# So that we can ignore them on subsequent runs
		awk -F= '{print $1}' <<< "$shellwatch_vars" > "$shellwatch_tmp_file"
	else
		( for i in $(seq 1 $(wc -l <<< "$shellwatch_var_names")); do
			shellwatch_var_name="$(awk -v i="$i" 'NR==i {print $1}' <<< "$shellwatch_var_names")"

			if grep -q "^$shellwatch_var_name$" "$shellwatch_tmp_file"; then
				continue
			fi

			shellwatch_var_value="$(awk -v i="$i" 'NR==i' <<< "$shellwatch_var_values")"
			echo "$shellwatch_var_name"
			echo "$shellwatch_var_value"
		done ) | yad --list --title "Shellwatch" --text "Line $latest_line" --column "vars" --column "values" --width=640 --height=480 --posx=-99999999 --posy=1
		unset i shellwatch_var_name shellwatch_var_value shellwatch_vars shellwatch_var_names shellwatch_var_values
	fi
}

function shellwatch_clean_up() {
	rm -f "$shellwatch_tmp_file"
}