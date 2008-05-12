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

/**
 * default cell for the table for score evaluation - allows
 * automatic setting of the vertical alignment to the top
 */

public class EvalDefaultCell extends javax.swing.table.DefaultTableCellRenderer {
  public EvalDefaultCell() {
    super();
    this.setVerticalAlignment(javax.swing.SwingConstants.TOP);
  }

}
