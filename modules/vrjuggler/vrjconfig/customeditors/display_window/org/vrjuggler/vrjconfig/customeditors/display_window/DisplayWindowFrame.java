/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * VR Juggler is (C) Copyright 1998-2003 by Iowa State University
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * -----------------------------------------------------------------
 * File:          $RCSfile$
 * Date modified: $Date$
 * Version:       $Revision$
 * -----------------------------------------------------------------
 *
 *************** <auto-copyright.pl END do not edit this line> ***************/

package org.vrjuggler.vrjconfig.customeditors.display_window;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import org.vrjuggler.jccl.config.ConfigBrokerProxy;
import org.vrjuggler.jccl.config.ConfigDefinition;
import org.vrjuggler.jccl.config.ConfigElement;

import org.vrjuggler.vrjconfig.customeditors.display_window.placer.PlacerSelectionEvent;
import org.vrjuggler.vrjconfig.customeditors.display_window.placer.PlacerSelectionListener;


public class DisplayWindowFrame
   extends JInternalFrame
   implements PlacerSelectionListener
{
   private static Cursor mInverseCursor;

   private static final int LEFT_EYE;
   private static final int RIGHT_EYE;
   private static final int STEREO;

   /**
    * Constructs the inverse cursor that will be used when the mouse pointer
    * is over DisplayWindowFrame components that represent a display window
    * that hides the mouse cursor.
    */
   static
   {
      Toolkit tk = Toolkit.getDefaultToolkit();
      ClassLoader loader = DisplayWindowFrame.class.getClassLoader();
      Image cursor_img =
         new ImageIcon(loader.getResource(EditorConstants.imageBase + "/inverse-cursor.png")).getImage();
      mInverseCursor = tk.createCustomCursor(cursor_img, new Point(8, 6),
                                             "inverse");

      ConfigBrokerProxy broker = new ConfigBrokerProxy();
      ConfigDefinition vp_def = broker.getRepository().get("surface_viewport");
      java.util.Map views = vp_def.getPropertyDefinition("view").getEnums();

      LEFT_EYE  = ((Integer) views.get("Left Eye")).intValue();
      RIGHT_EYE = ((Integer) views.get("Right Eye")).intValue();
      STEREO    = ((Integer) views.get("Stereo")).intValue();
   }

   public DisplayWindowFrame(String title, Dimension resolution,
                             Dimension desktopSize, ConfigElement elt)
   {
      super(title, true, true, true, false);

      mElement = elt;
      mResolution = resolution;
      mDesktopSize = desktopSize;

      try
      {
         mViewportEditor = new ViewportPlacer(this.getSize(), mElement);
         mViewportEditor.getPlacer().addPlacerSelectionListener(this);
         jbInit();
         placeMyself();

         mViewButtonGroup.add(mViewportLeftEyeItem);
         mViewButtonGroup.add(mViewportRightEyeItem);
         mViewButtonGroup.add(mViewportStereoItem);

         boolean active = ((Boolean) mElement.getProperty("active", 0)).booleanValue();
         mWinActiveItem.setSelected(active);

         boolean border = ((Boolean) mElement.getProperty("border", 0)).booleanValue();
         mBorderItem.setSelected(border);

         boolean pointer = ((Boolean) mElement.getProperty("hide_mouse", 0)).booleanValue();
         mPointerItem.setSelected(pointer);

         boolean stereo = ((Boolean) mElement.getProperty("stereo", 0)).booleanValue();
         mWinStereoItem.setSelected(stereo);

         boolean event_src = ((Boolean) mElement.getProperty("act_as_event_source", 0)).booleanValue();
         mEventSrcItem.setSelected(event_src);
      }
      catch(Exception e)
      {
         e.printStackTrace();
      }
   }

   public void valueChanged(PlacerSelectionEvent e)
   {
      mSelectedViewport = ((ConfigElement) e.getValue());

      boolean enabled = mSelectedViewport != null;
      mViewportPropsItem.setEnabled(enabled);
      mViewportActiveItem.setEnabled(enabled);
      mViewportLeftEyeItem.setEnabled(enabled);
      mViewportRightEyeItem.setEnabled(enabled);
      mViewportStereoItem.setEnabled(enabled);

      if ( enabled )
      {
         boolean active = mSelectedViewport.getProperty("active", 0) == Boolean.TRUE;
         mViewportActiveItem.setSelected(active);

         int view = ((Integer) mSelectedViewport.getProperty("view", 0)).intValue();
         if ( view == LEFT_EYE )
         {
         }
      }
   }

   /**
    * Changes the location of this internal frame based on the new resolution.
    * @param resolution Dimension
    */
   public void updateResolution(Dimension resolution)
   {
      mResolution = resolution;
      placeMyself();
   }

   public void updateDesktopSize(Dimension size)
   {
      mDesktopSize = size;
      placeMyself();
   }

   public ConfigElement getConfiguration()
   {
      return mElement;
   }

   public void addSurfaceViewport(ConfigElement surfaceVP)
   {
      mElement.addProperty("surface_viewports", surfaceVP);
   }

   public void addSimulatorViewport(ConfigElement simVP)
   {
      // When we have a simulator viewport, we *have* to act as an event
      // source.
      mElement.setProperty("act_as_event_source", 0, Boolean.TRUE);
      mEventSrcItem.setEnabled(false);

      mElement.addProperty("simulator_viewports", simVP);
   }

   public void removeSurfaceViewport(ConfigElement surfaceVP)
   {
      mElement.removeProperty("surface_viewports", surfaceVP);
   }

   public void removeSimulatorViewport(ConfigElement simVP)
   {
      mElement.removeProperty("simulator_viewports", simVP);

      // If we have reached a point where we no longer have any simulator
      // viewports, we can allow the user to change the event source status.
      if ( mElement.getPropertyValueCount("simulator_viewports") == 0 )
      {
         mEventSrcItem.setEnabled(true);
      }
   }

   private void jbInit() throws Exception
   {
      this.addMouseListener(new DisplayWindowFrame_this_mouseAdapter(this));
      mWinPropsItem.setText("Edit Window Properties ...");
      mWinActiveItem.setText("Active");
      mWinActiveItem.addActionListener(new DisplayWindowFrame_mActiveItem_actionAdapter(this));
      mBorderItem.setText("Has border");
      mBorderItem.addActionListener(new DisplayWindowFrame_mBorderItem_actionAdapter(this));
      mPointerItem.setText("Hide mouse pointer");
      mPointerItem.addActionListener(new DisplayWindowFrame_mPointerItem_actionAdapter(this));
      mWinStereoItem.setText("Render in stereo");
      mWinStereoItem.addActionListener(new DisplayWindowFrame_mStereoItem_actionAdapter(this));
      mEventSrcItem.setText("Acts as an event source");
      mEventSrcItem.addActionListener(new DisplayWindowFrame_mEventSrcItem_actionAdapter(this));
      mViewportPropsItem.setEnabled(false);
      mViewportPropsItem.setText("Edit Viewport Properties ...");
      mViewportActiveItem.setEnabled(false);
      mViewportActiveItem.setText("Is Viewport Active?");
      mViewportActiveItem.addActionListener(new DisplayWindowFrame_mViewportActiveItem_actionAdapter(this));
      mViewportLeftEyeItem.setEnabled(false);
      mViewportLeftEyeItem.setText("Left Eye");
      mViewportLeftEyeItem.addActionListener(new DisplayWindowFrame_mViewportLeftEyeItem_actionAdapter(this));
      mViewportRightEyeItem.setEnabled(false);
      mViewportRightEyeItem.setText("Right Eye");
      mViewportRightEyeItem.addActionListener(new DisplayWindowFrame_mViewportRightEyeItem_actionAdapter(this));
      mViewportStereoItem.setEnabled(false);
      mViewportStereoItem.setText("Stereo");
      mViewportStereoItem.addActionListener(new DisplayWindowFrame_mViewportStereoItem_actionAdapter(this));
      this.addMouseMotionListener(new DisplayWindowFrame_this_mouseMotionAdapter(this));
      this.addComponentListener(new DisplayWindowFrame_this_componentAdapter(this));
      mMainPanel.setLayout(mMainPanelLayout);
      mContextMenu.add(mWinPropsItem);
      mContextMenu.add(mWinActiveItem);
      mContextMenu.add(mBorderItem);
      mContextMenu.add(mPointerItem);
      mContextMenu.add(mWinStereoItem);
      mContextMenu.add(mEventSrcItem);
      mContextMenu.addSeparator();
      mContextMenu.add(mViewportPropsItem);
      mContextMenu.add(mViewportActiveItem);
      mContextMenu.add(mViewportLeftEyeItem);
      mContextMenu.add(mViewportRightEyeItem);
      mContextMenu.add(mViewportStereoItem);
      this.getContentPane().add(mMainPanel, BorderLayout.CENTER);
      mMainPanel.add(mViewportEditor, BorderLayout.CENTER);
   }

   private Point desktopToResolution(Point desktopPoint)
   {
      float width_conv  = (float) mResolution.width / (float) mDesktopSize.width;
      float height_conv = (float) mResolution.height / (float) mDesktopSize.height;
//      System.out.println("To-resolution width conversion: " + width_conv);
//      System.out.println("To-resolution height conversion: " + height_conv);
      return new Point((int) ((float) desktopPoint.x * width_conv),
                       (int) ((float) desktopPoint.y * height_conv));
   }

   private Point resolutionToDesktop(Point resolutionPoint)
   {
      float width_conv  = (float) mDesktopSize.width / (float) mResolution.width;
      float height_conv = (float) mDesktopSize.height / (float) mResolution.height;
//      System.out.println("To-desktop width conversion: " + width_conv);
//      System.out.println("To-desktop height conversion: " + height_conv);
      return new Point((int) ((float) resolutionPoint.x * width_conv),
                       (int) ((float) resolutionPoint.y * height_conv));
   }

   private void placeMyself()
   {
      int cfg_size_x = ((Integer) mElement.getProperty("size", 0)).intValue();
      int cfg_size_y = ((Integer) mElement.getProperty("size", 1)).intValue();
      Point size = resolutionToDesktop(new Point(cfg_size_x, cfg_size_y));
      System.out.println("Setting my size to " + size);
      this.setSize(size.x, size.y);

      int cfg_origin_x = ((Integer) mElement.getProperty("origin", 0)).intValue();
      int cfg_origin_y =
         mResolution.height -
            (((Integer) mElement.getProperty("origin", 1)).intValue() +
            cfg_size_y);
      Point origin = resolutionToDesktop(new Point(cfg_origin_x,
                                                   cfg_origin_y));
      System.out.println("Setting my origin to " + origin);
      this.setLocation(origin.x, origin.y);
   }

   private Dimension mResolution = null;
   private Dimension mDesktopSize = null;
   private ConfigElement mElement = null;
   private boolean mHideMouse = false;
   private boolean mMousePressed = false;
   private ViewportPlacer mViewportEditor = null;
   private ConfigElement mSelectedViewport = null;

   private JPopupMenu mContextMenu = new JPopupMenu();
   private JMenuItem mWinPropsItem = new JMenuItem();
   private JCheckBoxMenuItem mWinActiveItem = new JCheckBoxMenuItem();
   private JCheckBoxMenuItem mBorderItem = new JCheckBoxMenuItem();
   private JCheckBoxMenuItem mPointerItem = new JCheckBoxMenuItem();
   private JCheckBoxMenuItem mWinStereoItem = new JCheckBoxMenuItem();
   private JCheckBoxMenuItem mEventSrcItem = new JCheckBoxMenuItem();
   private JPanel mMainPanel = new JPanel();
   private BorderLayout mMainPanelLayout = new BorderLayout();

   private JMenuItem mViewportPropsItem = new JMenuItem();
   private JCheckBoxMenuItem mViewportActiveItem = new JCheckBoxMenuItem();
   private JRadioButtonMenuItem mViewportRightEyeItem = new JRadioButtonMenuItem();
   private JRadioButtonMenuItem mViewportStereoItem = new JRadioButtonMenuItem();
   private JRadioButtonMenuItem mViewportLeftEyeItem = new JRadioButtonMenuItem();
   private ButtonGroup mViewButtonGroup = new ButtonGroup();

   void this_mouseEntered(MouseEvent e)
   {
      if ( mHideMouse )
      {
         this.setCursor(mInverseCursor);
      }
   }

   void this_mouseExited(MouseEvent e)
   {
      if ( mHideMouse )
      {
         this.setCursor(Cursor.getDefaultCursor());
      }
   }

   void this_mouseClicked(MouseEvent e)
   {
      if ( ! checkForPopUp(e) )
      {
      }
   }

   void this_mousePressed(MouseEvent e)
   {
      if ( ! checkForPopUp(e) )
      {
         if ( e.getButton() == MouseEvent.BUTTON1 )
         {
            System.out.println("Mouse pressed");
            mMousePressed = true;
         }
      }
   }

   void this_mouseReleased(MouseEvent e)
   {
      if ( ! checkForPopUp(e) )
      {
         if ( e.getButton() == MouseEvent.BUTTON1 )
         {
            System.out.println("Mouse released");
            mMousePressed = false;
         }
      }
   }

   private boolean checkForPopUp(MouseEvent e)
   {
      if ( e.isPopupTrigger() )
      {
         mContextMenu.show(e.getComponent(), e.getX(), e.getY());
         return true;
      }
      else
      {
         return false;
      }
   }

   void windowActiveItemActionPerformed(ActionEvent e)
   {
      mElement.setProperty("active", 0,
                           Boolean.valueOf(mWinActiveItem.isSelected()));
   }

   void borderItemActionPerformed(ActionEvent e)
   {
      mElement.setProperty("border", 0,
                           Boolean.valueOf(mBorderItem.isSelected()));
   }

   void pointerItemActionPerformed(ActionEvent e)
   {
      mElement.setProperty("hide_mouse", 0,
                           Boolean.valueOf(mPointerItem.isSelected()));
      mHideMouse = mPointerItem.isSelected();
   }

   void windowStereoItemActionPerformed(ActionEvent e)
   {
      mElement.setProperty("stereo", 0,
                           Boolean.valueOf(mWinStereoItem.isSelected()));
   }

   void eventSrcItemActionPerformed(ActionEvent e)
   {
      mElement.setProperty("act_as_event_source", 0,
                           Boolean.valueOf(mEventSrcItem.isSelected()));
   }

   void viewportActiveItemSelected(ActionEvent e)
   {
      mSelectedViewport.setProperty("active", 0,
                                    Boolean.valueOf(mViewportActiveItem.isSelected()));
   }

   void viewportLeftEyeItemSelected(ActionEvent e)
   {
      mSelectedViewport.setProperty("view", 0, new Integer(LEFT_EYE));
   }

   void viewportRightEyeItemSelected(ActionEvent e)
   {
      mSelectedViewport.setProperty("view", 0, new Integer(RIGHT_EYE));
   }

   void viewporStereoItemSelected(ActionEvent e)
   {
      mSelectedViewport.setProperty("view", 0, new Integer(STEREO));
   }

   void this_mouseDragged(MouseEvent e)
   {
   }

   void this_mouseMoved(MouseEvent e)
   {
      // XXX: Is it really necessary to change the cursor for every mouse
      // motion event?
      if ( mHideMouse )
      {
         this.setCursor(mInverseCursor);
      }
   }

   void this_componentMoved(ComponentEvent e)
   {
//      if ( ! mMousePressed )
//      {
         updatePosition(e.getComponent().getBounds());
//      }
   }

   void this_componentResized(ComponentEvent e)
   {
//      if ( ! mMousePressed )
//      {
         Rectangle bounds = e.getComponent().getBounds();
         updateSize(bounds);
         updatePosition(bounds);
//      }
   }

   private void updatePosition(Rectangle newBounds)
   {
      int y_pos = mDesktopSize.height - (newBounds.y + newBounds.height);
      Point origin = desktopToResolution(new Point(newBounds.x, y_pos));
      mElement.setProperty("origin", 0, new Integer(origin.x));
      mElement.setProperty("origin", 1, new Integer(origin.y));
   }

   private void updateSize(Rectangle newBounds)
   {
      Point size = desktopToResolution(new Point(newBounds.width,
                                                 newBounds.height));
      mElement.setProperty("size", 0, new Integer(size.x));
      mElement.setProperty("size", 1, new Integer(size.y));
      mViewportEditor.setDesktopSize(this.getSize());
   }
}

class DisplayWindowFrame_this_mouseAdapter extends MouseAdapter
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_this_mouseAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void mouseEntered(MouseEvent e)
   {
      adaptee.this_mouseEntered(e);
   }
   public void mouseExited(MouseEvent e)
   {
      adaptee.this_mouseExited(e);
   }
   public void mouseClicked(MouseEvent e)
   {
      adaptee.this_mouseClicked(e);
   }
   public void mousePressed(MouseEvent e)
   {
      adaptee.this_mousePressed(e);
   }
   public void mouseReleased(MouseEvent e)
   {
      adaptee.this_mouseReleased(e);
   }
}

class DisplayWindowFrame_mActiveItem_actionAdapter implements ActionListener
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_mActiveItem_actionAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void actionPerformed(ActionEvent e)
   {
      adaptee.windowActiveItemActionPerformed(e);
   }
}

class DisplayWindowFrame_mBorderItem_actionAdapter implements ActionListener
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_mBorderItem_actionAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void actionPerformed(ActionEvent e)
   {
      adaptee.borderItemActionPerformed(e);
   }
}

class DisplayWindowFrame_mPointerItem_actionAdapter implements ActionListener
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_mPointerItem_actionAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void actionPerformed(ActionEvent e)
   {
      adaptee.pointerItemActionPerformed(e);
   }
}

class DisplayWindowFrame_mStereoItem_actionAdapter implements ActionListener
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_mStereoItem_actionAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void actionPerformed(ActionEvent e)
   {
      adaptee.windowStereoItemActionPerformed(e);
   }
}

class DisplayWindowFrame_mEventSrcItem_actionAdapter implements ActionListener
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_mEventSrcItem_actionAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void actionPerformed(ActionEvent e)
   {
      adaptee.eventSrcItemActionPerformed(e);
   }
}

class DisplayWindowFrame_this_mouseMotionAdapter extends MouseMotionAdapter
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_this_mouseMotionAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void mouseDragged(MouseEvent e)
   {
      adaptee.this_mouseDragged(e);
   }
   public void mouseMoved(MouseEvent e)
   {
      adaptee.this_mouseMoved(e);
   }
}

class DisplayWindowFrame_this_componentAdapter extends java.awt.event.ComponentAdapter
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_this_componentAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void componentMoved(ComponentEvent e)
   {
      adaptee.this_componentMoved(e);
   }
   public void componentResized(ComponentEvent e)
   {
      adaptee.this_componentResized(e);
   }
}

class DisplayWindowFrame_mViewportActiveItem_actionAdapter implements java.awt.event.ActionListener
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_mViewportActiveItem_actionAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void actionPerformed(ActionEvent e)
   {
      adaptee.viewportActiveItemSelected(e);
   }
}

class DisplayWindowFrame_mViewportLeftEyeItem_actionAdapter implements java.awt.event.ActionListener
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_mViewportLeftEyeItem_actionAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void actionPerformed(ActionEvent e)
   {
      adaptee.viewportLeftEyeItemSelected(e);
   }
}

class DisplayWindowFrame_mViewportRightEyeItem_actionAdapter implements java.awt.event.ActionListener
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_mViewportRightEyeItem_actionAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void actionPerformed(ActionEvent e)
   {
      adaptee.viewportRightEyeItemSelected(e);
   }
}

class DisplayWindowFrame_mViewportStereoItem_actionAdapter implements java.awt.event.ActionListener
{
   private DisplayWindowFrame adaptee;

   DisplayWindowFrame_mViewportStereoItem_actionAdapter(DisplayWindowFrame adaptee)
   {
      this.adaptee = adaptee;
   }
   public void actionPerformed(ActionEvent e)
   {
      adaptee.viewporStereoItemSelected(e);
   }
}
