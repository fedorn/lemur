/*==========================================================================
 * Copyright (c) 2008 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software (and below), and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
 */

package lemurproject.lemur.ui;

import java.awt.event.ActionEvent;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.*;
import java.awt.*;

/**
 * The dialog for saving evaluation scores in trec
 * format
 */

public class EvalScoreSaveDialog extends JDialog {

  private javax.swing.JButton btnCancel;
  private javax.swing.JButton btnFileBrowse;
  private javax.swing.JButton btnSave;
  private javax.swing.JCheckBox chkContinueQueryID;
  private javax.swing.JCheckBox chkConstrainIntScores;
  private javax.swing.JCheckBox chkOverwriteFile;
  private javax.swing.JLabel jLabel1;
  private javax.swing.JLabel jLabel2;
  private javax.swing.JLabel jLabel3;
  private javax.swing.JSeparator jSeparator1;
  private javax.swing.JSeparator jSeparator2;
  private javax.swing.JPanel pnlButtons;
  private javax.swing.JPanel pnlFileBrowser;
  private javax.swing.JPanel pnlQueryID;
  private javax.swing.JPanel pnlRunID;
  private javax.swing.JTextField txtFilename;
  private javax.swing.JTextField txtQueryID;
  private javax.swing.JTextField txtRunID;
  
  private JDialog thisDialog;
  
  private String[] resultIDs=new String[0];
  private Double[] scores=new Double[0];
  
  private boolean _okWasClicked=false;
  
  public EvalScoreSaveDialog(JFrame parent) {
    super(parent, true); // make modal
    thisDialog=this;
    buildDialog();
    
    this.addWindowFocusListener(new java.awt.event.WindowAdapter() {
      public void windowClosing(java.awt.event.WindowEvent event) {
        clearAndHide();
      }
    });
  }
  
  private void buildDialog() {
    
    java.awt.GridBagConstraints gridBagConstraints;

    pnlFileBrowser = new javax.swing.JPanel();
    jLabel1 = new javax.swing.JLabel();
    txtFilename = new javax.swing.JTextField();
    btnFileBrowse = new javax.swing.JButton();
    chkOverwriteFile = new javax.swing.JCheckBox();
    jSeparator1 = new javax.swing.JSeparator();
    pnlQueryID = new javax.swing.JPanel();
    jLabel2 = new javax.swing.JLabel();
    txtQueryID = new javax.swing.JTextField();
    chkContinueQueryID = new javax.swing.JCheckBox();
    jSeparator2 = new javax.swing.JSeparator();
    pnlRunID = new javax.swing.JPanel();
    jLabel3 = new javax.swing.JLabel();
    txtRunID = new javax.swing.JTextField();
    chkConstrainIntScores=new javax.swing.JCheckBox();
    pnlButtons = new javax.swing.JPanel();
    btnSave = new javax.swing.JButton();
    btnCancel = new javax.swing.JButton();

    setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
    setTitle("Save Evak Scores");
    getContentPane().setLayout(new java.awt.GridBagLayout());

    pnlFileBrowser.setLayout(new java.awt.BorderLayout());

    jLabel1.setText("Filename:");
    pnlFileBrowser.add(jLabel1, java.awt.BorderLayout.WEST);

    txtFilename.setColumns(24);
    pnlFileBrowser.add(txtFilename, java.awt.BorderLayout.CENTER);

    btnFileBrowse.setFont(new java.awt.Font("SansSerif", 0, 9));
    btnFileBrowse.setText("Browse...");
    btnFileBrowse.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        // open browse dialog
        browseForFile();
      }
    });
    pnlFileBrowser.add(btnFileBrowse, java.awt.BorderLayout.EAST);

    chkOverwriteFile.setText("Overwrite if file exists?");
    chkOverwriteFile.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        if (chkOverwriteFile.isSelected()) {
          chkContinueQueryID.setEnabled(false);
        }  else {
          chkContinueQueryID.setEnabled(true);
        }
      }
    });
    pnlFileBrowser.add(chkOverwriteFile, java.awt.BorderLayout.SOUTH);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridx = 0;
    gridBagConstraints.gridy = 0;
    gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
    getContentPane().add(pnlFileBrowser, gridBagConstraints);
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.ipady = 4;
    gridBagConstraints.insets = new java.awt.Insets(4, 0, 4, 0);
    getContentPane().add(jSeparator1, gridBagConstraints);

    pnlQueryID.setLayout(new java.awt.BorderLayout());

    jLabel2.setText("Query ID:");
    pnlQueryID.add(jLabel2, java.awt.BorderLayout.WEST);

    txtQueryID.setColumns(14);
    pnlQueryID.add(txtQueryID, java.awt.BorderLayout.CENTER);

    chkContinueQueryID.setText("Increment last query ID (if appending)");
    pnlQueryID.add(chkContinueQueryID, java.awt.BorderLayout.SOUTH);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    getContentPane().add(pnlQueryID, gridBagConstraints);
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    gridBagConstraints.ipady = 4;
    gridBagConstraints.insets = new java.awt.Insets(4, 0, 4, 0);
    getContentPane().add(jSeparator2, gridBagConstraints);

    pnlRunID.setLayout(new java.awt.BorderLayout());

    jLabel3.setText("Run ID (optional)");
    pnlRunID.add(jLabel3, java.awt.BorderLayout.WEST);
    pnlRunID.add(txtRunID, java.awt.BorderLayout.CENTER);
    
    chkConstrainIntScores.setText("Constrain Scores to Integer Values (using rounding)?");
    pnlRunID.add(chkConstrainIntScores, java.awt.BorderLayout.SOUTH);
    
    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    getContentPane().add(pnlRunID, gridBagConstraints);

    pnlButtons.setLayout(new java.awt.BorderLayout());

    btnSave.setText("Save");
    btnSave.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        _okWasClicked=saveResults();
        if (_okWasClicked) {
          thisDialog.setVisible(false);
        }
      }
    });
    pnlButtons.add(btnSave, java.awt.BorderLayout.WEST);

    btnCancel.setText("Cancel");
    btnCancel.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        clearAndHide();
      }
    });
    pnlButtons.add(btnCancel, java.awt.BorderLayout.EAST);

    gridBagConstraints = new java.awt.GridBagConstraints();
    gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
    gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
    getContentPane().add(pnlButtons, gridBagConstraints);

    pack();
  }
  
  private void setConstraints(GridBagConstraints gbc, int gridx, int gridy, int columnSpan, int anchor, int fill) {
    gbc.gridx=gridx; gbc.gridy=gridy;
    gbc.gridwidth=columnSpan;
    gbc.anchor=anchor;
    gbc.fill=fill;
  }
  
  private void browseForFile() {
    JFileChooser fileChooser=new JFileChooser();
    fileChooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
    fileChooser.setMultiSelectionEnabled(false);
    int retVal=fileChooser.showOpenDialog(this);
    if (retVal==JFileChooser.APPROVE_OPTION) {
      java.io.File selectedFile=fileChooser.getSelectedFile();
      String fullPath=selectedFile.getAbsolutePath();
      txtFilename.setText(fullPath);
    } // end if (retVal==JFileChooser.APPROVE_OPTION)
  }
  
  public void showDialogBox(String[] _resultIDs, Double[] _evalScores) {
    resultIDs=_resultIDs;
    scores=_evalScores;
    _okWasClicked=false;
    this.setSize(new java.awt.Dimension(420,240));
    this.setVisible(true);
  }
  
  public void clearAndHide() {
    _okWasClicked=false;
    this.setVisible(false);
  }

  public boolean okWasClicked() {
    return _okWasClicked;
  }
  
  public boolean saveResults() {
    boolean retVal=true;
    int thisQueryID=100;
    
    // are we creating a new file, or does one exist?
    String fileName=txtFilename.getText().trim();
    if (fileName.length()==0) { return false; }
    
    java.io.File resultFile=new java.io.File(fileName);
    boolean fileExists=resultFile.exists();
    boolean overwriteFile=true;

    // get the query ID if the user filled it in
    try {
      String queryIDText=txtQueryID.getText().trim();
      if ((queryIDText.length() > 0) && !chkContinueQueryID.isSelected()) {
        thisQueryID=Integer.parseInt(queryIDText);
      }
    } catch (NumberFormatException ex) {
      thisQueryID=100;
    }

    // if it exists, are we overwriting?
    if (fileExists && chkOverwriteFile.isSelected()) {
      // yes - overwrite
      overwriteFile=true;
    } else if (fileExists) {
      overwriteFile=false;  // append to file
      
      // if appending, are we incrementing from the last known QueryID?
      if (chkContinueQueryID.isSelected()) {
        // try and obtain the last known queryID
        java.io.BufferedReader reader=null;
        try {
          reader = new java.io.BufferedReader(new java.io.FileReader(resultFile));
          String lastLine="";
          String thisLine="";
          while (thisLine!=null) {
            if (thisLine.trim().length() > 0) {
              lastLine=thisLine.trim();
            }
            thisLine=reader.readLine();
          }
          
          if (lastLine.length() > 0) {
            String[] splitPieces=lastLine.split("\t");
            if (splitPieces.length > 0) {
              try {
                thisQueryID=Integer.parseInt(splitPieces[0]) + 1;
              } catch (NumberFormatException nEx) {
                thisQueryID=100;
              }
            }
          }
        } catch (FileNotFoundException ex) {
          JOptionPane.showMessageDialog(this, "An FileNotFound Exception occured while trying to obtain the last QueryID:\n" + ex.getMessage(), "FileNotFound Exception", JOptionPane.ERROR_MESSAGE);
          retVal=false;
        } catch (IOException ioEx) {
          JOptionPane.showMessageDialog(this, "An IO Exception occured while trying to obtain the last QueryID:\n" + ioEx.getMessage(), "I/O Exception", JOptionPane.ERROR_MESSAGE);
          retVal=false;
        } finally {
          if (reader!=null) {
            try {
              reader.close();
            } catch (IOException ex) {
            }
          }
        }
      }
    }
    
    if (!retVal) { return false; }
    
    java.io.PrintWriter out=null;
    try {
      // finally write our results
      if (overwriteFile) {
        // create new file
        if (resultFile.exists()) {
          resultFile.delete();
        }
        if (resultFile.createNewFile()) {
          out = new java.io.PrintWriter(resultFile);
        }
      } else {
        // try and open for append
        out=new java.io.PrintWriter(new java.io.FileWriter(resultFile, true));
      }
      
      boolean useRounding=false;
      if (chkConstrainIntScores.isSelected()) {
        useRounding=true;
      }
      
      for (int i=0; i < resultIDs.length && i < scores.length; i++) {
        if (scores[i]!=null) {
          if (useRounding) {
            int roundVal=(int)(Math.round(scores[i]));
            out.println(thisQueryID + "\t0\t" + resultIDs[i] + "\t" + roundVal);
          } else {
            out.println(thisQueryID + "\t0\t" + resultIDs[i] + "\t" + scores[i]);
          }
        }
      }
      JOptionPane.showMessageDialog(this, "Scores saved successfully.", "Scores Saved", JOptionPane.INFORMATION_MESSAGE);
    } catch (IOException ex) {
      JOptionPane.showMessageDialog(this, "An IO Exception occured while writing the file:\n" + ex.getMessage(), "I/O Exception", JOptionPane.ERROR_MESSAGE);
      retVal=false;
    } finally {
      if (out!=null) {
        out.close();
      }
    }
    
    if (out==null) { retVal=false; }
    
    return retVal;
  }
  
}
