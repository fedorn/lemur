/**
 * 
 */
package ireval;

import ireval.RetrievalEvaluator;
import ireval.SetRetrievalEvaluator;
import ireval.RetrievalEvaluator.Document;
import ireval.RetrievalEvaluator.Judgment;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.TreeMap;
import java.util.Vector;

import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.ListSelectionModel;
import javax.swing.UIManager;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.NumberTickUnit;
import org.jfree.chart.axis.TickUnits;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

/**
 * @author dfisher
 *
 */
/**
 * @author dfisher
 *
 */
/**
 * @author dfisher
 *
 */
public class IREvalGui {
	/**
	 * The main frame
	 */
	private JFrame frame;
	/**
	 * Relevance judgments filename
	 */
	private JTextField qrel;
	/**
	 * Baseline results filename
	 */
	private JTextField baselineResults;
	
	/**
	 * Treatment results filenames list
	 */
	private JList treatments;
    /** Underlying list model for the treatment filenames */
    private DefaultListModel treatmentModel;
 	/**
 	 * List of query ids available for single evaluation
 	 */
 	private JComboBox queries;
	/**
	 * The main output pane.
	 */
	private JTextPane outputPane;
	/**
	 * The busy cursor
	 */
	private Cursor waitCursor;
	/**
	 * The default cursor
	 */
	private Cursor defaultCursor;
	/**
	 * File chooser for selecting qrels and results files
	 */
	private JFileChooser fileChooser;
	/**
	 * Show the Recall/Precision graph button
	 */
	private JButton rpButton;
	/**
	 * Show the P@N graph button
	 */
	private JButton pAtButton;

	/**
	 * The currently selected directory for the file chooser.
	 * Initially, the current working directory at program start.
	 */
	private String workingDir;
	/**
	 * The id of the individual query being evaluated.
	 */
	private String queryID;
	/**
	 * The result set to be evaluated. When performing comparison
	 * runs, this set is used as the baseline
	 */
	private String baselineRanksFile;
	/**
	 * The relevance judgments file.
	 */
	private String judgmentsFile;
	/**
	 * The mapping of query ids to judgments.
	 */
	private TreeMap< String, ArrayList<Judgment> > judgments;
	/**
	 * The mapping of query ids to results for the baseline set.
	 */
	private TreeMap< String, ArrayList<Document> > baselineRanks;
	/**
	 * The collection of mappings of query ids to results for the
	 * treatment sets.
	 */
	private Vector<TreeMap< String, ArrayList<Document> > > ranksList;
	/**
	 * The collection of evaluators for the treatments sets.
	 */
	private Vector<SetRetrievalEvaluator> treatmentsVector;
	/**
	 * The evaluator for the baseline set.
	 */
	private SetRetrievalEvaluator baselineEvaluator;
	/**
	 * The evaluator for an individual query.
	 */
	private RetrievalEvaluator currentEvaluator;

	/**
	 * Initialize
	 */
	public IREvalGui() {
		waitCursor = Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR);
		defaultCursor = Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR);
		fileChooser = new JFileChooser();
		workingDir = System.getProperty("user.dir");
		queryID = "";
		baselineRanksFile = "";
		judgmentsFile ="";
		baselineEvaluator = null;
		ranksList = new Vector<TreeMap< String, ArrayList<Document> > >();
		treatmentsVector = new Vector<SetRetrievalEvaluator>();
		
	}
	/**
	 * Add a result set to the collection of treatments.
	 * Load the rankings and install the mapping into the
	 * ranksList.
	 */
	private void addTreatment() {
		String treatment = chooseFile();
		if (treatment.length() == 0) {
			return;
		}
		try {
			// load the rankings
			frame.setCursor(waitCursor);
			outputPane.setCursor(waitCursor);
			ranksList.add(ireval.Main.loadRanking(treatment));
			frame.setCursor(defaultCursor);
			outputPane.setCursor(defaultCursor);

		} catch (Exception e){
			frame.setCursor(defaultCursor);
			outputPane.setCursor(defaultCursor);
			outputPane.setText(e.getMessage());
			return;
		}
		// add it to the list
		treatmentModel.addElement(treatment);
	}
	
	/**
	 * Remove a treatment set from the collection of treatments.
	 */
	private void removeTreatment() {
		int idx = treatments.getSelectedIndex();
		if (idx == -1) return; // none selected
		ranksList.removeElementAt(idx);
		treatmentModel.remove(idx);
	}

	/**
	 * Display the Interpolated Recall/Precision graph.
	 * If a single query is currently being evaluated,
	 * plot only that query. Otherwise, plot the baseline
	 * and each treatment for all queries.
	 */
	private void showPlotRP(){
		String baseline = new File(baselineResults.getText()).getName();
		double[] precs;
		double prec = 0.0;
		XYSeriesCollection dataset = new XYSeriesCollection();
		XYSeries series = new XYSeries(baseline);

		if (currentEvaluator != null) { 
			precs = currentEvaluator.interpolatedPrecision();
			for (int i = 0; i < precs.length; i++) {
				series.add( prec, precs[i]);
				prec += 0.1;
			}
			dataset.addSeries(series);
		} else {
		precs = baselineEvaluator.interpolatedPrecision();
		for (int i = 0; i < precs.length; i++) {
			series.add( prec, precs[i]);
			prec += 0.1;
		}
		dataset.addSeries(series);
		// do the treatments...
		int counter = 0;
		for (SetRetrievalEvaluator eval : treatmentsVector) {
			prec = 0.0;
			String treatment = new File((String)treatmentModel.getElementAt(counter)).getName();
			precs = eval.interpolatedPrecision();
			series = new XYSeries(treatment);
			for (int i = 0; i < precs.length; i++) {
				series.add( prec, precs[i]);
				prec += 0.1;
			}
			dataset.addSeries(series);
			counter++;
		}
		}
		String label = "Interpolated Recall - Precision";
		if (queryID.length() > 0) label += " query:" + queryID;
		JFreeChart chart = ChartFactory.createXYLineChart(label,
				"Recall", "Precision", dataset, PlotOrientation.HORIZONTAL, true, true, false);
		XYPlot plot = (XYPlot) chart.getPlot();
		// show the points too
		XYLineAndShapeRenderer renderer = (XYLineAndShapeRenderer)plot.getRenderer();
		renderer.setBaseShapesVisible(true);
		
		plot.setBackgroundPaint(Color.WHITE);
		plot.setDomainGridlinePaint(Color.BLACK);
		plot.setRangeGridlinePaint(Color.BLACK);
		ValueAxis domain = plot.getDomainAxis();
		ValueAxis range = plot.getRangeAxis();
		TickUnits units = new TickUnits();
		DecimalFormat df = new DecimalFormat("0.0");
		units.add(new NumberTickUnit(0.1, df));
		domain.setStandardTickUnits(units);
		domain.setAutoRange(false);
		domain.setLowerBound(-0.05);
		domain.setUpperBound(1.05);
		range.setStandardTickUnits(units);
		range.setAutoRange(false);
		range.setLowerBound(-0.05);
		range.setUpperBound(1.05);
		JFrame f = new JFrame("Interpolated Recall - Precision");
		//f.setLocationRelativeTo(frame);
		f.setLocationByPlatform(true);
		ChartPanel p = new ChartPanel(chart);
		p.setPreferredSize(new Dimension(600,600));
		f.getContentPane().add(p);
		f.pack();
		f.setVisible(true);
	}

	/**
	 * Display the P@N graph.
	 * If a single query is currently being evaluated,
	 * plot only that query. Otherwise, plot the baseline
	 * and each treatment for all queries.
	 */
	private void showPlotPatN(){
		DefaultCategoryDataset series = new DefaultCategoryDataset();
		String baseline = new File(baselineResults.getText()).getName();
		int[] fixedPoints = SetRetrievalEvaluator.getFixedPoints();
		double [] precs;

		if (currentEvaluator != null) {
			// show for the current query only
			precs = currentEvaluator.precisionAtFixedPoints();
			for (int i = 0; i < precs.length; i++) {
				series.addValue(precs[i], baseline, new Integer(fixedPoints[i]));
			}

		} else {
			// show for the baseline (and treatments)
			precs = baselineEvaluator.precisionAtFixedPoints();
			for (int i = 0; i < precs.length; i++) {
				series.addValue(precs[i], baseline, new Integer(fixedPoints[i]));
			}
			
			// do the treatments...
			int counter = 0;
			for (SetRetrievalEvaluator eval : treatmentsVector) {
				String treatment = new File((String)treatmentModel.getElementAt(counter)).getName();
				precs = eval.precisionAtFixedPoints();
				for (int i = 0; i < precs.length; i++) {
					series.addValue(precs[i], treatment, new Integer(fixedPoints[i]));
				}
				counter++;
			}
		}
		String label = "Precision at N";
		if (queryID.length() > 0) label += " query:" + queryID;
		JFreeChart chart = ChartFactory.createBarChart(label,
				"N", "precision", series, PlotOrientation.VERTICAL, true, true, false);
		JFrame f = new JFrame("Precision @ N");
		//f.setLocationRelativeTo(frame);
		f.setLocationByPlatform(true);
		ChartPanel p = new ChartPanel(chart);
		p.setPreferredSize(new Dimension(600,600));
		f.getContentPane().add(p);
		f.pack();
		f.setVisible(true);
	}


	/** Evaluates a single query.
	 * @param query the query id to evaluate
	 * @param setEvaluator the collection of evaluators containing
	 * the RetrievalEvaluator for the query.
	 * @return the formatted output of the evaluation
	 */
	public String singleQuery( String query, SetRetrievalEvaluator setEvaluator ) {
		for( RetrievalEvaluator evaluator : setEvaluator.getEvaluators() ) {
			if (query.equals(evaluator.queryName())) {
				currentEvaluator = evaluator;
				return ireval.Main.singleQuery(query, evaluator);
			}
		}
		return "";
	}

	/**
	 * Displays a FileChooser for selecting a file.
	 * @return the full pathname of the selected file 
	 * or an empty string, if no file is selected.
	 */
	private String chooseFile() {
		fileChooser.setCurrentDirectory(new File(workingDir));
		if (fileChooser.showOpenDialog(frame) == JFileChooser.APPROVE_OPTION) {
			File file = fileChooser.getSelectedFile();
			workingDir = file.getPath();
			return file.getAbsolutePath();
		} else {
			return "";
		}
	}
	/**
	 * Updates the contents of the query id drop down list.
	 */
	private void updateQueryIds() {
		// update the combo box; Ought to make a model...
		String[] queryIDs=(String[])judgments.keySet().toArray(new String[judgments.keySet().size()]);

		// sort the set in alpha-numeric order
		java.util.Arrays.sort(queryIDs, new java.util.Comparator<String>() {
			public int compare(String a, String b) {
				try {
					Integer a1 = new Integer(a);
					Integer b1 = new Integer(b);
					return a1.compareTo(b1);
				} catch (NumberFormatException e) {
					// not an integer
					return a.compareTo(b);
				}}});
		queries.setEnabled(false);
		queries.removeAllItems();
		queries.addItem("-all-");
		for (String q : queryIDs)
			queries.addItem(q);
		queries.setEnabled(true);	
	}

	/**
	 * Builds the UI and shows it.
	 */
	private  void createAndShowGUI() {
		try {
			UIManager.setLookAndFeel( UIManager.getSystemLookAndFeelClassName() );
		} catch (Exception e) { }

		frame = new JFrame();
		frame.setLocation(30,30);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setTitle("IREval");
		JPanel panel = new JPanel();
		// Layout
		GridBagLayout gb = new GridBagLayout();
		GridBagConstraints gbc = new GridBagConstraints();
		panel.setLayout(gb);

		gbc.anchor = GridBagConstraints.WEST;
		gbc.insets = new Insets(5,5,5,5);
		gbc.weightx = 0;
		gbc.weighty = 1;
		gbc.gridwidth = 1;
		gbc.gridy = 0;
		JLabel lbl = new JLabel("Judgments");
		gb.setConstraints(lbl, gbc);
		panel.add(lbl);
		gbc.gridx = 1;
		gbc.gridwidth = 2;
		// needs a change listener so the field can be cleared...
		qrel = new JTextField(25);
		gb.setConstraints(qrel, gbc);
		panel.add(qrel);
		JButton b1 = new JButton("   load qrels");
		b1.setToolTipText("Open a file chooser to select a qrels file.");
		gbc.gridx = 3;
		gbc.gridwidth = GridBagConstraints.REMAINDER;
		gb.setConstraints(b1, gbc);
		panel.add(b1);
		b1.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String fname = chooseFile();
				qrel.setText(fname);
				if (!fname.equals(judgmentsFile)) {
					baselineEvaluator = null;
					judgments = null;
					treatmentsVector.clear();
					outputPane.setText(""); // output is voided
					queryID = "";
					queries.setVisible(false);
					rpButton.setVisible(false);
					pAtButton.setVisible(false);
					try {
						frame.setCursor(waitCursor);
						outputPane.setCursor(waitCursor);
						judgments = ireval.Main.loadJudgments(fname);
						judgmentsFile = fname;
						updateQueryIds();
						frame.setCursor(defaultCursor);
						outputPane.setCursor(defaultCursor);
					} catch (FileNotFoundException e1) {
						frame.setCursor(defaultCursor);
						outputPane.setCursor(defaultCursor);
						outputPane.setText(e1.getMessage());
					} catch (IOException e1) {
						frame.setCursor(defaultCursor);
						outputPane.setCursor(defaultCursor);
						outputPane.setText(e1.getMessage());
					}
				}
			}
		}
		);

		lbl = new JLabel("Results");
		gbc.gridy = 1;
		gbc.gridx = 0;
		gbc.gridwidth = 1;
		gb.setConstraints(lbl, gbc);
		panel.add(lbl);
		baselineResults = new JTextField(25);
		gbc.gridy = 1;
		gbc.gridx = 1;
		gbc.gridwidth = 2;
		gb.setConstraints(baselineResults, gbc);
		panel.add(baselineResults);
		JButton b2 = new JButton("load results");
		b2.setToolTipText("Open a file choose to select a result set.");
		gbc.gridy = 1;
		gbc.gridx = 3;
		gbc.gridwidth = GridBagConstraints.REMAINDER;
		gb.setConstraints(b2, gbc);
		panel.add(b2);
		b2.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String fname = chooseFile();
				baselineResults.setText(fname);
				if (!fname.equals(baselineRanksFile)) {
					baselineEvaluator = null; // evaluator is voided
					outputPane.setText(""); // output is voided
					queryID = "";
					queries.setVisible(false);
					rpButton.setVisible(false);
					pAtButton.setVisible(false);
					try {
						frame.setCursor(waitCursor);
						outputPane.setCursor(waitCursor);
						baselineRanks = ireval.Main.loadRanking(fname);
						baselineRanksFile = fname;
						frame.setCursor(defaultCursor);
						outputPane.setCursor(defaultCursor);
					} catch (FileNotFoundException e1) {
						frame.setCursor(defaultCursor);
						outputPane.setCursor(defaultCursor);
						outputPane.setText(e1.getMessage());
					} catch (IOException e1) {
						frame.setCursor(defaultCursor);
						outputPane.setCursor(defaultCursor);
						outputPane.setText(e1.getMessage());
					}
				}
			}

	}
		);
		treatmentModel = new DefaultListModel();
		treatments = new JList(treatmentModel);
		treatments.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		treatments.setVisibleRowCount(3);
		JScrollPane listScrollPane = new JScrollPane(treatments);
		listScrollPane.setPreferredSize(new Dimension(400, 60));
		

		lbl = new JLabel("Results");
		gbc.gridy = 2;
		gbc.gridx = 0;
		gbc.gridwidth = 1;
		gb.setConstraints(lbl, gbc);
		panel.add(lbl);
		gbc.gridy = 2;
		gbc.gridx = 1;
		gbc.gridwidth = 2;
		gbc.gridheight = 2;
		gb.setConstraints(listScrollPane, gbc);
		panel.add(listScrollPane);

		JButton b4 = new JButton("load results");
		b4.setToolTipText("Open a file chooser to select a result set.");
		gbc.gridy = 2;
		gbc.gridx = 3;
		gbc.gridwidth = GridBagConstraints.REMAINDER;
		gbc.gridheight = 1;
		gb.setConstraints(b4, gbc);
		panel.add(b4);
		b4.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				addTreatment();
			}
		}
		);

		JButton b5 = new JButton("remove selected");
		b4.setToolTipText("remove the selected element..");
		gbc.gridy = 3;
		gbc.gridx = 3;
		gbc.gridwidth = GridBagConstraints.REMAINDER;
		gbc.gridheight = 1;
		gb.setConstraints(b5, gbc);
		panel.add(b5);
		b5.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				removeTreatment();
			}
		}
		);

		JButton b3 = new JButton("evaluate results");
		b3.setToolTipText("Run a summary evaluation.");
		//b3.setEnabled(false); // have to interact with listeners on the text fields.
		gbc.gridy = 4;
		gbc.gridx = 0;
		gbc.gridwidth = 1;
		gb.setConstraints(b3, gbc);
		panel.add(b3);
		b3.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e) {
				frame.setCursor(waitCursor);
				outputPane.setCursor(waitCursor);
				// do we have to change the judgments?
				String fname = qrel.getText();
				if (!fname.equals(judgmentsFile)) {
					judgments = null;
					try {
						judgments = ireval.Main.loadJudgments(fname);
						judgmentsFile = fname;
						updateQueryIds();
					} catch (FileNotFoundException e1) {
						frame.setCursor(defaultCursor);
						outputPane.setCursor(defaultCursor);
						outputPane.setText(e1.getMessage());
						return;
					} catch (IOException e1) {
						frame.setCursor(defaultCursor);
						outputPane.setCursor(defaultCursor);
						outputPane.setText(e1.getMessage());
						return;
					}
				}
				
				// do we have to change the baseline results?
				fname = baselineResults.getText();
				if (!fname.equals(baselineRanksFile)) {
					baselineEvaluator = null; // evaluator is voided
					baselineRanks = null;
					try {
						baselineRanks = ireval.Main.loadRanking(fname);
						baselineRanksFile = fname;
					} catch (FileNotFoundException e1) {
						frame.setCursor(defaultCursor);
						outputPane.setCursor(defaultCursor);
						outputPane.setText(e1.getMessage());
						return;
					} catch (IOException e1) {
						frame.setCursor(defaultCursor);
						outputPane.setCursor(defaultCursor);
						outputPane.setText(e1.getMessage());
						return;
					}
				}
					// Still have to check, in case values were typed/pasted in.
				currentEvaluator = null;
				if (judgments == null) {
					// bad things here...
					outputPane.setText("No judgments!");
					frame.setCursor(defaultCursor);
					outputPane.setCursor(defaultCursor);
					return;
				}
				if (baselineRanks == null) {
					// bad things here
					outputPane.setText("No baseline result set!");
					frame.setCursor(defaultCursor);
					outputPane.setCursor(defaultCursor);
					return;
				}
				if (baselineEvaluator == null) {
					baselineEvaluator = ireval.Main.create(baselineRanks, judgments);
				}
				if (ranksList.size() == 0) {
					queries.setEnabled(false);
					queries.setSelectedIndex(0);
					queries.setEnabled(true);

					queries.setVisible(true);
					rpButton.setVisible(true); // have to do this other places
					pAtButton.setVisible(true);
					String evalOut = ireval.Main.singleEvaluation(baselineEvaluator, false);
					outputPane.setText(evalOut);
				} else {
					treatmentsVector.clear();
					for (int i = 0; i < ranksList.size(); i++)
						treatmentsVector.add(ireval.Main.create(ranksList.get(i), judgments));
					queries.setVisible(false);
					rpButton.setVisible(true); // have to do this other places
					pAtButton.setVisible(true);
					int idx = treatments.getSelectedIndex();
					if (idx == -1) idx = 0; // none selected, take first
					// or could do all pairwise.
					SetRetrievalEvaluator eval2 = treatmentsVector.get(idx);
					String treatment = new File((String)treatmentModel.getElementAt(idx)).getName();			
					String baselineName = new File(baselineResults.getText()).getName();

					String evalOut = ireval.Main.comparisonEvaluation(baselineEvaluator, eval2, baselineName, treatment );
					outputPane.setText(evalOut);
				}
				frame.setCursor(defaultCursor);
				outputPane.setCursor(defaultCursor);
			}
		}
		);
		queries = new JComboBox();
		queries.setToolTipText("Select an individual query to evaluate.");
		queries.setVisible(false);
		queries.setEnabled(false);
		gbc.gridy = 4;
		gbc.gridx = 1;
		gb.setConstraints(queries, gbc);
		panel.add(queries);
		queries.addActionListener(new ActionListener() {
			int oldItemIndex=-1;
			public void actionPerformed(ActionEvent e) {
				// modify the scores when changed...
				JComboBox cb=(JComboBox)e.getSource();
				if (!cb.isEnabled()) return;
				int newItemIndex=cb.getSelectedIndex();
				if (newItemIndex==oldItemIndex) { return; }
				queryID = (String) cb.getItemAt(newItemIndex);
				oldItemIndex=newItemIndex;
				// need to handle eval == null case
				frame.setCursor(waitCursor);
				outputPane.setCursor(waitCursor);
				if (queryID.equals("-all-")) {
					currentEvaluator = null;
					queryID = "";
					String evalOut = ireval.Main.singleEvaluation(baselineEvaluator, true);
					outputPane.setText(evalOut);
				} else {
					String s = singleQuery(queryID, baselineEvaluator);
					outputPane.setText(s);
				}
				frame.setCursor(defaultCursor);
				outputPane.setCursor(defaultCursor);
			}});

		rpButton = new JButton("R/P");
		rpButton.setToolTipText("Display the Interpolated Recall -- Precision graph.");
		gbc.gridy = 4;
		gbc.gridx = 2;
		gb.setConstraints(rpButton, gbc);
		rpButton.setVisible(false);
		panel.add(rpButton);
		rpButton.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e) {
				showPlotRP();
			}
		});

		pAtButton = new JButton("P@N");
		pAtButton.setToolTipText("Display the P@N histogram.");
		gbc.gridy = 4;
		gbc.gridx = 3;
		gbc.gridwidth = GridBagConstraints.REMAINDER;
		gb.setConstraints(pAtButton, gbc);
		pAtButton.setVisible(false);
		panel.add(pAtButton);
		pAtButton.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e) {
				showPlotPatN();
			}
		});

		outputPane = new JTextPane();
		outputPane.setFont(new Font("Monospaced", Font.PLAIN, 14));
		outputPane.setPreferredSize(new Dimension(600, 600));
		JScrollPane sp = new JScrollPane(outputPane);
		sp.setPreferredSize(new Dimension(600, 600));
		gbc.gridy = 5;
		gbc.gridx = 0;
		gbc.gridwidth = GridBagConstraints.REMAINDER;
		gb.setConstraints(sp, gbc);
		panel.add(sp);
		frame.getContentPane().setLayout(new BorderLayout());
		frame.getContentPane().add("Center", panel);
		frame.pack();
		frame.setVisible(true);
	}

	/** Creates the IREval GUI and starts it.
	 * @param args unused
	 */
	public static void main(String[] args) {
		//Schedule a job for the event-dispatching thread:
		//creating and showing this application's GUI.
		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				IREvalGui ir = new IREvalGui();
				ir.createAndShowGUI();
			}
		});
	}
}
