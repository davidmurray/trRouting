
#ifndef TR_OD_TRIPS_CACHE_FETCHER
#define TR_OD_TRIPS_CACHE_FETCHER

#include <string>
#include <vector>
#include <fcntl.h>
#include <boost/uuid/string_generator.hpp>
#include <capnp/serialize-packed.h>
#include "cache_fetcher.hpp"
#include "od_trip.hpp"
#include "point.hpp"
#include "capnp/odTripCollection.capnp.h"
#include "capnp/odTrip.capnp.h"
#include "spdlog/spdlog.h"

namespace TrRouting
{

  int CacheFetcher::getOdTrips(
    std::vector<std::unique_ptr<OdTrip>>& ts,
    std::map<boost::uuids::uuid, int>& tIndexesByUuid,
    std::map<boost::uuids::uuid, int>& dataSourceIndexesByUuid,
    std::map<boost::uuids::uuid, int>& householdIndexesByUuid,
    std::map<boost::uuids::uuid, int>& personIndexesByUuid,
    std::map<boost::uuids::uuid, int>& nodeIndexesByUuid,
    std::string customPath
  )
  {

    using T           = OdTrip;
    using TCollection = odTripCollection::OdTripCollection;
    using cT          = odTrip::OdTrip;

    std::string tStr  = "odTrips";
    std::string TStr  = "OdTrips";

    std::string cacheFileName{tStr};
    boost::uuids::string_generator uuidGenerator;

    spdlog::info("Fetching {} from cache... {}", tStr, customPath);

    for(std::map<boost::uuids::uuid, int>::iterator iter = dataSourceIndexesByUuid.begin(); iter != dataSourceIndexesByUuid.end(); ++iter)
    {
      boost::uuids::uuid dataSourceUuid = iter->first;

      std::string cacheFilePath {"dataSources/" + boost::uuids::to_string(dataSourceUuid) + "/" + cacheFileName};

      int filesCount {CacheFetcher::getCacheFilesCount(getFilePath(cacheFilePath + ".capnpbin.count", customPath))};

      spdlog::info("files count odTrips: {} path: {}", filesCount, cacheFilePath);

      for (int i = 0; i < filesCount; i++)
      {
        std::string filePath {cacheFilePath + ".capnpbin" + (filesCount > 1 ? "." + std::to_string(i) : "")};
        std::string cacheFilePath = getFilePath(filePath, customPath);

        int fd = open(cacheFilePath.c_str(), O_RDWR);
        if (fd < 0)
        {
          int err = errno;
          if (err == ENOENT)
          {
            spdlog::error("missing {} cache files!", filePath);
          }
          else
          {
            spdlog::error("Error opening cache file {} : {} ", filePath, err);
          }
          continue;
        }

        try
        {   
          ::capnp::PackedFdMessageReader capnpTCollectionMessage(fd, {512 * 1024 * 1024});
          TCollection::Reader capnpTCollection = capnpTCollectionMessage.getRoot<TCollection>();
          for (cT::Reader capnpT : capnpTCollection.getOdTrips())
          {
            std::string uuid           {capnpT.getUuid()};
            std::string dataSourceUuid {capnpT.getDataSourceUuid()};
            std::string householdUuid  {capnpT.getHouseholdUuid()};
            std::string personUuid     {capnpT.getPersonUuid()};

            std::unique_ptr<T> t               = std::make_unique<T>();
            std::unique_ptr<Point> origin      = std::make_unique<Point>();
            std::unique_ptr<Point> destination = std::make_unique<Point>();

            t->uuid                     = uuidGenerator(uuid);
            t->id                       = capnpT.getId();
            t->expansionFactor          = capnpT.getExpansionFactor();
            if (t->expansionFactor == -1.0) {
              t->expansionFactor = 1.0;
            }
            t->internalId               = capnpT.getInternalId();
            t->departureTimeSeconds     = capnpT.getDepartureTimeSeconds();
            t->arrivalTimeSeconds       = capnpT.getArrivalTimeSeconds();
            t->walkingTravelTimeSeconds = capnpT.getWalkingTravelTimeSeconds();
            t->cyclingTravelTimeSeconds = capnpT.getCyclingTravelTimeSeconds();
            t->drivingTravelTimeSeconds = capnpT.getDrivingTravelTimeSeconds();

            origin->latitude          = ((double)capnpT.getOriginLatitude())       / 1000000.0;
            origin->longitude         = ((double)capnpT.getOriginLongitude())      / 1000000.0;
            destination->latitude     = ((double)capnpT.getDestinationLatitude())  / 1000000.0;
            destination->longitude    = ((double)capnpT.getDestinationLongitude()) / 1000000.0;
            t->origin                 = std::move(origin);
            t->destination            = std::move(destination);

            t->dataSourceIdx   = dataSourceUuid.length() > 0 ? dataSourceIndexesByUuid[uuidGenerator(dataSourceUuid)] : -1;
            t->householdIdx    = householdUuid.length()  > 0 ? householdIndexesByUuid[uuidGenerator(householdUuid)]   : -1;
            t->personIdx       = personUuid.length()     > 0 ? personIndexesByUuid[uuidGenerator(personUuid)]         : -1;

            switch (capnpT.getMode()) {
              case odTrip::OdTrip::Mode::NONE             : t->mode = "none";            break;
              case odTrip::OdTrip::Mode::WALKING          : t->mode = "walking";         break;
              case odTrip::OdTrip::Mode::CYCLING          : t->mode = "cycling";         break;
              case odTrip::OdTrip::Mode::CAR_DRIVER       : t->mode = "carDriver";       break;
              case odTrip::OdTrip::Mode::CAR_PASSENGER    : t->mode = "carPassenger";    break;
              case odTrip::OdTrip::Mode::MOTORCYCLE       : t->mode = "motorcycle";      break;
              case odTrip::OdTrip::Mode::TRANSIT          : t->mode = "transit";         break;
              case odTrip::OdTrip::Mode::PARATRANSIT      : t->mode = "paratransit";     break;
              case odTrip::OdTrip::Mode::TAXI             : t->mode = "taxi";            break;
              case odTrip::OdTrip::Mode::SCHOOL_BUS       : t->mode = "schoolBus";       break;
              case odTrip::OdTrip::Mode::OTHER_BUS        : t->mode = "otherBus";        break;
              case odTrip::OdTrip::Mode::INTERCITY_BUS    : t->mode = "intercityBus";    break;
              case odTrip::OdTrip::Mode::INTERCITY_TRAIN  : t->mode = "intercityTrain";  break;
              case odTrip::OdTrip::Mode::PLANE            : t->mode = "plane";           break;
              case odTrip::OdTrip::Mode::FERRY            : t->mode = "ferry";           break;
              case odTrip::OdTrip::Mode::PARK_AND_RIDE    : t->mode = "parkAndRide";     break;
              case odTrip::OdTrip::Mode::KISS_AND_RIDE    : t->mode = "kissAndRide";     break;
              case odTrip::OdTrip::Mode::BIKE_AND_RIDE    : t->mode = "bikeAndRide";     break;
              case odTrip::OdTrip::Mode::MULTIMODAL_OTHER : t->mode = "multimodalOther"; break;
              case odTrip::OdTrip::Mode::OTHER            : t->mode = "other";           break;
              case odTrip::OdTrip::Mode::UNKNOWN          : t->mode = "unknown";         break;
            }

            switch (capnpT.getOriginActivity()) {
              case odTrip::OdTrip::Activity::NONE             : t->originActivity = "none";            break;
              case odTrip::OdTrip::Activity::HOME             : t->originActivity = "home";            break;
              case odTrip::OdTrip::Activity::WORK_USUAL       : t->originActivity = "workUsual";       break;
              case odTrip::OdTrip::Activity::WORK_NON_USUAL   : t->originActivity = "workNonUsual";    break;
              case odTrip::OdTrip::Activity::SCHOOL_USUAL     : t->originActivity = "schoolUsual";     break;
              case odTrip::OdTrip::Activity::SCHOOL_NON_USUAL : t->originActivity = "schoolNonUsual";  break;
              case odTrip::OdTrip::Activity::SHOPPING         : t->originActivity = "shopping";        break;
              case odTrip::OdTrip::Activity::LEISURE          : t->originActivity = "leisure";         break;
              case odTrip::OdTrip::Activity::SERVICE          : t->originActivity = "service";         break;
              case odTrip::OdTrip::Activity::SECONDARY_HOME   : t->originActivity = "secondaryHome";   break;
              case odTrip::OdTrip::Activity::VISITING_FRIENDS : t->originActivity = "visitingFriends"; break;
              case odTrip::OdTrip::Activity::DROP_SOMEONE     : t->originActivity = "dropSomeone";     break;
              case odTrip::OdTrip::Activity::FETCH_SOMEONE    : t->originActivity = "fetchSomeone";    break;
              case odTrip::OdTrip::Activity::RESTAURANT       : t->originActivity = "restaurant";      break;
              case odTrip::OdTrip::Activity::MEDICAL          : t->originActivity = "medical";         break;
              case odTrip::OdTrip::Activity::WORSHIP          : t->originActivity = "worship";         break;
              case odTrip::OdTrip::Activity::ON_THE_ROAD      : t->originActivity = "onTheRoad";       break;
              case odTrip::OdTrip::Activity::OTHER            : t->originActivity = "other";           break;
              case odTrip::OdTrip::Activity::UNKNOWN          : t->originActivity = "unknown";         break;
            }

            switch (capnpT.getDestinationActivity()) {
              case odTrip::OdTrip::Activity::NONE             : t->destinationActivity = "none";            break;
              case odTrip::OdTrip::Activity::HOME             : t->destinationActivity = "home";            break;
              case odTrip::OdTrip::Activity::WORK_USUAL       : t->destinationActivity = "workUsual";       break;
              case odTrip::OdTrip::Activity::WORK_NON_USUAL   : t->destinationActivity = "workNonUsual";    break;
              case odTrip::OdTrip::Activity::SCHOOL_USUAL     : t->destinationActivity = "schoolUsual";     break;
              case odTrip::OdTrip::Activity::SCHOOL_NON_USUAL : t->destinationActivity = "schoolNonUsual";  break;
              case odTrip::OdTrip::Activity::SHOPPING         : t->destinationActivity = "shopping";        break;
              case odTrip::OdTrip::Activity::LEISURE          : t->destinationActivity = "leisure";         break;
              case odTrip::OdTrip::Activity::SERVICE          : t->destinationActivity = "service";         break;
              case odTrip::OdTrip::Activity::SECONDARY_HOME   : t->destinationActivity = "secondaryHome";   break;
              case odTrip::OdTrip::Activity::VISITING_FRIENDS : t->destinationActivity = "visitingFriends"; break;
              case odTrip::OdTrip::Activity::DROP_SOMEONE     : t->destinationActivity = "dropSomeone";     break;
              case odTrip::OdTrip::Activity::FETCH_SOMEONE    : t->destinationActivity = "fetchSomeone";    break;
              case odTrip::OdTrip::Activity::RESTAURANT       : t->destinationActivity = "restaurant";      break;
              case odTrip::OdTrip::Activity::MEDICAL          : t->destinationActivity = "medical";         break;
              case odTrip::OdTrip::Activity::WORSHIP          : t->destinationActivity = "worship";         break;
              case odTrip::OdTrip::Activity::ON_THE_ROAD      : t->destinationActivity = "onTheRoad";       break;
              case odTrip::OdTrip::Activity::OTHER            : t->destinationActivity = "other";           break;
              case odTrip::OdTrip::Activity::UNKNOWN          : t->destinationActivity = "unknown";         break;
            }

            const unsigned int originNodesCount {capnpT.getOriginNodesUuids().size()};
            std::vector<int> originNodesIdx(originNodesCount);
            std::vector<int> originNodesTravelTimesSeconds(originNodesCount);
            std::vector<int> originNodesDistancesMeters(originNodesCount);
            for (int i = 0; i < originNodesCount; i++)
            {
              std::string nodeUuid {capnpT.getOriginNodesUuids()[i]};
              originNodesIdx               [i] = nodeIndexesByUuid[uuidGenerator(nodeUuid)];
              originNodesTravelTimesSeconds[i] = capnpT.getOriginNodesTravelTimes()[i];
              originNodesDistancesMeters   [i] = capnpT.getOriginNodesDistances()[i];
            }
            t->originNodesIdx                = originNodesIdx;
            t->originNodesTravelTimesSeconds = originNodesTravelTimesSeconds;
            t->originNodesDistancesMeters    = originNodesDistancesMeters;

            const unsigned int destinationNodesCount {capnpT.getDestinationNodesUuids().size()};
            std::vector<int> destinationNodesIdx(destinationNodesCount);
            std::vector<int> destinationNodesTravelTimesSeconds(destinationNodesCount);
            std::vector<int> destinationNodesDistancesMeters(destinationNodesCount);
            for (int i = 0; i < destinationNodesCount; i++)
            {
              std::string nodeUuid {capnpT.getDestinationNodesUuids()[i]};
              destinationNodesIdx               [i] = nodeIndexesByUuid[uuidGenerator(nodeUuid)];
              destinationNodesTravelTimesSeconds[i] = capnpT.getDestinationNodesTravelTimes()[i];
              destinationNodesDistancesMeters   [i] = capnpT.getDestinationNodesDistances()[i];
            }
            t->destinationNodesIdx                = destinationNodesIdx;
            t->destinationNodesTravelTimesSeconds = destinationNodesTravelTimesSeconds;
            t->destinationNodesDistancesMeters    = destinationNodesDistancesMeters;

            tIndexesByUuid[t->uuid] = ts.size();
            ts.push_back(std::move(t));

          }

          spdlog::info("parsed {} od trips", ts.size());
        }
        catch (const kj::Exception& e)
        {
          spdlog::error("Error opening cache file {}: {}", filePath, e.getDescription().cStr());
        }
        catch (...)
        {
          spdlog::error("Unknown error occurred {} ", filePath);
        }

        close(fd);

      }

    }
    return 0;

  }

}

#endif // TR_OD_TRIPS_CACHE_FETCHER
