package ireval;

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
import java.awt.datatransfer.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.text.DecimalFormat;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;
import java.util.Vector;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.ListSelectionModel;
import javax.swing.TransferHandler;
import javax.swing.UIManager;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableColumn;

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
public class IREvalGui {
        /**
         * The main frame
         */
        private JFrame frame;

        private JTable resultsTable;
        private DefaultTableModel resultsTableModel;

        /**
         * Relevance judgments filename
         */
        private JTextField qrel;

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
        private JButton evaluateButton;

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
         * The collection of mappings of query ids to results for the
         * treatment sets.
         */
        private HashMap< String, TreeMap<String, ArrayList<Document>>> ranksMap;
        /**
         * The collection of evaluators for the results sets.
         */
        private HashMap<String, SetRetrievalEvaluator> treatmentsMap;

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
                judgments = null;
                currentEvaluator = null;
                ranksMap = new HashMap< String, TreeMap<String, ArrayList<Document>>>();
                treatmentsMap = new HashMap< String, SetRetrievalEvaluator>();
        }
        /**
         * Add a result set to the collection of treatments.
         * Load the rankings and install the mapping into the
         * ranksList.
         */
        private void addTreatment() {           
                Runnable r = new Runnable() {
                        public void run () {
                                Vector<String> treatments = chooseFiles();
                                if (treatments.size() == 0) {
                                        return;
                                }
                                String[] treat = new String[1];
                                frame.setCursor(waitCursor);
                                outputPane.setCursor(waitCursor);
                                for (String treatment : treatments) {
                                        try {
                                                // load the rankings
                                                ranksMap.put(treatment, ireval.Main.loadRanking(treatment) );
                                        } catch (Exception e){
                                                frame.setCursor(defaultCursor);
                                                outputPane.setCursor(defaultCursor);
                                                outputPane.setText("Unable to load: " + treatment + " " + e.toString());
                                                return;
                                        } catch (OutOfMemoryError error) {
                                                frame.setCursor(defaultCursor);
                                                outputPane.setCursor(defaultCursor);
                                                outputPane.setText("Out of memory error loading: " + treatment);
                                                return;         
                                        }
                                        // add it to the list
                                        treat[0] = treatment;
                                        resultsTableModel.addRow(treat);
                                }
                                rpButton.setEnabled(judgments != null);
                                pAtButton.setEnabled(judgments != null);
                                queries.setEnabled(judgments != null);  
                                evaluateButton.setEnabled(judgments != null);   

                                frame.setCursor(defaultCursor);
                                outputPane.setCursor(defaultCursor);
                        }
                };
                Thread t = new Thread(r);
                t.start();
        }

        /**
         * Remove a treatment set from the collection of treatments.
         */
        private void removeTreatment() {
                int[] rows = resultsTable.getSelectedRows();
                if (rows.length == 0) return; // none selected
                for (int idx : rows) {
                        String item = (String)resultsTable.getValueAt(idx, 0);
                        ranksMap.remove(item);
                        treatmentsMap.remove(item);
                        resultsTableModel.removeRow(idx);
                }
                if (ranksMap.size() == 0) {
                        rpButton.setEnabled(false);
                        pAtButton.setEnabled(false);
                        queries.setEnabled(false);
                        evaluateButton.setEnabled(false);
                }
        }

        /**
         * Display the Interpolated Recall/Precision graph.
         * If a single query is currently being evaluated,
         * plot only that query. Otherwise, plot the baseline
         * and each treatment for all queries.
         */
        private void showPlotRP(){
                double[] precs;
                double prec = 0.0;
                XYSeriesCollection dataset = new XYSeriesCollection();
                XYSeries series;
                int []rows = resultsTable.getSelectedRows();
                int idx;
                if (currentEvaluator != null) {
                        if (rows.length == 0) idx = 0; else idx = rows[0];
                        String baseline = new File((String)resultsTable.getValueAt(idx,0)).getName();
                        series = new XYSeries(baseline);
                        precs = currentEvaluator.interpolatedPrecision();
                        for (int i = 0; i < precs.length; i++) {
                                series.add( prec, precs[i]);
                                prec += 0.1;
                        }
                        dataset.addSeries(series);
                } else {
                        // do the treatments...
                        // need to take into account selected elements.
                        SetRetrievalEvaluator eval;
                        // rework this to use the map
                        if (rows.length == 0) {rows = new int[1]; rows[0] = 0;}
                        for ( int counter : rows) {
                                prec = 0.0;
                                String treatment = (String)resultsTableModel.getValueAt(counter,0);
                                eval = treatmentsMap.get(treatment);
                                precs = eval.interpolatedPrecision();
                                series = new XYSeries(basename(treatment));
                                for (int i = 0; i < precs.length; i++) {
                                        series.add( prec, precs[i]);
                                        prec += 0.1;
                                }
                                dataset.addSeries(series);
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
                String iconFile = "properties/icon.gif";
                ImageIcon lemurIcon = createImageIcon(iconFile);
                f.setIconImage(lemurIcon.getImage());
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
                int[] fixedPoints = SetRetrievalEvaluator.getFixedPoints();
                double [] precs;
                int []rows = resultsTable.getSelectedRows();
                int idx;

                if (currentEvaluator != null) {
                        if (rows.length == 0) idx = 0; else idx = rows[0];

                        String baseline = new File((String)resultsTable.getValueAt(idx,0)).getName();
                        // show for the current query only
                        precs = currentEvaluator.precisionAtFixedPoints();
                        for (int i = 0; i < precs.length; i++) {
                                series.addValue(precs[i], baseline, new Integer(fixedPoints[i]));
                        }

                } else {
                        // show for the baseline (and treatments)
                        if (rows.length == 0) {rows = new int[1]; rows[0] = 0;}
                        SetRetrievalEvaluator eval;
                        for ( int counter : rows) {
                                String treatment = (String)resultsTableModel.getValueAt(counter,0);
                                eval = treatmentsMap.get(treatment);
                                precs = eval.precisionAtFixedPoints();
                                for (int i = 0; i < precs.length; i++) {
                                        series.addValue(precs[i], basename(treatment), new Integer(fixedPoints[i]));
                                }
                        }
                }
                String label = "Precision at N";
                if (queryID.length() > 0) label += " query:" + queryID;
                JFreeChart chart = ChartFactory.createBarChart(label,
                                "N", "precision", series, PlotOrientation.VERTICAL, true, true, false);
                JFrame f = new JFrame("Precision @ N");
                String iconFile = "properties/icon.gif";
                ImageIcon lemurIcon = createImageIcon(iconFile);
                f.setIconImage(lemurIcon.getImage());
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
        private String singleQuery( String query, SetRetrievalEvaluator setEvaluator ) {
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
                fileChooser.setMultiSelectionEnabled(false);
                if (fileChooser.showOpenDialog(frame) == JFileChooser.APPROVE_OPTION) {
                        File file = fileChooser.getSelectedFile();
                        workingDir = file.getPath();
                        return file.getAbsolutePath();
                } else {
                        return "";
                }
        }

        private Vector<String> chooseFiles() {
                fileChooser.setCurrentDirectory(new File(workingDir));
                fileChooser.setMultiSelectionEnabled(true);
                Vector<String> names = new Vector<String>();
                if (fileChooser.showOpenDialog(frame) == JFileChooser.APPROVE_OPTION) {
                        File[] files = fileChooser.getSelectedFiles();
                        for (File file : files ) {
                                workingDir = file.getPath();
                                names.add(file.getAbsolutePath());
                        }
                } 
                return names;
        }
        /**
         * Updates the contents of the query id drop down list.
         */
        private void updateQueryIds() {
                // update the combo box;
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
        }

        private void loadQrel() {
                String fname = chooseFile();
                if (!fname.equals(judgmentsFile)) {
                        baselineEvaluator = null;
                        judgments = null;
                        outputPane.setText(""); // output is voided
                        queryID = "";
                        queries.setEnabled(false);
                        rpButton.setEnabled(false);
                        pAtButton.setEnabled(false);
                        evaluateButton.setEnabled(false);
                        try {
                                frame.setCursor(waitCursor);
                                outputPane.setCursor(waitCursor);
                                judgments = ireval.Main.loadJudgments(fname);
                                qrel.setText(fname);
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
                        } catch (Exception e2) {
                                frame.setCursor(defaultCursor);
                                outputPane.setCursor(defaultCursor);
                                outputPane.setText("Unable to load: " + fname + " " + e2.toString());           
                        }
                        queries.setEnabled(ranksMap.size() > 0);
                        rpButton.setEnabled(ranksMap.size() > 0);
                        pAtButton.setEnabled(ranksMap.size() > 0);
                        evaluateButton.setEnabled(ranksMap.size() > 0);

                }
        }

        private String basename(String fname) {
                return new File(fname).getName();
        }

        private void doEvaluate() {
                frame.setCursor(waitCursor);
                outputPane.setCursor(waitCursor);
                // do we have to change the judgments?
                String fname = qrel.getText();
                if (!fname.equals(judgmentsFile)) {
                        judgments = null;
                        baselineEvaluator = null; // evaluator is voided
                        treatmentsMap.clear(); // all evaluators are voided.
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
                if (judgments == null) {
                        // bad things here...
                        outputPane.setText("No judgments!");
                        frame.setCursor(defaultCursor);
                        outputPane.setCursor(defaultCursor);
                        return;
                }
                currentEvaluator = null;
                queryID = "";
                if (treatmentsMap.size() != ranksMap.size()) {
                        treatmentsMap.clear();
                        for (String treat : ranksMap.keySet()) {
                                treatmentsMap.put(treat, ireval.Main.create(ranksMap.get(treat), judgments));
                        }               
                }

                if (ranksMap.size() == 1) {
                        queries.setEnabled(false);
                        queries.setSelectedIndex(0);
                        queries.setEnabled(true);
                        rpButton.setEnabled(true);
                        pAtButton.setEnabled(true);
                        // do we have to change the baseline results?
                        fname = (String) resultsTable.getValueAt(0,0);
                        if (!fname.equals(baselineRanksFile)) {
                                baselineEvaluator = null; // evaluator is voided
                                baselineRanksFile = fname;
                        }

                        if (baselineEvaluator == null) {
                                baselineEvaluator = treatmentsMap.get(baselineRanksFile);
                        }
                        String evalOut = ireval.Main.singleEvaluation(baselineEvaluator, false);
                        outputPane.setText(evalOut);
                        outputPane.setCaretPosition(0);
                } else {
                        queries.setEnabled(false);
                        queries.setSelectedIndex(0);
                        rpButton.setEnabled(true);
                        pAtButton.setEnabled(true);
                        SetRetrievalEvaluator eval2 ;
                        String treatment ;                      
                        String baselineName;

                        int idx = resultsTable.getSelectedRow();
                        int []rows = resultsTable.getSelectedRows();
                        if (rows.length == 0 || rows.length == 1) {
                                if (idx == -1) idx = 0; // none selected, take first
                                // otherwise, treat the single selection as the baseline.
                                baselineName = (String)resultsTable.getValueAt(idx,0);
                                baselineEvaluator = treatmentsMap.get(baselineName);
                                queries.setEnabled(true);

                                String evalOut = ireval.Main.singleEvaluation(baselineEvaluator, false );
                                outputPane.setText(evalOut);
                                outputPane.setCaretPosition(0);
                        } else {
                                // if there are more than two, do pairwise, treating first
                                // as the baseline.
                                baselineName = (String)resultsTable.getValueAt(rows[0],0);
                                baselineEvaluator = treatmentsMap.get(baselineName);
                                StringBuffer evalOut = new StringBuffer();
                                for (int i = 1; i < rows.length; i++) {
                                        idx = rows[i];
                                        treatment = (String)resultsTable.getValueAt(idx,0);                     
                                        eval2 = treatmentsMap.get(treatment);

                                        evalOut.append(comparisonEvaluation(baselineEvaluator, eval2, basename(baselineName), basename(treatment)));
                                        evalOut.append("\n");
                                }
                                outputPane.setText(evalOut.toString());
                                outputPane.setCaretPosition(0);

                        }
                }
                frame.setCursor(defaultCursor);
                outputPane.setCursor(defaultCursor);
        }

        /**
         * Builds the UI and shows it.
         */
        private  void createAndShowGUI() {
                try {
                        UIManager.setLookAndFeel( UIManager.getSystemLookAndFeelClassName() );
                } catch (Exception e) { }
                String logoFile = "properties/lemur_logo.gif";
                String iconFile = "properties/icon.gif";
                ImageIcon lemurIcon = createImageIcon(iconFile);

                frame = new JFrame();
                frame.setLocation(30,30);
                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                frame.setIconImage(lemurIcon.getImage());
                frame.setTitle("IREval");
                JPanel panel = new JPanel();

                ImageIcon logo = createImageIcon(logoFile);
                JLabel logoLabel = new JLabel(logo, JLabel.CENTER);
                // Layout
                GridBagLayout gb = new GridBagLayout();
                GridBagConstraints gbc = new GridBagConstraints();
                panel.setLayout(gb);

                gbc.anchor = GridBagConstraints.EAST;
                gbc.insets = new Insets(5,5,5,5);
                gbc.weightx = 0;
                gbc.weighty = 0;
                gbc.gridwidth = 1;
                gbc.gridy = 0;
                JLabel lbl = new JLabel("Judgments");
                gb.setConstraints(lbl, gbc);
                panel.add(lbl);
                gbc.anchor = GridBagConstraints.WEST;

                gbc.gridx = 1;
                gbc.gridwidth = 2;
                qrel = new JTextField(32);
                qrel.setMinimumSize(qrel.getPreferredSize());
                qrel.setEditable(false);
                gb.setConstraints(qrel, gbc);
                panel.add(qrel);
                JButton b1 = new JButton("   Load Qrels");
                b1.setToolTipText("Open a file chooser to select a qrels file.");
                gbc.gridx = 3;
                gbc.gridwidth = GridBagConstraints.REMAINDER;
                gb.setConstraints(b1, gbc);
                panel.add(b1);
                b1.addActionListener(new ActionListener() {
                        public void actionPerformed(ActionEvent e) {
                                loadQrel();
                        }});

                lbl = new JLabel("Baseline");
                gbc.gridy = 1;
                gbc.gridx = 0;
                gbc.gridwidth = 1;
                gbc.anchor = GridBagConstraints.EAST;
                gb.setConstraints(lbl, gbc);
                panel.add(lbl);
                String [] colNames = {"Result Set"};
                resultsTableModel = new DefaultTableModel(colNames,0);
                resultsTable = new JTable(resultsTableModel);
                resultsTable.setDragEnabled(true);
                resultsTable.setTransferHandler(new TableTransferHandler());
                //resultsTable.setFillsViewportHeight(true); // jdk 1.6
                resultsTable.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
                resultsTable.getSelectionModel().addListSelectionListener(new ListSelectionListener() {
                        public void  valueChanged(ListSelectionEvent event){
                                if (event.getValueIsAdjusting()) {
                                        return;
                                }
                                int []rows = resultsTable.getSelectedRows();
                                // ought to check that it can be enabled.
                                if (rows.length == 1) {
                                        queries.setEnabled(false);
                                        queries.setSelectedIndex(0);
                                        queries.setEnabled(true);

                                } else {
                                        queries.setEnabled(false);
                                }
                        }
                });
                resultsTable.getTableHeader().setToolTipText("<html>Select one element for a single evaluation. Select a two or more elements for a pairwise evaluation.<br>When multiple elements are selected, the first in the listing is treated as the baseline.<br>Result sets can be reordered by dragging.</html>");

                DefaultTableCellRenderer renderer =
                        new DefaultTableCellRenderer();

                renderer.setToolTipText("<html>Select one element for a single evaluation. Select a two or more elements for a pairwise evaluation.<br>When multiple elements are selected, the first in the listing is treated as the baseline.<br>Result sets can be reordered by dragging.</html>");
                TableColumn column = resultsTable.getColumnModel().getColumn(0);
                column.setCellRenderer(renderer);
                JScrollPane listScrollPane = new JScrollPane(resultsTable);
                listScrollPane.setPreferredSize(new Dimension(400, 100));
                listScrollPane.setMinimumSize(new Dimension(400, 100));
                lbl = new JLabel("Results");
                gbc.gridy = 2;
                gbc.gridx = 0;
                gbc.gridwidth = 1;
                gbc.anchor = GridBagConstraints.EAST;

                gb.setConstraints(lbl, gbc);
                panel.add(lbl);
                gbc.gridy = 1;
                gbc.gridx = 1;
                gbc.gridwidth = 2;
                gbc.gridheight = 2;
                gbc.anchor = GridBagConstraints.WEST;
                gb.setConstraints(listScrollPane, gbc);
                panel.add(listScrollPane);

                JButton b4 = new JButton("Load Results");
                b4.setToolTipText("Open a file chooser to select one or more result sets.");
                gbc.gridy = 1;
                gbc.gridx = 3;
                gbc.gridwidth = GridBagConstraints.REMAINDER;
                gbc.gridheight = 1;
                gb.setConstraints(b4, gbc);
                panel.add(b4);
                b4.addActionListener(new ActionListener() {
                        public void actionPerformed(ActionEvent e) {
                                addTreatment();
                        }});

                JButton b5 = new JButton("Remove Results");
                b5.setToolTipText("Remove the selected result sets..");
                gbc.gridy = 2;
                gbc.gridx = 3;
                gbc.gridwidth = GridBagConstraints.REMAINDER;
                gbc.gridheight = 1;
                gb.setConstraints(b5, gbc);
                panel.add(b5);
                b5.addActionListener(new ActionListener() {
                        public void actionPerformed(ActionEvent e) {
                                removeTreatment();
                        }});
                JPanel pnl = new JPanel();
                GridBagLayout gb1 = new GridBagLayout();
                GridBagConstraints gbc1 = new GridBagConstraints();
                pnl.setLayout(gb1);
                evaluateButton = new JButton("Evaluate Result Sets");
                evaluateButton.setToolTipText("Run a summary evaluation.");
                evaluateButton.setEnabled(false);
                gbc.gridy = 4;
                gbc.gridx = 1;
                gbc.gridwidth = GridBagConstraints.REMAINDER;
                gb.setConstraints(pnl, gbc);

                panel.add(pnl);
                gbc1.gridy = 0;
                gbc1.gridx = 0;
                gbc1.gridwidth = 1;
                gb1.setConstraints(evaluateButton, gbc1);
                pnl.add(evaluateButton);
                evaluateButton.addActionListener(new ActionListener(){
                        public void actionPerformed(ActionEvent e) {
                                doEvaluate();
                        }});
                queries = new JComboBox();
                queries.setToolTipText("Select an individual query to evaluate.");
                queries.setEnabled(false);
                gbc1.gridy = 0;
                gbc1.gridx = 3;
                gbc1.gridwidth = 1;
                gb1.setConstraints(queries, gbc1);

                pnl.add(queries);
                queries.addActionListener(new ActionListener() {
                        public void actionPerformed(ActionEvent e) {
                                JComboBox cb=(JComboBox)e.getSource();
                                if (!cb.isEnabled()) return;
                                int newItemIndex=cb.getSelectedIndex();
                                queryID = (String) cb.getItemAt(newItemIndex);
                                frame.setCursor(waitCursor);
                                outputPane.setCursor(waitCursor);
                                if (treatmentsMap.size() != ranksMap.size()) {
                                        treatmentsMap.clear();
                                        for (String treat : ranksMap.keySet()) {
                                                treatmentsMap.put(treat, ireval.Main.create(ranksMap.get(treat), judgments));
                                        }               
                                }
                                // do we have to change the baseline results?
                                int row = resultsTable.getSelectedRow();
                                if (row == -1) row = 0;
                                String fname = (String) resultsTable.getValueAt(row,0);
                                if (!fname.equals(baselineRanksFile)) {
                                        baselineEvaluator = null; // evaluator is voided
                                        baselineRanksFile = fname;
                                }

                                if (baselineEvaluator == null) {
                                        baselineEvaluator = treatmentsMap.get(baselineRanksFile);
                                }
                                if (queryID.equals("-all-")) {
                                        currentEvaluator = null;
                                        queryID = "";
                                        String evalOut = ireval.Main.singleEvaluation(baselineEvaluator, true);
                                        outputPane.setText(evalOut);

                                } else {
                                        String s = singleQuery(queryID, baselineEvaluator);
                                        outputPane.setText(s);
                                }
                                outputPane.setCaretPosition(0);
                                frame.setCursor(defaultCursor);
                                outputPane.setCursor(defaultCursor);
                        }});

                rpButton = new JButton("R/P");
                rpButton.setToolTipText("Display the Interpolated Recall -- Precision graph.");
                rpButton.setEnabled(false);
                gbc1.gridy = 0;
                gbc1.gridx = 1;
                gbc.gridwidth = 1;
                gb1.setConstraints(rpButton, gbc1);

                pnl.add(rpButton);
                rpButton.addActionListener(new ActionListener(){
                        public void actionPerformed(ActionEvent e) {
                                if (treatmentsMap.size() != ranksMap.size()) {
                                        treatmentsMap.clear();
                                        for (String treat : ranksMap.keySet()) {
                                                treatmentsMap.put(treat, ireval.Main.create(ranksMap.get(treat), judgments));
                                        }               
                                }
                                showPlotRP();
                        }});

                pAtButton = new JButton("P@N");
                pAtButton.setToolTipText("Display the P@N histogram.");
                pAtButton.setEnabled(false);
                gbc1.gridy = 0;
                gbc1.gridx = 2;
                gb1.setConstraints(pAtButton, gbc1);

                pnl.add(pAtButton);
                pAtButton.addActionListener(new ActionListener(){
                        public void actionPerformed(ActionEvent e) {
                                if (treatmentsMap.size() != ranksMap.size()) {
                                        treatmentsMap.clear();
                                        for (String treat : ranksMap.keySet()) {
                                                treatmentsMap.put(treat, ireval.Main.create(ranksMap.get(treat), judgments));
                                        }               
                                }
                                showPlotPatN();
                        }});

                outputPane = new JTextPane();
                outputPane.setEditable(false);
                outputPane.setFont(new Font("Monospaced", Font.PLAIN, 14));
                outputPane.setPreferredSize(new Dimension(600, 600));
                outputPane.setMinimumSize(new Dimension(400, 400));
                JScrollPane sp = new JScrollPane(outputPane);
                sp.setPreferredSize(new Dimension(600, 600));
                sp.setMinimumSize(new Dimension(400, 400));
                gbc.gridy = 5;
                gbc.gridx = 0;
                gbc.gridwidth = GridBagConstraints.REMAINDER;
                gbc.fill = GridBagConstraints.HORIZONTAL;
                gb.setConstraints(sp, gbc);
                panel.add(sp);
                gbc.gridy = 6;
                gbc.fill = GridBagConstraints.NONE;
                gb.setConstraints(logoLabel, gbc);
                panel.add(logoLabel);

                frame.getContentPane().setLayout(new BorderLayout());
                frame.getContentPane().add("Center", panel);
                frame.pack();
                frame.setVisible(true);
        }

        /** Returns an ImageIcon, or null if the path was invalid.
        @param path the image file to load.
        @return an ImageIcon, or null if the path was invalid.
         */
        protected static ImageIcon createImageIcon(String path) {
                java.net.URL imgURL = ireval.Main.class.getResource(path);
                if (imgURL != null) {
                        return new ImageIcon(imgURL);
                } else {
                        return null;
                }
        }


        private String comparisonEvaluation( SetRetrievalEvaluator baseline, SetRetrievalEvaluator treatment, 
                                String baselineName, String treatmentName ) {

                StringWriter s = new StringWriter();
                PrintWriter out = new PrintWriter(s);
                String[] metrics = { "averagePrecision", "rPrecision", "ndcg", "bpref", "P5", "P10", "P20" };
                String[] metricLabels = { "Average Precision", "R-Precision", "NDCG", "bpref", "P@5", "P@10", "P@20" };

                String formatString = "%1$-30s%2$6.4f\n";
                String integerFormatString = "%1$-30s%2$6d\n";
                out.println("Comparing baseline: " + baselineName + " to treatment: " + treatmentName + "\n");
                if (treatment == null) return "NOPE";
                int i = 0;
                for( String metric : metrics ) {
                        Map<String, Double> baselineMetric = baseline.evaluateAll( metric );
                        Map<String, Double> treatmentMetric = treatment.evaluateAll( metric );

                        SetRetrievalComparator comparator = new SetRetrievalComparator( baselineMetric, treatmentMetric );
                        out.println("\t\t" + metricLabels[i]);
                        out.format( formatString, baselineName, comparator.meanBaselineMetric() );
                        out.format( formatString, treatmentName, comparator.meanTreatmentMetric() );

                        out.format( integerFormatString, baselineName + " better", comparator.countBaselineBetter() );
                        out.format( integerFormatString, treatmentName + " better", comparator.countTreatmentBetter() );
                        out.format( integerFormatString, "No difference", comparator.countEqual() );

                        out.format( formatString, "t-test", comparator.pairedTTest() );
                        out.format( formatString, "randomized", comparator.randomizedTest() );
                        out.format( formatString, "sign test", comparator.signTest() );
                        out.println("____________________________________");
                        i++;
                }

                return s.toString();
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

abstract class StringTransferHandler extends TransferHandler {

        protected abstract String exportString(JComponent c);

        protected abstract void importString(JComponent c, String str);

        protected abstract void cleanup(JComponent c, boolean remove);

        protected Transferable createTransferable(JComponent c) {
                return new StringSelection(exportString(c));
        }

        public int getSourceActions(JComponent c) {
                return COPY_OR_MOVE;
        }

        public boolean importData(JComponent c, Transferable t) {
                if (canImport(c, t.getTransferDataFlavors())) {
                        try {
                                String str = (String) t.getTransferData(DataFlavor.stringFlavor);
                                importString(c, str);
                                return true;
                        } catch (UnsupportedFlavorException ufe) {
                        } catch (IOException ioe) {
                        }
                }

                return false;
        }

        protected void exportDone(JComponent c, Transferable data, int action) {
                cleanup(c, action == MOVE);
        }

        public boolean canImport(JComponent c, DataFlavor[] flavors) {
                for (int i = 0; i < flavors.length; i++) {
                        if (DataFlavor.stringFlavor.equals(flavors[i])) {
                                return true;
                        }
                }
                return false;
        }
}


class TableTransferHandler extends StringTransferHandler {
        private static final long serialVersionUID = 1L;

        private int[] rows = null;

        private int addIndex = -1; //Location where items were added

        private int addCount = 0; //Number of items added.

        protected String exportString(JComponent c) {
                JTable table = (JTable) c;
                rows = table.getSelectedRows();
                int colCount = table.getColumnCount();

                StringBuffer buff = new StringBuffer();

                for (int i = 0; i < rows.length; i++) {
                        for (int j = 0; j < colCount; j++) {
                                Object val = table.getValueAt(rows[i], j);
                                buff.append(val == null ? "" : val.toString());
                                if (j != colCount - 1) {
                                        buff.append(",");
                                }
                        }
                        if (i != rows.length - 1) {
                                buff.append("\n");
                        }
                }

                return buff.toString();
        }

        protected void importString(JComponent c, String str) {
                JTable target = (JTable) c;
                DefaultTableModel model = (DefaultTableModel) target.getModel();
                int index = target.getSelectedRow();

                //Prevent the user from dropping data back on itself.
                //For example, if the user is moving rows #4,#5,#6 and #7 and
                //attempts to insert the rows after row #5, this would
                //be problematic when removing the original rows.
                //So this is not allowed.
                if (rows != null && index >= rows[0] - 1
                                && index <= rows[rows.length - 1]) {
                        rows = null;
                        return;
                }

                int max = model.getRowCount();
                if (index < 0) {
                        index = max;
                } else {
                        index++;
                        if (index > max) {
                                index = max;
                        }
                }
                addIndex = index;
                String[] values = str.split("\n");
                addCount = values.length;
                int colCount = target.getColumnCount();
                for (int i = 0; i < values.length && i < colCount; i++) {
                        model.insertRow(index++, values[i].split(","));
                }
        }

        protected void cleanup(JComponent c, boolean remove) {
                JTable source = (JTable) c;
                if (remove && rows != null) {
                        DefaultTableModel model = (DefaultTableModel) source.getModel();

                        //If we are moving items around in the same table, we
                        //need to adjust the rows accordingly, since those
                        //after the insertion point have moved.
                        if (addCount > 0) {
                                for (int i = 0; i < rows.length; i++) {
                                        if (rows[i] > addIndex) {
                                                rows[i] += addCount;
                                        }
                                }
                        }
                        for (int i = rows.length - 1; i >= 0; i--) {
                                model.removeRow(rows[i]);
                        }
                }
                rows = null;
                addCount = 0;
                addIndex = -1;
        }
}
