/*==========================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software (and below), and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
 */
package lemurproject.lemur.ui;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;

import java.io.*;
import java.util.*;
import java.lang.*;

import lemurproject.lemur.*;

public class LemurRet extends JPanel {

    private final static String SLASH = System.getProperty("file.separator");
    private final static String optsFile = "LemurGUI.opts";
    private final static String INQUERY = "InQuery (structured queries)";
    private final static String OKAPI = "Okapi";
    private final static String KLDIV = "KL-divergence (LM)";
    private final static String INDRI = "Indri Query Language";
    //  private final static String MYINDEX = "index.key";
    private final static String NODOCUMENTS = "<html>No documents found.</html>";
    private final static String ICONSTRING = "properties/icon.gif";

    JFrame parent;
    JDialog setDialog;
    JComboBox indexBox;
    JTextField queryField;

    String workingDir;
    Color relColor, novelColor;

    JPanel resultsPane;
    JEditorPane resultsDisplay;
    JLabel status;
    //  Vector queryResults = new Vector();
    IndexedReal[] queryResults;
    
    
    // keep track of which index we just searched (combo box might change)
    String searchIndex, searchQuery;

    // Paging
    int firstshown = 1;
    int lastshown;
    int maxresults = -1;

    // for settings dialog
    JDialog settingsDialog;
    JCheckBox loadCheck, openCheck, hlCheck, markupCheck, appendCheck;
    JComboBox retriBox;
    JPanel paramsPane;
    JTextField numresultsF;
    JTextField beliefF, fbtermsF, fbcoefF;
    JTextField bm25k1F, bm25bF, bm25k3F, bm25qtfF, feedbackF;

    // Settings defaults
    GuiSettings guiSettings = new GuiSettings();

    // showing documents
    JDialog resdocDialog;
    JTextPane resdocTPane;
    JButton prevBtn;
    JButton nextBtn;


    // JNI api elements.
    Index index;
    ArrayAccumulator accum = null;
    RetrievalMethod model = null;
    
    public LemurRet(JFrame f) {
        super(true);
        parent = f;
        queryResults = new IndexedReal[0];

        relColor = new Color(235, 255, 255);
        novelColor = new Color(255, 235, 235);
        workingDir = System.getProperty("user.dir");

        //load in previously used indexes
        initIndexTable();

        JPanel menus = new JPanel();
        GridBagLayout gb = new GridBagLayout();
        GridBagConstraints gbc = new GridBagConstraints();
        menus.setLayout(gb);
        setLayout(new BorderLayout());
        setBackground(Color.darkGray);
        setPreferredSize(new Dimension(600, 650));

        //        JMenuBar menubar = createMenu();
        gbc.weightx = 1.0;
        gbc.weighty = 1.0;
        gbc.anchor = GridBagConstraints.NORTHWEST;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        //        gb.setConstraints(menubar, gbc);
        //        menus.add(menubar);

        JPanel queryPanel = createQueryPanel();
        gb.setConstraints(queryPanel, gbc);
        menus.add(queryPanel);

        add("North", menus);
        resultsPane = makeResultsPanel();
        gbc.weighty = 1.0;
        gbc.fill = GridBagConstraints.BOTH;
        gb.setConstraints(resultsPane, gbc);
        add("Center", resultsPane);
        resultsPane.setVisible(false);


        guiSettings.openwin = false;
        guiSettings.highlight = true;
        guiSettings.showmarkup = true;
        guiSettings.appendfile = false;
        guiSettings.retmethod = KLDIV;
        guiSettings.numresults = 10;
        // InQuery ret parms
        guiSettings.belief = 0.4;
        guiSettings.fbterms = 50;
        guiSettings.fbcoef = 0.5;
        // Okapi parms
        guiSettings.bm25k1 = 1.2;
        guiSettings.bm25b = 0.75;
        guiSettings.bm25k3 = 7;
        guiSettings.bm25qtf = 0.5;
        guiSettings.feedback = 50;

        settingsDialog = createSettingsDialog();
        settingsDialog.setVisible(false); // jdk 1.5 replaces .show()


        FileInputStream fis = null;
        ObjectInputStream in = null;
        try
            {
                fis = new FileInputStream(optsFile);
                in = new ObjectInputStream(fis);
                guiSettings = (GuiSettings)in.readObject();
                in.close();
            }
        catch(FileNotFoundException ex)
            {
                // ok, will create new file and use defaults
            }
        catch(IOException ex)
            {
                ex.printStackTrace();
            }
        catch(ClassNotFoundException ex)
            {
                ex.printStackTrace();
            }


        resetParamValues();

        resdocTPane = new JTextPane();
        resdocDialog = createDocDisplay(resdocTPane);
        resdocDialog.setVisible(false);
    }

    /****************** Create Panels *********************/
    private JMenuBar createMenu() {
        JMenuBar menu = new JMenuBar();
        JMenu progMenu = new JMenu("Program");
        progMenu.setMnemonic('P');
        JMenu indexMenu = new JMenu("Indexes");
        indexMenu.setMnemonic('I');
        JMenu help = new JMenu("Help");
        help.setMnemonic('H');

        JMenuItem progsett = new JMenuItem("Options");
        progsett.setMnemonic('O');
        progsett.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    settingsDialog.setVisible(true); // jdk 1.5 replaces .show()
                }
            });


        JMenuItem progexp = new JMenuItem("Export results");
        progexp.setMnemonic('E');
        progexp.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    String nr = status.getText();
                    if (!resultsPane.isVisible() || nr.startsWith(NODOCUMENTS))
                        errorMsg(parent, "No results to export.");
                    else
                        exportResultsAction();
                }
            });

        JMenuItem progclear = new JMenuItem("Clear results");
        progclear.setMnemonic('C');
        progclear.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    resultsPane.setVisible(false);
                }
            });

        JMenuItem progexit = new JMenuItem("Exit");
        progexit.setMnemonic('X');
        progexit.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    System.exit(0);
                }
            });

        progMenu.add(progsett);
        progMenu.add(progexp);
        progMenu.add(progclear);
        progMenu.add( new JSeparator() );
        progMenu.add(progexit);
        /*
          JMenuItem indexadd = new JMenuItem("Load Index");
          indexadd.setMnemonic('L');
          indexadd.addActionListener( new ActionListener() {
          public void actionPerformed(ActionEvent e) {
          }
          });

          JMenuItem indexrm = new JMenuItem("Remove Index");
          indexrm.setMnemonic('R');
          indexrm.addActionListener( new ActionListener() {
          public void actionPerformed(ActionEvent e) {
          }
          });

          indexMenu.add(indexadd);
          indexMenu.add(indexrm);
        */
        JMenuItem helpabout = new JMenuItem("About");
        helpabout.setMnemonic('A');
        helpabout.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    JDialog mydialog = createAboutDialog();
                    mydialog.setSize(new Dimension(300,400));
                    mydialog.setVisible(true);// jdk 1.5 replaces .show()
                }
            });

        JMenuItem helpman = new JMenuItem("Help");
        helpman.setEnabled(false);
        helpman.setMnemonic('H');
        helpman.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                }
            });

        help.add(helpman);
        help.add(new JSeparator());
        help.add(helpabout);

        menu.add(progMenu);
        //    menu.add(indexMenu);
        menu.add(help);
        return menu;
    }

    private JDialog createSettingsDialog() {
        JDialog setDialog = new JDialog(parent, "Lemur GUI program options");
        setDialog.getContentPane().setLayout(new BorderLayout(0, 10));
        setDialog.setSize(new Dimension(320,500));
        //    setDialog.setResizable(false);

        JPanel top = new JPanel();
        top.setLayout(new GridLayout(5,1));
        top.setPreferredSize(new Dimension(300,145));
        top.setBorder(BorderFactory.createTitledBorder("General"));
        loadCheck = new JCheckBox("Load current indexes on next program start");
        top.add(loadCheck);
        loadCheck.setEnabled(false);
        openCheck = new JCheckBox("Open each document in its own window");
        top.add(openCheck);
        hlCheck = new JCheckBox("Highlight query terms in documents");
        hlCheck.setSelected(true);
        top.add(hlCheck);
        markupCheck = new JCheckBox("Show mark up tags in documents ");
        markupCheck.setSelected(true);
        top.add(markupCheck);
        appendCheck = new JCheckBox("When exporting results, append to file");
        appendCheck.setSelected(false);
        top.add(appendCheck);

        JPanel bottom = new JPanel();
        bottom.setLayout(new FlowLayout(FlowLayout.LEFT));
        bottom.setPreferredSize(new Dimension(300,260));
        bottom.setBorder(BorderFactory.createTitledBorder("Retrieval"));
        JLabel name = new JLabel("Results per page:");
        bottom.add(name);
        numresultsF = new JTextField(String.valueOf(guiSettings.numresults),3);
        numresultsF.setHorizontalAlignment(JTextField.RIGHT);
        bottom.add(numresultsF);

        JLabel retl = new JLabel("Search using retrieval method:");
        bottom.add(retl);
        retriBox = new JComboBox();
        retriBox.addItem(INQUERY);
        retriBox.addItem(OKAPI);
        retriBox.addItem(KLDIV);
        retriBox.addItem(INDRI);
        retriBox.addItemListener( new ItemListener() {
                public void itemStateChanged(ItemEvent evt) {
                    ((CardLayout)paramsPane.getLayout()).show(paramsPane, (String)evt.getItem());
                }
            });
        bottom.add(retriBox);
        name = new JLabel("ADVANCED PARAMETERS");
        bottom.add(name);

        paramsPane = new JPanel();
        paramsPane.setLayout(new CardLayout());
        paramsPane.add(createInqPanel(),INQUERY);
        paramsPane.add(createKLPanel(),KLDIV);
        paramsPane.add(createOkapiPanel(), OKAPI);
        paramsPane.add(createIndriPanel(), INDRI);
        bottom.add(paramsPane);

        JPanel general = new JPanel();
        general.setSize(new Dimension(300,405));
        general.add(top);
        general.add(bottom);

        // make ok/cancel
        JButton okBtn, cancelBtn;
        okBtn = new JButton("OK");
        okBtn.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    setParamValues();
                    settingsDialog.setVisible(false);
                }
            });

        cancelBtn = new JButton("Cancel");
        cancelBtn.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    resetParamValues();
                    settingsDialog.setVisible(false);
                }
            });

        Box bottomPane = new Box(BoxLayout.X_AXIS);
        bottomPane.add(bottomPane.createHorizontalGlue());
        bottomPane.add(okBtn);
        bottomPane.add(bottomPane.createRigidArea(new Dimension(10, 0)));
        bottomPane.add(cancelBtn);
        bottomPane.add(bottomPane.createRigidArea(new Dimension(10, 50)));

        setDialog.getContentPane().add("Center", general);
        setDialog.getContentPane().add("South", bottomPane);

        return setDialog;
    }

    private JPanel createInqPanel() {
        JPanel inq = new JPanel();
        inq.setPreferredSize(new Dimension(250,75));
        inq.setLayout(new FlowLayout(FlowLayout.LEFT));

        JLabel lbl = new JLabel("Default document belief:");
        inq.add(lbl);
        beliefF = new JTextField(String.valueOf(guiSettings.belief),3);
        beliefF.setHorizontalAlignment(JTextField.RIGHT);
        inq.add(beliefF);

        lbl = new JLabel("Rocchio feedback coefficient:");
        inq.add(lbl);
        fbcoefF = new JTextField(String.valueOf(guiSettings.fbcoef),3);
        fbcoefF.setHorizontalAlignment(JTextField.RIGHT);
        inq.add(fbcoefF);

        lbl = new JLabel("Number of feedback terms:");
        inq.add(lbl);
        fbtermsF = new JTextField(String.valueOf(guiSettings.fbterms),3);
        fbtermsF.setHorizontalAlignment(JTextField.RIGHT);
        inq.add(fbtermsF);

        return inq;
    }

    private JPanel createKLPanel() {
        JPanel kl = new JPanel();
        kl.setPreferredSize(new Dimension(250,75));
        kl.setLayout(new FlowLayout(FlowLayout.LEFT));

        JLabel lbl = new JLabel("KL using default parameters:");
        kl.add(lbl);
        lbl = new JLabel(" Dirichlet Prior smoothing method (1000)");
        kl.add(lbl);
        lbl = new JLabel(" Interpolate smoothing strategy");
        kl.add(lbl);
        return kl;
    }

    private JPanel createIndriPanel() {
        JPanel indr = new JPanel();
        indr.setPreferredSize(new Dimension(250,75));
        indr.setLayout(new FlowLayout(FlowLayout.LEFT));

        JLabel lbl = new JLabel("No additional parameters.");
        indr.add(lbl);
        lbl = new JLabel(" For more info, see Lemur website");
        indr.add(lbl);
        lbl = new JLabel(" http://www.lemurproject.org");
        indr.add(lbl);
        return indr;
    }

    private JPanel createOkapiPanel() {
        JPanel okapi = new JPanel();
        okapi.setPreferredSize(new Dimension(250,125));
        okapi.setLayout(new FlowLayout(FlowLayout.LEFT));

        JLabel lbl = new JLabel("Value for BM25 K1 :");
        okapi.add(lbl);
        bm25k1F = new JTextField(String.valueOf(guiSettings.bm25k1),3);
        bm25k1F.setHorizontalAlignment(JTextField.RIGHT);
        okapi.add(bm25k1F);

        lbl = new JLabel("Value for BM25 B   :");
        okapi.add(lbl);
        bm25bF = new JTextField(String.valueOf(guiSettings.bm25b),3);
        bm25bF.setHorizontalAlignment(JTextField.RIGHT);
        okapi.add(bm25bF);

        lbl = new JLabel("Value for BM25 K3 :");
        okapi.add(lbl);
        bm25k3F = new JTextField(String.valueOf(guiSettings.bm25k3),3);
        bm25k3F.setHorizontalAlignment(JTextField.RIGHT);
        okapi.add(bm25k3F);

        lbl = new JLabel("Expanded query term TF weight:");
        okapi.add(lbl);
        bm25qtfF = new JTextField(String.valueOf(guiSettings.bm25qtf),3);
        bm25qtfF.setHorizontalAlignment(JTextField.RIGHT);
        okapi.add(bm25qtfF);

        lbl = new JLabel("Number of feedback terms:");
        okapi.add(lbl);
        feedbackF = new JTextField(String.valueOf(guiSettings.feedback),3);
        feedbackF.setHorizontalAlignment(JTextField.RIGHT);
        okapi.add(feedbackF);

        return okapi;
    }

    private JDialog createAboutDialog() {
        JDialog abtDialog = new JDialog(parent, "Retrieval GUI for the Lemur Toolkit", true);
        abtDialog.getContentPane().setLayout(new FlowLayout());
        //    abtDialog.getContentPane().add(new JLabel(new ImageIcon(System.getProperty("java.class.path")+SLASH+"lti-logo.gif")));
        //    ImageIcon logo = new ImageIcon("lemur_logo.gif");
        ImageIcon logo = createImageIcon("lemur_logo.gif");
        JLabel label = new JLabel("<html><font face=arial size=3 color=#000000><center><h4>Lemur Toolkit GUI</h4>(using Lemur version 4.0)<BR>June 2005<br><br>The Lemur Toolkit was developed <BR>in collaboration between the <br><BR><b>Language Technologies Institute<BR>Carnegie Mellon Univeristy<BR></b>Pittsburgh, PA 15213<br><BR>and<BR><br><b>Center for Intelligent Information Retrieval<BR>University of Massachusetts<BR></b>Amherst, MA 01003<br><BR><BR></font><font face=arial size=3 color=#000000>For more information, visit <BR>www.lemurproject.org</font><html>", logo, JLabel.CENTER);
        label.setVerticalTextPosition(JLabel.BOTTOM);
        label.setHorizontalTextPosition(JLabel.CENTER);

        abtDialog.getContentPane().add(label);    
        abtDialog.getContentPane().setBackground(Color.white);
        abtDialog.setResizable(false);
        return abtDialog;
    }

    private JPanel createQueryPanel() {
        JPanel panel = new JPanel();
        GridBagLayout gb = new GridBagLayout();
        GridBagConstraints gbc = new GridBagConstraints();
        panel.setLayout(gb);

        JLabel indlabel = new JLabel("Index to search:");
        indlabel.setVerticalAlignment(JLabel.BOTTOM);
        gbc.anchor = GridBagConstraints.WEST;
        gbc.weightx = 0;
        gbc.gridwidth = 1;
        gbc.insets = new Insets(5,5,5,5);
        gb.setConstraints(indlabel, gbc);
        panel.add(indlabel);

        indexBox = new JComboBox();
        // indexBox.addItem(MYINDEX);
        indexBox.addItem("Open index to start searching");
        gbc.gridwidth = 2;
        gbc.anchor = GridBagConstraints.WEST;
        gbc.insets = new Insets(5,5,5,5);
        gb.setConstraints(indexBox, gbc);
        panel.add(indexBox);

        JButton addBtn = new JButton ("Open");
        addBtn.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (((String)indexBox.getSelectedItem()).equals("Open index to start searching")) {
                        if (addIndexAction())
                            indexBox.removeItemAt(0);
                    } else {
                        addIndexAction();
                    }
                }
            });
        addBtn.setToolTipText("Add another index to the list");
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.weightx = 1;
        gb.setConstraints(addBtn, gbc);
        panel.add(addBtn);

        JLabel querylabel = new JLabel("Search query:");
        querylabel.setVerticalAlignment(JLabel.BOTTOM);
        gbc.gridwidth = 1;
        gbc.anchor = GridBagConstraints.WEST;
        gbc.weightx = 0;
        gbc.insets = new Insets(5,5,5,5);
        gb.setConstraints(querylabel, gbc);
        panel.add(querylabel);

        queryField = new JTextField("enter query here");
        queryField.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (((String)indexBox.getSelectedItem()).equals("Open index to start searching"))
                        errorMsg(parent, "Open index before attempting search");
                    else
                        searchAction();
                }
            });
        gbc.anchor = GridBagConstraints.WEST;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridwidth = 2;
        gbc.weightx = 1;
        gbc.insets = new Insets(5,5,5,5);
        gb.setConstraints(queryField, gbc);
        panel.add(queryField);

        JButton searchBtn = new JButton ("Search");
        searchBtn.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (((String)indexBox.getSelectedItem()).equals("Open index to start searching"))
                        errorMsg(parent, "Open index before attempting search");
                    else
                        searchAction();
                }
            });
        searchBtn.setToolTipText("Start searching index for query");
        gbc.fill = GridBagConstraints.NONE;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gb.setConstraints(searchBtn, gbc);
        panel.add(searchBtn);

        return panel;
    }

    private JPanel makeResultsPanel() {
        JPanel panel = new JPanel();
        GridBagLayout gb = new GridBagLayout();
        GridBagConstraints gbc = new GridBagConstraints();
        panel.setLayout(gb);

        //    JLabel lab = new JLabel("");
        //    lab.setVerticalAlignment(JLabel.BOTTOM);
        //    lab.setVerticalTextPosition(JLabel.BOTTOM);
        gbc.anchor = GridBagConstraints.WEST;
        gbc.insets = new Insets(5,5,5,5);
        gbc.weightx = 0;
        gbc.gridwidth = 1;
        //    gb.setConstraints(lab, gbc);
        //    panel.add(lab);

        status = new JLabel("0");
        status.setOpaque(true);
        status.setToolTipText("Change documents per page in Program->Options menu");
        status.setVerticalAlignment(JLabel.BOTTOM);
        status.setVerticalTextPosition(JLabel.BOTTOM);
        gbc.weightx = 1;
        gbc.gridwidth = 2;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gb.setConstraints(status, gbc);
        panel.add(status);

        prevBtn = new JButton ("Prev");
        prevBtn.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    prevPageAction();
                }
            });
        prevBtn.setToolTipText("Previous page");
        gbc.fill = GridBagConstraints.NONE;
        gbc.weightx = 0;
        gbc.gridx = GridBagConstraints.RELATIVE;
        gb.setConstraints(prevBtn, gbc);
        panel.add(prevBtn);

        nextBtn = new JButton ("Next");
        nextBtn.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (lastshown - firstshown + 1 == guiSettings.numresults)
                        nextPageAction();
                    else
                        // If the user changed the documents per page we just want to redisplay
                        pageAction();
                }
            });
        nextBtn.setToolTipText("Next page");
        gbc.fill = GridBagConstraints.NONE;
        gbc.weightx = 0;
        gbc.gridx = GridBagConstraints.RELATIVE;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gb.setConstraints(nextBtn, gbc);
        panel.add(nextBtn);

        resultsDisplay = new JEditorPane();
        resultsDisplay.setEditable(false);
        resultsDisplay.setContentType("text/html");
        resultsDisplay.addHyperlinkListener( new HyperlinkListener() {
                public void hyperlinkUpdate(HyperlinkEvent e) {
                    if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
                        linkClickAction(e.getURL().toString());
                    }
                }
            });

        JScrollPane scroller = new JScrollPane(resultsDisplay);
        scroller.getViewport().setPreferredSize(new Dimension(575, 450));
        gbc.insets = new Insets(1,1,1,1);
        gbc.fill = GridBagConstraints.BOTH;
        gbc.gridheight = GridBagConstraints.REMAINDER;
        gbc.weighty = 1;
        gb.setConstraints(scroller, gbc);
        panel.add(scroller);

        return panel;
    }

    private JDialog createDocDisplay(JTextPane tp) {
        JDialog doc = new JDialog(parent, "Document");
        doc.setLocation(50,50);
        tp.setEditable(false);
        tp.setCaretPosition(0);
        JScrollPane scroller = new JScrollPane(tp);
        scroller.getViewport().setPreferredSize(new Dimension(575, 450));
        doc.getContentPane().add(scroller);
        doc.setSize(new Dimension(575, 500));
        return doc;
    }

    /***************** Button Actions **********************/
    private void exportResultsAction() {
        JFileChooser fc = new JFileChooser();
        FileExtension ext = new FileExtension("txt", "textfile");
        fc.addChoosableFileFilter(ext);
        fc.setFileFilter(ext);
        fc.setCurrentDirectory(new File(workingDir));
        fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
        if (fc.showSaveDialog(parent) == JFileChooser.APPROVE_OPTION) {
            File file = fc.getSelectedFile();
            if (!guiSettings.appendfile && file.exists()) {
                String mesg = new String("This file exists. Do you want to overwrite file?");
                int answer = JOptionPane.showConfirmDialog(parent, mesg, "File already exists", JOptionPane.YES_NO_OPTION);
                if (answer == JOptionPane.NO_OPTION)
                    return;
            }
            writeResults(file);
        }
    }


    private boolean addIndexAction() {
        JFileChooser fc = new JFileChooser();
        //    String[] types = {"inv", "ifp", "ind", "key"};
        String[] types = {"inv", "ifp", "key"};
        FileExtension ext = new FileExtension(types, "index toc file");
        fc.addChoosableFileFilter(ext);
        fc.setFileFilter(ext);
        fc.setCurrentDirectory( new File(workingDir) );
        //    fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
        fc.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
        if (fc.showOpenDialog(parent) == JFileChooser.APPROVE_OPTION) {
            File file = fc.getSelectedFile();
            // if user double clicked a directory to select,
            // we get the directory name as the selected file
            // in the intended directory.
            // so check that the file exists and is a directory.
            // if not, try the parent directory.
            if (! file.exists()) {
                file = file.getParentFile();
            }
            indexBox.addItem(file.getAbsolutePath());
            indexBox.setSelectedItem(file.getAbsolutePath());
            workingDir = file.getPath();
            try {
                index = IndexManager.openIndex(file.getAbsolutePath());
            } catch (Exception e) {
                // index open threw, shouldn't
                return false;
            }
            
            
            index.setProps();
            return true;
        } else {
            return false;
        }
    }

    private void searchAction() {
        searchIndex = (String)indexBox.getSelectedItem();
        searchQuery = queryField.getText();
        firstshown = 1;
        lastshown = 0;
        maxresults = -1;
        queryResults = new IndexedReal[0];
        //    queryResults.clear();
    
        pageAction();
    }

    private void nextPageAction() {
        if ((maxresults < 0) || (maxresults >= firstshown + guiSettings.numresults)) {
            firstshown += guiSettings.numresults;
            pageAction();
        } else {
            // This is the last page
            errorMsg(parent, "There are no more documents in the index.");
        }
    }

    private void prevPageAction() {
        if ((firstshown -= guiSettings.numresults) < 1)
            firstshown = 1;
        pageAction();
    }

    private void pageAction() {
        status.setText("Searching...");
        status.update(status.getGraphics());
        try {
            if ((maxresults < 0) && queryResults.length < firstshown + guiSettings.numresults)
                queryResults = sendQuery(searchQuery);
        } catch (Exception e) {
            errorMsg(parent, "An error occurred while searching.\n\n" + e.toString());
        }
        status.setText("Displaying...");
        status.update(status.getGraphics());
  
        showResults(formatResults(queryResults));
    }

    private void setParamValues() {
        guiSettings.loadind = loadCheck.isSelected();
        guiSettings.openwin = openCheck.isSelected();
        guiSettings.highlight = hlCheck.isSelected();
        guiSettings.showmarkup = markupCheck.isSelected();
        guiSettings.appendfile = appendCheck.isSelected();
        guiSettings.retmethod = (String)retriBox.getSelectedItem();
        guiSettings.numresults = Integer.parseInt(numresultsF.getText());

        // need to reset the retrieval model here potentially.
        model = null;
        accum = null;

        String retmet = (String)retriBox.getSelectedItem();
        //    if (retmet.equals(INQUERY)) {
        // InQuery ret parms
        guiSettings.belief = Double.parseDouble(beliefF.getText());
        guiSettings.fbterms = Integer.parseInt(fbtermsF.getText());
        guiSettings.fbcoef = Double.parseDouble(fbcoefF.getText());
        //    } else if (retmet.equals(OKAPI)) {
        // Okapi parms
        guiSettings.bm25k1 = Double.parseDouble(bm25k1F.getText());
        guiSettings.bm25b = Double.parseDouble(bm25bF.getText());
        guiSettings.bm25k3 = Double.parseDouble(bm25k3F.getText());
        guiSettings.bm25qtf = Double.parseDouble(bm25qtfF.getText());
        guiSettings.feedback = Integer.parseInt(feedbackF.getText());
        //    }

        // Save to file
        FileOutputStream fos = null;
        ObjectOutputStream out = null;
        try {
            fos = new FileOutputStream(optsFile);
            out = new ObjectOutputStream(fos);
            out.writeObject(guiSettings);
            out.close();
        }
        catch (IOException ex) {
            ex.printStackTrace();
        }

    }

    private void resetParamValues() {
        loadCheck.setSelected(guiSettings.loadind);
        openCheck.setSelected(guiSettings.openwin);
        hlCheck.setSelected(guiSettings.highlight);
        markupCheck.setSelected(guiSettings.showmarkup);
        appendCheck.setSelected(guiSettings.appendfile);
        retriBox.setSelectedItem(guiSettings.retmethod);
        numresultsF.setText(String.valueOf(guiSettings.numresults));

        // InQuery ret parms
        beliefF.setText(String.valueOf(guiSettings.belief));
        fbtermsF.setText(String.valueOf(guiSettings.fbterms));
        fbcoefF.setText(String.valueOf(guiSettings.fbcoef));

        // Okapi parms
        bm25k1F.setText(String.valueOf(guiSettings.bm25k1));
        bm25bF.setText(String.valueOf(guiSettings.bm25b));
        bm25k3F.setText(String.valueOf(guiSettings.bm25k3));
        bm25qtfF.setText(String.valueOf(guiSettings.bm25qtf));
        feedbackF.setText(String.valueOf(guiSettings.feedback));
    }

    private void linkClickAction(String href) {

        try {
            if (href.startsWith("file:document:")) {
                href = href.replaceFirst("file:document:","");
                System.out.println("Getting dM for "+searchIndex);
                DocumentManager dm = index.docManager(index.document(href));
                String content = dm.getDoc(href);
                DefaultStyledDocument contentdoc = new DefaultStyledDocument();
                contentdoc.insertString(0, content, SimpleAttributeSet.EMPTY);
                String title = dm.docElement(href,"TITLE");
                if (title == null)
                    title = dm.docElement(href,"HEADLINE");
                if (title == null)
                    title = href;
                //hack
                if (title.length() > 100) title = title.substring(0, 100);
                //
                Query qry = Query.makeQuery(stripOpQuery(searchQuery));
                TMatch[] matches = MatchInfo.getMatches(index, qry, index.document(href));

                if (guiSettings.openwin) {
                    JDialog doc = new JDialog(parent, title);
                    doc.setLocation(50,50);
                    JTextPane docDisplay = new JTextPane();
                    docDisplay.setEditable(false);
                    if (!guiSettings.showmarkup)
                        docDisplay.setContentType("text/html");
                    // Can't use docDisplay.setText(content) since it may change match offsets
                    docDisplay.setDocument(contentdoc);
                    docDisplay.setCaretPosition(0);
                    if (guiSettings.highlight)
                        if (matches.length > 0)
                            highlightText(docDisplay, matches);
                        else 
                            // do what we can
                            highlightText(docDisplay, searchQuery);
                    JScrollPane scroller = new JScrollPane(docDisplay);
                    scroller.getViewport().setPreferredSize(new Dimension(575, 450));
                    doc.getContentPane().add(scroller);
                    doc.setSize(new Dimension(575, 500));
                    doc.setVisible(true);
                } else {
                    if (guiSettings.showmarkup)
                        resdocTPane.setContentType("text/plain");
                    else
                        resdocTPane.setContentType("text/html");
                    resdocTPane.getHighlighter().removeAllHighlights();
                    // Can't use resdocTPane.setText(content) since it may change match offsets
                    resdocTPane.setDocument(contentdoc);
                    resdocDialog.setTitle(title);
                    resdocTPane.setCaretPosition(0);
                    if (guiSettings.highlight)
                        if (matches.length > 0)
                            highlightText(resdocTPane, matches);
                        else 
                            highlightText(resdocTPane, searchQuery);
                    resdocDialog.setVisible(true); // jdk 1.5 replaces .show()
                }
            }
        }
        catch (Exception e) {
            errorMsg(parent, "An error occurred while displaying the document.\n\n" + e.toString());
        }
    }

    /****************** SUPPORTING METHODS *****************/
    private void errorMsg(Component par, String mesg) {
        JOptionPane.showMessageDialog(par, mesg, "Error", JOptionPane.ERROR_MESSAGE);
    }

    private void initIndexTable() {
    }

    private void saveIndexTable() {
    }

    // highlight text using match info
    private void highlightText(JTextPane docDisplay, TMatch[] matches) {
        // use component default selection color
        DefaultHighlighter.DefaultHighlightPainter painter = new DefaultHighlighter.DefaultHighlightPainter(null);

        Highlighter hl = docDisplay.getHighlighter();
        try {
            Document doc = docDisplay.getDocument();
            for (int i=0; i<matches.length; i++) {
                hl.addHighlight(matches[i].start,matches[i].end + 1,painter);
            }
        }
        catch (BadLocationException ex) {
        }
    }

    // "fake" highlighting
    private void highlightText(JTextPane docDisplay, String searchQuery) {
        // use component default selection color
        DefaultHighlighter.DefaultHighlightPainter painter = new DefaultHighlighter.DefaultHighlightPainter(null);
	
        Highlighter hl = docDisplay.getHighlighter();
        try {
            Document doc = docDisplay.getDocument();
            String text = doc.getText(0, doc.getLength());
            StringTokenizer stok = new StringTokenizer(searchQuery, "() ");
            String token;
            int pos=0;
            while (stok.hasMoreTokens()) {
                token = stok.nextToken();
                //				System.err.println("processing token " + token);
                // ignore operators
                if (token.startsWith("#"))
                    continue;
                // only find as whole word
                //token = " " + token;
                //pos = text.indexOf(token, pos);
                pos = indexOfWord(text, token, pos);
                while (pos >= 0) {
                    hl.addHighlight(pos,pos+token.length(),painter);
                    //	pos = text.indexOf(token, pos+token.length());
                    pos = indexOfWord(text, token, pos+token.length());
                }
            }
        }
        catch (BadLocationException ex) {
        }
    }

    private String stripOpQuery(String opquery) {
        StringTokenizer stok = new StringTokenizer(opquery, "() ");
        String token;
        String stripped = "";   
        while (stok.hasMoreTokens()) {
            token = stok.nextToken();
            if (!token.startsWith("#")) {
                stripped += token;
                stripped += " ";
            }
        }
        return stripped;
    }

    // return index of the first occurrence of this word, case insensitive
    private int indexOfWord(String text, String word, int start) {
        String cap = " " + word.toUpperCase();
        String small = " " + word.toLowerCase();
        String reg = " " + Character.toUpperCase(word.charAt(0)) + word.substring(1);
        int capi = text.indexOf(cap,start);
        int smalli = text.indexOf(small,start);
        int wordi = text.indexOf(reg, start);
        int ret = -1;

        // return the smallest value not -1 unless all are -1
        if (capi == -1) {
            if (wordi == -1)
                ret = smalli;
            else {
                if (smalli == -1)
                    ret = wordi;
                else
                    ret = Math.min(wordi, smalli);
            }
        } else {
            if (wordi == -1) {
                if (smalli == -1)
                    ret = capi;
                else
                    ret = Math.min(capi, smalli);
            } else {
                if (smalli == -1)
                    ret = Math.min(capi, wordi);
                else
                    ret = Math.min(Math.min(capi, wordi),smalli);
            }
        }

        if (ret == -1)
            return ret;

        return ret+1;
    }

    private void writeResults(File output) {
        try {
            PrintWriter filewrite =  new PrintWriter(new FileWriter(output,guiSettings.appendfile));
            filewrite.println("query \""+searchQuery+"\" in " + searchIndex);
            for (int i=0;i<queryResults.length;i++) {
                IndexedReal ds = queryResults[i];
                filewrite.println(i+1 + " "+index.document(ds.ind) + " " +ds.val);
            }
            filewrite.close();
        }
        catch (IOException ex) {
            errorMsg(parent, "Sorry, error writing to file: " + ex);
        } catch (Exception e) {
            errorMsg(parent, "Sorry, error writing document id to file: " + e);
        }
        
    }

    private void showResults(String res) {
        resultsDisplay.setText(res);
        resultsDisplay.setCaretPosition(0);
        //String n = new String();
        //n = " " + queryResults.size();
        String countertext;
        if (maxresults == 0)
            countertext = NODOCUMENTS;
        else {
            countertext = "<html>Documents "+firstshown+" to "+lastshown;
            if (maxresults >= 0)
                countertext += " out of " + maxresults;
            countertext += " &nbsp <font size=2></font></html>";
        }
        status.setText(countertext);
    
        if (firstshown == 1)
            prevBtn.setEnabled(false);
        else
            prevBtn.setEnabled(true);
        if ((maxresults < 0) || (maxresults >= firstshown + guiSettings.numresults))
            nextBtn.setEnabled(true);
        else
            nextBtn.setEnabled(false);

        resultsPane.setVisible(true);
    }

    private String formatResults(IndexedReal[] results) {
        StringBuffer buf = new StringBuffer();

        try {
            //    System.out.println("Getting dM for "+searchIndex);
            DocumentManager dm = index.docManager(1); /// first doc

            buf.append("<ol style='list-style: outside; margin-left: 35;'>");
            for (int i=firstshown-1;i<results.length && i<firstshown+guiSettings.numresults-1;i++) {
                String docid = (index.document(results[i].ind));
                //System.out.println("DEBUG: "+docid);
                lastshown=i+1;
                buf.append("<li value='"+(i+1)+"'><a href=\"file:document:");
                buf.append(docid);
                buf.append("\">");
                String title = dm.docElement(docid,"TITLE");
                if (title == null)
                    title = dm.docElement(docid,"HEADLINE");
                if (title != null) {
                    //hack
                    if (title.length() > 100) title = title.substring(0, 100);
                    //
                    buf.append(title);
                } else
                    buf.append(docid);
                buf.append("</a><BR>");

                // Get the matches from MatchInfo
                String doc = dm.getDoc(docid);
                Query qry = Query.makeQuery(stripOpQuery(searchQuery));
                TMatch[] docmatches = MatchInfo.getMatches(index, qry, index.document(docid));
                if (docmatches.length > 0) {
                    // Display first three matches with passage30 for each, try to break on whitespace
                    int start, end;  // bounds of passage
                    int first, last; // occurrences of whitespace
                    int finalpassage = docmatches.length - 1;
                    if (finalpassage > 3) finalpassage = 3;
                    // rethink this.
                    for (int m=0; m < finalpassage; m++) {
                        start = docmatches[m].start - 30;
                        end = docmatches[m].end + 30;
                        if (start < 0)
                            start = 0;
                        if (end >= doc.length())
                            end = doc.length() - 1;
                        first = start;
                        last = end;
                        for (int w = (docmatches[m].start + start) / 2; w > start; w--) {
                            if (doc.charAt(w) == ' ') 
                                first = w;
                        }
                        for (int w = (docmatches[m].end + end) / 2; w < end; w++) {
                            if (doc.charAt(w) == ' ') 
                                last = w;
                        }

                        docmatches[m].start = first;
                        docmatches[m].end = last;
                    }
                    buf.append("...");
                    if (finalpassage > 1) {
                        // Combine overlapping passages
                        for (int m=1; m < finalpassage; m++) {
                            // fixme
                            if (false /*&& docmatches[m*2-1] >= docmatches[m*2]*/) 
                                docmatches[m*2] = docmatches[m*2-2];
                            else {
                                // no overlap so display previous passage
                                String passage = doc.substring(docmatches[m].start,docmatches[m].end);
                                passage = passage.replaceAll("<","&lt;");
                                passage = passage.replaceAll(">","&gt;");
                                buf.append(passage + "...");
                            }
                        }
                    }
                    // Display last passage (max third)
                    String passage = doc.substring(docmatches[finalpassage].start,docmatches[finalpassage].end);
                    passage = passage.replaceAll("<","&lt;");
                    passage = passage.replaceAll(">","&gt;");
                    buf.append(passage + "...<BR>");
                }

                buf.append("<i>");
                String score = Double.toString(results[i].val);
                // set precision to 3 decimal points at most
                int scoreend = score.indexOf(".") +4;
                if (scoreend < score.length())
                    score = score.substring(0,scoreend);
                buf.append(score);
                buf.append("</i><br>&nbsp;");
            }
            buf.append("</ol>");

        }
        catch (Exception e) {
            errorMsg(parent, "An error occurred while displaying the results.\n\n" + e.toString());
        }

        return buf.toString();
    }

    private IndexedReal[] sendQuery(String query) {
        // sloppy
        String retmethod = guiSettings.retmethod;
        // if params change, have to remake the model.
        if (retmethod.equals(INQUERY)) {
            retmethod = "inq_struct";
        } else if (retmethod.equals(OKAPI)) {
            retmethod = "okapi";
        } else if (retmethod.equals(KLDIV)) {
            retmethod = "kl";
        } else if (retmethod.equals(INDRI)) {
            retmethod = "indri";
        }
        if (model == null) {
            // if params change, have to remake the model.
            try {
                
                if (retmethod.equals("inq_struct")) {
                    lemur.ParamSet("defaultBelief", 
                                   Double.toString(guiSettings.belief));
                    lemur.ParamSet("feedbackPosCoeff", 
                                   Double.toString(guiSettings.fbcoef));
                    lemur.ParamSet("feedbackTermCount", 
                                   Integer.toString(guiSettings.fbterms));
                } else if (retmethod.equals("okapi")) {
                    lemur.ParamSet("BM25K1", Double.toString(guiSettings.bm25k1));
                    lemur.ParamSet("BM25B", Double.toString(guiSettings.bm25b));
                    lemur.ParamSet("BM25K3", Double.toString(guiSettings.bm25k3));
                    lemur.ParamSet("BM25QTF", Double.toString(guiSettings.bm25qtf));
                    lemur.ParamSet("feedbackTermCount", 
                                   Integer.toString(guiSettings.feedback));
                }
            } catch (Exception e) {
                errorMsg(parent, "Unable to set parameters " + e);  
                return null;
            }
            
            try {
                accum = new ArrayAccumulator(index.docCount());
            } catch (Exception e) {
                errorMsg(parent, "Unable to get docCount " + e);  
                return null;
            }
            try {            
                model = RetMethodManager.createModel(index, accum, retmethod);
            } catch (Exception e) {
                errorMsg(parent, "Unable to create retrieval model: " + e);
                return null;
            }
        }
        IndexedReal[] res;
        try {
            res =  model.runQuery(searchQuery);
        } catch (Exception e) {
            errorMsg(parent, "Unable to run query: " + e);
            return null;
        }
        
        if (res.length < firstshown + guiSettings.numresults)
            // We now know how many documents there are
            maxresults = res.length;
        return res;
    }

    /** Returns an ImageIcon, or null if the path was invalid.
        @param path the image file to load.
        @return an ImageIcon, or null if the path was invalid.
    */
    protected static ImageIcon createImageIcon(String path)
    {
        java.net.URL imgURL = LemurRet.class.getResource(path);
        if (imgURL != null)
            {
                return new ImageIcon(imgURL);
            }
        else
            {
                return null;
            }
    }
    private static void createAndShowGUI() {
        try {
            UIManager.setLookAndFeel( UIManager.getSystemLookAndFeelClassName() );
        } catch (Exception e) { }

        JFrame frame = new JFrame();
        frame.setLocation(30,30);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setIconImage(createImageIcon(ICONSTRING).getImage());
        LemurRet panel = new LemurRet(frame);

        frame.setTitle("Lemur Retrieval GUI");
        frame.getContentPane().setLayout(new BorderLayout());
        frame.getContentPane().add("Center", panel);
        JMenuBar menubar = panel.createMenu();
        frame.setJMenuBar(menubar);
        frame.pack();
        frame.setVisible(true);
    }
    
    /******************** MAIN *****************************/
    public static void main(String[] args) {
	//Schedule a job for the event-dispatching thread:
	//creating and showing this application's GUI.
	javax.swing.SwingUtilities.invokeLater(new Runnable() {
		public void run() {
		    createAndShowGUI();
		}
	    });
        /*******************************************************/
    }
}

