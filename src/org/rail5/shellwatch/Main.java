package org.rail5.shellwatch;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Font;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import javax.swing.*;
import javax.swing.plaf.basic.BasicScrollBarUI;
import javax.swing.text.BadLocationException;

import org.rail5.shellwatch.GUIHelpers.CustomFontLoader;
import org.rail5.shellwatch.GUIHelpers.RoundedBorder;

class Main {
	/* Positioning & sizing constants */
	static final int frameWidth        = 800;
	static final int frameHeight       = 600;
	static final int itemLeftOffset    = 10;
	static final int itemTopOffset     = 18;

	static final int menuBarHeight     = 24;
	static final int lineSpacing       = 30;
	static final int checkboxSpacing   = 150;
	static final int checkboxHeight    = 23;
	static final int checkboxWidth     = 106;

	static final int fontSize          = 14;
	static final int largerFontSize    = 18;
	static final int buttonHeight      = 28;
	static final int textFieldHeight   = 21;

	static final int itemRightMargin   = frameWidth - itemLeftOffset;
	static final int itemBottomMargin  = frameHeight - itemTopOffset - menuBarHeight;

	static final int fullWidth         = frameWidth - (3 * itemLeftOffset);
	static final int fullHeight        = frameHeight - (3 * itemTopOffset);

	/* Color constants */
	static final Color backgroundColor             = Color.decode("#1e1e1e");
	static final Color foregroundColor             = Color.decode("#D9D9D9");
	static final Color textFieldBackgroundColor    = Color.decode("#B2B2B2");
	static final Color textFieldForegroundColor    = Color.decode("#656565");
	static final Color buttonBackgroundColor       = Color.decode("#2e2e2e");
	static final Color lighterBackgroundColor      = Color.decode("#3e3e3e");
	static final Color roundedBorderColor          = Color.decode("#979797");
	static final Color onClickColor                = Color.decode("#232323");
	static final Color onFocusColor                = Color.decode("#353535");

	/* Font */
	static final Font defaultFont  = CustomFontLoader.loadFont("/fonts/Lato.ttf", fontSize);
	static final Font largerFont   = CustomFontLoader.loadFont("/fonts/Lato.ttf", largerFontSize);

	static String varsFile;
	static String callbackFile;

	static ShellVarList vars;

	static String sourceFileContents = "";

	static boolean initNoArg = false;

	public static ShellVarList loadVars() {
		if (initNoArg) {
			initNoArg = false;
			return new ShellVarList();
		}
		// Shellwatch proper will write "ping {linenumber}" to the callback file when ready

		// Open varsFile and read the variables
		ShellVarList result = new ShellVarList();

		// Read the callback file until it says "ping"
		String callback = "";
		while (true) {
			try (BufferedReader br = new BufferedReader(new FileReader(callbackFile))) {
				callback = br.readLine();
				if (callback != null && callback.startsWith("ping")) {
					break;
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		// Get the line number from the callback
		String[] parts = callback.split(" ");
		if (parts.length != 2) {
			System.out.println("Invalid callback: " + callback);
			System.exit(1);
		}

		if (parts[1].equals("done")) {
			// Delete the callback file and exit
			java.io.File file = new java.io.File(callbackFile);
			file.delete();
			System.exit(0);
		}

		result.lineNumber = Integer.parseInt(parts[1]);

		try (BufferedReader br = new BufferedReader(new FileReader(varsFile))) {
			String line;
			while ((line = br.readLine()) != null) {
				result.add(ShellVar.fromString(line));
			}
		} catch (IOException e) {
			e.printStackTrace();
		}

		return result;
	}

	public static void loadScriptSource(String scriptFile) {
		if (scriptFile.equals("")) {
			return;
		}

		try (BufferedReader br = new BufferedReader(new FileReader(scriptFile))) {
			String line;
			while ((line = br.readLine()) != null) {
				sourceFileContents += line + "\n";
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void loadScript(String scriptFile) {
		// Terminate any currently-running script (send "done" to the callback file)
		if (callbackFile != null) {
			try {
				java.io.FileWriter fw = new java.io.FileWriter(callbackFile, false);
				fw.write("pong done");
				fw.close();
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}

		// Call 'shellwatch' with the new script file
		String[] cmd = { "shellwatch", scriptFile, "1", callbackFile };
		try {
			Runtime.getRuntime().exec(cmd);
		} catch (IOException e) {
			e.printStackTrace();
		}

		// Load the new script source
		loadScriptSource(scriptFile);

		// Wait for the new location of the vars file and callback file to be written to the current callback file
		String newVarsFile = "";
		String newCallbackFile = "";
		while (true) {
			try (BufferedReader br = new BufferedReader(new FileReader(callbackFile))) {
				String line;
				// The first line is the path to the new vars file
				if ((line = br.readLine()) != null) {
					newVarsFile = line;
				}
				// The second line is the path to the new callback file
				if ((line = br.readLine()) != null) {
					newCallbackFile = line;
				}
				if (!newVarsFile.equals("") && !newCallbackFile.equals("")) {
					break;
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		// Update the vars file and callback file
		varsFile = newVarsFile;
		callbackFile = newCallbackFile;

		// Load the new variables
		vars = loadVars();
	}

	public static void main(String[] args) {
		String sourceFile = "";
		if (args.length == 0) {
			initNoArg = true;
		} else if (args.length == 3) {
			sourceFile = args[0];
			varsFile = args[1];
			callbackFile = args[2];
		} else {
			System.out.println("Usage: java -jar shellwatch.jar <source file> <vars file> <callback file>");
			System.out.println("Or: java -jar shellwatch.jar");
			System.exit(1);
		}

		loadScriptSource(sourceFile);

		/* Frame */
		JFrame frame = new JFrame("Shellwatch");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(frameWidth, frameHeight);
		JPanel panel = new JPanel();
		panel.setLayout(null);
		panel.setBackground(backgroundColor);

		/* Components */

		/* Menu bar */
		JMenuBar menuBar = new JMenuBar();
		menuBar.setBackground(backgroundColor);
		menuBar.setForeground(foregroundColor);
		frame.setJMenuBar(menuBar);

		/* Menu */
		JMenu fileMenu = new JMenu("File");
		fileMenu.setFont(defaultFont);
		fileMenu.setForeground(foregroundColor);
		fileMenu.setBackground(backgroundColor);
		menuBar.add(fileMenu);

		/* Menu items */
		JMenuItem openItem = new JMenuItem("Open Script");
		openItem.setFont(defaultFont);
		openItem.setForeground(foregroundColor);
		openItem.setBackground(backgroundColor);
		fileMenu.add(openItem);

		// Tiny top panel to hold the 'step' button
		JPanel topPanel = new JPanel();
		topPanel.setBackground(backgroundColor);
		topPanel.setLayout(null);

		// 'Terminate' button
		JButton terminateButton = new JButton("Terminate");
		terminateButton.setFont(defaultFont);
		terminateButton.setBackground(buttonBackgroundColor);
		terminateButton.setForeground(foregroundColor);
		terminateButton.setFocusPainted(false);
		terminateButton.setBorder(new RoundedBorder(10, roundedBorderColor, 2));
		terminateButton.setBounds(0, 3, 100, buttonHeight); // Initial position

		// Handler for the 'Terminate' button
		terminateButton.addActionListener(e -> {
			// Write "done" to the callback file to signal that we're done
			try {
				java.io.FileWriter fw = new java.io.FileWriter(callbackFile, false);
				fw.write("pong done");
				fw.close();
			} catch (IOException ex) {
				ex.printStackTrace();
			}
			System.exit(0);
		});

		topPanel.add(terminateButton);

		// 'Step' button
		JButton stepButton = new JButton("Step");
		stepButton.setFont(defaultFont);
		stepButton.setBackground(buttonBackgroundColor);
		stepButton.setForeground(foregroundColor);
		stepButton.setFocusPainted(false);
		stepButton.setBorder(new RoundedBorder(10, roundedBorderColor, 2));
		stepButton.setBounds((frameWidth - 100) / 2, 3, 100, buttonHeight); // Initial position

		// Add a component listener to adjust the button position when the frame is resized
		frame.addComponentListener(new java.awt.event.ComponentAdapter() {
			public void componentResized(java.awt.event.ComponentEvent evt) {
				stepButton.setBounds((frame.getWidth() - 100) / 2, 3, 100, buttonHeight);
			}
		});

		topPanel.add(stepButton);

		// Add a label to the right of the button telling us the line number
		JLabel lineNumberLabel = new JLabel("Line: 0");
		lineNumberLabel.setFont(defaultFont);
		lineNumberLabel.setForeground(foregroundColor);
		lineNumberLabel.setBounds((frameWidth - 100) / 2 + 100, 3, 100, buttonHeight);
		topPanel.add(lineNumberLabel);

		// Add a component listener to adjust the label position when the frame is resized
		frame.addComponentListener(new java.awt.event.ComponentAdapter() {
			public void componentResized(java.awt.event.ComponentEvent evt) {
				stepButton.setBounds((frame.getWidth() - 100) / 2, 3, 100, buttonHeight);
				lineNumberLabel.setBounds((frame.getWidth() - 100) / 2 + 100, 3, 100, buttonHeight);
			}
		});

		// Set the size of the top panel to just fit the button
		topPanel.setPreferredSize(new java.awt.Dimension(frameWidth, buttonHeight + 6));

		// Left panel
		// The left panel contains the table of variables
		JPanel leftPanel = new JPanel();
		leftPanel.setBackground(backgroundColor);
		leftPanel.setLayout(null);

		// Table of variables
		vars = loadVars();
		String[] columnNames = {"Variable", "Value"};

		JTable table = new JTable(vars.toTable(), columnNames) {
			@Override
			public boolean isCellEditable(int row, int column) {
				return false; // Make the table read-only
			}
		};

		table.setFont(defaultFont);
		table.setBackground(textFieldBackgroundColor);
		table.setForeground(textFieldForegroundColor);
		table.setRowHeight(lineSpacing);
		table.getTableHeader().setFont(largerFont);
		table.getTableHeader().setBackground(buttonBackgroundColor);
		table.getTableHeader().setForeground(foregroundColor);

		// Set the background color for the empty table
		table.setFillsViewportHeight(true);
		table.setOpaque(false);
		JScrollPane tableScrollPane = new JScrollPane(table);
		tableScrollPane.getViewport().setBackground(backgroundColor);
		tableScrollPane.setBorder(BorderFactory.createEmptyBorder());
		leftPanel.setLayout(new BorderLayout());
		leftPanel.add(tableScrollPane, BorderLayout.CENTER);

		// Right panel
		// The right panel contains the source code
		JPanel rightPanel = new JPanel();
		rightPanel.setBackground(backgroundColor);
		rightPanel.setLayout(null);

		// Text area for displaying the source file contents
		JTextArea sourceTextArea = new JTextArea(sourceFileContents);
		sourceTextArea.setFont(defaultFont);
		sourceTextArea.setBackground(textFieldBackgroundColor);
		sourceTextArea.setForeground(textFieldForegroundColor);
		sourceTextArea.setEditable(false); // Make the text area read-only
		sourceTextArea.setLineWrap(true);
		sourceTextArea.setWrapStyleWord(true);

		// Scroll pane for the text area
		JScrollPane scrollPane = new JScrollPane(sourceTextArea);
		scrollPane.setBorder(BorderFactory.createEmptyBorder());
		scrollPane.getVerticalScrollBar().setUI(new BasicScrollBarUI() {
			@Override
			protected void configureScrollBarColors() {
				this.thumbColor = lighterBackgroundColor;
			}
		});

		// Add action listener to the 'Step' button
		stepButton.addActionListener(e -> {
			// Write "pong" to the callback file to signal that we're ready for the next step
			try {
				java.io.FileWriter fw = new java.io.FileWriter(callbackFile, false);
				fw.write("pong");
				fw.close();
			} catch (IOException ex) {
				ex.printStackTrace();
			}

			vars.clear();
			vars = loadVars();
			// Populate the table with the updated variables
			table.setModel(new javax.swing.table.DefaultTableModel(vars.toTable(), columnNames));

			// Update the line number label
			lineNumberLabel.setText("Line: " + vars.lineNumber);

			// Clear any existing highlights
			sourceTextArea.getHighlighter().removeAllHighlights();

			// Highlight the current line in the source code
			int lineToHighlight = vars.lineNumber;
			try {
				int startOffset = sourceTextArea.getLineStartOffset(lineToHighlight - 1);
				int endOffset = sourceTextArea.getLineEndOffset(lineToHighlight - 1);
				sourceTextArea.setCaretPosition(startOffset);
				sourceTextArea.moveCaretPosition(endOffset);
				sourceTextArea.getHighlighter().addHighlight(startOffset, endOffset, new javax.swing.text.DefaultHighlighter.DefaultHighlightPainter(Color.YELLOW));
			} catch (BadLocationException ex) {
				ex.printStackTrace();
			}
		});

		// Add action listener to the 'Open Script' menu item
		openItem.addActionListener(e -> {
			// Open a file chooser dialog
			JFileChooser fileChooser = new JFileChooser();
			int returnValue = fileChooser.showOpenDialog(null);
			if (returnValue == JFileChooser.APPROVE_OPTION) {
				java.io.File selectedFile = fileChooser.getSelectedFile();
				vars.clear();
				loadScript(selectedFile.getAbsolutePath());

				// Populate the table with the updated variables
				table.setModel(new javax.swing.table.DefaultTableModel(vars.toTable(), columnNames));
				
				// Update the textarea with the new source code
				sourceTextArea.setText(sourceFileContents);

				// Update the line number label
				lineNumberLabel.setText("Line: 0");
			}
		});

		// Use BorderLayout to ensure the text area re-scales with the panel
		rightPanel.setLayout(new BorderLayout());
		rightPanel.add(scrollPane, BorderLayout.CENTER);

		// Add the scroll pane to the right panel
		rightPanel.add(scrollPane);

		// Split pane to hold the left and right panels
		JSplitPane splitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, leftPanel, rightPanel);
		splitPane.setDividerLocation(frameWidth / 2);
		splitPane.setDividerSize(0); // No visible divider
		splitPane.setResizeWeight(0.5); // Evenly distribute space between panels
		splitPane.setBorder(null); // No border

		// Add the top panel and split pane to the main panel
		panel.setLayout(new BorderLayout());
		panel.add(topPanel, BorderLayout.NORTH);
		panel.add(splitPane, BorderLayout.CENTER);

		/* Begin */
		frame.add(panel);
		frame.setVisible(true);
	}
}
