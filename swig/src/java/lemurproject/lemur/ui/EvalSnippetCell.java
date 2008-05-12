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
import javax.swing.*;
import javax.swing.table.*;

/**
 * Snippet cell for eval score table - sets the cell to 
 * html and wraps string in body tags
 */
public class EvalSnippetCell extends JEditorPane implements TableCellRenderer {
  public EvalSnippetCell() {
    super();
    this.setEditable(false);
    this.setContentType("text/html");
  }

  public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
    String wrappedString="<html><body>" + (String)value + "</body></html>";
    this.setText(wrappedString);
    return this;
  }

}
