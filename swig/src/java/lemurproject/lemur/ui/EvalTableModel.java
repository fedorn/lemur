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

import lemurproject.lemur.*;
import javax.swing.table.AbstractTableModel;

/**
 * Table model for the score evaluation table
 * 
 */
public class EvalTableModel extends AbstractTableModel {
  private static  String[] fieldColumnNames= {"Rank", "Eval. Score", "Score", "Title", "Snippet" };
  
  private class EvalTableRowValue {
    public Double evalScore;
    public IndexedReal result;
    public String title;
    public String snippet;
    public EvalTableRowValue(IndexedReal _result, String _title, String _snippet) {
      this.evalScore=null;
      this.result=_result;
      this.title=_title;
      this.snippet=_snippet;
    }
  }
  
  private int rankOffset;
  private java.util.Vector results;
  
  public EvalTableModel() {
    results=new java.util.Vector();
  }

  public String getColumnName(int col) {
    return fieldColumnNames[col];
  }
 
  public int getRowCount() {
    return results.size();
  }

  public int getColumnCount() {
    return 5;
  }
  
  public void setRankOffset(int newOffset) {
    rankOffset=newOffset;
  }

  public int getRankOffset() {
    return rankOffset;
  }
  
  public void clearAll() {
    results.clear();
  }
  
  public void clearScores() {
    int vecSize=results.size();
    for (int i=0; i < vecSize; i++) {
      ((EvalTableRowValue)results.get(i)).evalScore=null;
    }
    this.fireTableDataChanged();
  }
  
  public void addItem(IndexedReal _result, String _title, String _snippet) {
    results.add(new EvalTableRowValue(_result, _title, _snippet));
  }

  public void addItem(IndexedReal _result, String _title, String _snippet, Double _evalScore) {
    EvalTableRowValue thisItem=new EvalTableRowValue(_result, _title, _snippet);
    thisItem.evalScore=_evalScore;
    results.add(thisItem);
  }
  
  public void setEvalScore(int rowIndex, Double newScore) {
    if (results.size() < (rowIndex+1)) {
      return;
    }
    
   ((EvalTableRowValue)results.get(rowIndex)).evalScore=newScore;
  }
  
  public boolean isCellEditable(int row, int col) {
    if (col==1) { return true; }
    return false;
  }
  
  public void setValueAt(Object value, int rowIndex, int columnIndex) {
    if (columnIndex==1) {
      try {
        String thisValString=((String)value).trim();
        if (thisValString.length()==0) {
          setEvalScore(rowIndex, null);
        } else {
          Double thisVal=Double.parseDouble(thisValString);
          setEvalScore(rowIndex, thisVal);
        }
      } catch (NumberFormatException ex) {
        // leave the value alone if not allowed
      }
    }
  }

  public Object getValueAt(int rowIndex, int columnIndex) {
    if (results.size() < (rowIndex+1)) {
      return null;
    }
    
    if (columnIndex <0 || columnIndex > 4) {
      return null;
    }
    
    switch (columnIndex) {
      case 0: {
        return new Integer(rowIndex+rankOffset);
      }
      case 1: {
        Double evalScore=((EvalTableRowValue)results.get(rowIndex)).evalScore;
        if (evalScore==null) {
          return "";
        } else {
          return evalScore.toString();
        }
      }
      case 2: {
        String score = Double.toString(((EvalTableRowValue)results.get(rowIndex)).result.val);
        // set precision to 3 decimal points at most
        int scoreend = score.indexOf(".") +4;
        if (scoreend < score.length())
            score = score.substring(0,scoreend);
        return new Double(score);
      }
      case 3: {
        return ((EvalTableRowValue)results.get(rowIndex)).title;
      }
      case 4: {
        return ((EvalTableRowValue)results.get(rowIndex)).snippet;
      }
      default: {
        return null;
      }
    }
  }
  

}
