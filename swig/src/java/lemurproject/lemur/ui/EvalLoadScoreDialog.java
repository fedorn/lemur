/*
 * EvalLoadScoreDialog.java
 *
 */

package lemurproject.lemur.ui;

/**
 *
 * @author  mhoy
 */
public class EvalLoadScoreDialog extends javax.swing.JDialog {

  private String[] SetQueryIDs;

  public boolean OKWasPressed=false;
  public String selectedValue="";

  /** Creates new form EvalLoadScoreDialog */
  public EvalLoadScoreDialog(java.awt.Frame parent, boolean modal, String[] queryIDs) {
    super(parent, modal);
    SetQueryIDs=queryIDs;
    initComponents();
    OKWasPressed=false;
  }

  private void initComponents() {

    lblChoose = new javax.swing.JLabel();
    cboQueryIDs = new javax.swing.JComboBox(SetQueryIDs);
    pnlButtons = new javax.swing.JPanel();
    jSeparator1 = new javax.swing.JSeparator();
    btnOK = new javax.swing.JButton();
    btnCancel = new javax.swing.JButton();

    setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);

    lblChoose.setText("Available Query IDs:");
    getContentPane().add(lblChoose, java.awt.BorderLayout.WEST);

    getContentPane().add(cboQueryIDs, java.awt.BorderLayout.CENTER);

    pnlButtons.setLayout(new java.awt.BorderLayout());
    pnlButtons.add(jSeparator1, java.awt.BorderLayout.NORTH);

    btnOK.setText("OK");
    btnOK.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        btnOKActionPerformed(evt);
      }
    });
    pnlButtons.add(btnOK, java.awt.BorderLayout.WEST);

    btnCancel.setText("Cancel");
    btnCancel.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        btnCancelActionPerformed(evt);
      }
    });
    pnlButtons.add(btnCancel, java.awt.BorderLayout.EAST);

    getContentPane().add(pnlButtons, java.awt.BorderLayout.SOUTH);
    
    this.setMinimumSize(new java.awt.Dimension(280, 88));
    this.setPreferredSize(new java.awt.Dimension(280, 88));

    pack();
  }

  private void btnOKActionPerformed(java.awt.event.ActionEvent evt) {
    if (cboQueryIDs.getSelectedIndex() >= 0) {
      OKWasPressed=true;
      selectedValue=(String)cboQueryIDs.getItemAt(cboQueryIDs.getSelectedIndex());
    }
    this.setVisible(false);
  }

  private void btnCancelActionPerformed(java.awt.event.ActionEvent evt) {
    OKWasPressed=false;
    this.setVisible(false);
  }

  private javax.swing.JButton btnCancel;
  private javax.swing.JButton btnOK;
  private javax.swing.JComboBox cboQueryIDs;
  private javax.swing.JSeparator jSeparator1;
  private javax.swing.JLabel lblChoose;
  private javax.swing.JPanel pnlButtons;

}
