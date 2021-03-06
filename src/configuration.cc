/*
 *  This file is part of MUSIC.
 *  Copyright (C) 2007, 2008, 2009 INCF
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
#include "music/debug.hh" // Must be included first on BG/L

#include <cstdlib>
#include <cstring>

#include "music/configuration.hh"
#include "music/ioutils.hh"
#include "music/error.hh"

namespace MUSIC {
  
  const char* const Configuration::configEnvVarName = "_MUSIC_CONFIG_";

  /*
   * This is the syntax of the _MUSIC_CONFIG_ variable:
   *
   * POSTPONE:COLOR means that configuration is postponed until the
   * first port is created.  At this time, the variable is expected to
   * contain the full configuration information:
   *
   *
   * APPLICATIONNAME:COLOR:APPLICATIONMAP:CONNECTIVITYMAP:CONFIGDICT
   *
   * APPLICATIONNAME = name of this application (string)
   *
   * COLOR = index of section in music configuration file
   *
   * APPLICATIONMAP = SIZE:...:NAMEk:NPROCk:...
   *
   * SIZE = number of applications (integer)
   *
   * NAMEk = name of application k
   *
   * NPROCk = number of processes in application k
   *
   * CONNECTIVITYMAP = SIZE:...:PORTNAMEk:DIRECTIONk:WIDTHk:CONNECTIONSk:...
   *
   * SIZE = number of ports of this application
   *
   * PORTNAMEk = port name
   *
   * DIRECTIONk = port direction, 0 = OUTPUT, 1 = INPUT
   *
   * WIDTHk = port width
   *
   * CONNECTIONSk = SIZE:...:
   *                RECAPPNAME:RECPORTNAME:RECPORTCODE:REMOTELEADER:NREMOTEPROCS
   *                :...
   *
   * RECAPPNAME = name of receiving application
   *
   * RECPORTNAME = name of receiver port
   *
   * RECPORTCODE = code unique for every receiver port
   *
   * REMOTELEADER = lowest rank of the remote application
   *
   * NREMOTEPROCS = number of processes in the remote application
   *
   * CONFIGDICT = ...:VARNAMEk:VALUEk:...
   */
  
  Configuration::Configuration (std::string name, int color, Configuration* def)
    : applicationName_ (name), color_ (color), defaultConfig (def)
  {
    
  }

  
  Configuration::Configuration ()
    : postponeSetup_ (false), defaultConfig (0)
  {
    char* configStr = getenv (configEnvVarName);
    MUSIC_LOG0 ("config: " << configStr);
    if (configStr == NULL)
      {
	launchedByMusic_ = false;
	applications_ = new ApplicationMap ();
	connectivityMap_ = new Connectivity ();
      }
    else if (strncmp (configStr, "POSTPONE", 8) == 0)
      {
	launchedByMusic_ = true;
	postponeSetup_ = true;
	color_ = atoi (&configStr[9]); // after "POSTPONE:"
	applications_ = new ApplicationMap ();
	connectivityMap_ = new Connectivity ();
      }
    else
      {
	launchedByMusic_ = true;
	std::istringstream env (configStr);
	applicationName_ = IOUtils::read (env);
	env.ignore ();
	env >> color_;
	env.ignore ();
	applications_ = new ApplicationMap (env);
	env.ignore ();
	connectivityMap_ = new Connectivity (env);
	// parse config string
	while (!env.eof ())
	  {
	    env.ignore ();
	    std::string name = IOUtils::read (env, '=');
	    env.ignore ();
	    insert (name, IOUtils::read (env));
	  }
      }
  }


  Configuration::~Configuration ()
  {
    delete connectivityMap_;
    delete applications_;
  }

  
  void
  Configuration::write (std::ostringstream& env, Configuration* mask)
  {
    std::map<std::string, std::string>::iterator pos;
    for (pos = dict.begin (); pos != dict.end (); ++pos)
      {
	std::string name = pos->first;
	if (!(mask && mask->lookup (name)))
	  {
	    env << ':' << name << '=';
	    IOUtils::write (env, pos->second);
	  }
      }
  }

  
  void
  Configuration::writeEnv ()
  {
    std::ostringstream env;
    env << applicationName_ << ':' << color_ << ':';
    applications_->write (env);
    env << ':';
    connectivityMap_->write (env);
    write (env, 0);
    defaultConfig->write (env, this);
    setenv (configEnvVarName, env.str ().c_str (), 1);
  }

  
  bool
  Configuration::lookup (std::string name)
  {
    return dict.find (name) != dict.end ();
  }

  
  bool
  Configuration::lookup (std::string name, std::string* result)
  {
    std::map<std::string, std::string>::iterator pos = dict.find (name);
    if (pos != dict.end ())
      {
	*result = pos->second;
	return true;
      }
    else
      return defaultConfig && defaultConfig->lookup (name, result);
  }

  
  bool
  Configuration::lookup (std::string name, int* result)
  {
    std::map<std::string, std::string>::iterator pos = dict.find (name);
    if (pos != dict.end ())
      {
	std::istringstream iss (pos->second);
	if ((iss >> *result).fail ())
	  {
	    std::ostringstream oss;
	    oss << "var " << name << " given wrong type (" << pos->second
		<< "; expected int) in config file";
	    error (oss.str ());
	  }
	return true;
      }
    else
      return defaultConfig && defaultConfig->lookup (name, result);
  }

  
  bool
  Configuration::lookup (std::string name, double* result)
  {
    std::map<std::string, std::string>::iterator pos = dict.find (name);
    if (pos != dict.end ())
      {
	std::istringstream iss (pos->second);
	if ((iss >> *result).fail ())
	  {
	    std::ostringstream oss;
	    oss << "var " << name << " given wrong type (" << pos->second
		<< "; expected double) in config file";
	    error (oss.str ());
	  }
	return true;
      }
    else
      return defaultConfig && defaultConfig->lookup (name, result);
  }

  
  void
  Configuration::insert (std::string name, std::string value)
  {
    dict.insert (std::make_pair (name, value));
  }

  
  ApplicationMap*
  Configuration::applications ()
  {
    return applications_;
  }

  
  void
  Configuration::setApplications (ApplicationMap* a)
  {
    applications_ = a;
  }

  
  Connectivity*
  Configuration::connectivityMap ()
  {
    return connectivityMap_;
  }


  void
  Configuration::setConnectivityMap (Connectivity* c)
  {
    connectivityMap_ = c;
  }

}
