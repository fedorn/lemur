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
  private javax.swing.JCheckBox chkConstrainIntScores;
  private javax.swing.JCheckBox chkOverwriteFile;
  private javax.swing.JLabel jLabel1;     // text "Filename"
  private javax.swing.JLabel lblWarningFloatScores; 
  private javax.swing.JLabel jLabel3;     // text "Run ID (optional)"
  private javax.swing.JSeparator jSeparator1;
  private javax.swing.JPanel pnlButtons;
  private javax.swing.JPanel pnlFileBrowser;
  private javax.swing.JPanel pnlRunID;
  private javax.swing.JTextField txtFilename;
  private javax.swing.JTextField txtRunID;
  
  private JDialog thisDialog;
  private JFrame _parent;
  
  // hash of queryID->HashMap(DocID, Score)
  public java.util.HashMap loadedEvalFile=null;
  private java.util.HashMap resultsToSave=null;
  
  private boolean _okWasClicked=false;
  
  public EvalScoreSaveDialog(JFrame parent) {
    super(parent, true); // make modal
    thisDialog=this;
    _parent=parent;
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
    pnlRunID = new javax.swing.JPanel();
    jLabel3 = new javax.swing.JLabel();
    txtRunID = new javax.swing.JTextField();
    chkConstrainIntScores=new javax.swing.JCheckBox();
    pnlButtons = new javax.swing.JPanel();
    btnSave = new javax.swing.JButton();
    btnCancel = new javax.swing.JButton();

    setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
    setTitle("Save Eval Scores");
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

    // query IDs are now set in the input to the dialog
    // as part of the items to save
    
    pnlRunID.setLayout(new java.awt.BorderLayout());

    jLabel3.setText("Run ID (optional)");
    pnlRunID.add(jLabel3, java.awt.BorderLayout.WEST);
    pnlRunID.add(txtRunID, java.awt.BorderLayout.CENTER);
    
    JPanel pnlConstraints=new JPanel();
    pnlConstraints.setLayout(new BorderLayout());

    lblWarningFloatScores=new JLabel("<html>* <b><i>Warning</i></b>: Some evaluation programs may not accept<p>floating point judgment scores.</html>");
    pnlConstraints.add(lblWarningFloatScores, BorderLayout.SOUTH);
    lblWarningFloatScores.setVisible(false);
    
    chkConstrainIntScores.setText("Constrain Scores to Integer Values (using rounding)?");
    chkConstrainIntScores.setSelected(true);
    chkConstrainIntScores.addActionListener(new java.awt.event.ActionListener() {
        public void actionPerformed(ActionEvent e) {
          JCheckBox chkObject=(JCheckBox)e.getSource();
          if (chkObject.isSelected()) {
            lblWarningFloatScores.setVisible(false);
          } else {
            lblWarningFloatScores.setVisible(true);
          }
        }          
    });
    pnlConstraints.add(chkConstrainIntScores, BorderLayout.CENTER);
    
    pnlRunID.add(pnlConstraints, java.awt.BorderLayout.SOUTH);
    
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
  
  public void showDialogBox(java.util.HashMap querySet) {
    _okWasClicked=false;
    if (querySet==null || querySet.size()==0) {
      JOptionPane.showMessageDialog(_parent, "No evaluation scores currently exist to be saved.", "No Eval. Scores", JOptionPane.WARNING_MESSAGE);
      this.setVisible(false);
      return;
    }
    resultsToSave=querySet;
    this.setSize(new java.awt.Dimension(420,208));
    this.setVisible(true);
  }
  
  public void clearAndHide() {
    _okWasClicked=false;
    this.setVisible(false);
  }

  public boolean okWasClicked() {
    return _okWasClicked;
  }
  
  public boolean loadEvalFile(String evalFilePath) {
    boolean retVal=true;
    
    if (loadedEvalFile!=null) {
      loadedEvalFile.clear();
      loadedEvalFile=null;
    }
    
    java.io.BufferedReader reader=null;
    try {
      reader = new java.io.BufferedReader(new java.io.FileReader(evalFilePath));
      String lastLine="";
      String thisLine=reader.readLine();
      while (thisLine!=null) {
        if (thisLine.length() > 0) {
          String[] splitPieces=thisLine.split("\\s", 4);
          if (splitPieces.length==4) {
            // out.println(thisQueryID + "\t0\t" + resultIDs[i] + "\t" + roundVal);
            String thisQueryID=splitPieces[0];
            String thisResultID=splitPieces[2];
            Double thisScore=null;
            try {
              thisScore=new Double(splitPieces[3]);
            } catch (java.lang.NumberFormatException ex) {
              thisScore=null;
            }
            
            if (loadedEvalFile==null) { 
              loadedEvalFile=new java.util.HashMap();
            }
            
            java.util.HashMap queryMap=(java.util.HashMap)loadedEvalFile.get(thisQueryID);
            if (queryMap==null) {
              queryMap=new java.util.HashMap();
            }
            queryMap.put(thisResultID, thisScore);
            loadedEvalFile.put(thisQueryID, queryMap);
          } // end if (splitPieces.length==4)
        } // end if (thisLine.length() > 0)
        thisLine=reader.readLine();
      } // end while(thisLine!=null)
    } catch (FileNotFoundException ex) {
      JOptionPane.showMessageDialog(this, "An FileNotFound Exception occured while loading eval file:\n" + ex.getMessage(), "FileNotFound Exception", JOptionPane.ERROR_MESSAGE);
      if (loadedEvalFile!=null) {
        loadedEvalFile.clear();
        loadedEvalFile=null;
      }
      retVal=false;
    } catch (IOException ioEx) {
      JOptionPane.showMessageDialog(this, "An IO Exception occured while trying while loading eval file:\n" + ioEx.getMessage(), "I/O Exception", JOptionPane.ERROR_MESSAGE);
      if (loadedEvalFile!=null) {
        loadedEvalFile.clear();
        loadedEvalFile=null;
      }
      retVal=false;
    } finally {
      if (reader!=null) {
        try {
          reader.close();
        } catch (IOException ex) {
        }
      }
    }
    return retVal;
  }
  
  public boolean saveResults() {
    boolean retVal=true;
    
    // are we creating a new file, or does one exist?
    String fileName=txtFilename.getText().trim();
    if (fileName.length()==0) { return false; }
    
    java.io.File resultFile=new java.io.File(fileName);
    boolean fileExists=resultFile.exists();
    
    // get the query IDs
    java.util.Set queryIDSet=resultsToSave.keySet();
    String[] queryIDs=(String[])queryIDSet.toArray(new String[queryIDSet.size()]);

    // and sort them
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

    // if it exists, are we overwriting?
    if (fileExists && chkOverwriteFile.isSelected()) {
      // yes - overwrite
      // don't do anything
    } else if (fileExists) {
      // appending...
      // ensure we don't have conflicting query IDs
      if (loadEvalFile(fileName) && (loadedEvalFile!=null)) {
        java.util.Vector vecConflictingIDs=new java.util.Vector();
        // String workingQueryID="" + thisQueryID;
        for (int i=0; i < queryIDs.length; i++) {
          if (loadedEvalFile.containsKey(queryIDs[i])) {
            vecConflictingIDs.add(queryIDs[i]);
          }
        }

        if (vecConflictingIDs.size() > 0) {
          java.lang.StringBuffer warningString=new java.lang.StringBuffer();
          warningString.append("The following query ID(s) already exist in the output file:\n");
          java.util.Iterator vIter=vecConflictingIDs.iterator();
          boolean hasOne=false;
          while (vIter.hasNext()) {
            if ((warningString.length() % 80)==0) {
              warningString.append("\n");
            }
            String thisVID=(String)vIter.next();
            if (hasOne) { warningString.append("; "); }
            warningString.append(thisVID);
            hasOne=true;
          }
          warningString.append("\nDo you want to replace these queries?");
          int replaceVal=JOptionPane.showConfirmDialog(_parent, warningString.toString(), "Queries Already Exist in Output", JOptionPane.YES_NO_OPTION);
          if (replaceVal==JOptionPane.NO_OPTION) {
            // don't do anything
            return false;
          }
        } // end if (vecConflictingIDs.size() > 0)
          
        // not overwriting file - have to go through and
        // merge the loaded / not loaded 
        // from the loaded set into the save set
        // skipping any queryIDs that exist in the set 
        // to be saved

        java.util.Set loadedIDSet=loadedEvalFile.keySet();
        java.util.Iterator loadedIter=loadedIDSet.iterator();
        while (loadedIter.hasNext()) {
          String thisLoadedID=(String)loadedIter.next();
          java.util.HashMap loadedHashmap=(java.util.HashMap)loadedEvalFile.get(thisLoadedID);
          if (loadedHashmap!=null) {
            // if queryID contained within the save - don't overwrite
            if (!resultsToSave.containsKey(thisLoadedID)) {
              resultsToSave.put(thisLoadedID, loadedHashmap);
            }
          } // end if (loadedHashmap!=null)
        } // end while (loadedIter.hasNext())

        // and get the new keyset for the save file
        queryIDSet=resultsToSave.keySet();
        queryIDs=(String[])queryIDSet.toArray(new String[queryIDSet.size()]);
        // and sort them
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
        
      } // end if (loadEvalFile(fileName))
    } // end if (fileExists && chkOverwriteFile.isSelected())
    
    if (!retVal) { return false; }
    
    java.io.PrintWriter out=null;
    try {
      // finally write our results
      // create new file
      // do this regardless because our
      // save results will contain the complete set
      // even if we are appending at this point
      if (resultFile.exists()) {
        resultFile.delete();
      }
      if (resultFile.createNewFile()) {
        out = new java.io.PrintWriter(resultFile);
      }
      
      boolean useRounding=false;
      if (chkConstrainIntScores.isSelected()) {
        useRounding=true;
      }
      
      // now we can just write these out...
      for (int i=0; i < queryIDs.length; i++) {
        // replace any whitespace in the query ID with underscores
        writeOutQuery(queryIDs[i], useRounding, out);
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
  
  private class DocIDScore implements java.lang.Comparable {
    public String docID;
    public Double docScore;
    
    public DocIDScore(String _docID, Double _docScore) {
      docID=_docID;
      docScore=_docScore;
    }

    public int compareTo(Object o) {
      DocIDScore scoreToCompareTo=(DocIDScore)o;
      if (docScore==null) { return 1; }
      if (scoreToCompareTo==null || scoreToCompareTo.docScore==null) { return -1; }
      if (docScore < scoreToCompareTo.docScore) { return 1; }
      if (docScore > scoreToCompareTo.docScore) { return -1; }
      return 0;
    }
  } // end private class DocIDScore 
  
  private void writeOutQuery(String whichQueryID, boolean useRounding, java.io.PrintWriter out) throws java.io.IOException {
    java.util.HashMap theseQueryScores=(java.util.HashMap)resultsToSave.get(whichQueryID);
    if (theseQueryScores!=null) {
      java.util.Set scoreSet=theseQueryScores.entrySet();
      java.util.Map.Entry[] scoreSetArray=(java.util.Map.Entry[])scoreSet.toArray(new java.util.Map.Entry[scoreSet.size()]);
      DocIDScore[] scores=new DocIDScore[scoreSetArray.length];
      for (int i=0; i < scoreSetArray.length; i++) {
        scores[i]=new DocIDScore((String)scoreSetArray[i].getKey(), (Double)scoreSetArray[i].getValue());
      }
      java.util.Arrays.sort(scores);
      for (int s=0; s < scores.length; s++) {
        if (scores[s].docScore!=null) {
          if (useRounding) {
            int roundVal=(int)(Math.round(scores[s].docScore));
            out.println(whichQueryID + "\t0\t" + scores[s].docID + "\t" + roundVal);
          } else {
            out.println(whichQueryID + "\t0\t" + scores[s].docID + "\t" + scores[s].docScore);
          }
        } // end if (scores[s].docScore!=null)
      } // end for (int s=0; s < scores.length; s++)
    } /// end  if (theseQueryScores!=null)
  } // end private void writeOutQuery()
  
}
