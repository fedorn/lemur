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

import java.awt.Component;
import javax.swing.event.*;
import javax.swing.table.*;
import javax.swing.*;
/**
 *
 * A title cell for the eval table to allow opening the document
 * in a new window on click
 */
public class EvalTableTitleCell  extends JEditorPane implements TableCellRenderer {

  public EvalTableTitleCell() {
    this.setEditable(false);
    this.setContentType("text/html");
  }
  
  public Component getTableCellRendererComponent( JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column )  {
    setText((String)value);
    return this;
  }

}
