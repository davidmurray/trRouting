#include "osrm_fetcher.hpp"
#include "json.hpp"
#include "point.hpp"
#include "node.hpp"
#include "client_http.hpp"
#include "spdlog/spdlog.h"

namespace TrRouting
{

  //TODO Remove this initialization when this data is not static anymore
  bool OsrmFetcher::birdDistanceAccessibilityEnabled = false;
  std::string OsrmFetcher::osrmWalkingPort="";
  std::string OsrmFetcher::osrmCyclingPort="";
  std::string OsrmFetcher::osrmDrivingPort="";
  std::string OsrmFetcher::osrmWalkingHost="";
  std::string OsrmFetcher::osrmCyclingHost="";
  std::string OsrmFetcher::osrmDrivingHost="";

  std::vector<NodeTimeDistance> OsrmFetcher::getAccessibleNodesFootpathsFromPoint(const Point &point, const std::map<boost::uuids::uuid, Node> &nodes, std::string mode, int maxWalkingTravelTime, float walkingSpeedMetersPerSecond, bool reversed)
  {
    if (OsrmFetcher::birdDistanceAccessibilityEnabled)
    {
      return getNodesFromBirdDistance(point, nodes, maxWalkingTravelTime, walkingSpeedMetersPerSecond);
    }
    else
    {
      return getNodesFromOsrm(point, nodes, mode, maxWalkingTravelTime, walkingSpeedMetersPerSecond, reversed);
    }
  }

  std::vector<NodeTimeDistance> OsrmFetcher::getNodesFromBirdDistance(const Point &point, const std::map<boost::uuids::uuid, Node> &nodes, int maxWalkingTravelTime, float walkingSpeedMetersPerSecond)
  {
    std::vector<NodeTimeDistance> accessibleNodesFootpaths;

    auto lengthOfOneDegree = calculateLengthOfOneDegree(point);
    float maxDistanceMetersSquared = calculateMaxDistanceSquared(maxWalkingTravelTime, walkingSpeedMetersPerSecond);
    float distanceMetersSquared;

    spdlog::debug("use of bird distance ");

    for (auto &&[uuid,node] : nodes)
    {
      distanceMetersSquared = calculateNodeDistanceSquared(node.point.get(), point, lengthOfOneDegree);

      if (distanceMetersSquared <= maxDistanceMetersSquared)
      {
        int distanceMeters = sqrt(distanceMetersSquared);
        int travelTimeSeconds = distanceMeters / walkingSpeedMetersPerSecond;
        accessibleNodesFootpaths.push_back(NodeTimeDistance(node, travelTimeSeconds, distanceMeters));
      }
    }

    spdlog::debug("fetched footpaths using bird distance ({} footpaths found)", accessibleNodesFootpaths.size());

    return accessibleNodesFootpaths;
  }

  std::vector<NodeTimeDistance> OsrmFetcher::getNodesFromOsrm(const Point &point, const std::map<boost::uuids::uuid, Node> &nodes, std::string mode, int maxWalkingTravelTime, float walkingSpeedMetersPerSecond, bool reversed)
  {
    std::vector<std::reference_wrapper<const Node>> birdDistanceAccessibleNodeIndexes;
    std::vector<NodeTimeDistance> accessibleNodesFootpaths;

    auto lengthOfOneDegree = calculateLengthOfOneDegree(point);
    float maxDistanceMetersSquared = calculateMaxDistanceSquared(maxWalkingTravelTime, walkingSpeedMetersPerSecond);
    float distanceMetersSquared;

    spdlog::debug("osrm with host {} and port {}", OsrmFetcher::osrmWalkingHost, OsrmFetcher::osrmWalkingPort);

    std::string queryString = "/table/v1/" + mode + "/" + std::to_string(point.longitude) + "," + std::to_string(point.latitude);

    for (auto &&[uuid,node] : nodes)
    {
      distanceMetersSquared = calculateNodeDistanceSquared(node.point.get(), point, lengthOfOneDegree);

      if (distanceMetersSquared <= maxDistanceMetersSquared)
      {
        birdDistanceAccessibleNodeIndexes.push_back(node);
        queryString += ";" + std::to_string(node.point.get()->longitude) + "," + std::to_string(node.point.get()->latitude);
      }
    }

    queryString += "?annotations=duration,distance";

    if (reversed)
    {
      queryString += "&destinations=0";
    }
    else
    {
      queryString += "&sources=0";
    }

    using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;
    HttpClient client(OsrmFetcher::osrmWalkingHost + ":" + OsrmFetcher::osrmWalkingPort);
    auto s = client.request("GET", queryString);

    std::stringstream responseJsonSs;
    responseJsonSs << s->content.rdbuf();
    nlohmann::json responseJson = nlohmann::json::parse(responseJsonSs.str());

    if (responseJson["durations"] != nullptr && responseJson["distances"] != nullptr && responseJson["durations"][0] != nullptr && responseJson["distances"][0] != nullptr)
    {
      int numberOfDurations = responseJson["durations"][0].size();
      int numberOfDistances = responseJson["distances"][0].size();

      if (numberOfDurations > 0 && numberOfDistances > 0)
      {
        int travelTimeSeconds;
        int distanceMeters;
        for (int i = 1; i < numberOfDurations; i++) // ignore first (duration with itself)
        {
          travelTimeSeconds = (int)ceil((float)responseJson["durations"][0][i]);
          if (travelTimeSeconds <= maxWalkingTravelTime)
          {
            distanceMeters = (int)ceil((float)responseJson["distances"][0][i]);
            accessibleNodesFootpaths.push_back(NodeTimeDistance(birdDistanceAccessibleNodeIndexes[i - 1],
                                                                travelTimeSeconds,
                                                                distanceMeters));
          }
        }
      }
    }

    spdlog::debug("fetched osrm footpaths ({} footpaths found)",  accessibleNodesFootpaths.size());

    return accessibleNodesFootpaths;
  }

  std::tuple<float, float> OsrmFetcher::calculateLengthOfOneDegree(const Point &point)
  {
    // Taylor series approximation of the longitude and latitude length functions (For more info: https://gis.stackexchange.com/questions/75528/understanding-terms-in-length-of-degree-formula)
    float lengthOfOneDegreeOfLongitude = 111412.84 * cos(point.latitude * M_PI / 180) - 93.5 * cos(3 * point.latitude * M_PI / 180);
    float lengthOfOneDegreeOflatitude = 111132.92 - 559.82 * cos(2 * point.latitude * M_PI / 180) + 1.175 * cos(4 * point.latitude * M_PI / 180);

    return std::make_tuple(lengthOfOneDegreeOfLongitude, lengthOfOneDegreeOflatitude);
  }

  float OsrmFetcher::calculateMaxDistanceSquared(int maxWalkingTravelTime, float walkingSpeedMetersPerSecond)
  {
    return (maxWalkingTravelTime * walkingSpeedMetersPerSecond) * (maxWalkingTravelTime * walkingSpeedMetersPerSecond);
  }

  float OsrmFetcher::calculateNodeDistanceSquared(const Point *node, const Point &point, const std::tuple<float, float> &lengthOfOneDegree)
  {
    float distanceXMeters = (node->longitude - point.longitude) * std::get<0>(lengthOfOneDegree);
    float distanceYMeters = (node->latitude - point.latitude) * std::get<1>(lengthOfOneDegree);
    float distanceMetersSquared = distanceXMeters * distanceXMeters + distanceYMeters * distanceYMeters;

    return distanceMetersSquared;
  }

}
