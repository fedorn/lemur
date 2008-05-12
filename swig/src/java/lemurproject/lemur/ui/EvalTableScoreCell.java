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

/**
 * Editable score cell for the score evaluation table
 */
public class EvalTableScoreCell extends AbstractCellEditor implements javax.swing.table.TableCellEditor {
  JPanel mainPanel=new JPanel();
  JTextField component=new JTextField();
  
  public EvalTableScoreCell() {
    super();
    component.setBorder(BorderFactory.createEtchedBorder(javax.swing.border.EtchedBorder.LOWERED));
    mainPanel.setLayout(new java.awt.BorderLayout());
    mainPanel.add(component, java.awt.BorderLayout.NORTH);
  }
  
  public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column) {
    if (value==null) {
      component.setText("");
    } else {
      component.setText((String)value);
    }
    return mainPanel;
  }

  public Object getCellEditorValue() {
    return component.getText();
  }

}
