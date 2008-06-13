/*==========================================================================
 * Copyright (c) 2004-2008 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software (and below), and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
 */
package lemurproject.lemur.ui;

import java.util.logging.Level;
import java.util.logging.Logger;
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

    JCheckBox chkUseEvalPanel;

    String workingDir;
    Color relColor, novelColor;

    JPanel resultsPane;

    JPanel pnlResults;
    JEditorPane resultsDisplay;

    JTable tblEvalResults;
    JButton btnSaveEvalResults;
    JButton btnLoadEvalScores;
    JButton btnAddNewQueryID;
    JComboBox cboCurrentlyLoadedQueries;
    JPanel pnlSaveAndLoadScores;

    boolean qrelsChangedGlobally=false;
    boolean qrelsChangedCurrentID=false;

    //boolean qrelsWereChanged=false;
    //boolean qrelsCanBeSaved=false;
    java.util.HashMap loadedQrels=null;

    JLabel status;
    //  Vector queryResults = new Vector();
    IndexedReal[] queryResults=new IndexedReal[0];

    // keeps track of any eval scores
    // - 1 for each queryResult item
    // null indicates that the item was not scored
    Double[] evalScores=new Double[0];

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
        guiSettings.appendfile = false;        // default retrieval method
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

        parent.addComponentListener(new ComponentListener() {
          public void componentResized(ComponentEvent e) {
            setEvalTableColumnSizes();
          }
          public void componentMoved(ComponentEvent e) { }
          public void componentShown(ComponentEvent e) {
            setEvalTableColumnSizes();
          }
          public void componentHidden(ComponentEvent e) { }
        });
        parent.addWindowListener(new WindowListener() {
          public void windowClosing(WindowEvent e) {
            checkSaveEvalResults(true);
          }
          public void windowOpened(WindowEvent e) { }
          public void windowClosed(WindowEvent e) { }
          public void windowIconified(WindowEvent e) { }
          public void windowDeiconified(WindowEvent e) { }
          public void windowActivated(WindowEvent e) { }
          public void windowDeactivated(WindowEvent e) { }
        });
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
        // helpman.setEnabled(false);
        helpman.setMnemonic('H');
        helpman.addActionListener( new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                  showHelpScreen();
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
        retriBox.addItem(INDRI);        retriBox.addItem(INQUERY);
        retriBox.addItem(OKAPI);
        retriBox.addItem(KLDIV);        retriBox.addItemListener( new ItemListener() {
                public void itemStateChanged(ItemEvent evt) {
                    ((CardLayout)paramsPane.getLayout()).show(paramsPane, (String)evt.getItem());
                }
            });
        bottom.add(retriBox);
        name = new JLabel("ADVANCED PARAMETERS");
        bottom.add(name);

        paramsPane = new JPanel();
        paramsPane.setLayout(new CardLayout());
        paramsPane.add(createIndriPanel(), INDRI);        paramsPane.add(createInqPanel(),INQUERY);
        paramsPane.add(createKLPanel(),KLDIV);
        paramsPane.add(createOkapiPanel(), OKAPI);
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
        inq.setPreferredSize(new Dimension(250,75));                inq.setLayout(new GridBagLayout());                GridBagConstraints gbc=new GridBagConstraints();                gbc.anchor=GridBagConstraints.NORTHWEST;        gbc.fill=GridBagConstraints.HORIZONTAL;        gbc.gridx=0;        gbc.gridy=0;        inq.add(new JLabel("Default document belief: "), gbc);        gbc.gridx=1;        beliefF = new JTextField(String.valueOf(guiSettings.belief),3);        beliefF.setHorizontalAlignment(JTextField.RIGHT);        inq.add(beliefF, gbc);

        gbc.gridx=0;        gbc.gridy=1;        inq.add(new JLabel("Rocchio feedback coefficient: "), gbc);        gbc.gridx=1;        fbcoefF = new JTextField(String.valueOf(guiSettings.fbcoef),3);        fbcoefF.setHorizontalAlignment(JTextField.RIGHT);        inq.add(fbcoefF, gbc);

        gbc.gridx=0;        gbc.gridy=2;        inq.add(new JLabel("Number of feedback terms:"), gbc);        gbc.gridx=1;        fbtermsF = new JTextField(String.valueOf(guiSettings.fbterms),3);        fbtermsF.setHorizontalAlignment(JTextField.RIGHT);        inq.add(fbtermsF, gbc);
        return inq;    }

    private JPanel createKLPanel() {
        JPanel kl = new JPanel();
        kl.setPreferredSize(new Dimension(250,75));
        kl.setLayout(new FlowLayout(FlowLayout.LEFT));
        JLabel lbl = new JLabel("KL using default parameters:");        kl.add(lbl);
        lbl = new JLabel(" Dirichlet Prior smoothing method (1000)");        kl.add(lbl);
        lbl = new JLabel(" Interpolate smoothing strategy");        kl.add(lbl);
        return kl;    }
    private JPanel createIndriPanel() {
        JPanel indr = new JPanel();
        indr.setPreferredSize(new Dimension(250,75));        indr.setLayout(new FlowLayout(FlowLayout.LEFT));
        JLabel lbl = new JLabel("No additional parameters.");        indr.add(lbl);
        lbl = new JLabel(" For more info, see Lemur website");        indr.add(lbl);
        lbl = new JLabel(" http://www.lemurproject.org");        indr.add(lbl);
        return indr;    }
    private JPanel createOkapiPanel() {
        JPanel okapi = new JPanel();
        okapi.setPreferredSize(new Dimension(250,125));
        okapi.setLayout(new GridBagLayout());                GridBagConstraints gbc=new GridBagConstraints();                gbc.anchor=GridBagConstraints.NORTHWEST;        gbc.fill=GridBagConstraints.HORIZONTAL;        gbc.gridx=0;        gbc.gridy=0;        okapi.add(new JLabel("Value for BM25 K1 : "), gbc);
        gbc.gridx=1;
        bm25k1F = new JTextField(String.valueOf(guiSettings.bm25k1),3);        bm25k1F.setHorizontalAlignment(JTextField.RIGHT);        okapi.add(bm25k1F, gbc);
        gbc.gridx=0;        gbc.gridy=1;        okapi.add(new JLabel("Value for BM25 B : "), gbc);        gbc.gridx=1;        bm25bF = new JTextField(String.valueOf(guiSettings.bm25b),3);        bm25bF.setHorizontalAlignment(JTextField.RIGHT);        okapi.add(bm25bF, gbc);
        gbc.gridx=0;        gbc.gridy=2;        okapi.add(new JLabel("Value for BM25 K3 : "), gbc);        gbc.gridx=1;        bm25k3F = new JTextField(String.valueOf(guiSettings.bm25k3),3);        bm25k3F.setHorizontalAlignment(JTextField.RIGHT);        okapi.add(bm25k3F, gbc);

        gbc.gridx=0;        gbc.gridy=3;        okapi.add(new JLabel("Expanded query term TF weight: "), gbc);        gbc.gridx=1;        bm25qtfF = new JTextField(String.valueOf(guiSettings.bm25qtf),3);        bm25qtfF.setHorizontalAlignment(JTextField.RIGHT);        okapi.add(bm25qtfF, gbc);

        gbc.gridx=0;        gbc.gridy=4;        okapi.add(new JLabel("Number of feedback terms: "), gbc);        gbc.gridx=1;        feedbackF = new JTextField(String.valueOf(guiSettings.feedback),3);        feedbackF.setHorizontalAlignment(JTextField.RIGHT);        okapi.add(feedbackF, gbc);
        return okapi;
    }

    private JDialog createAboutDialog() {
        JDialog abtDialog = new JDialog(parent, "Retrieval GUI for the Lemur Toolkit", true);
        abtDialog.getContentPane().setLayout(new FlowLayout());
        //    abtDialog.getContentPane().add(new JLabel(new ImageIcon(System.getProperty("java.class.path")+SLASH+"lti-logo.gif")));
        //    ImageIcon logo = new ImageIcon("lemur_logo.gif");
        ImageIcon logo = createImageIcon("lemur_logo.gif");
        JLabel label = new JLabel("<html><font face=arial size=3 color=#000000><center><h4>Lemur Toolkit GUI</h4>The Lemur Toolkit was developed <BR>in collaboration between the <br><BR><b>Language Technologies Institute<BR>Carnegie Mellon Univeristy<BR></b>Pittsburgh, PA 15213<br><BR>and<BR><br><b>Center for Intelligent Information Retrieval<BR>University of Massachusetts<BR></b>Amherst, MA 01003<br><BR><BR></font><font face=arial size=3 color=#000000>For more information, visit <BR>www.lemurproject.org</font><html>", logo, JLabel.CENTER);
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

        JSeparator sepEval=new JSeparator(JSeparator.HORIZONTAL);
        gbc.anchor = GridBagConstraints.WEST;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.weightx = 1;
        gbc.insets = new Insets(5,5,5,5);
        gb.setConstraints(sepEval, gbc);
        panel.add(sepEval);

        chkUseEvalPanel=new JCheckBox("Use Evaluation Panel for Results?");
        chkUseEvalPanel.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            chkUseEvalPanel_OnChange();
          }
        });
        gbc.anchor = GridBagConstraints.NORTHWEST;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridwidth = 1;
        gbc.weightx = 1;
        gbc.insets = new Insets(5,5,5,5);
        gb.setConstraints(chkUseEvalPanel, gbc);

        panel.add(chkUseEvalPanel);

        pnlSaveAndLoadScores=new JPanel();
        pnlSaveAndLoadScores.setLayout(new BorderLayout());

        btnSaveEvalResults=new JButton("Save Judgment Scores");
        btnSaveEvalResults.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            if (checkSaveEvalResults(false) && !qrelsChangedGlobally) {
              btnSaveEvalResults.setVisible(false);
            }
          }
        });
        btnSaveEvalResults.setVisible(false);
        pnlSaveAndLoadScores.add(btnSaveEvalResults, BorderLayout.WEST);

        btnLoadEvalScores=new JButton("Load Judgment Scores");
        btnLoadEvalScores.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            doLoadEvalScores();
          }
        });
        btnLoadEvalScores.setVisible(true);
        pnlSaveAndLoadScores.add(btnLoadEvalScores, BorderLayout.EAST);

        JPanel pnlLoadedQueries=new JPanel();
        pnlLoadedQueries.setLayout(new BorderLayout());

        JLabel lblLoadedQueries=new JLabel("Active Query ID:");
        lblLoadedQueries.setHorizontalAlignment(JLabel.RIGHT);
        pnlLoadedQueries.add(lblLoadedQueries, BorderLayout.WEST);

        cboCurrentlyLoadedQueries=new JComboBox();
        cboCurrentlyLoadedQueries.addItem("-- none --");
        cboCurrentlyLoadedQueries.addActionListener(new ActionListener() {
          int oldItemIndex=-1;
          public void actionPerformed(ActionEvent e) {
            // modify the scores when changed...
            JComboBox cb=(JComboBox)e.getSource();
            int newItemIndex=cb.getSelectedIndex();
            if (newItemIndex==oldItemIndex) { return; }

            // if we have scores that have changed - prompt the user to update

            if (qrelsChangedCurrentID && oldItemIndex > 0) {
              shouldWeUpdateScores(oldItemIndex, true);
            }
            // qrelsChangedCurrentID=false;
            loadInNewScores(newItemIndex);

            oldItemIndex=newItemIndex;
          }
        });
        pnlLoadedQueries.add(cboCurrentlyLoadedQueries, BorderLayout.CENTER);

        btnAddNewQueryID=new JButton("New Query ID");
        btnAddNewQueryID.setFont(new java.awt.Font("SansSerif", 0, 9));
        btnAddNewQueryID.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            doAddNewQueryID();
          }
        });
        pnlLoadedQueries.add(btnAddNewQueryID, BorderLayout.EAST);

        pnlSaveAndLoadScores.add(pnlLoadedQueries, BorderLayout.SOUTH);

        gbc.anchor=GridBagConstraints.EAST;
        gbc.fill = GridBagConstraints.NONE;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.weightx = 1;
        gbc.insets = new Insets(5,5,5,5);
        gb.setConstraints(pnlSaveAndLoadScores, gbc);
        panel.add(pnlSaveAndLoadScores);

        pnlSaveAndLoadScores.setVisible(false);

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

        pnlResults=new JPanel(new CardLayout());
        pnlResults.addComponentListener(new java.awt.event.ComponentListener() {
          public void componentResized(ComponentEvent e) {
            setEvalTableColumnSizes();
          }

          public void componentMoved(ComponentEvent e) {
            // -- nothing
          }

          public void componentShown(ComponentEvent e) {
            setEvalTableColumnSizes();
          }

          public void componentHidden(ComponentEvent e) {
            // -- nothing
          }
        });

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
        pnlResults.add(scroller, "regular");

        EvalTableModel mdlEvalResults=new EvalTableModel();
        tblEvalResults=new JTable(mdlEvalResults);

        for (int i=0; i < 5; i++) {
          javax.swing.table.TableColumn thisCol=tblEvalResults.getColumnModel().getColumn(i);
          switch (i) {
            case 0: thisCol.setCellRenderer(new EvalDefaultCell()); break;
            case 1: {
              EvalTableScoreCell editorCell=new EvalTableScoreCell();
              editorCell.addCellEditorListener(new javax.swing.event.CellEditorListener() {
                public void editingStopped(ChangeEvent e) {
                  int selectedRowEdit=tblEvalResults.getSelectedRow();
                  int thisIndex=(firstshown-1+selectedRowEdit);
                  if ((thisIndex >= 0) && (thisIndex < queryResults.length)) {
                    EvalTableModel mdlEvalResults=(EvalTableModel)tblEvalResults.getModel();
                    String thisValString=(String)mdlEvalResults.getValueAt(selectedRowEdit, 1);
                    Double newValue=null;
                    if (thisValString.length() > 0) {
                      newValue=Double.parseDouble(thisValString);
                    }

                    boolean scoreChanged=false;
                    if (newValue!=evalScores[thisIndex]) {
                      scoreChanged=true;
                      if (newValue!=null && evalScores[thisIndex]!=null) {
                        scoreChanged=(!newValue.equals(evalScores[thisIndex]));
                      }
                    }

                    if (scoreChanged) {
                      if (cboCurrentlyLoadedQueries.getSelectedIndex() < 1) {
                        // we must have a current query ID!
                        JOptionPane.showMessageDialog(parent, "You must have a query ID selected to enter a score.", "No Query ID selected", JOptionPane.ERROR_MESSAGE);
                        mdlEvalResults.setEvalScore(selectedRowEdit, evalScores[thisIndex]);
                      } else {
                        evalScores[thisIndex]=newValue;

                        // set that we've changed at least one qrel
                      qrelsChangedCurrentID=true;

                      } // end if (cboCurrentlyLoadedQueries.getSelectedIndex() < 1)
                    } // end if(scoreChanged)
                  }
                  // see if we need to show the scores button
                  if (qrelsChangedCurrentID) {
                      btnSaveEvalResults.setVisible(true);
                  }
                }
                public void editingCanceled(ChangeEvent e) { }
              });
              thisCol.setCellEditor(editorCell);
            } break;
            case 2: thisCol.setCellRenderer(new EvalDefaultCell()); break;
            case 3: thisCol.setCellRenderer(new EvalTableTitleCell()); break;
            case 4: thisCol.setCellRenderer(new EvalSnippetCell()); break;
            default: // nothing
          }
        }

        tblEvalResults.setEnabled(true);
        tblEvalResults.setColumnSelectionAllowed(false);
        tblEvalResults.setRowSelectionAllowed(false);
        tblEvalResults.setAutoResizeMode(JTable.AUTO_RESIZE_ALL_COLUMNS);
        tblEvalResults.addMouseListener(new MouseListener() {
          public void mouseClicked(MouseEvent e) {
            int rowIndex=tblEvalResults.rowAtPoint(e.getPoint());
            int columnIndex=tblEvalResults.columnAtPoint(e.getPoint());
            if (columnIndex==3) {
              String hrefAtRow=(String)((EvalTableModel)tblEvalResults.getModel()).getValueAt(rowIndex, columnIndex);
              // extract the hrefValue
              int hrefPos=hrefAtRow.indexOf("href=\"");
              if (hrefPos > 0) {
                int closingQuote=hrefAtRow.indexOf("\"", hrefPos+6);
                if (closingQuote > 0) {
                  String hrefSub=hrefAtRow.substring(hrefPos+6, closingQuote);
                  linkClickAction(hrefSub);
                }
              }
            }
          }
          public void mousePressed(MouseEvent e) { }
          public void mouseReleased(MouseEvent e) { }
          public void mouseEntered(MouseEvent e) { }
          public void mouseExited(MouseEvent e) { }
        });
        JScrollPane scpResults=new JScrollPane(tblEvalResults);
        scpResults.getViewport().setPreferredSize(new Dimension(575, 450));
        pnlResults.add(scpResults, "eval");


        gbc.insets = new Insets(1,1,1,1);
        gbc.fill = GridBagConstraints.BOTH;
        gbc.gridheight = GridBagConstraints.REMAINDER;
        gbc.weighty = 1;
        gb.setConstraints(pnlResults, gbc);
        panel.add(pnlResults);

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
        String[] types = {"key"};
        FileExtension ext = new FileExtension(types, "index toc file");
        fc.addChoosableFileFilter(ext);
        fc.setFileFilter(ext);
        fc.setCurrentDirectory( new File(workingDir) );
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
      // first - check and see if we have any previous
      // eval scores to save...
      if (qrelsChangedCurrentID) {
        int whichQueryIndex=cboCurrentlyLoadedQueries.getSelectedIndex();
        if (whichQueryIndex > 0) {
          // if (!shouldWeUpdateScores(whichQueryIndex)) {
          shouldWeUpdateScores(whichQueryIndex, true);
          //  return;
          // }
        }
      }

      searchIndex = (String)indexBox.getSelectedItem();
      searchQuery = queryField.getText();
      firstshown = 1;
      lastshown = 0;
      maxresults = -1;
      queryResults = new IndexedReal[0];
      clearEvalResults();

      // set the query ID to -- no query selected --
      // cboCurrentlyLoadedQueries.setSelectedIndex(0);

      pageAction();
      setScoresForQuery();
    }

    private void setEvalTableColumnSizes() {
        tblEvalResults.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);

        for (int i=0; i < 5; i++) {
          javax.swing.table.TableColumn thisCol=tblEvalResults.getColumnModel().getColumn(i);
          switch (i) {
            case 0: { thisCol.setPreferredWidth(70); } break;
            case 1: { thisCol.setPreferredWidth(100); } break;
            case 2: { thisCol.setPreferredWidth(70); } break;
            case 3: { thisCol.setPreferredWidth(120); } break;
            case 4: { thisCol.setPreferredWidth(pnlResults.getWidth()-380); } break;
            default:
              // nothing;
          }
        }
        tblEvalResults.setRowHeight(64);
    }

    private void chkUseEvalPanel_OnChange() {
      CardLayout resultsLayout=(CardLayout)(pnlResults.getLayout());
      if (chkUseEvalPanel.isSelected()) {
        resultsLayout.show(pnlResults, "eval");
        pnlSaveAndLoadScores.setVisible(true);
      } else {
        resultsLayout.show(pnlResults, "regular");
        pnlSaveAndLoadScores.setVisible(false);
      }
      setEvalTableColumnSizes();
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

                // if this is a brand new search - set up the
                // eval scores array
                if (evalScores.length==0 && queryResults.length>0) {
                  clearEvalResults();
                }

        } catch (Exception e) {
            errorMsg(parent, "An error occurred while searching.\n\n" + e.toString());
        }
        status.setText("Displaying...");
        status.update(status.getGraphics());

        showResults(formatResults(queryResults));

        setEvalTableColumnSizes();
    }

    private boolean shouldWeUpdateScores(int oldIdIndex, boolean showPrompt) {
      if (loadedQrels==null) { return false; }

      boolean retVal=false;
      int updateScores=JOptionPane.NO_OPTION;
      if (showPrompt) {
        updateScores=JOptionPane.showConfirmDialog(parent, "Judgments have been modified.\nDo you want to save these?", "Judgments Modified", JOptionPane.YES_NO_OPTION);
      }
      if (!showPrompt || updateScores==JOptionPane.YES_OPTION) {
        String oldQueryID=(String)cboCurrentlyLoadedQueries.getItemAt(oldIdIndex);
        java.util.HashMap mappedScores=(java.util.HashMap)loadedQrels.get(oldQueryID);
        if (mappedScores==null) {
          mappedScores=new java.util.HashMap();
        }

        try {
          for (int i=0; (i < queryResults.length) && (i < evalScores.length) && (mappedScores!=null); i++) {
            String thisDocID = (index.document(queryResults[i].ind));
            Double currentMapping=(Double)mappedScores.get(thisDocID);
            if (currentMapping!=null && evalScores[i]==null) {
              // clear this mapping
              mappedScores.remove(thisDocID);
            } else {
              // set the mapping
              mappedScores.put(thisDocID, evalScores[i]);
            }
          } // end for (int i=0; (i < evalScores.length) && (queryMap!=null); i++)
        } catch (java.lang.Exception ex) {
        }

        loadedQrels.put(oldQueryID, mappedScores);
        retVal=true;
        qrelsChangedGlobally=true;
      }
      return retVal;
    }

    private void setScoresForQuery() {
      int idIndex=cboCurrentlyLoadedQueries.getSelectedIndex();

      if (idIndex < 1) { return; }
      // clear any eval scores
      clearEvalResults();

      String newQueryID=(String)cboCurrentlyLoadedQueries.getItemAt(idIndex);
      java.util.HashMap queryMap=(java.util.HashMap)loadedQrels.get(newQueryID);
      if (queryMap==null) {
        return;
      }

      int numScoresSet=0;
      try {
        for (int i=0; (i < queryResults.length) && (i < evalScores.length) && (queryMap!=null); i++) {
          String thisDocID = (index.document(queryResults[i].ind));
          Double thisVal=(Double)queryMap.get(thisDocID);
          if (thisVal!=null) {
            evalScores[i]=thisVal;
            numScoresSet++;
          } else {
            // clear this eval from the current results set
            evalScores[i]=null;
          } // end if (thisVal!=null)
        } // end for (int i=0; (i < evalScores.length) && (queryMap!=null); i++)
      } catch (java.lang.Exception ex) {
        numScoresSet=0;
      }

      if (numScoresSet>0) {
        // qrelsChangedGlobally=true;
        // qrelsChangedCurrentID=false;
        // btnSaveEvalResults.setVisible(true);
        showResults(formatResults(queryResults));
      }

    }

    private void loadInNewScores(int newIdIndex) {
      if (loadedQrels==null) { return; }

      // clear any eval scores
      clearEvalResults();

      if (newIdIndex==0) { return; }

      String newQueryID=(String)cboCurrentlyLoadedQueries.getItemAt(newIdIndex);
      java.util.HashMap queryMap=(java.util.HashMap)loadedQrels.get(newQueryID);
      if (queryMap==null) {
        return;
      }

      int numScoresSet=0;
      try {
        for (int i=0; (i < queryResults.length) && (i < evalScores.length) && (queryMap!=null); i++) {
          String thisDocID = (index.document(queryResults[i].ind));
          Double thisVal=(Double)queryMap.get(thisDocID);
          if (thisVal!=null) {
            evalScores[i]=thisVal;
            numScoresSet++;
          } else {
            // clear this eval from the current results set
            evalScores[i]=null;
          } // end if (thisVal!=null)
        } // end for (int i=0; (i < evalScores.length) && (queryMap!=null); i++)
      } catch (java.lang.Exception ex) {
        numScoresSet=0;
      }

      // upon load - clear our flags
      qrelsChangedCurrentID=false;
      if (!qrelsChangedGlobally) {
        btnSaveEvalResults.setVisible(false);
      }

      if (numScoresSet>0) {
        showResults(formatResults(queryResults));
      }

    }

    private void doAddNewQueryID() {
      // popup dialog box for new query ID
      String newQueryID=JOptionPane.showInputDialog(parent,
              "Enter new Query ID:\n(Note, this will clear any existing judgments\n and make the new QueryID the active one.)",
              "Enter Query ID",
              JOptionPane.QUESTION_MESSAGE);

      if (newQueryID==null) { return; }

      newQueryID=newQueryID.replaceAll("\\s", "_");

      if (loadedQrels==null) {
        loadedQrels=new java.util.HashMap();
      }

      // ensure it doesn't already exist in loaded queries
      java.util.HashMap checkExisting=(java.util.HashMap)loadedQrels.get(newQueryID);
      if (checkExisting!=null) {
        // already exists
        JOptionPane.showMessageDialog(parent,
                "The new Query ID you entered already exists!",
                "Query ID Exists",
                JOptionPane.WARNING_MESSAGE);
        return;
      }

      loadedQrels.put(newQueryID, new java.util.HashMap());

      // reset the combo box and make this one active
      java.util.Set keySet=loadedQrels.keySet();
      String[] queryIDs=(String[])keySet.toArray(new String[keySet.size()]);

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

      // add in the "-- none --" to the top
      java.util.ArrayList qList=new java.util.ArrayList(Arrays.asList(queryIDs));
      qList.add(0, "-- none --");
      queryIDs=(String[])qList.toArray(new String[qList.size()]);

      // set the scores in the cboLoadedQueries
      cboCurrentlyLoadedQueries.setModel(new javax.swing.DefaultComboBoxModel(queryIDs));

      for (int i=0; i < queryIDs.length; i++) {
        if (newQueryID.compareTo(queryIDs[i])==0) {
          cboCurrentlyLoadedQueries.setSelectedIndex(i);
          return;
        }
      }

      // clear any eval scores
      clearEvalResults();
    }

    private void clearEvalResults() {
      evalScores=new Double[queryResults.length];
      for (int i=0; i < evalScores.length; i++) {
        evalScores[i]=null;
      }
      qrelsChangedCurrentID=false;
      if (!qrelsChangedGlobally) {
        btnSaveEvalResults.setVisible(false);
      }
      if (evalScores.length > 0) {
        showResults(formatResults(queryResults));
      }
    }

    private void doLoadEvalScores() {
      // show the load dialog box
      JFileChooser fileChooser=new JFileChooser();
      fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
      fileChooser.setMultiSelectionEnabled(false);
      int retVal=fileChooser.showOpenDialog(this);
      if (retVal==JFileChooser.APPROVE_OPTION) {
        java.io.File selectedFile=fileChooser.getSelectedFile();
        String fullPath=selectedFile.getAbsolutePath();
        EvalScoreSaveDialog dlgSaveScores=new EvalScoreSaveDialog(parent);
        if (dlgSaveScores.loadEvalFile(fullPath)) {
          if (dlgSaveScores.loadedEvalFile==null || (dlgSaveScores.loadedEvalFile.size()==0)) {
            JOptionPane.showMessageDialog(parent, "No Judgments were loaded", "No Judgments loaded...", JOptionPane.INFORMATION_MESSAGE);
            return;
          }

          // upon load - clear our flags
          qrelsChangedGlobally=false;
          qrelsChangedCurrentID=false;
          btnSaveEvalResults.setVisible(false);

          loadedQrels=dlgSaveScores.loadedEvalFile;

          java.util.Set keySet=loadedQrels.keySet();
          String[] queryIDs=(String[])keySet.toArray(new String[keySet.size()]);

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

          // add in the "-- none --" to the top
          java.util.ArrayList qList=new java.util.ArrayList(Arrays.asList(queryIDs));
          qList.add(0, "-- none --");
          queryIDs=(String[])qList.toArray(new String[qList.size()]);

          // set the scores in the cboLoadedQueries
          cboCurrentlyLoadedQueries.setModel(new javax.swing.DefaultComboBoxModel(queryIDs));
          clearEvalResults();
          cboCurrentlyLoadedQueries.setSelectedIndex(0);
        }
      } // end if (retVal==JFileChooser.APPROVE_OPTION)
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

    public void linkClickAction(String href) {

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

        setEvalTableColumnSizes();
    }

    private String formatResults(IndexedReal[] results) {
        StringBuffer buf = new StringBuffer();

        EvalTableModel mdlEvalTable=(EvalTableModel)tblEvalResults.getModel();
        mdlEvalTable.clearAll();

        try {
            //    System.out.println("Getting dM for "+searchIndex);
            DocumentManager dm = index.docManager(1); /// first doc

            buf.append("<ol style='list-style: outside; margin-left: 35;'>");
            mdlEvalTable.setRankOffset(firstshown);
            for (int i=firstshown-1;i<results.length && i<firstshown+guiSettings.numresults-1;i++) {
                mdlEvalTable.addItem(queryResults[i], getTitle(queryResults[i]), getSnippet(queryResults[i]), evalScores[i]);

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

            tblEvalResults.setModel(mdlEvalTable);
            tblEvalResults.repaint();
        }
        catch (Exception e) {
            errorMsg(parent, "An error occurred while displaying the results.\n\n" + e.toString());
        }
        return buf.toString();
    }

    private String getTitle(IndexedReal result) {
      String retVal="(no title)";
      try {
        DocumentManager dm = index.docManager(1); /// first doc
        String docid = (index.document(result.ind));
        retVal="<a href=\"file:document:" + docid + "\">";
        String title = dm.docElement(docid,"TITLE");
        if (title == null)
            title = dm.docElement(docid,"HEADLINE");
        if (title != null) {
            if (title.length() > 100) title = title.substring(0, 100);
        } else {
            title=docid;
        }
        retVal+=title+"</a>";
      } catch (Exception ex) {
        return "(error getting title)";
      }
      return retVal;
    }

    private String getSnippet(IndexedReal result) {
      StringBuffer buf = new StringBuffer();
      try {
        DocumentManager dm = index.docManager(1); /// first doc
        String docid = (index.document(result.ind));
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
      } catch (Exception e) {
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

    private boolean checkSaveEvalResults(boolean showPrompt) {

      // check to make sure if we have any items to save
      // to memory before saving to disk
      if (qrelsChangedCurrentID) {
        int whichQueryIndex=cboCurrentlyLoadedQueries.getSelectedIndex();
        if (whichQueryIndex > 0) {
          if (!shouldWeUpdateScores(whichQueryIndex, false)) {
            return false;
          }
        }
      }

      // ensure that at least one qrel was modified
      if (!qrelsChangedGlobally) { return true; }

      // prompt the user to see if they want to save or not
      int valSaveEval=JOptionPane.NO_OPTION;
      if (showPrompt) {
        valSaveEval=JOptionPane.showConfirmDialog(
            this,
            "You have judgment scores and queries entered that are not saved. Would you\n" +
            "like to save them now? (Note that if they are not saved, they\n" +
            "will be lost)",
            "Save Eval Scores?",
            JOptionPane.YES_NO_OPTION
            );
      }
      if (!showPrompt || valSaveEval==JOptionPane.YES_OPTION) {
        // bring up eval score save dialog box
        EvalScoreSaveDialog dlgSaveScores=new EvalScoreSaveDialog(parent);

        String[] resultIDs=pullResultIDs();
        dlgSaveScores.showDialogBox(loadedQrels);
        if (dlgSaveScores.okWasClicked()) {
          dlgSaveScores.setVisible(false);

          // DON'T clear the scores
          // for (int i=0; (i < evalScores.length && hasResults); i++) {
          //  evalScores[i]=null;
          //}

          // ((EvalTableModel)tblEvalResults.getModel()).clearScores();
          qrelsChangedCurrentID=false;
          qrelsChangedGlobally=false;
          btnSaveEvalResults.setVisible(false);

          return true;
        }
        return false;
      }
      return true;
    }

    private boolean hasEvalResults() {
      boolean hasResults=false;
      for (int i=0; (i < evalScores.length && !hasResults); i++) {
        if (evalScores[i]!=null) { return true; }
      }
      return hasResults;
    }

    private String[] pullResultIDs() {
      String[] retVal=new String[queryResults.length];
      if (queryResults.length > 0) {
        try {
          DocumentManager dm = index.docManager(1); /// first doc
          for (int i=0; i < queryResults.length; i++) {
            retVal[i]=(index.document(queryResults[i].ind));
          }
        } catch (Exception ex) {
          retVal=new String[0];
        }
      }
      return retVal;
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

    private void showHelpScreen() {
      // bring up the help dialog box
      LemurRetHelp helpScreen=new LemurRetHelp();
      helpScreen.displayHelp();
    }

}

