#!/bin/env python
# Main app for osgnav remote gui
#
import os;
import sys;
import os.path

# Add another path to the search path
sys.path.insert(0,os.path.abspath("../Server"))
#sys.path.insert(0,os.path.abspath("../deps/Pyro"))   # Pyro deps

import MainWindow;
import ServerConfig

from qt import *
import qt

use_test_facade = True
use_qtpe = True

# See if we are on an embedded system
try:
   from qtpe import *
   import qtpe
except ImportError:
   use_qtpe = False      # On desktop

# ------- Globals ---------

def getRemoteNavFacade():
   """ Return reference to an object implementing RemoteNavFacade. """
   facade = None
   if use_test_facade:                            # Test facade
      import RemoteNavFacade
      facade = RemoteNavFacade.TestRemoteNavFacade()
   else:                                          # Pyro remote face
      import Pyro.naming, Pyro.core, Pyro.protocol;    # Only import if we need it. Python is cool. :)
      from Pyro.errors import PyroError,NamingError

      Pyro.core.initClient();
      locator = Pyro.naming.NameServerLocator()
      print "Searching for nameserver: ", ServerConfig.nameserver_address
      
      try:
         ns = locator.getNS(host=ServerConfig.nameserver_address);
         facade_uri = ns.resolve(ServerConfig.facade_obj_name);
         facade = Pyro.core.getAttrProxyForURI(facade_uri);
      except PyroError, X:
         print "Can't get nameserver and remote object:"
         print "   ns:", ServerConfig.nameserver_address
         print "  Exception:", X;
         ns = None;
         print "Giving up.\nAre you sure the server is running?\nbye.\n";
         sys.exit();
      else:
         print "Found remote facade object: URI:", facade_uri;
      
   return facade


def startApplication():
   if use_qtpe:
      print "Running on qtpe..."
      app = qtpe.QPEApplication(sys.argv)
   else:
      print "Running on standard qt..."
      app = QApplication(sys.argv)

   # Setup the facade   
   facade = getRemoteNavFacade()
   
   # Setup the application   
   QObject.connect(app,SIGNAL("lastWindowClosed()"),app,SLOT("quit()"))
   w = MainWindow.MainWindow()
   w.setFacade(facade)        # Give it handle to the facade
   w.reloadGuiView()          # Trigger gui update
   app.setMainWidget(w)
   w.show()
   if use_qtpe:
      w.showMaximized() 
   app.exec_loop()

#
# "Main" function
#
if __name__ == "__main__":
   startApplication();
   print "---------- Done -----------"
