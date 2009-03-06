/*
 *  This file is part of MUSIC.
 *  Copyright (C) 2009 INCF
 *
 *  MUSIC is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MUSIC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//#define MUSIC_DEBUG 1
#include "music/debug.hh"

#include "music/connection.hh"

namespace MUSIC {

  OutputConnection::OutputConnection (Connector* connector,
				      int maxBuffered,
				      int elementSize)
    : Connection (connector, maxBuffered),
      elementSize_ (elementSize)
  {
  }

  InputConnection::InputConnection (Connector* connector,
				    int maxBuffered,
				    ClockState accLatency,
				    bool interpolate)
    : Connection (connector, maxBuffered),
      accLatency_ (accLatency),
      interpolate_ (interpolate)
  {
  }

  
}