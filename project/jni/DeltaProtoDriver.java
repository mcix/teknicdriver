import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.event.ActionListener;

public class DeltaProtoDriver {
   static {
      System.loadLibrary("deltateknic"); 
   }

   private native long openPort( int portNum );
   private native void closePort( long pointer );
   private native void moveTo( long pointer, int x, int y);

   private native void setVar( long pointer, int x);
   private native int getVar( long pointer );
 
   // Test Driver
   public static void main(String[] args) {
      DeltaProtoDriver dpd= new DeltaProtoDriver();  // Invoke native method

      int port= 4;

      if( args.length > 0 ) {
        port= Integer.valueOf( args[0] );
      }

      long p= dpd.openPort( port );


      JFrame frame = new JFrame("Simple GUI"); 
      frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); 

      JPanel panel = new JPanel();
      frame.add(panel);


      JButton buttonx = new JButton("move to 100 100");
      panel.add(buttonx);
      buttonx.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent evt) {
              System.out.println("move to 100 100");
              dpd.moveTo(p, 1000, 1000);
          }
      });


      JButton buttonclose = new JButton("close");
      panel.add(buttonclose);
      buttonclose.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent evt) {
              System.out.println("close");
              dpd.closePort( p );
              frame.dispatchEvent(new WindowEvent(frame, WindowEvent.WINDOW_CLOSING));
          }
      });

      JButton buttonget = new JButton("get");
      panel.add(buttonget);
      buttonget.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent evt) {
              System.out.println("getVar " + dpd.getVar( p ) );
          }
      });

      System.out.println("setVar 123" );
      dpd.setVar( p, 123 );
      System.out.println("getVar " + dpd.getVar( p ) );

      System.out.println("setVar 456" );
      dpd.setVar( p, 456 );
      System.out.println("getVar " + dpd.getVar( p ) );

      frame.setLocationRelativeTo(null); 
      frame.pack(); 
      frame.setVisible(true); 

   }
}