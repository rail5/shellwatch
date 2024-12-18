package org.rail5.shellwatch;

import java.util.ArrayList;

public class ShellVarList extends ArrayList<ShellVar> {
	public int lineNumber = 0;

	public Object[][] toTable() {
		Object[][] result = new Object[this.size()][2];
		for (int i = 0; i < this.size(); i++) {
			result[i][0] = this.get(i).name;
			result[i][1] = this.get(i).value;
		}
		return result;
	}
}
