package lemurproject.lemur.ui;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Vector;

import javax.swing.JTextArea;
import lemurproject.indri.*;

public class JBuildIndex extends Thread {
    static {
        System.loadLibrary("lemur_jni");
    }

    private JTextArea messages;
    private String param;
    private LemurIndexGUI parent;

    public JBuildIndex(LemurIndexGUI gui, JTextArea mesgarea) 
    {
        messages = mesgarea;
        param = "";
        parent = gui;
    }

    public void setParam(String pfile) 
    {
        param = pfile;
    }
    
    public void run() {
        while (true) {
            try {
                //System.err.println("Thread waiting");
                synchronized (this) 
                    {
                        wait();
                    }
            } catch (InterruptedException ex) {
                //System.out.println(ex);
                // ignore
            } try {
            		this.buildIndex(param);			
            		parent.buildDone();
            } catch (Exception e) {
                displayMessage("ERROR WHILE BUILDING: \n" + e.toString());
                parent.buildFailed();
            }
        }
    }

    public void displayMessage(String mesg) 
    {
        messages.append(mesg + "\n");
        try 
            {
                int l = messages.getText().length();
                messages.scrollRectToVisible(messages.modelToView(l));
            } 
        catch (javax.swing.text.BadLocationException exc) 
            {
            }
    }
    
    public native void buildIndex(String paramsfile);

}
