package org.vrjuggler.jccl.editors;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;
import java.awt.event.*;

import org.vrjuggler.jccl.config.*;

public class ConfigContextEditor
   extends JPanel
{
   public ConfigContextEditor()
   {
      // Init the GUI
      try
      {
         jbInit();
      }
      catch (Exception e)
      {
         e.printStackTrace();
      }

      // Set up the tree
      mContextModel = new ConfigContextModel();
      mElementTree.setModel(mContextModel);
      mElementTree.setCellRenderer(new ConfigContextCellRenderer());
      mElementTree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);

      // Setup a listener for the tree so that when an element is selected, the
      // appropriate element is displayed in the property sheet.
      mElementTree.addTreeSelectionListener(new TreeSelectionListener()
      {
         public void valueChanged(TreeSelectionEvent evt)
         {
            DefaultMutableTreeNode node =
               (DefaultMutableTreeNode)mElementTree.getLastSelectedPathComponent();
            Object value = node.getUserObject();
            if (value == null || !(value instanceof ConfigElement))
            {
               return;
            }

            ConfigElement elt = (ConfigElement)value;
            mElementPropSheet.setElement(elt);
         
            if (mCustomEditors != null)
            {
               for(Iterator itr = mCustomEditors.iterator() ; itr.hasNext() ; )
               {
                  CustomEditor editor = (CustomEditor)itr.next(); 
                  mTabPane.remove(mTabPane.indexOfTab(editor.getTitle()));
               }
               mCustomEditors = null;
            }

            mCustomEditors = CustomEditorRegistry.findEditors(elt.getDefinition().getToken());
 
            if (mCustomEditors != null)
            {

               for(Iterator itr = mCustomEditors.iterator() ; itr.hasNext() ; )
               {
                  CustomEditor editor = (CustomEditor)itr.next(); 
                  editor.setConfigElement(elt);
                  mTabPane.add(editor.getPanel(), editor.getTitle());
               }
            } 
         }
      });
   }

   /**
    * Sets the configuration context this editor should be editing.
    *
    * @param context      the context to edit
    */
   public void setContext(ConfigContext context)
   {
      mContextModel.setContext(context);
      java.util.List elts = getBroker().getElements(context);
      if (elts.size() > 0)
      {
         ConfigElement elt = (ConfigElement)elts.get(0);
         mElementPropSheet.setElement(elt);
      }

      mElementTree.expandPath(new TreePath(mContextModel.getRoot()));
   }

   /**
    * Gets the configuration context this editor is editing.
    *
    * @return  the context being edited
    */
   public ConfigContext getContext()
   {
      return mContextModel.getContext();
   }

   /**
    * Gets a handle to the configuration broker.
    */
   protected ConfigBroker getBroker()
   {
      if (mBroker == null)
      {
         synchronized (this)
         {
            if (mBroker == null)
            {
               mBroker = new ConfigBrokerProxy();
            }
         }
      }
      return mBroker;
   }

   /**
    * Initializes the GUI. Auto-generated by JBuilder.
    */
   private void jbInit()
      throws Exception
   {
      JPanel temp = new JPanel();
      temp.setLayout(new BorderLayout());
      
      treePane.setLayout(treeLayout);

      this.setLayout(mBaseLayout);
      mElementTreeScrollPane.setMinimumSize(new Dimension(0, 0));
      mElementTree.setRootVisible(false);
      mElementTree.setShowsRootHandles(true);
      mBaseSplitPane.setOneTouchExpandable(true);
      this.add(mBaseSplitPane, BorderLayout.CENTER);
      mBaseSplitPane.add(treePane, JSplitPane.LEFT);
      mBaseSplitPane.add(mTabPane, JSplitPane.RIGHT);
      mTabPane.add(mElementPropSheetScrollPane, "DefaultEditor");
      
      mElementPropSheetScrollPane.getViewport().add(mElementPropSheet, null);
      mElementTreeScrollPane.getViewport().add(mElementTree, null);

      
      
      
      treeToolbar.setFloatable(false);

      addBtn.setText("Add");
      //addBtn.setEnabled(false);
      removeBtn.setText("Remove");
      //removeBtn.setEnabled(false);
 
      addBtn.addActionListener(new ActionListener()
      {
         public void actionPerformed(ActionEvent evt)
         {
            addAction(evt);
         }
      });
      removeBtn.addActionListener(new ActionListener()
      {
         public void actionPerformed(ActionEvent evt)
         {
            ;//removeAction(evt);
         }
      });

      treePane.add(treeToolbar, BorderLayout.NORTH);
      treePane.add(mElementTreeScrollPane, BorderLayout.CENTER);
      treeToolbar.add(addBtn);
      treeToolbar.add(removeBtn);
   }

   private ConfigBroker broker = null;
   /**
    * Gets the cached config broker proxy instance.
    */
   private ConfigBroker getConfigBroker()
   {
      if (broker == null)
      {
         broker = new ConfigBrokerProxy();
      }
      return broker;
   }

   /**
    * Adds a new config chunk to the current ConfigChunkDB.
    */
   protected void addAction(ActionEvent evt)
   {
      DefaultMutableTreeNode node = (DefaultMutableTreeNode)
                     mElementTree.getLastSelectedPathComponent();

      // Get a DB of all known ChunkDescs
      ConfigDefinitionRepository temp = getConfigBroker().getRepository();
      java.util.List defs = temp.getAllLatest();
      //java.util.List descs = getConfigBroker().getDescs(getContext());

      // Ask the user to choose a base ChunkDesc
      ConfigDefinitionChooser chooser = new ConfigDefinitionChooser();
      chooser.setDefinitions(defs);
      int result = chooser.showDialog(this);

      // If the user did not cancel their choice, make a new ConfigChunk for
      // the chose ChunkDesc
      if (result == ConfigDefinitionChooser.APPROVE_OPTION)
      {
         ConfigElementFactory tempfac = new ConfigElementFactory(defs);
         //ConfigElement element = new ConfigElement(chooser.getSelectedDefinition());
         ConfigElement element = tempfac.create("CHANGEME", chooser.getSelectedDefinition());
            //         chunk.setName(configChunkDB.getNewName(chunk.getDesc().getName()));
//         chunk.setName(chunk.getDesc().getName()); // TODO: Compute a unique name

         // Make sure this add goes through successfully
         if (! getConfigBroker().add(getContext(), element))
         {
            JOptionPane.showMessageDialog(this,
                                          "There are no configuration files active.",
                                          "Error",
                                          JOptionPane.ERROR_MESSAGE);
            return;
         }

// TODO: Make this work again
         // Make sure the new node gets selected
//         List chunk_nodes = getNodesFor(chunk);
//         if (chunk_nodes.size() > 0)
//         {
//            TreeNode chunk_node = (TreeNode)chunk_nodes.get(0);
//            TreePath path = new TreePath(treeModel.getPathToRoot(chunk_node));
//            mElementTree.setSelectionPath(path);
//         }
      }
   }



   private BorderLayout mBaseLayout = new BorderLayout();
   private JSplitPane mBaseSplitPane = new JSplitPane();
   private JTabbedPane mTabPane = new JTabbedPane();
   private JScrollPane mElementTreeScrollPane = new JScrollPane();
   private JTree mElementTree = new JTree();
   private JScrollPane mElementPropSheetScrollPane = new JScrollPane();

   // CustomEditor stuff
   private JScrollPane mCustomConfigElementEditorScrollPane = new JScrollPane();
   private java.util.List mCustomEditors = null;
   
   private ConfigElementPropertySheet mElementPropSheet = new ConfigElementPropertySheet();
   
   private BorderLayout treeLayout = new BorderLayout();
   private JPanel treePane = new JPanel();
   private JToolBar treeToolbar = new JToolBar();
   private JButton addBtn = new JButton();
   private JButton removeBtn = new JButton();

   /** The data model used to represent the context. */
   private ConfigContextModel mContextModel;

   /** The handle to the configuration broker. */
   private ConfigBroker mBroker;
}

class ConfigContextCellRenderer
   extends DefaultTreeCellRenderer
{
   public ConfigContextCellRenderer()
   {
      ClassLoader loader = getClass().getClassLoader();
      try
      {
         // Get all the icons we'll ever need now so we can reuse them later
         mElementIcon = new ImageIcon(loader.getResource(
            "org/vrjuggler/jccl/editors/images/configchunk.gif"));
         mPropertyIcon = new ImageIcon(loader.getResource(
            "org/vrjuggler/jccl/editors/images/property.gif"));
         mCategoryIcon = new ImageIcon(loader.getResource(
            "org/vrjuggler/jccl/editors/images/category.gif"));
      }
      catch (NullPointerException npe)
      {
         // Images aren't available ... oh well
      }
   }

   public Component getTreeCellRendererComponent(
                        JTree tree, Object node, boolean selected,
                        boolean expanded, boolean leaf, int row,
                        boolean focused)
   {
      // Get the default settings from the UI LAF
      super.getTreeCellRendererComponent(tree, node, selected,
                                         expanded, leaf, row, focused);

      Object value = ((DefaultMutableTreeNode)node).getUserObject();

      // Display the name of configuration elements
      if (value instanceof ConfigElement)
      {
         setText(((ConfigElement)value).getName());
         setFont(null);
         setIcon(mElementIcon);
      }
      // Display the name of properties
      else if (value instanceof PropertyDefinition)
      {
         setText(((PropertyDefinition)value).getName());
         setFont(tree.getFont().deriveFont(Font.ITALIC));
         setIcon(mPropertyIcon);
      }
      // Display the name of categories
      else if (value instanceof Category)
      {
         setText(((Category)value).getName());
         setFont(tree.getFont().deriveFont(Font.BOLD));
         setIcon(null);
      }

      return this;
   }

   private Icon mElementIcon;
   private Icon mPropertyIcon;
   private Icon mCategoryIcon;
}
