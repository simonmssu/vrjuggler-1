/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * VR Juggler is (C) Copyright 1998-2011 by Iowa State University
 *
 * Original Authors:
 *   Allen Bierbaum, Christopher Just,
 *   Patrick Hartling, Kevin Meinert,
 *   Carolina Cruz-Neira, Albert Baker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *************** <auto-copyright.pl END do not edit this line> ***************/

package org.vrjuggler.tweek.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;


/**
 * Simple about box for the PlxView GUI.  This was generated by JBuilder 4.
 *
 * @version $Revision$
 */
public class AboutBox extends JDialog implements ActionListener
{
   JPanel panel1 = new JPanel();
   JPanel panel2 = new JPanel();
   JPanel insetsPanel1 = new JPanel();
   JPanel insetsPanel2 = new JPanel();
   JPanel insetsPanel3 = new JPanel();
   JButton button1 = new JButton();
   JLabel imageControl1 = new JLabel();
   ImageIcon imageIcon;
   JLabel label1 = new JLabel();
   JLabel label2 = new JLabel();
   JLabel label3 = new JLabel();
   JLabel label4 = new JLabel();
   BorderLayout borderLayout1 = new BorderLayout();
   BorderLayout borderLayout2 = new BorderLayout();
   FlowLayout flowLayout1 = new FlowLayout();
   GridLayout gridLayout1 = new GridLayout();
   String version = "1.5";

   public AboutBox(Frame parent) {
      super(parent);
      enableEvents(AWTEvent.WINDOW_EVENT_MASK);
      try {
         jbInit();
      }
      catch(Exception e) {
         e.printStackTrace();
      }
      pack();
      setLocationRelativeTo(parent);
   }

   private void jbInit() throws Exception  {
      try
      {
         imageIcon = new ImageIcon(ClassLoader.getSystemResource("org/vrjuggler/tweek/gui/tweek.gif"));
         imageControl1.setIcon(imageIcon);
      }
      catch (NullPointerException e)
      {
         /* Do nothing. */;
      }

      this.setTitle("About");
      setResizable(false);
      panel1.setLayout(borderLayout1);
      panel2.setLayout(borderLayout2);
      insetsPanel1.setLayout(flowLayout1);
      insetsPanel2.setLayout(flowLayout1);
      insetsPanel2.setBorder(new EmptyBorder(10, 10, 10, 10));
      gridLayout1.setRows(4);
      gridLayout1.setColumns(1);
      label1.setText("Tweek Bean Loader");
      label2.setText(version);
      label3.setText("By The VR Juggler Team");
      insetsPanel3.setLayout(gridLayout1);
      insetsPanel3.setBorder(new EmptyBorder(10, 60, 10, 10));
      button1.setText("Ok");
      button1.addActionListener(this);
      insetsPanel2.add(imageControl1, null);
      panel2.add(insetsPanel2, BorderLayout.WEST);
      this.getContentPane().add(panel1, null);
      insetsPanel3.add(label1, null);
      insetsPanel3.add(label2, null);
      insetsPanel3.add(label3, null);
      insetsPanel3.add(label4, null);
      panel2.add(insetsPanel3, BorderLayout.CENTER);
      insetsPanel1.add(button1, null);
      panel1.add(insetsPanel1, BorderLayout.SOUTH);
      panel1.add(panel2, BorderLayout.NORTH);
   }

   protected void processWindowEvent(WindowEvent e) {
      if (e.getID() == WindowEvent.WINDOW_CLOSING) {
         cancel();
      }
      super.processWindowEvent(e);
   }

   void cancel() {
      dispose();
   }

   public void actionPerformed(ActionEvent e) {
      if (e.getSource() == button1) {
         cancel();
      }
   }
}
