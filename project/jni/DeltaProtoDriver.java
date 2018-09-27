package nl.bytesoflife;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.event.ActionListener;



public class DeltaProtoDriver {
   static {
      System.loadLibrary("deltateknic"); 
   }

   private static final int TRUE=1;
   private static final int FALSE=0;

   private native long openPort( int portNum );
   private native void home( long pointer );
   private native void closePort( long pointer );
   private native void moveTo( long pointer, int x, int y, int acc, int vel, int release);
   private native void moveToDelay( long pointer, int x, int y, int acc, int vel, int release, int delayX, int delayY);
   private native void moveToX( long pointer, int x, int acc, int vel, int release);
   private native void moveToY( long pointer, int y, int acc, int vel, int release);
   private native void release( long pointer, int release);
   private native double getPositionX( long pointer );
   private native double getPositionY( long pointer );
   private native int getMaxAcc( long pointer );
   private native int getMaxVel( long pointer );
   private native int getMinAcc( long pointer );
   private native int getMinVel( long pointer );

   protected int minAcc;
   protected int minVel;
   protected int maxAcc;
   protected int maxVel;

   public int getAccByPercentage( int perc ) {
      Double x= ((double)(maxAcc - minAcc) / 100);
      x= x * perc;
      return x.intValue() + minAcc;
   }

   public int getVelByPercentage( int perc ) {
      Double x= ((double)(maxVel - minVel) / 100);
      x= x * perc;
      return x.intValue() + minVel;
   }
 
   // Test Driver
   public static void main(String[] args) {
      DeltaProtoDriver dpd= new DeltaProtoDriver();  // Invoke native method

      int port= 4;

      if( args.length > 0 ) {
        port= Integer.valueOf( args[0] );
      }

      long p= dpd.openPort( port );

      dpd.home(p);
      dpd.maxAcc= dpd.getMaxAcc(p);
      dpd.maxVel= dpd.getMaxVel(p);
      dpd.minAcc= dpd.getMinAcc(p);
      dpd.minVel= dpd.getMinVel(p);

      JFrame frame = new JFrame("Simple GUI"); 
      frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); 

      JPanel panel = new JPanel();
      frame.add(panel);

      JTextField tfx= new JTextField("0", 10);
      panel.add(tfx);
      JTextField tfy= new JTextField("0", 10);
      panel.add(tfy);

      JSlider slider= new JSlider(JSlider.VERTICAL, 0, 100, 0);
      panel.add(slider);

      JButton buttonx = new JButton("move to");
      panel.add(buttonx);
      buttonx.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent evt) {
              
              int x= Integer.valueOf(tfx.getText());
              int y= Integer.valueOf(tfy.getText());

              int acc= dpd.getAccByPercentage( (int)slider.getValue() );
              int vel= dpd.getVelByPercentage( (int)slider.getValue() );

              System.out.println("move to " + x + " " + y);

              dpd.moveTo(p, x, y, acc, vel, FALSE, 500, 0);
          }
      });

      JButton buttonget = new JButton("getPosition");
      panel.add(buttonget);
      buttonget.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent evt) {
              System.out.println("getPositionX " + dpd.getPositionX( p ) );
              System.out.println("getPositionY " + dpd.getPositionY( p ) );
          }
      });

      JButton buttonrelease = new JButton("release");
      panel.add(buttonrelease);
      buttonrelease.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent evt) {
              dpd.release(p, TRUE);
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

      frame.setLocationRelativeTo(null); 
      frame.pack(); 
      frame.setVisible(true); 

   }
}