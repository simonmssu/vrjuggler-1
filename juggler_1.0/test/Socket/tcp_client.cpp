/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * VR Juggler is (C) Copyright 1998, 1999, 2000 by Iowa State University
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

#include <stdio.h>
#include <VPR/IO/Socket/vjSocketStream.h>


int
main (int argc, char* argv[]) {
    vjSocketStream* sock;

    if ( argc == 2 ) {
        sock = new vjSocketStream(argv[1]);
    }
    else if ( argc == 3 ) {
        sock = new vjSocketStream(argv[1], (unsigned short) atoi(argv[2]));
    }
    else {
        return 1;
    }

    if ( sock->open() ) {
        char buffer[40];

        if ( sock->connect() ) {
            ssize_t bytes;

            bytes = sock->recv(buffer, 40);

            if ( bytes > 0 ) {
                printf("Read %d bytes from server\n", bytes);
                printf("    Got '%s'\n", buffer);
            }

            sock->close();
        }
    }

    delete sock;

    return 0;
}