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

#include "music/configuration.hh" // Must be included first on BG/L
#ifdef USE_MPI
#include <mpi.h>
#endif
extern "C" {
#include <stdlib.h>
}
#include "music/ioutils.hh"
#include "music/error.hh"
#include <iostream>
#include <fstream>
namespace MUSIC {
  
  const char* const Configuration::configEnvVarName = "_MUSIC_CONFIG_";
  const char* const Configuration::mapFileName = "music.map";
  
  Configuration::Configuration (std::string name, int color, Configuration* def)
    : applicationName_ (name), color_ (color), defaultConfig (def)
  {
    
  }
#ifdef USE_MPI
  Configuration::Configuration ()
    : defaultConfig (0)
  {
    std::string configStr;
    getEnv (&configStr);
    MUSIC_LOG0 ("config: " << configStr);
    if (configStr.length() == 0)
      {
	launchedByMusic_ = false;
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
#endif

  Configuration::~Configuration ()
  {
    delete connectivityMap_;
    delete applications_;
  }
#ifdef USE_MPI
  void
  Configuration::getEnv( std::string* result)
  {
#ifdef __bgp__
	  int rank = MPI::COMM_WORLD.Get_rank ();
	  std::ifstream mapFile;
	  char* buffer;
	  int size = 0;
	  if(rank == 0){
		  mapFile.open(mapFileName);
		  if (!mapFile.is_open())
		  {
			  std::ostringstream oss;
			  oss << "File <music.map> is not found. To generate the file run MUSIC with -f option.";
			  error (oss.str ());
		  }

          size = mapFile.tellg();
          mapFile.seekg( 0, std::ios_base::end );
          long cur_pos = mapFile.tellg();
		  size = cur_pos - size;
		  mapFile.seekg( 0, std::ios_base::beg );
	  }
	  MPI::COMM_WORLD.Bcast(&size, 1,  MPI::INT, 0);
	  buffer = new char[size];

	  if(rank == 0)
		  mapFile.read ( buffer, size );
	  MPI::COMM_WORLD.Bcast(buffer, size,  MPI::BYTE, 0);
	  parseMapFile(rank, std::string(buffer), result);
	  //std::cerr << rank << ":"<<*result<<std::endl;
	  if(rank == 0)
		  mapFile.close();
#else
	  result->assign(getenv(configEnvVarName));
#endif

  }
#endif
  void
  Configuration::parseMapFile(int rank, std::string map_file, std::string *result)
  {
	  int first, last, pos, sLen;
	  pos = 0;
	  sLen = map_file.length();
	  do{
	  size_t occ_m = map_file.find_first_of ("-", pos);
	  size_t occ_s = map_file.find_first_of ("\t ", pos);
	  first = atoi( map_file.substr(0,occ_m > occ_s ? occ_s:occ_m).c_str());
	  if(occ_m > occ_s){
		  occ_m=occ_s;
		  last = first;
	  }
	  else
		  last = atoi( map_file.substr(occ_m+1,occ_s-occ_m-1).c_str());
	  size_t occ_e = map_file.find_first_of ("\n", pos);
	  *result = map_file.substr(occ_s+1,occ_e-occ_s-1);
	  if (rank <= last && rank >= first)
		  return;
	  pos = occ_e+1;
	  }while(pos < sLen);
	  std::ostringstream oss;
	  oss << "There is a mismatch between the information in the file <music.map> and the amount of given ranks." << std::endl
			  << "Try to generate the file (run MUSIC with -f option) again.";
	  error (oss.str ());


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
  //std::cerr << "looking..."<<name << std::endl;
    std::map<std::string, std::string>::iterator pos = dict.find (name);
    if (pos != dict.end ())
      {
	*result = pos->second;
	// std::cerr << "found:"<< pos->second << std::endl;
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
