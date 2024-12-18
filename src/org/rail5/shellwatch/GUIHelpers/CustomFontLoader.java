package org.rail5.shellwatch.GUIHelpers;

import java.awt.Font;
import java.awt.FontFormatException;
import java.io.IOException;
import java.io.InputStream;

public class CustomFontLoader {

	public static Font loadFont(String path, float size) {
		try {
			InputStream fontStream = CustomFontLoader.class.getResourceAsStream(path);
			Font font = Font.createFont(Font.TRUETYPE_FONT, fontStream);
			font = font.deriveFont(size);
			return font;
		} catch (FontFormatException | IOException e) {
			System.out.println("Font not found, using Arial");
			return new Font("Arial", Font.PLAIN, (int) size);
		}
	}
}
