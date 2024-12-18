package org.rail5.shellwatch.GUIHelpers;

import java.awt.*;
import javax.swing.border.Border;

public class RoundedBorder implements Border {

	private final int radius;
	private final Color color;
	private final int thickness;

	public RoundedBorder(int radius, Color color, int thickness) {
		this.radius = radius;
		this.color = color;
		this.thickness = thickness;
	}

	@Override
	public Insets getBorderInsets(Component c) {
		return new Insets(this.thickness, this.thickness, this.thickness, this.thickness);
	}

	@Override
	public boolean isBorderOpaque() {
		return true;
	}

	@Override
	public void paintBorder(Component c, Graphics g, int x, int y, int width, int height) {
	  Graphics2D g2d = (Graphics2D) g.create();

	  g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

	  g2d.setColor(this.color);
	  g2d.setStroke(new BasicStroke((float) this.thickness));

	  int offset = this.thickness / 2;
	  int arc = this.radius;

	  int adjustedWidth = width - this.thickness;
	  int adjustedHeight = height - this.thickness;

	  g2d.drawRoundRect(
		  x + offset,
		  y + offset,
		  adjustedWidth,
		  adjustedHeight,
		  arc,
		  arc
	  );

	  g2d.dispose();
	}

}
