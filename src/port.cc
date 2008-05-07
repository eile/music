/*
 *  This file is part of MUSIC.
 *  Copyright (C) 2008 INCF
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

#include "music/port.hh"

namespace MUSIC {

  bool
  port::is_connected ()
  {
    return true;
  }


  bool
  port::has_width ()
  {
    return _width != -1;
  }


  int
  port::width ()
  {
    return _width;
  }


  void
  cont_output_port::map (data_map* dmap)
  {
  }

  
  void
  cont_output_port::map (data_map* dmap, int max_buffered)
  {
  }

  
  void
  cont_input_port::map (data_map* dmap, double delay, bool interpolate)
  {
  }

  
  void
  cont_input_port::map (data_map* dmap,
			int max_buffered,
			bool interpolate)
  {
  }

  
  void
  cont_input_port::map (data_map* dmap,
			double delay,
			int max_buffered,
			bool interpolate)
  {
  }

  
  void
  event_output_port::map (index_map* indices)
  {
  }

  
  void
  event_output_port::map (index_map* indices, int max_buffered)
  {
  }

  
  void
  event_input_port::map (index_map* indices,
			 event_handler* handle_event,
			 double acc_latency)
  {
  }

  
  void
  event_input_port::map (index_map* indices,
			 event_handler* handle_event,
			 double acc_latency,
			 int max_buffered)
  {
  }

  
  void
  message_output_port::map ()
  {
  }

  
  void
  message_output_port::map (int max_buffered)
  {
  }

  
  void
  message_input_port::map (message_handler* handler, double acc_latency)
  {
  }

  
  void
  message_input_port::map (message_handler* handler,
			   double acc_latency,
			   int max_buffered)
  {
  }

  
}
