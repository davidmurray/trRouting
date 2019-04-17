#include "server_http.hpp"
#include "client_http.hpp"

//Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>

//Added for the default_resource example
//#include <boost/filesystem.hpp>
//#include <boost/tokenizer.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/algorithm/string/replace.hpp>

#include <vector>
#include <algorithm>
#include <string>
//#include <fstream>
#include <iostream>
#include <iterator>
#include <curses.h>
//#include <locale>


#include <boost/optional.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/program_options.hpp>

//#include <osrm/osrm.hpp>
//#include <osrm/engine_config.hpp>
//#include <osrm/table_parameters.hpp>

//#include "toolbox.hpp"
//#include "gtfs_fetcher.hpp"
//#include "csv_fetcher.hpp"
//#include "cache_fetcher.hpp"
//#include "calculation_time.hpp"
#include "parameters.hpp"
#include "scenario.hpp"
#include "calculator.hpp"
#include "combinations.hpp"
#include "program_options.hpp"

using namespace TrRouting;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

int stepCount = 1;

std::string consoleRed        = "";
std::string consoleGreen      = "";
std::string consoleYellow     = "";
std::string consoleCyan       = "";
std::string consoleMagenta    = "";
std::string consoleResetColor = "";

//Added for the default_resource example
void default_resource_send(const HttpServer &server, const std::shared_ptr<HttpServer::Response> &response,
                           const std::shared_ptr<std::ifstream> &ifs);

int main(int argc, char** argv) {
  
  int serverPort {4000};
  
  boost::uuids::string_generator uuidGeneratorMain;

  // Set params:
  ProgramOptions programOptions;
  programOptions.parseOptions(argc, argv);
  Parameters algorithmParams;
  algorithmParams.setDefaultValues();
  
  // setup program options:
  
  // setup console colors:
  // (this will create a new terminal window, check if the terminal is color-capable and then it will close the terminal window with endwin()):
  initscr();
  start_color();
  if (has_colors())
  {
    consoleRed        = "\033[0;31m";
    consoleGreen      = "\033[1;32m";
    consoleYellow     = "\033[1;33m";
    consoleCyan       = "\033[0;36m";
    consoleMagenta    = "\033[0;35m";
    consoleResetColor = "\033[0m";
  }
  else
  {
    consoleRed        = "";
    consoleGreen      = "";
    consoleYellow     = "";
    consoleCyan       = "";
    consoleMagenta    = "";
    consoleResetColor = "";
  }
  endwin();
    
  std::cout << "Starting transit routing on port " << consoleGreen << programOptions.port << consoleResetColor 
            << " for the project: " << consoleGreen << programOptions.projectShortname << consoleResetColor 
            << std::endl << std::endl;
  
  algorithmParams.projectShortname       = programOptions.projectShortname;
  algorithmParams.dataFetcherShortname   = programOptions.dataFetcherShortname;
  algorithmParams.osrmWalkingPort        = programOptions.osrmWalkingPort;
  algorithmParams.osrmCyclingPort        = programOptions.osrmCyclingPort;
  algorithmParams.osrmDrivingPort        = programOptions.osrmDrivingPort;
  algorithmParams.osrmWalkingHost        = programOptions.osrmWalkingHost;
  algorithmParams.osrmCyclingHost        = programOptions.osrmCyclingHost;
  algorithmParams.osrmDrivingHost        = programOptions.osrmDrivingHost;
  algorithmParams.osrmWalkingFilePath    = programOptions.osrmWalkingFilePath;
  algorithmParams.osrmCyclingFilePath    = programOptions.osrmCyclingFilePath;
  algorithmParams.osrmDrivingFilePath    = programOptions.osrmDrivingFilePath;
  algorithmParams.osrmWalkingUseLib      = programOptions.osrmWalkingUseLib;
  algorithmParams.osrmCyclingUseLib      = programOptions.osrmCyclingUseLib;
  algorithmParams.osrmDrivingUseLib      = programOptions.osrmDrivingUseLib;

  GtfsFetcher  gtfsFetcher     = GtfsFetcher();
  algorithmParams.gtfsFetcher  = &gtfsFetcher;
  CsvFetcher   csvFetcher      = CsvFetcher();
  algorithmParams.csvFetcher   = &csvFetcher;
  CacheFetcher cacheFetcher    = CacheFetcher();
  algorithmParams.cacheFetcher = &cacheFetcher;
  
  Calculator calculator(algorithmParams);
  int i = 0;
  
  std::cout << "preparing server..." << std::endl;
  
  //HTTP-server using 1 thread
  //Unless you do more heavy non-threaded processing in the resources,
  //1 thread is usually faster than several threads
  HttpServer server;
  server.config.port = serverPort;

  server.resource["^/route/v1/transit[/]?\\?([0-9a-zA-Z&=_,:/.-]+)$"]["GET"]=[&server, &calculator](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
    
    calculator.algorithmCalculationTime.start();
    calculator.benchmarking.clear();

    /*calculator.benchmarking["reset_1"]             = 0;
    calculator.benchmarking["reset_2"]             = 0;
    calculator.benchmarking["reset_3"]             = 0;
    calculator.benchmarking["reset_4"]             = 0;
    calculator.benchmarking["reset_5"]             = 0;*/
    calculator.benchmarking["test"]                = 0;
    calculator.benchmarking["forward_calculation"] = 0;
    calculator.benchmarking["reset"]               = 0;
    //calculator.benchmarking["forward_journey"]     = 0;
    calculator.benchmarking["reverse_calculation"] = 0;
    //calculator.benchmarking["reverse_journey"]     = 0;
    //calculator.benchmarking["generating_results"]  = 0;
    //calculator.benchmarking["transferable_nodes"]  = 0;
    /*calculator.benchmarking["count_1"]              = 0;
    calculator.benchmarking["count_2"]              = 0;
    calculator.benchmarking["count_3"]              = 0;
    calculator.benchmarking["count_4"]              = 0;
    calculator.benchmarking["count_5"]              = 0;*/
    int countOdTripsCalculated {0};

    //calculator.algorithmCalculationTime.startStep();
    
    std::cout << "calculating request..." << std::endl;
    
    std::string resultStr;
    std::string csv;
    
    std::cout << request->path << std::endl;
    
    bool saveToFile {false}; // save result to file instead of returning it in response
    std::string calculationName {"trRoutingResult"};
    std::string fileFormat {"json"}; // csv can be used, but only with calculateAllTrips = true
    
    if (request->path_match.size() >= 1)
    {
      std::vector<std::string> parametersWithValues;
      boost::uuids::string_generator uuidGenerator;

      std::string queryString = request->path_match[1];
      
      boost::split(parametersWithValues, queryString, boost::is_any_of("&"));
      
      float originLatitude, originLongitude, destinationLatitude, destinationLongitude;
      boost::optional<boost::uuids::uuid> startingNodeUuid, endingNodeUuid;
      boost::optional<boost::uuids::uuid> odTripUuid; // when calculating for only one trip
      std::vector<int> accessNodeTravelTimesSeconds;
      std::vector<int> egressNodeTravelTimesSeconds;
      std::vector<std::pair<int,int>> odTripsPeriods; // pair: start_at_seconds, end_at_seconds
      std::vector<std::string> odTripsGenders;
      std::vector<std::string> odTripsAgeGroups;
      std::vector<std::string> odTripsOccupations;
      std::vector<std::string> odTripsActivities;
      std::vector<std::string> odTripsModes;
      
      int odTripsSampleSize {-1}; // for testing only
      int batchNumber  {1}; // when using multiple batches (parallele calculations)
      int batchesCount {1};
      bool alternatives {false}; // calculate alternatives or not
      
      calculator.params.calculateAllOdTrips = false;
      calculator.params.onlyServicesIdx.clear();
      calculator.params.exceptServicesIdx.clear();
      calculator.params.onlyLinesIdx.clear();
      calculator.params.exceptLinesIdx.clear();
      calculator.params.onlyModesIdx.clear();
      calculator.params.exceptModesIdx.clear();
      calculator.params.onlyAgenciesIdx.clear();
      calculator.params.exceptAgenciesIdx.clear();
      calculator.params.onlyNodesIdx.clear();
      calculator.params.exceptNodesIdx.clear();
      //calculator.params.odTripsPeriods     = odTripsPeriods;
      //calculator.params.odTripsGenders     = odTripsGenders;
      //calculator.params.odTripsAgeGroups   = odTripsAgeGroups;
      //calculator.params.odTripsOccupations = odTripsOccupations;
      //calculator.params.odTripsActivities  = odTripsActivities;
      //calculator.params.odTripsModes       = odTripsModes;
      
      std::vector<std::string> parameterWithValueVector;
      std::vector<std::string> latitudeLongitudeVector;
      std::vector<std::string> dateVector;
      std::vector<std::string> timeVector;
      std::vector<std::string> onlyServiceUuidsVector;
      std::vector<std::string> exceptServiceUuidsVector;
      std::vector<std::string> onlyLineUuidsVector;
      std::vector<std::string> exceptLineUuidsVector;
      std::vector<std::string> onlyModeShortnamesVector;
      std::vector<std::string> exceptModeShortnamesVector;
      std::vector<std::string> onlyAgencyUuidsVector;
      std::vector<std::string> exceptAgencyUuidsVector;
      std::vector<std::string> onlyNodeUuidsVector;
      std::vector<std::string> exceptNodeUuidsVector;
      std::vector<std::string> accessNodeUuidsVector;
      std::vector<std::string> accessNodeTravelTimesSecondsVector;
      std::vector<std::string> egressNodeUuidsVector;
      std::vector<std::string> egressNodeTravelTimesSecondsVector;
      std::vector<std::string> odTripsPeriodsVector;
      std::vector<std::string> odTripsGendersVector;
      std::vector<std::string> odTripsAgeGroupsVector;
      std::vector<std::string> odTripsOccupationsVector;
      std::vector<std::string> odTripsActivitiesVector;
      std::vector<std::string> odTripsModesVector;

      int departureTimeHour, departureTimeMinutes, arrivalTimeHour, arrivalTimeMinutes;
      
      calculator.params.forwardCalculation                     = true;
      calculator.params.detailedResults                        = false;
      calculator.params.returnAllNodesResult                   = false;
      calculator.params.transferOnlyAtSameStation              = false;
      calculator.params.transferBetweenSameLine                = true;
      calculator.params.origin                                 = Point();
      calculator.params.destination                            = Point();
      calculator.params.routingDateYear                        = 0;
      calculator.params.routingDateMonth                       = 0;
      calculator.params.routingDateDay                         = 0;
      calculator.params.originNodeIdx                          = -1;
      calculator.params.destinationNodeIdx                     = -1;
      calculator.params.odTrip                                 = NULL;
      calculator.params.debugDisplay                           = false;
      calculator.params.maxNumberOfTransfers                   = -1;
      calculator.params.minWaitingTimeSeconds                  = 5 * 60;
      calculator.params.departureTimeHour                      = -1;
      calculator.params.departureTimeMinutes                   = -1;
      calculator.params.departureTimeSeconds                   = -1;
      calculator.params.arrivalTimeHour                        = -1;
      calculator.params.arrivalTimeMinutes                     = -1;
      calculator.params.arrivalTimeSeconds                     = -1;
      calculator.params.maxTotalTravelTimeSeconds              = MAX_INT;
      calculator.params.maxAccessWalkingTravelTimeSeconds      = 20 * 60;
      calculator.params.maxEgressWalkingTravelTimeSeconds      = 20 * 60;
      calculator.params.maxTransferWalkingTravelTimeSeconds    = 20 * 60;
      calculator.params.maxTotalWalkingTravelTimeSeconds       = 60 * 60;
      calculator.params.maxOnlyWalkingAccessTravelTimeRatio    = 1.5;
      calculator.params.transferPenaltySeconds                 = 0;
      calculator.params.accessMode                             = "walking";
      calculator.params.egressMode                             = "walking";
      calculator.params.noResultSecondMode                     = "driving";
      calculator.params.tryNextModeIfRoutingFails              = false;
      calculator.params.noResultNextAccessTimeSecondsIncrement = 5 * 60;
      calculator.params.maxNoResultNextAccessTimeSeconds       = 40 * 60;
      calculator.params.calculateByNumberOfTransfers           = false;
      calculator.params.alternatives                           = false;
      calculator.params.accessNodesIdx.clear();
      calculator.params.egressNodesIdx.clear();
      calculator.params.accessNodeTravelTimesSeconds.clear();
      calculator.params.egressNodeTravelTimesSeconds.clear();

      for(auto & parameterWithValue : parametersWithValues)
      {
        boost::split(parameterWithValueVector, parameterWithValue, boost::is_any_of("="));
        
        if (parameterWithValueVector[0] == "origin")
        {
          boost::split(latitudeLongitudeVector, parameterWithValueVector[1], boost::is_any_of(","));
          originLatitude           = std::stof(latitudeLongitudeVector[0]);
          originLongitude          = std::stof(latitudeLongitudeVector[1]);
          calculator.params.origin = Point(originLatitude, originLongitude);
        }
        else if (parameterWithValueVector[0] == "destination")
        {
          boost::split(latitudeLongitudeVector, parameterWithValueVector[1], boost::is_any_of(","));
          destinationLatitude           = std::stof(latitudeLongitudeVector[0]);
          destinationLongitude          = std::stof(latitudeLongitudeVector[1]);
          calculator.params.destination = Point(destinationLatitude, destinationLongitude);
        }
        else if (parameterWithValueVector[0] == "calculation_name")
        {
          calculationName = parameterWithValueVector[1];
        }
        else if (parameterWithValueVector[0] == "file_format")
        {
          fileFormat = parameterWithValueVector[1];
        }
        else if (parameterWithValueVector[0] == "batch")
        {
          batchNumber = std::stoi(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "num_batches")
        {
          batchesCount = std::stoi(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "date")
        {
          boost::split(dateVector, parameterWithValueVector[1], boost::is_any_of("/"));
          calculator.params.routingDateYear      = std::stoi(dateVector[0]);
          calculator.params.routingDateMonth     = std::stoi(dateVector[1]);
          calculator.params.routingDateDay       = std::stoi(dateVector[2]);
        }
        else if (parameterWithValueVector[0] == "time"
                 || parameterWithValueVector[0] == "departure"
                 || parameterWithValueVector[0] == "departure_time"
                 || parameterWithValueVector[0] == "start_time"
                )
        {
          boost::split(timeVector, parameterWithValueVector[1], boost::is_any_of(":"));
          departureTimeHour    = std::stoi(timeVector[0]);
          departureTimeMinutes = std::stoi(timeVector[1]);
        }
        else if (parameterWithValueVector[0] == "arrival_time"
                 || parameterWithValueVector[0] == "arrival"
                 || parameterWithValueVector[0] == "end_time"
                )
        {
          boost::split(timeVector, parameterWithValueVector[1], boost::is_any_of(":"));
          arrivalTimeHour    = std::stoi(timeVector[0]);
          arrivalTimeMinutes = std::stoi(timeVector[1]);
        }
        else if (parameterWithValueVector[0] == "departure_seconds"
                 || parameterWithValueVector[0] == "departure_time_seconds"
                 || parameterWithValueVector[0] == "start_time_seconds"
                )
        {
          calculator.params.departureTimeSeconds = std::stoi(parameterWithValueVector[1]);
          if (calculator.params.departureTimeSeconds < 0)
          {
            calculator.params.departureTimeSeconds = -1;
          }
        }
        else if (parameterWithValueVector[0] == "arrival_seconds"
                 || parameterWithValueVector[0] == "arrival_time_seconds"
                 || parameterWithValueVector[0] == "end_time_seconds"
                )
        {
          calculator.params.arrivalTimeSeconds = std::stoi(parameterWithValueVector[1]);
          if (calculator.params.arrivalTimeSeconds < 0)
          {
            calculator.params.arrivalTimeSeconds = -1;
          }
        }
        else if (parameterWithValueVector[0] == "access_node_uuids")
        {
          boost::split(accessNodeUuidsVector, parameterWithValueVector[1], boost::is_any_of(","));
          boost::uuids::uuid accessNodeUuid;
          for(std::string accessNodeUuidStr : accessNodeUuidsVector)
          {
            accessNodeUuid = uuidGenerator(accessNodeUuidStr);
            if (calculator.nodeIndexesByUuid.count(accessNodeUuid) == 1)
            {
              calculator.params.accessNodesIdx.push_back(calculator.nodeIndexesByUuid[accessNodeUuid]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "egress_node_uuids")
        {
          boost::split(egressNodeUuidsVector, parameterWithValueVector[1], boost::is_any_of(","));
          boost::uuids::uuid egressNodeUuid;
          for(std::string egressNodeUuidStr : egressNodeUuidsVector)
          {
            egressNodeUuid = uuidGenerator(egressNodeUuidStr);
            if (calculator.nodeIndexesByUuid.count(egressNodeUuid) == 1)
            {
              calculator.params.egressNodesIdx.push_back(calculator.nodeIndexesByUuid[egressNodeUuid]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "od_trip_uuid")
        {
          odTripUuid = uuidGenerator(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "od_trips")
        {
          if (parameterWithValueVector[1] == "true" || parameterWithValueVector[1] == "1") { calculator.params.calculateAllOdTrips = true; }
        }
        else if (parameterWithValueVector[0] == "od_trips_sample_size")
        {
          odTripsSampleSize = std::stoi(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "od_trips_periods")
        {
          boost::split(odTripsPeriodsVector, parameterWithValueVector[1], boost::is_any_of(","));
          int periodIndex {0};
          int startAtSeconds;
          int endAtSeconds;
          for(std::string odTripsTime : odTripsPeriodsVector)
          {
            if (periodIndex % 2 == 0)
            {
              startAtSeconds = std::stoi(odTripsTime);
            }
            else
            {
              endAtSeconds = std::stoi(odTripsTime);
              odTripsPeriods.push_back(std::make_pair(startAtSeconds, endAtSeconds));
            }
            periodIndex++;
          }
          //calculator.params.odTripsPeriods = odTripsPeriods;
        }
        else if (parameterWithValueVector[0] == "od_trips_age_groups")
        {
          boost::split(odTripsAgeGroupsVector, parameterWithValueVector[1], boost::is_any_of(","));
          for(std::string odTripsAgeGroup : odTripsAgeGroupsVector)
          {
            odTripsAgeGroups.push_back(odTripsAgeGroup);
          }
          //calculator.params.odTripsAgeGroups = odTripsAgeGroups;
        }
        else if (parameterWithValueVector[0] == "od_trips_genders")
        {
          boost::split(odTripsGendersVector, parameterWithValueVector[1], boost::is_any_of(","));
          for(std::string odTripsGender : odTripsGendersVector)
          {
            odTripsGenders.push_back(odTripsGender);
          }
          //calculator.params.odTripsGenders = odTripsGenders;
        }
        else if (parameterWithValueVector[0] == "od_trips_occupations")
        {
          boost::split(odTripsOccupationsVector, parameterWithValueVector[1], boost::is_any_of(","));
          for(std::string odTripsOccupation : odTripsOccupationsVector)
          {
            odTripsOccupations.push_back(odTripsOccupation);
          }
          //calculator.params.odTripsOccupations = odTripsOccupations;
        }
        else if (parameterWithValueVector[0] == "od_trips_activities")
        {
          boost::split(odTripsActivitiesVector, parameterWithValueVector[1], boost::is_any_of(","));
          for(std::string odTripsActivity : odTripsActivitiesVector)
          {
            odTripsActivities.push_back(odTripsActivity);
          }
          //calculator.params.odTripsActivities = odTripsActivities;
        }
        else if (parameterWithValueVector[0] == "od_trips_modes")
        {
          boost::split(odTripsModesVector, parameterWithValueVector[1], boost::is_any_of(","));
          for(std::string odTripsMode : odTripsModesVector)
          {
            odTripsModes.push_back(odTripsMode);
          }
          //calculator.params.odTripsModes = odTripsModes;
        }
        
        else if (parameterWithValueVector[0] == "access_node_travel_times_seconds" || parameterWithValueVector[0] == "access_node_travel_times")
        {
          boost::split(accessNodeTravelTimesSecondsVector, parameterWithValueVector[1], boost::is_any_of(","));
          for(std::string accessNodeTravelTimeSeconds : accessNodeTravelTimesSecondsVector)
          {
            accessNodeTravelTimesSeconds.push_back(std::stoi(accessNodeTravelTimeSeconds));
          }
          calculator.params.accessNodeTravelTimesSeconds = accessNodeTravelTimesSeconds;
        }
        else if (parameterWithValueVector[0] == "egress_node_travel_times_seconds" || parameterWithValueVector[0] == "egress_node_travel_times")
        {
          boost::split(egressNodeTravelTimesSecondsVector, parameterWithValueVector[1], boost::is_any_of(","));
          for(std::string egressNodeTravelTimeSeconds : egressNodeTravelTimesSecondsVector)
          {
            egressNodeTravelTimesSeconds.push_back(std::stoi(egressNodeTravelTimeSeconds));
          }
          calculator.params.egressNodeTravelTimesSeconds = egressNodeTravelTimesSeconds;
        }
        else if (parameterWithValueVector[0] == "only_service_uuids")
        {
          boost::split(onlyServiceUuidsVector, parameterWithValueVector[1], boost::is_any_of(","));
          boost::uuids::uuid onlyServiceUuid;
          for(std::string onlyServiceUuidStr : onlyServiceUuidsVector)
          {
            onlyServiceUuid = uuidGenerator(onlyServiceUuidStr);
            if (calculator.serviceIndexesByUuid.count(onlyServiceUuid) == 1)
            {
              calculator.params.onlyServicesIdx.push_back(calculator.serviceIndexesByUuid[onlyServiceUuid]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "except_service_uuids")
        {
          boost::split(exceptServiceUuidsVector, parameterWithValueVector[1], boost::is_any_of(","));
          boost::uuids::uuid exceptServiceUuid;
          for(std::string exceptServiceUuidStr : exceptServiceUuidsVector)
          {
            exceptServiceUuid = uuidGenerator(exceptServiceUuidStr);
            if (calculator.serviceIndexesByUuid.count(exceptServiceUuid) == 1)
            {
              calculator.params.exceptServicesIdx.push_back(calculator.serviceIndexesByUuid[exceptServiceUuid]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "only_node_uuids")
        {
          boost::split(onlyNodeUuidsVector, parameterWithValueVector[1], boost::is_any_of(","));
          boost::uuids::uuid onlyNodeUuid;
          for(std::string onlyNodeUuidStr : onlyNodeUuidsVector)
          {
            onlyNodeUuid = uuidGenerator(onlyNodeUuidStr);
            if (calculator.nodeIndexesByUuid.count(onlyNodeUuid) == 1)
            {
              calculator.params.onlyNodesIdx.push_back(calculator.nodeIndexesByUuid[onlyNodeUuid]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "except_node_uuids")
        {
          boost::split(exceptNodeUuidsVector, parameterWithValueVector[1], boost::is_any_of(","));
          boost::uuids::uuid exceptNodeUuid;
          for(std::string exceptNodeUuidStr : exceptNodeUuidsVector)
          {
            exceptNodeUuid = uuidGenerator(exceptNodeUuidStr);
            if (calculator.nodeIndexesByUuid.count(exceptNodeUuid) == 1)
            {
              calculator.params.exceptNodesIdx.push_back(calculator.nodeIndexesByUuid[exceptNodeUuid]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "scenario_uuid")
        {
          boost::uuids::uuid scenarioUuid {uuidGenerator(parameterWithValueVector[1])};
          if (calculator.scenarioIndexesByUuid.count(scenarioUuid) == 1)
          {
            Scenario scenario {calculator.scenarios[calculator.scenarioIndexesByUuid[scenarioUuid]]};
            calculator.params.onlyServicesIdx   = scenario.servicesIdx;
            calculator.params.onlyLinesIdx      = scenario.onlyLinesIdx;
            calculator.params.onlyAgenciesIdx   = scenario.onlyAgenciesIdx;
            calculator.params.onlyNodesIdx      = scenario.onlyNodesIdx;
            calculator.params.onlyModesIdx      = scenario.onlyModesIdx;
            calculator.params.exceptLinesIdx    = scenario.exceptLinesIdx;
            calculator.params.exceptAgenciesIdx = scenario.exceptAgenciesIdx;
            calculator.params.exceptNodesIdx    = scenario.exceptNodesIdx;
            calculator.params.exceptModesIdx    = scenario.exceptModesIdx;
          }
        }
        else if (parameterWithValueVector[0] == "only_line_uuids")
        {
          boost::split(onlyLineUuidsVector, parameterWithValueVector[1], boost::is_any_of(","));
          boost::uuids::uuid onlyLineUuid;
          for(std::string onlyLineUuidStr : onlyLineUuidsVector)
          {
            onlyLineUuid = uuidGenerator(onlyLineUuidStr);
            if (calculator.lineIndexesByUuid.count(onlyLineUuid) == 1)
            {
              calculator.params.onlyLinesIdx.push_back(calculator.lineIndexesByUuid[onlyLineUuid]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "except_line_uuids")
        {
          boost::split(exceptLineUuidsVector, parameterWithValueVector[1], boost::is_any_of(","));
          boost::uuids::uuid exceptLineUuid;
          for(std::string exceptLineUuidStr : exceptLineUuidsVector)
          {
            exceptLineUuid = uuidGenerator(exceptLineUuidStr);
            if (calculator.lineIndexesByUuid.count(exceptLineUuid) == 1)
            {
              calculator.params.exceptLinesIdx.push_back(calculator.lineIndexesByUuid[exceptLineUuid]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "only_modes")
        {
          boost::split(onlyModeShortnamesVector, parameterWithValueVector[1], boost::is_any_of(","));
          for(std::string onlyModeShortname : onlyModeShortnamesVector)
          {
            if (calculator.modeIndexesByShortname.count(onlyModeShortname) == 1)
            {
              calculator.params.onlyModesIdx.push_back(calculator.modeIndexesByShortname[onlyModeShortname]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "except_modes")
        {
          boost::split(exceptModeShortnamesVector, parameterWithValueVector[1], boost::is_any_of(","));
          for(std::string exceptModeShortname : exceptModeShortnamesVector)
          {
            if (calculator.modeIndexesByShortname.count(exceptModeShortname) == 1)
            {
              calculator.params.exceptModesIdx.push_back(calculator.modeIndexesByShortname[exceptModeShortname]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "only_agency_uuids")
        {
          boost::split(onlyAgencyUuidsVector, parameterWithValueVector[1], boost::is_any_of(","));
          boost::uuids::uuid onlyAgencyUuid;
          for(std::string onlyAgencyUuidStr : onlyAgencyUuidsVector)
          {
            onlyAgencyUuid = uuidGenerator(onlyAgencyUuidStr);
            if (calculator.agencyIndexesByUuid.count(onlyAgencyUuid) == 1)
            {
              calculator.params.onlyAgenciesIdx.push_back(calculator.agencyIndexesByUuid[onlyAgencyUuid]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "except_agency_uuids")
        {
          boost::split(exceptAgencyUuidsVector, parameterWithValueVector[1], boost::is_any_of(","));
          boost::uuids::uuid exceptAgencyUuid;
          for(std::string exceptAgencyUuidStr : exceptAgencyUuidsVector)
          {
            exceptAgencyUuid = uuidGenerator(exceptAgencyUuidStr);
            if (calculator.agencyIndexesByUuid.count(exceptAgencyUuid) == 1)
            {
              calculator.params.onlyAgenciesIdx.push_back(calculator.agencyIndexesByUuid[exceptAgencyUuid]);
            }
          }
        }
        else if (parameterWithValueVector[0] == "starting_node_uuid"
                 || parameterWithValueVector[0] == "start_node_uuid"
                 || parameterWithValueVector[0] == "origin_node_uuid"
                )
        {
          boost::uuids::uuid originNodeUuid {uuidGenerator(parameterWithValueVector[1])};
          if (calculator.nodeIndexesByUuid.count(originNodeUuid) == 1)
          {
            calculator.params.originNodeIdx = calculator.nodeIndexesByUuid[originNodeUuid];
          }
        }
        else if (parameterWithValueVector[0] == "ending_node_uuid"
                 || parameterWithValueVector[0] == "end_node_uuid"
                 || parameterWithValueVector[0] == "destination_node_uuid"
                )
        {
          boost::uuids::uuid destinationNodeUuid {uuidGenerator(parameterWithValueVector[1])};
          if (calculator.nodeIndexesByUuid.count(destinationNodeUuid) == 1)
          {
            calculator.params.destinationNodeIdx = calculator.nodeIndexesByUuid[destinationNodeUuid];
          }
        }
        /*else if (parameterWithValueVector[0] == "max_number_of_transfers" || parameterWithValueVector[0] == "max_transfers")
        {
          calculator.params.maxNumberOfTransfers = std::stoi(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "calculate_by_number_of_transfers" || parameterWithValueVector[0] == "by_num_transfers")
        {
          if (parameterWithValueVector[1] == "true" || parameterWithValueVector[1] == "1") { calculator.params.calculateByNumberOfTransfers = true; }
        }*/ // seems impossible with CSA without calculating all alternatives
        else if (parameterWithValueVector[0] == "save_to_file")
        {
          if (parameterWithValueVector[1] == "true" || parameterWithValueVector[1] == "1") { saveToFile = true; }
        }
        else if (parameterWithValueVector[0] == "debug")
        {
          if (parameterWithValueVector[1] == "true" || parameterWithValueVector[1] == "1") { calculator.params.debugDisplay = true; }
        }
        else if (parameterWithValueVector[0] == "min_waiting_time" || parameterWithValueVector[0] == "min_waiting_time_minutes")
        {
          calculator.params.minWaitingTimeSeconds = std::stoi(parameterWithValueVector[1]) * 60;
          if (calculator.params.minWaitingTimeSeconds < 0)
          {
            calculator.params.minWaitingTimeSeconds = 0;
          }
        }
        else if (parameterWithValueVector[0] == "min_waiting_time_seconds")
        {
          calculator.params.minWaitingTimeSeconds = std::stoi(parameterWithValueVector[1]);
          if (calculator.params.minWaitingTimeSeconds < 0)
          {
            calculator.params.minWaitingTimeSeconds = 0;
          }
        }
        else if (parameterWithValueVector[0] == "max_travel_time" || parameterWithValueVector[0] == "max_travel_time_minutes")
        {
          calculator.params.maxTotalTravelTimeSeconds = std::stoi(parameterWithValueVector[1]) * 60;
          if (calculator.params.maxTotalTravelTimeSeconds == 0)
          {
            calculator.params.maxTotalTravelTimeSeconds = MAX_INT;
          }
        }
        else if (parameterWithValueVector[0] == "max_travel_time_seconds")
        {
          calculator.params.maxTotalTravelTimeSeconds = std::stoi(parameterWithValueVector[1]);
          if (calculator.params.maxTotalTravelTimeSeconds == 0)
          {
            calculator.params.maxTotalTravelTimeSeconds = MAX_INT;
          }
        }
        else if (parameterWithValueVector[0] == "max_access_travel_time" || parameterWithValueVector[0] == "max_access_travel_time_minutes")
        {
          calculator.params.maxAccessWalkingTravelTimeSeconds = std::stoi(parameterWithValueVector[1]) * 60;
        }
        else if (parameterWithValueVector[0] == "max_access_travel_time_seconds")
        {
          calculator.params.maxAccessWalkingTravelTimeSeconds = std::stoi(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "alternatives_max_added_travel_time_minutes" || parameterWithValueVector[0] == "alt_max_added_travel_time")
        {
          calculator.params.alternativesMaxAddedTravelTimeSeconds = std::stoi(parameterWithValueVector[1]) * 60;
        }
        else if (parameterWithValueVector[0] == "alternatives_max_added_travel_time_seconds" || parameterWithValueVector[0] == "alt_max_added_travel_time_seconds")
        {
          calculator.params.alternativesMaxAddedTravelTimeSeconds = std::stoi(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "max_only_walking_access_travel_time_ratio")
        {
          calculator.params.maxOnlyWalkingAccessTravelTimeRatio = std::stof(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "alternatives_max_travel_time_ratio" || parameterWithValueVector[0] == "alt_max_ratio")
        {
          calculator.params.alternativesMaxTravelTimeRatio = std::stof(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "alternatives_min_max_travel_time_minutes" || parameterWithValueVector[0] == "alt_min_max_travel_time")
        {
          calculator.params.minAlternativeMaxTravelTimeSeconds = std::stoi(parameterWithValueVector[1]) * 60;
        }
        else if (parameterWithValueVector[0] == "alternatives_min_max_travel_time_seconds" || parameterWithValueVector[0] == "alt_min_max_travel_time_seconds")
        {
          calculator.params.minAlternativeMaxTravelTimeSeconds = std::stoi(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "max_egress_travel_time" || parameterWithValueVector[0] == "max_egress_travel_time_minutes")
        {
          calculator.params.maxEgressWalkingTravelTimeSeconds = std::stoi(parameterWithValueVector[1]) * 60;
        }
         else if (parameterWithValueVector[0] == "max_egress_travel_time_seconds")
        {
          calculator.params.maxEgressWalkingTravelTimeSeconds = std::stoi(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "max_transfer_travel_time" || parameterWithValueVector[0] == "max_transfer_travel_time_minutes")
        {
          calculator.params.maxTransferWalkingTravelTimeSeconds = std::stoi(parameterWithValueVector[1]) * 60;
        }
        else if (parameterWithValueVector[0] == "max_transfer_travel_time_seconds")
        {
          calculator.params.maxTransferWalkingTravelTimeSeconds = std::stoi(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "transfer_penalty" || parameterWithValueVector[0] == "transfer_penalty_minutes")
        {
          calculator.params.transferPenaltySeconds = std::stoi(parameterWithValueVector[1]) * 60;
        }
        else if (parameterWithValueVector[0] == "walking_speed_factor" || parameterWithValueVector[0] == "walk_factor") // > 1.0 means faster walking
        {
          calculator.params.walkingSpeedFactor = std::stof(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "max_alternatives" || parameterWithValueVector[0] == "max_alt")
        {
          calculator.params.maxAlternatives = std::stoi(parameterWithValueVector[1]);
        }
        else if (parameterWithValueVector[0] == "alternatives" || parameterWithValueVector[0] == "alt")
        {
          if (parameterWithValueVector[1] == "true" || parameterWithValueVector[1] == "1") { calculator.params.alternatives = true; }
        }
        else if (parameterWithValueVector[0] == "return_all_nodes_results"
                 || parameterWithValueVector[0] == "return_all_nodes_result"
                 || parameterWithValueVector[0] == "all_nodes"
                )
        {
          if (parameterWithValueVector[1] == "true" || parameterWithValueVector[1] == "1") { calculator.params.returnAllNodesResult = true; }
        }
        else if (parameterWithValueVector[0] == "transfer_only_at_same_station"
                 || parameterWithValueVector[0] == "transfer_only_at_station"
                )
        {
          if (parameterWithValueVector[1] == "true" || parameterWithValueVector[1] == "1") { calculator.params.transferOnlyAtSameStation = true; }
        }
        else if (parameterWithValueVector[0] == "detailed"
                 || parameterWithValueVector[0] == "detailed_results"
                 || parameterWithValueVector[0] == "detailed_result"
                )
        {
          if (parameterWithValueVector[1] == "true" || parameterWithValueVector[1] == "1") { calculator.params.detailedResults = true; }
        }
        else if (parameterWithValueVector[0] == "transfer_between_same_line"
                 || parameterWithValueVector[0] == "allow_same_line_transfer"
                 || parameterWithValueVector[0] == "transfers_between_same_line"
                 || parameterWithValueVector[0] == "allow_same_line_transfers"
                )
        {
          if (parameterWithValueVector[1] == "false" || parameterWithValueVector[1] == "0") { calculator.params.transferBetweenSameLine = false; }
        }
        else if (parameterWithValueVector[0] == "reverse")
        {
          if (parameterWithValueVector[1] == "true" || parameterWithValueVector[1] == "1") { calculator.params.forwardCalculation = false; }
        }
        
      }

      if (calculator.params.departureTimeSeconds >= 0)
      {
        calculator.params.forwardCalculation = true;
      }
      else if (calculator.params.arrivalTimeSeconds >= 0)
      {
        calculator.params.forwardCalculation = false;
      }
      else if (departureTimeHour >= 0 && departureTimeMinutes >= 0)
      {
        calculator.params.departureTimeHour    = departureTimeHour;
        calculator.params.departureTimeMinutes = departureTimeMinutes;
        calculator.params.forwardCalculation   = true;
      }
      else if(arrivalTimeHour >= 0 && arrivalTimeMinutes >= 0)
      {
        calculator.params.arrivalTimeHour    = arrivalTimeHour;
        calculator.params.arrivalTimeMinutes = arrivalTimeMinutes;
        calculator.params.forwardCalculation = false;
      }
      
      //for (auto & ageGroup : odTripsAgeGroups)
      //{
      //  std::cerr << ageGroup << std::endl;
      //}
      
            
      if (calculator.params.alternatives)
      {
        RoutingResult routingResult;
        
        OdTrip odTrip;
        bool foundOdTrip{false};
        if (odTripUuid.is_initialized())
        {
          for (auto & _odTrip : calculator.odTrips) // todo: we should keep a map of odTripIndexesByUuid...
          {
            if (_odTrip.uuid == odTripUuid)
            {
              odTrip      = _odTrip;
              foundOdTrip = true;
              break;
            }
          }
        }
        
        nlohmann::json json;
        nlohmann::json alternativeJson;
        
        if (foundOdTrip)
        {
          std::cout << "od trip uuid " << odTrip.uuid << std::endl;
          calculator.params.origin      = odTrip.origin;
          calculator.params.destination = odTrip.destination;
          calculator.params.odTrip      = &odTrip;
          json["odTripUuid"] = boost::uuids::to_string(odTrip.uuid);
        }
        
        json["alternatives"] = nlohmann::json::array();

        std::vector<int>                 foundLinesIdx;
        std::vector< std::vector<int> >  allCombinations;
        std::map<std::vector<int>, bool> alreadyCalculatedCombinations;
        std::map<std::vector<int>, bool> alreadyFoundLinesIdx;
        std::map<std::vector<int>, int>  foundLinesIdxTravelTimeSeconds;
        std::vector<int>                 combinationsKs;
        int maxTravelTime;
        int numAlternatives = 1;
        int maxAlternatives = calculator.params.maxAlternatives;
        int lastFoundedAtNum = 0;
        int departureTimeSeconds = -1;
        std::vector<std::string> lineShortnames;
        
        std:: cout << numAlternatives << "." << std::endl;
        std::cout << "initialMaxTotalTravelTimeSeconds: " << calculator.params.maxTotalTravelTimeSeconds << std::endl;
        
        routingResult = calculator.calculate();
        
        if (routingResult.status == "success")
        {
          lineShortnames.clear();
          for(auto lineIdx : routingResult.linesIdx)
          {
            lineShortnames.push_back(calculator.lines[lineIdx].shortname);
          }
          
          alternativeJson = {};
          alternativeJson["status"]                        = routingResult.status;
          alternativeJson["travelTimeSeconds"]             = routingResult.travelTimeSeconds;
          alternativeJson["minimizedTravelTimeSeconds"]    = routingResult.travelTimeSeconds - routingResult.firstWaitingTimeSeconds + calculator.params.minWaitingTimeSeconds;
          alternativeJson["departureTimeSeconds"]          = routingResult.departureTimeSeconds;
          alternativeJson["minimizedDepartureTimeSeconds"] = routingResult.minimizedDepartureTimeSeconds;
          alternativeJson["arrivalTimeSeconds"]            = routingResult.arrivalTimeSeconds;
          alternativeJson["numberOfTransfers"]             = routingResult.numberOfTransfers;
          alternativeJson["inVehicleTravelTimeSeconds"]    = routingResult.inVehicleTravelTimeSeconds;
          alternativeJson["transferTravelTimeSeconds"]     = routingResult.transferTravelTimeSeconds;
          alternativeJson["waitingTimeSeconds"]            = routingResult.waitingTimeSeconds;
          alternativeJson["accessTravelTimeSeconds"]       = routingResult.accessTravelTimeSeconds;
          alternativeJson["egressTravelTimeSeconds"]       = routingResult.egressTravelTimeSeconds;
          alternativeJson["transferWaitingTimeSeconds"]    = routingResult.transferWaitingTimeSeconds;
          alternativeJson["firstWaitingTimeSeconds"]       = routingResult.firstWaitingTimeSeconds;
          alternativeJson["nonTransitTravelTimeSeconds"]   = routingResult.nonTransitTravelTimeSeconds;
          alternativeJson["inVehicleTravelTimesSeconds"]   = routingResult.inVehicleTravelTimesSeconds;
          alternativeJson["lineUuids"]                     = routingResult.lineUuids;
          alternativeJson["lineShortnames"]                = lineShortnames;
          alternativeJson["modeShortnames"]                = routingResult.modeShortnames;
          alternativeJson["agencyUuids"]                   = routingResult.agencyUuids;
          alternativeJson["boardingNodeUuids"]             = routingResult.boardingNodeUuids;
          alternativeJson["unboardingNodeUuids"]           = routingResult.unboardingNodeUuids;
          alternativeJson["tripUuids"]                     = routingResult.tripUuids;
          alternativeJson["alternativeSequence"]           = numAlternatives;
          json["alternatives"].push_back(alternativeJson);
          json["status"] = "success";
          departureTimeSeconds = routingResult.departureTimeSeconds + routingResult.firstWaitingTimeSeconds - calculator.params.minWaitingTimeSeconds;
                
          maxTravelTime = calculator.params.alternativesMaxTravelTimeRatio * routingResult.travelTimeSeconds;
          if (maxTravelTime < calculator.params.minAlternativeMaxTravelTimeSeconds)
          {
            calculator.params.maxTotalTravelTimeSeconds = calculator.params.minAlternativeMaxTravelTimeSeconds;
          }
          else if (maxTravelTime > routingResult.travelTimeSeconds + calculator.params.alternativesMaxAddedTravelTimeSeconds)
          {
            calculator.params.maxTotalTravelTimeSeconds = routingResult.travelTimeSeconds + calculator.params.alternativesMaxAddedTravelTimeSeconds;
          }
          else
          {
            calculator.params.maxTotalTravelTimeSeconds = maxTravelTime;
          }
          
          calculator.params.departureTimeSeconds = departureTimeSeconds;
          
          std::cout << "minAlternativeMaxTravelTimeSeconds: " << calculator.params.minAlternativeMaxTravelTimeSeconds << std::endl;
          std::cout << "alternativesMaxAddedTravelTimeSeconds: " << calculator.params.alternativesMaxAddedTravelTimeSeconds << std::endl;
          std::cout << "alternativesMaxTravelTimeRatio: " << calculator.params.alternativesMaxTravelTimeRatio << std::endl;
          std::cout << "fastestTravelTimeSeconds: " << routingResult.travelTimeSeconds << std::endl;
          std::cout << "maxTotalTravelTimeSeconds: " << calculator.params.maxTotalTravelTimeSeconds << std::endl;
          
          foundLinesIdx = routingResult.linesIdx;
          std::stable_sort(foundLinesIdx.begin(),foundLinesIdx.end());
          alreadyFoundLinesIdx[foundLinesIdx]           = true;
          foundLinesIdxTravelTimeSeconds[foundLinesIdx] = routingResult.travelTimeSeconds;
          lastFoundedAtNum = 1;
          //std::cout << "fastest line ids: ";
          //for (auto lineUuid : foundLineUuids)
          //{
          //  std::cout << calculator.lines[calculator.lineIndexesByUuid[lineUuid]].shortname << " ";
          //}
          //std::cout << std::endl;
          combinationsKs.clear();
          for (int i = 1; i <= foundLinesIdx.size(); i++) { combinationsKs.push_back(i); }
          for (auto k : combinationsKs)
          {
            Combinations<int> combinations(foundLinesIdx, k);
            //std::cout << "\nk = " << k << std::endl;
            for (auto newCombination : combinations)
            {
              std::stable_sort(newCombination.begin(), newCombination.end());
              allCombinations.push_back(newCombination);
              alreadyCalculatedCombinations[newCombination] = true;
            }
          }
          
          std::vector<int> combination;
          for (int i = 0; i < allCombinations.size(); i++)
          {
            if (numAlternatives <= maxAlternatives)
            {
              //std:: cout << std::endl << numAlternatives << "." << std::endl;
              combination = allCombinations.at(i);
              calculator.params.exceptLinesIdx = combination;
              //std::cout << "except line Uuids: ";
              //for (auto lineUuid : combination)
              //{
              //  std::cout << calculator.lines[calculator.lineIndexesByUuid[lineUuid]].shortname << " ";
              //}
              //std::cout << std::endl;
              routingResult = calculator.calculate(false, false);
              
              if (routingResult.status == "success")
              {
              
                foundLinesIdx = routingResult.linesIdx;
                std::stable_sort(foundLinesIdx.begin(),foundLinesIdx.end());
                
                if (foundLinesIdx.size() > 0 && alreadyFoundLinesIdx.count(foundLinesIdx) == 0)
                {
                  lineShortnames.clear();
                  for(auto lineIdx : routingResult.linesIdx)
                  {
                    lineShortnames.push_back(calculator.lines[lineIdx].shortname);
                  }
                  
                  numAlternatives += 1;
                  
                  alternativeJson = {};
                  alternativeJson["status"]                        = routingResult.status;
                  alternativeJson["travelTimeSeconds"]             = routingResult.travelTimeSeconds;
                  alternativeJson["minimizedTravelTimeSeconds"]    = routingResult.travelTimeSeconds - routingResult.firstWaitingTimeSeconds + calculator.params.minWaitingTimeSeconds;
                  alternativeJson["departureTimeSeconds"]          = routingResult.departureTimeSeconds;
                  alternativeJson["minimizedDepartureTimeSeconds"] = routingResult.minimizedDepartureTimeSeconds;
                  alternativeJson["arrivalTimeSeconds"]            = routingResult.arrivalTimeSeconds;
                  alternativeJson["numberOfTransfers"]             = routingResult.numberOfTransfers;
                  alternativeJson["inVehicleTravelTimeSeconds"]    = routingResult.inVehicleTravelTimeSeconds;
                  alternativeJson["transferTravelTimeSeconds"]     = routingResult.transferTravelTimeSeconds;
                  alternativeJson["waitingTimeSeconds"]            = routingResult.waitingTimeSeconds;
                  alternativeJson["accessTravelTimeSeconds"]       = routingResult.accessTravelTimeSeconds;
                  alternativeJson["egressTravelTimeSeconds"]       = routingResult.egressTravelTimeSeconds;
                  alternativeJson["transferWaitingTimeSeconds"]    = routingResult.transferWaitingTimeSeconds;
                  alternativeJson["firstWaitingTimeSeconds"]       = routingResult.firstWaitingTimeSeconds;
                  alternativeJson["nonTransitTravelTimeSeconds"]   = routingResult.nonTransitTravelTimeSeconds;
                  alternativeJson["inVehicleTravelTimesSeconds"]   = routingResult.inVehicleTravelTimesSeconds;
                  alternativeJson["lineUuids"]                     = routingResult.lineUuids;
                  alternativeJson["lineShortnames"]                = lineShortnames;
                  alternativeJson["modeShortnames"]                = routingResult.modeShortnames;
                  alternativeJson["agencyUuids"]                   = routingResult.agencyUuids;
                  alternativeJson["boardingNodeUuids"]             = routingResult.boardingNodeUuids;
                  alternativeJson["unboardingNodeUuids"]           = routingResult.unboardingNodeUuids;
                  alternativeJson["tripUuids"]                     = routingResult.tripUuids;
                  alternativeJson["alternativeSequence"]           = numAlternatives;
                  json["alternatives"].push_back(alternativeJson);
                
                  //std::cout << "travelTimeSeconds: " << routingResult.travelTimeSeconds << " line Uuids: ";
                  //for (auto lineUuid : foundLineUuids)
                  //{
                  //  std::cout << calculator.lines[calculator.lineIndexesByUuid[lineUuid]].shortname << " ";
                  //}
                  //std::cout << std::endl;
                  combinationsKs.clear();
                  
                  lastFoundedAtNum = numAlternatives;
                  alreadyFoundLinesIdx[foundLinesIdx] = true;
                  foundLinesIdxTravelTimeSeconds[foundLinesIdx] = routingResult.travelTimeSeconds;
                  for (int i = 1; i <= foundLinesIdx.size(); i++) { combinationsKs.push_back(i); }
                  for (auto k : combinationsKs)
                  {
                    Combinations<int> combinations(foundLinesIdx, k);
                    //std::cout << "\nk = " << k << std::endl;
                    for (auto newCombination : combinations)
                    {
                      newCombination.insert( newCombination.end(), combination.begin(), combination.end() );
                      std::stable_sort(newCombination.begin(), newCombination.end());
                      if (alreadyCalculatedCombinations.count(newCombination) == 0)
                      {
                        allCombinations.push_back(newCombination);
                        alreadyCalculatedCombinations[newCombination] = true;
                      }
                    }
                  }
                  
                  combination.clear();
                }
              }
            }
          }
        }
        else
        {
          json["status"] = "failed";
        }
        
        std::cout << std::endl;
        int i {1};
        //for (auto foundLineUuids : alreadyFoundLineUuids)
        //{
        //  std::cout << i << ". ";
        //  for(auto lineUuid : foundLineUuids.first)
        //  {
        //    std::cout << calculator.lines[calculator.lineIndexesByUuid[lineUuid]].shortname << " ";
        //  }
        //  std::cout << " tt: " << (foundLineUuidsTravelTimeSeconds[foundLineUuids.first] / 60);
        //  i++;
        //  std::cout << std::endl;
        //}
        
        std::cout << "last alternative found at: " << lastFoundedAtNum << " on a total of " << maxAlternatives << " calculations" << std::endl;
        
        //return 0;
        
        resultStr = json.dump(2);
      }
      
      
      if (!calculator.params.alternatives && (calculator.params.calculateAllOdTrips || odTripUuid.is_initialized() ))
      {
        RoutingResult routingResult;
        //std::map<boost::uuids::uuid, std::map<int, float>> tripsLegsProfile; // parent map key: trip uuid, nested map key: connection sequence, value: number of trips using this connection
        //std::map<boost::uuids::uuid, std::map<int, std::pair<float, std::vector<boost::uuids::uuid>>>> pathsLegsProfile; // parent map key: trip uuid, nested map key: connection sequence, value: number of trips using this connection
        std::map<boost::uuids::uuid, float> lineProfiles; // key: line uuid, value: count od trips using this line
        std::map<boost::uuids::uuid, std::vector<std::vector<float>>> pathProfiles; // key: path uuid, value: [index: segment index, value: [index: hourOfDay, demand]]
        std::map<boost::uuids::uuid, std::vector<float>> pathTotalProfiles; // key: path uuid, value: [index: segment index, value: totalDemand]
        int  legTripIdx;
        int  legLineIdx;
        int  legPathIdx;
        Path legPath;
        int  connectionDepartureTimeSeconds;
        int  connectionDepartureTimeHour;
        bool atLeastOneOdTrip {false};
        bool atLeastOneCompatiblePeriod {false};
        bool attributesMatches {true};
        int  odTripsCount = calculator.odTrips.size();
        float maximumSegmentHourlyDemand = 0.0;
        float maximumSegmentTotalDemand  = 0.0;
        std::string ageGroup;
        
        int legConnectionStartIdx;
        int legConnectionEndIdx;
        
        nlohmann::json json;
        nlohmann::json odTripJson;
        nlohmann::json lineProfilesJson;
        nlohmann::json pathProfilesJson;
        //nlohmann::json pathsOdTripsProfilesSequenceJson;
        //std::vector<unsigned long long> pathsOdTripsProfilesOdTripUuids;
        
        if (fileFormat == "csv" && batchNumber == 1) // write header only on first batch, so we can easily append subsequent batches to the same csv file
        {
          // write csv header:
          csv += "uuid,internalId,status,ageGroup,gender,occupation,destinationActivity,mode,expansionFactor,travelTimeSeconds,onlyWalkingTravelTimeSeconds,"
                 "declaredDepartureTimeSeconds,departureTimeSeconds,minimizedDepartureTimeSeconds,arrivalTimeSeconds,numberOfTransfers,inVehicleTravelTimeSeconds,"
                 "transferTravelTimeSeconds,waitingTimeSeconds,accessTravelTimeSeconds,egressTravelTimeSeconds,transferWaitingTimeSeconds,"
                 "firstWaitingTimeSeconds,nonTransitTravelTimeSeconds,lineUuids,modeShortnames,agencyUuids,boardingNodeUuids,unboardingNodeUuids,tripUuids\n";
        }
        else
        {
          json["odTrips"] = nlohmann::json::array();
        }
                
        int i {0};
        int j {0};
        bool resetFilters {true};

        for (auto & line : calculator.lines)
        {
          lineProfiles[line.uuid] = 0.0;
        }

        std::vector<float> demandByHourOfDay;
        for (int i = 0; i <= 28; i++)
        {
          demandByHourOfDay.push_back(0.0);
        }

        /*for (auto & path : calculator.paths)
        {
          pathProfiles[path.uuid] = std::vector<std::vector<float>>(path.nodesIdx.size() - 1, demandByHourOfDay);
        }*/

        for (auto & odTrip : calculator.odTrips)
        {
          
          if ( i % batchesCount != batchNumber - 1) // when using multiple parallel calculators
          {
            i++;
            continue;
          }
          
          attributesMatches          = true;
          atLeastOneCompatiblePeriod = false;
          
          // verify that od trip matches selected attributes:
          if ( (odTripsAgeGroups.size()   > 0 && std::find(odTripsAgeGroups.begin(), odTripsAgeGroups.end(), calculator.persons[odTrip.personIdx].ageGroup)       == odTripsAgeGroups.end()) 
            || (odTripsGenders.size()     > 0 && std::find(odTripsGenders.begin(), odTripsGenders.end(), calculator.persons[odTrip.personIdx].gender)             == odTripsGenders.end())
            || (odTripsOccupations.size() > 0 && std::find(odTripsOccupations.begin(), odTripsOccupations.end(), calculator.persons[odTrip.personIdx].occupation) == odTripsOccupations.end())
            || (odTripsActivities.size()  > 0 && std::find(odTripsActivities.begin(), odTripsActivities.end(), odTrip.destinationActivity)                        == odTripsActivities.end())
            || (odTripsModes.size()       > 0 && std::find(odTripsModes.begin(), odTripsModes.end(), odTrip.mode)                                                 == odTripsModes.end())
          )
          {
            attributesMatches = false;
          }

          // verify that od trip matches at least one selected period:
          for (auto & period : odTripsPeriods)
          {
            if (odTrip.departureTimeSeconds >= period.first && odTrip.departureTimeSeconds < period.second)
            {
              atLeastOneCompatiblePeriod = true;
            }
          }
          
          if (attributesMatches && (atLeastOneCompatiblePeriod || odTripsPeriods.size() == 0) && (!odTripUuid.is_initialized() || odTripUuid == odTrip.uuid) )
          {
            std::cout /*<< /"od trip uuid " << odTrip.uuid << " ("*/ << (i+1) << "/" << odTripsCount /*<< ")"*/ << std::endl;// << " dts: " << odTrip.departureTimeSeconds << " atLeastOneCompatiblePeriod: " << (atLeastOneCompatiblePeriod ? "true " : "false ") << "attributesMatches: " << (attributesMatches ? "true " : "false ") << std::endl;
            
            calculator.params.origin      = odTrip.origin;
            calculator.params.destination = odTrip.destination;
            calculator.params.odTrip      = &odTrip;
            routingResult = calculator.calculate(true, resetFilters); // reset filters only on first calculation
            resetFilters  = false;
            countOdTripsCalculated++;
            //int benchmarkingStart = calculator.algorithmCalculationTime.getEpoch();
            if (true/*routingResult.status == "success"*/)
            {
              atLeastOneOdTrip = true;
              if (routingResult.legs.size() > 0)
              {
                if (fileFormat != "csv")
                {
                  for (auto & leg : routingResult.legs)
                  {
                    legTripIdx            = std::get<0>(leg);
                    legLineIdx            = std::get<1>(leg);
                    legPathIdx            = std::get<2>(leg);
                    legPath               = calculator.paths[legPathIdx];
                    legConnectionStartIdx = std::get<3>(leg);
                    legConnectionEndIdx   = std::get<4>(leg);
                    lineProfiles[calculator.lines[legLineIdx].uuid] += odTrip.expansionFactor;

                    if (pathProfiles.find(legPath.uuid) == pathProfiles.end())
                    {
                      pathProfiles[legPath.uuid] = std::vector<std::vector<float>>(legPath.nodesIdx.size() - 1, demandByHourOfDay);
                      pathTotalProfiles[legPath.uuid] = std::vector<float>(legPath.nodesIdx.size() - 1, 0.0);
                    }

                    for (int connectionIndex = legConnectionStartIdx; connectionIndex <= legConnectionEndIdx; connectionIndex++)
                    {
                      connectionDepartureTimeSeconds = calculator.tripConnectionDepartureTimes[legTripIdx][connectionIndex];
                      calculator.tripConnectionDemands[legTripIdx][connectionIndex] += odTrip.expansionFactor;
                      connectionDepartureTimeHour    = connectionDepartureTimeSeconds / 3600;
                      //std::cout << "pUuid:" << legPath.uuid << " dth:" << connectionDepartureTimeHour << " cI:" << connectionIndex << " oldD:" << pathProfiles[legPath.uuid][connectionIndex][connectionDepartureTimeHour] << std::endl;
                      pathProfiles[legPath.uuid][connectionIndex][connectionDepartureTimeHour] += odTrip.expansionFactor;
                      pathTotalProfiles[legPath.uuid][connectionIndex] += odTrip.expansionFactor;
                      if (maximumSegmentHourlyDemand < pathProfiles[legPath.uuid][connectionIndex][connectionDepartureTimeHour])
                      {
                        maximumSegmentHourlyDemand = pathProfiles[legPath.uuid][connectionIndex][connectionDepartureTimeHour];
                      }
                      if (maximumSegmentTotalDemand < pathTotalProfiles[legPath.uuid][connectionIndex])
                      {
                        maximumSegmentTotalDemand = pathTotalProfiles[legPath.uuid][connectionIndex];
                      }
                    }
                  }
                }
              }
              
              if (fileFormat == "csv")
              {
                //ageGroup = odTrip.ageGroup;
                //std::replace( ageGroup.begin(), ageGroup.end(), '-', '_' ); // remove dash so Excel does not convert to age groups to numbers...
                csv += boost::uuids::to_string(odTrip.uuid) + ",\"" + odTrip.internalId + "\",\"" + routingResult.status + "\",\"" /*+ ageGroup*/ + "\",\"" /*+ odTrip.gender*/ + "\",\"" /*+ odTrip.occupation*/ + "\",\"";
                csv += odTrip.destinationActivity + "\",\"" + odTrip.mode + "\"," + std::to_string(odTrip.expansionFactor) + "," + std::to_string(routingResult.travelTimeSeconds) + ",";
                csv += std::to_string(odTrip.walkingTravelTimeSeconds) + "," + std::to_string(odTrip.departureTimeSeconds) + "," + std::to_string(routingResult.departureTimeSeconds) + "," + std::to_string(routingResult.minimizedDepartureTimeSeconds) + ",";
                csv += std::to_string(routingResult.arrivalTimeSeconds) + "," + std::to_string(routingResult.numberOfTransfers) + "," + std::to_string(routingResult.inVehicleTravelTimeSeconds) + ",";
                csv += std::to_string(routingResult.transferTravelTimeSeconds) + "," + std::to_string(routingResult.waitingTimeSeconds) + "," + std::to_string(routingResult.accessTravelTimeSeconds) + ",";
                csv += std::to_string(routingResult.egressTravelTimeSeconds) + "," + std::to_string(routingResult.transferWaitingTimeSeconds) + "," + std::to_string(routingResult.firstWaitingTimeSeconds) + ",";
                csv += std::to_string(routingResult.nonTransitTravelTimeSeconds) + ",";
                
                int countLineUuids = routingResult.lineUuids.size();
                j = 0;
                for (auto & lineUuid : routingResult.lineUuids)
                {
                  csv += boost::uuids::to_string(lineUuid);
                  if (j < countLineUuids - 1)
                  {
                    csv += "|";
                  }
                  j++;
                }
                csv += ",";
                j = 0;
                for (auto & modeShortname : routingResult.modeShortnames)
                {
                  csv += modeShortname;
                  if (j < countLineUuids - 1)
                  {
                    csv += "|";
                  }
                  j++;
                }
                csv += ",";
                j = 0;
                for (auto & agencyUuid : routingResult.agencyUuids)
                {
                  csv += boost::uuids::to_string(agencyUuid);
                  if (j < countLineUuids - 1)
                  {
                    csv += "|";
                  }
                  j++;
                }
                csv += ",";
                j = 0;
                for (auto & boardingNodeUuid : routingResult.boardingNodeUuids)
                {
                  csv += boost::uuids::to_string(boardingNodeUuid);
                  if (j < countLineUuids - 1)
                  {
                    csv += "|";
                  }
                  j++;
                }
                csv += ",";
                j = 0;
                for (auto & unboardingNodeUuid : routingResult.unboardingNodeUuids)
                {
                  csv += boost::uuids::to_string(unboardingNodeUuid);
                  if (j < countLineUuids - 1)
                  {
                    csv += "|";
                  }
                  j++;
                }
                csv += ",";
                j = 0;
                for (auto & tripUuid : routingResult.tripUuids)
                {
                  csv += boost::uuids::to_string(tripUuid);
                  if (j < countLineUuids - 1)
                  {
                    csv += "|";
                  }
                  j++;
                }
                csv += "\n";
              }
              else
              {
                odTripJson = {};
                odTripJson["uuid"]                          = boost::uuids::to_string(odTrip.uuid);
                odTripJson["status"]                        = routingResult.status;
                /*odTripJson["ageGroup"]                    = calculator.persons[odTrip.personIdx].ageGroup; // this fails (segmentation fault)...
                odTripJson["gender"]                        = calculator.persons[odTrip.personIdx].gender;
                odTripJson["occupation"]                    = calculator.persons[odTrip.personIdx].occupation;*/
                odTripJson["internalId"]                    = odTrip.internalId;
                odTripJson["originActivity"]                = odTrip.originActivity;
                odTripJson["destinationActivity"]           = odTrip.destinationActivity;
                odTripJson["declaredMode"]                  = odTrip.mode;
                odTripJson["expansionFactor"]               = odTrip.expansionFactor;
                odTripJson["travelTimeSeconds"]             = routingResult.travelTimeSeconds;
                odTripJson["minimizedTravelTimeSeconds"]    = routingResult.travelTimeSeconds - routingResult.firstWaitingTimeSeconds + calculator.params.minWaitingTimeSeconds;
                odTripJson["onlyWalkingTravelTimeSeconds"]  = odTrip.walkingTravelTimeSeconds;
                odTripJson["declaredDepartureTimeSeconds"]  = odTrip.departureTimeSeconds;
                odTripJson["declaredArrivalTimeSeconds"]    = odTrip.arrivalTimeSeconds;
                odTripJson["departureTimeSeconds"]          = routingResult.departureTimeSeconds;
                odTripJson["minimizedDepartureTimeSeconds"] = routingResult.minimizedDepartureTimeSeconds;
                odTripJson["arrivalTimeSeconds"]            = routingResult.arrivalTimeSeconds;
                odTripJson["numberOfTransfers"]             = routingResult.numberOfTransfers;
                odTripJson["inVehicleTravelTimeSeconds"]    = routingResult.inVehicleTravelTimeSeconds;
                odTripJson["transferTravelTimeSeconds"]     = routingResult.transferTravelTimeSeconds;
                odTripJson["waitingTimeSeconds"]            = routingResult.waitingTimeSeconds;
                odTripJson["accessTravelTimeSeconds"]       = routingResult.accessTravelTimeSeconds;
                odTripJson["egressTravelTimeSeconds"]       = routingResult.egressTravelTimeSeconds;
                odTripJson["transferWaitingTimeSeconds"]    = routingResult.transferWaitingTimeSeconds;
                odTripJson["firstWaitingTimeSeconds"]       = routingResult.firstWaitingTimeSeconds;
                odTripJson["nonTransitTravelTimeSeconds"]   = routingResult.nonTransitTravelTimeSeconds;
                odTripJson["lineUuids"]                     = Toolbox::uuidsToStrings(routingResult.lineUuids);
                odTripJson["modesShortnames"]               = routingResult.modeShortnames;
                odTripJson["agencyUuids"]                   = Toolbox::uuidsToStrings(routingResult.agencyUuids);
                odTripJson["boardingNodeUuids"]             = Toolbox::uuidsToStrings(routingResult.boardingNodeUuids);
                odTripJson["unboardingNodeUuids"]           = Toolbox::uuidsToStrings(routingResult.unboardingNodeUuids);
                odTripJson["tripUuids"]                     = Toolbox::uuidsToStrings(routingResult.tripUuids);
                json["odTrips"].push_back(odTripJson);
              }
            }
            //calculator.benchmarking["generating_results"] += calculator.algorithmCalculationTime.getEpoch() - benchmarkingStart;
          }
          i++;
          if (odTripsSampleSize >= 0 && i >= odTripsSampleSize)
          {
            break;
          }
        }
        if (fileFormat != "csv")
        {
          json["maxSegmentHourlyDemand"] = maximumSegmentHourlyDemand;
          json["maxSegmentTotalDemand"]  = maximumSegmentTotalDemand;
          lineProfilesJson = {};
          for (auto & lineCount : lineProfiles)
          {
            lineProfilesJson[boost::uuids::to_string(lineCount.first)] = lineCount.second;
          }
          json["lineProfiles"] = lineProfilesJson;
          
          pathProfilesJson = {};
          for (auto & pathProfile : pathProfiles)
          {
            pathProfilesJson[boost::uuids::to_string(pathProfile.first)] = pathProfile.second;
            //pathsOdTripsProfilesSequenceJson = {};
            /*for (auto & segmentProfile : pathProfile)
            {
              
              //pathsOdTripsProfilesOdTripUuids.clear();
              //for (auto & odTripUuid : std::get<1>(sequenceProfile.second))
              //{
              //  pathsOdTripsProfilesOdTripUuids.push_back()
              //}
              //pathsOdTripsProfilesSequenceJson[std::to_string(sequenceProfile.first)] = {{"demand", std::get<0>(sequenceProfile.second)}, {"odTripUuids", Toolbox::uuidsToStrings(std::get<1>(sequenceProfile.second))}};
            }*/
            //pathsOdTripsProfilesJson[boost::uuids::to_string(pathProfile.first)] = pathsProfile;
          }
          json["pathProfiles"] = pathProfilesJson;
          resultStr = json.dump(2);
        }
        if (calculator.params.calculateAllOdTrips && fileFormat == "csv")
        {
          
          if (saveToFile)
          {
            calculationName += "__batch_" + std::to_string(batchNumber) + "_of_" + std::to_string(batchesCount);
            std::cerr << "writing csv file" << std::endl;
            std::ofstream csvFile;
            //csvFile.imbue(std::locale("en_US.UTF8"));
            csvFile.open(calculationName + ".csv", std::ios_base::trunc);
            csvFile << csv;
            csvFile.close();
            //resultStr = "{\"status\": \"success\", \"format\": \"csv\", \"filename\": \"" + calculationName + ".csv\"}";
          }
        }
      }
      else if (!calculator.params.alternatives)
      {
        resultStr = calculator.calculate().json;
      }

      //calculator.algorithmCalculationTime.stopStep();
      
      //std::cout << "-- parsing request -- " << calculator.algorithmCalculationTime.getStepDurationMicroseconds() << " microseconds\n";
      
      //calculator.reset();
      
      //calculator.algorithmCalculationTime.stopStep();
      //calculator.algorithmCalculationTime.startStep();
      
      //calculator.resetAccessEgressModes();
      
      //calculator.algorithmCalculationTime.stopStep();
      
      //std::cout << "-- reset access egress modes -- " << calculator.algorithmCalculationTime.getStepDurationMicroseconds() << " microseconds\n";
      
      
      //calculator.algorithmCalculationTime.startStep();
      if (saveToFile && fileFormat != "csv")
      {
        std::cerr << "writing json file" << std::endl;
        std::ofstream jsonFile;
        //jsonFile.imbue(std::locale("en_US.UTF8"));
        jsonFile.open(calculationName + ".json", std::ios_base::trunc);
        jsonFile << resultStr;
        jsonFile.close();
      }
      
    }
    else
    {
      resultStr = "{\"status\": \"failed\", \"error\": \"Wrong or malformed query\"}";
    }
    
    std::cerr << "-- total -- " << calculator.algorithmCalculationTime.getDurationMicrosecondsNoStop() << " microseconds\n";
    for (auto & benchmark : calculator.benchmarking)
    {
      std::cerr << "  -- " << benchmark.first << " -- " << benchmark.second / 1000 << " ms ";
      if (countOdTripsCalculated > 0)
      {
        std::cerr << " (" << (benchmark.second / countOdTripsCalculated / 1000) << " per odTrip)";
      }
      std::cerr << "\n";
    }
    if (countOdTripsCalculated > 0)
    {
      std::cerr << "  -- number of od trips calculated -- " << countOdTripsCalculated << "\n";
    }
    
    //calculator.algorithmCalculationTime.stop();
      
    //std::cerr << "-- calculation time -- " << calculator.algorithmCalculationTime.getDurationMicroseconds() << " microseconds\n";
    
    if (fileFormat == "csv")
    {
      *response << "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/csv; charset=utf-8\r\nContent-Length: " << csv.length() << "\r\n\r\n" << csv;
    }
    else
    {
      *response << "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: " << resultStr.length() << "\r\n\r\n" << resultStr;
    }
    
  };
  
  
  std::cout << "starting server..." << std::endl;
  std::thread server_thread([&server](){
      server.start();
  });
    
  // Wait for server to start so that the client can connect:
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  std::cout << "ready." << std::endl;
  
  server_thread.join();
  
  std::cout << "done..." << std::endl;
  
  return 0;
}



void default_resource_send(const HttpServer &server, const std::shared_ptr<HttpServer::Response> &response, const std::shared_ptr<std::ifstream> &ifs) {
  
  // Read and send 128 KB at a time:
  static std::vector<char> buffer(131072);
  
  // Safe when server is running on one thread:
  std::streamsize read_length;
  if ((read_length = ifs->read(&buffer[0], buffer.size()).gcount())>0)
  {
    response->write(&buffer[0], read_length);
    if (read_length==static_cast<std::streamsize>(buffer.size()))
    {
      server.send(response, [&server, response, ifs](const boost::system::error_code &ec)
      {
        if (!ec)
        {
          default_resource_send(server, response, ifs);
        }
        else
        {
          std::cerr << "Connection interrupted" << std::endl;
        }
      });
    }
  }
}
