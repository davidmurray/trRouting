#ifndef TR_DATABASE_FETCHER
#define TR_DATABASE_FETCHER

#include <pqxx/pqxx> 
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <math.h>
#include <boost/algorithm/string.hpp>
//#include <cereal/archives/binary.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <stdlib.h>
#include <osrm/osrm.hpp>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "calculation_time.hpp"
#include "stop.hpp"
#include "route.hpp"
#include "point.hpp"
#include "trip.hpp"

#include "capnp/stopsCollection.capnp.h"
#include "capnp/routesCollection.capnp.h"
#include "capnp/tripsCollection.capnp.h"
#include "capnp/connectionsCollection.capnp.h"
#include "capnp/footpathsCollection.capnp.h"
#include "capnp/odTripsCollection.capnp.h"
#include "capnp/odTripFootpathsCollection.capnp.h"

#include "od_trip.hpp"
#include "tuple_boost_serialize.hpp"
#include "toolbox.hpp"
#include "cache_fetcher.hpp"
#include "osrm_fetcher.hpp"
#include "parameters.hpp"

namespace TrRouting
{
  
  class DatabaseFetcher
  {
  
  public:
    
    DatabaseFetcher() {}
    DatabaseFetcher(std::string _customDbSetupStr) : customDbSetupStr(_customDbSetupStr) {
      pgConnectionPtr = NULL;
      getConnectionPtr();
    }
    
    pqxx::connection* getConnectionPtr();
    void disconnect();
    void openConnection();
    bool isConnectionOpen();
    
    const std::pair<std::vector<Stop> , std::map<unsigned long long,int>> getStops( std::string applicationShortname);
    const std::pair<std::vector<Route>, std::map<unsigned long long,int>> getRoutes(std::string applicationShortname);
    const std::pair<std::vector<Trip> , std::map<unsigned long long,int>> getTrips( std::string applicationShortname);
    const std::pair<std::vector<std::tuple<int,int,int,int,int,short,short,int>>, std::vector<std::tuple<int,int,int,int,int,short,short,int>>> getConnections(std::string applicationShortname, std::map<unsigned long long,int> stopIndexesById, std::map<unsigned long long,int> tripIndexesById);
    const std::pair<std::vector<std::tuple<int,int,int>>, std::vector<std::pair<long long,long long>>> getFootpaths(std::string applicationShortname, std::map<unsigned long long,int> stopIndexesById);
    const std::tuple<std::vector<OdTrip>, std::map<unsigned long long,int>, std::vector<std::pair<int,int>>> getOdTrips(std::string applicationShortname, std::vector<Stop> stops, Parameters& params);
    //const std::vector<std::pair<int,int>> getOdTripFootpaths(std::vector<OdTrip> odTrips, std::vector<Stop> stops, Parameters& params);
    
  private:
    
    pqxx::connection* pgConnectionPtr;
    std::string customDbSetupStr;
    
  };
    
}

#endif // TR_DATABASE_FETCHER
