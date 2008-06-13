/*
 * LemurRetHelp.java
 *
 * Created on June 13, 2008, 12:05 AM
 */

package lemurproject.lemur.ui;

import java.io.IOException;
import javax.swing.event.HyperlinkEvent;

/**
 *
 * @author  mhoy
 */
public class LemurRetHelp extends javax.swing.JFrame implements javax.swing.event.HyperlinkListener {
  
  /** Creates new form LemurRetHelp */
  public LemurRetHelp() {
    initComponents();
    txtMain.addHyperlinkListener(this);
  }
  
  private void initComponents() {

    pnlScrollPane = new javax.swing.JScrollPane();
    txtMain = new javax.swing.JEditorPane();

    setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
    setTitle("Lemur Retrieval Help");

    pnlScrollPane.setPreferredSize(new java.awt.Dimension(480, 600));

    txtMain.setPreferredSize(new java.awt.Dimension(480, 600));
    pnlScrollPane.setViewportView(txtMain);

    getContentPane().add(pnlScrollPane, java.awt.BorderLayout.CENTER);

    pack();
  }
  
  private javax.swing.JScrollPane pnlScrollPane;
  private javax.swing.JEditorPane txtMain;

  public void displayHelp() {
    txtMain.setEditable(false);
    txtMain.setContentType("text/html");
    
    this.setPreferredSize(new java.awt.Dimension(400, 600));
    this.setVisible(true);
    try {
      txtMain.setPage(LemurRetHelp.class.getResource("properties/docs/LemurRet.html"));
    } catch (java.io.IOException ioEx) {
    }
  }
  
  public void hyperlinkUpdate(HyperlinkEvent e) {
    if (e.getEventType()==HyperlinkEvent.EventType.ACTIVATED) {
      try {
        txtMain.setPage(e.getURL());
      } catch (java.io.IOException ioEx) {
      }
    }
  }
  
}
