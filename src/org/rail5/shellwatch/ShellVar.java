package org.rail5.shellwatch;

public class ShellVar {
	public String name;
	public String value;

	public ShellVar(String name, String value) {
		this.name = name;
		this.value = value;
	}

	public String toString() {
		return name + "=" + value;
	}

	public static ShellVar fromString(String line) {
		String[] parts = line.split("=");
		switch (parts.length) {
			case 0:
				return new ShellVar("", "");
			case 1:
				return new ShellVar(parts[0], "");
			case 2:
				return new ShellVar(parts[0], parts[1]);
			default:
				return new ShellVar(parts[0], parts[1]);
		}
	}

	public static boolean equals(ShellVar a, ShellVar b) {
		return a.name.equals(b.name);
	}

	public String getName() {
		return name;
	}

	public String getValue() {
		return value;
	}
}
