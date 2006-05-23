/*==========================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software (and below), and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
package lemurproject.lemur.ui;

import java.io.Serializable;

public class GuiSettings implements Serializable {
  public boolean loadind;
  public boolean openwin;
  public boolean highlight;
  public boolean showmarkup;
  public boolean appendfile;
  public String retmethod;
  public int numresults;
    // InQuery ret parms
  public double belief;
  public int fbterms;
  public double fbcoef;
    // Okapi parms
  public double bm25k1;
  public double bm25b;
  public double bm25k3;
  public double bm25qtf;
  public int feedback;
}

