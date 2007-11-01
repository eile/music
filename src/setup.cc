/*
 *  This file is part of MUSIC.
 *  Copyright (C) 2007 CSC, KTH
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

#include <mpi.h>
#include "music/setup.hh"
#include "music/runtime.hh"
#include "music/parse.hh"

namespace MUSIC {

  setup::setup (int& argc, char**& argv)
  {
    MPI::Init (argc, argv);
    int my_rank = MPI::COMM_WORLD.Get_rank ();
    _config = new configuration ();
    myCommunicator = MPI::COMM_WORLD.Split (_config->color (), my_rank);
    string binary;
    _config->lookup ("binary", &binary);
    string args;
    _config->lookup ("args", &args);
    argv = parse_args (binary, args, &argc);
  }


  MPI::Intracomm
  setup::communicator ()
  {
    return myCommunicator;
  }


  bool
  setup::config (string var, string* result)
  {
    return _config->lookup (var, result);
  }

  bool
  setup::config (string var, int* result)
  {
    return _config->lookup (var, result);
  }

  bool
  setup::config (string var, double* result)
  {
    return _config->lookup (var, result);
  }

  bool
  is_port (string identifier)
  {
  }

  int
  port_size (string identifier)
  {
  }

  void
  setup::publish (data_map* map, std::string identifier)
  {
  }


  runtime*
  setup::done ()
  {
    return new runtime (myCommunicator);
  }

}
