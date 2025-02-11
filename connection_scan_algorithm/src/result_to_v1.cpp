#include "json.hpp"
#include "constants.hpp"
#include "result_constants.hpp"
#include "result_to_v1.hpp"
#include "toolbox.hpp"
#include "parameters.hpp"
#include "routing_result.hpp"
#include "trip.hpp"
#include "agency.hpp"
#include "line.hpp"
#include "mode.hpp"
#include "node.hpp"
#include "path.hpp"


namespace TrRouting
{

  /**
   * @brief Visitor for the result's steps
   */
  class StepToV1Visitor: public StepVisitor<nlohmann::json> {
  private:
    nlohmann::json response;
  public:
    nlohmann::json getResult() override { return response; }
    void visitBoardingStep(const BoardingStep& step) override;
    void visitUnboardingStep(const UnboardingStep& step) override;
    void visitWalkingStep(const WalkingStep& step) override;
  };

  /**
   * @brief Visitor for the result object
   */
  class ResultToV1Visitor: public ResultVisitor<nlohmann::json> {
  private:
    nlohmann::json response;
    RouteParameters& params;
  public:
    ResultToV1Visitor(RouteParameters& _params): params(_params) {
      // Nothing to initialize
    }
    nlohmann::json getResult() override { return response; }
    void visitSingleCalculationResult(const SingleCalculationResult& result) override;
    void visitAlternativesResult(const AlternativesResult& result) override;
    void visitAllNodesResult(const AllNodesResult& result) override;
  };

  void StepToV1Visitor::visitBoardingStep(const BoardingStep& step)
  {
    nlohmann::json stepJson;
    stepJson["action"] = "board";
    stepJson["agencyAcronym"] = step.trip.agency.acronym;
    stepJson["agencyName"] = step.trip.agency.name;
    stepJson["agencyUuid"] = boost::uuids::to_string(step.trip.agency.uuid);
    stepJson["lineShortname"] = step.trip.line.shortname;
    stepJson["lineLongname"] = step.trip.line.longname;
    stepJson["lineUuid"] = boost::uuids::to_string(step.trip.line.uuid);
    stepJson["pathUuid"] = boost::uuids::to_string(step.trip.path.uuid);
    stepJson["modeName"] = step.trip.line.mode.name;
    stepJson["mode"] = step.trip.line.mode.shortname;
    stepJson["tripUuid"] = boost::uuids::to_string(step.trip.uuid);
    stepJson["legSequenceInTrip"] = step.legSequenceInTrip;
    stepJson["stopSequenceInTrip"] = step.stopSequenceInTrip;
    stepJson["nodeName"] = step.node.name;
    stepJson["nodeCode"] = step.node.code;
    stepJson["nodeUuid"] = boost::uuids::to_string(step.node.uuid);
    stepJson["nodeCoordinates"] = {step.node.point->longitude, step.node.point->latitude};
    stepJson["departureTime"] = Toolbox::convertSecondsToFormattedTime(step.departureTime);
    stepJson["departureTimeSeconds"] = step.departureTime;
    stepJson["waitingTimeSeconds"] = step.waitingTime;
    stepJson["waitingTimeMinutes"] = Toolbox::convertSecondsToMinutes(step.waitingTime);
    response = stepJson;
  }

  void StepToV1Visitor::visitUnboardingStep(const UnboardingStep& step)
  {
    nlohmann::json stepJson;
    stepJson["action"] = "unboard";
    stepJson["agencyAcronym"] = step.trip.agency.acronym;
    stepJson["agencyName"] = step.trip.agency.name;
    stepJson["agencyUuid"] = boost::uuids::to_string(step.trip.agency.uuid);
    stepJson["lineShortname"] = step.trip.line.shortname;
    stepJson["lineLongname"] = step.trip.line.longname;
    stepJson["lineUuid"] = boost::uuids::to_string(step.trip.line.uuid);
    stepJson["pathUuid"] = boost::uuids::to_string(step.trip.path.uuid);
    stepJson["modeName"] = step.trip.line.mode.name;
    stepJson["mode"] = step.trip.line.mode.shortname;
    stepJson["tripUuid"] = boost::uuids::to_string(step.trip.uuid);
    stepJson["legSequenceInTrip"] = step.legSequenceInTrip;
    stepJson["stopSequenceInTrip"] = step.stopSequenceInTrip;
    stepJson["nodeName"] = step.node.name;
    stepJson["nodeCode"] = step.node.code;
    stepJson["nodeUuid"] = boost::uuids::to_string(step.node.uuid);
    stepJson["nodeCoordinates"] = {step.node.point->longitude, step.node.point->latitude};
    stepJson["arrivalTime"] = Toolbox::convertSecondsToFormattedTime(step.arrivalTime);
    stepJson["arrivalTimeSeconds"] = step.arrivalTime;
    stepJson["inVehicleTimeSeconds"] = step.inVehicleTime;
    stepJson["inVehicleTimeMinutes"] = Toolbox::convertSecondsToMinutes(step.inVehicleTime);
    stepJson["inVehicleDistanceMeters"] = step.inVehicleDistanceMeters;
    response = stepJson;
  }

  void StepToV1Visitor::visitWalkingStep(const WalkingStep& step)
  {
    nlohmann::json stepJson;
    stepJson["action"] = "walking";
    stepJson["type"] = step.walkingType == walking_step_type::ACCESS ? "access" : step.walkingType == walking_step_type::EGRESS ? "egress" : "transfer";
    stepJson["travelTimeSeconds"] = step.travelTime;
    stepJson["travelTimeMinutes"] = Toolbox::convertSecondsToMinutes(step.travelTime);
    stepJson["distanceMeters"] = step.distanceMeters;
    stepJson["departureTime"] = Toolbox::convertSecondsToFormattedTime(step.departureTime);
    stepJson["arrivalTime"] = Toolbox::convertSecondsToFormattedTime(step.arrivalTime);
    stepJson["departureTimeSeconds"] = step.departureTime;
    stepJson["arrivalTimeSeconds"] = step.arrivalTime;
    if (step.walkingType != walking_step_type::EGRESS) {
      stepJson["readyToBoardAt"] = Toolbox::convertSecondsToFormattedTime(step.readyToBoardAt);
    }
    response = stepJson;
  }

  void ResultToV1Visitor::visitSingleCalculationResult(const SingleCalculationResult& result)
  {
    nlohmann::json json;
    json["status"] = STATUS_SUCCESS;
    json["origin"] = { params.getOrigin()->latitude, params.getOrigin()->longitude };
    json["destination"] = { params.getDestination()->latitude, params.getDestination()->longitude };
    json["departureTime"] = Toolbox::convertSecondsToFormattedTime(result.departureTime);
    json["departureTimeSeconds"] = result.departureTime;
    // Add initial times only if the departure time was requested
    if (params.isForwardCalculation()) {
      json["initialDepartureTime"] = Toolbox::convertSecondsToFormattedTime(params.getTimeOfTrip());
      json["initialDepartureTimeSeconds"] = params.getTimeOfTrip();
      json["initialLostTimeAtDepartureSeconds"] = result.departureTime - params.getTimeOfTrip();
      json["initialLostTimeAtDepartureMinutes"] = Toolbox::convertSecondsToMinutes(result.departureTime - params.getTimeOfTrip());
    }
    json["arrivalTime"] = Toolbox::convertSecondsToFormattedTime(result.arrivalTime);
    json["arrivalTimeSeconds"] = result.arrivalTime;
    json["totalTravelTimeMinutes"] = Toolbox::convertSecondsToMinutes(result.totalTravelTime);
    json["totalTravelTimeSeconds"] = result.totalTravelTime;
    json["totalDistanceMeters"] = result.totalDistance;
    json["totalInVehicleTimeMinutes"] = Toolbox::convertSecondsToMinutes(result.totalInVehicleTime);
    json["totalInVehicleTimeSeconds"] = result.totalInVehicleTime;
    json["totalInVehicleDistanceMeters"] = result.totalInVehicleDistance;
    json["totalNonTransitTravelTimeMinutes"] = Toolbox::convertSecondsToMinutes(result.totalNonTransitTravelTime);
    json["totalNonTransitTravelTimeSeconds"] = result.totalNonTransitTravelTime;
    json["totalNonTransitDistanceMeters"] = result.totalNonTransitDistance;
    json["numberOfBoardings"] = result.numberOfBoardings;
    json["numberOfTransfers"] = result.numberOfTransfers;
    json["transferWalkingTimeMinutes"] = Toolbox::convertSecondsToMinutes(result.transferWalkingTime);
    json["transferWalkingTimeSeconds"] = result.transferWalkingTime;
    json["transferWalkingDistanceMeters"] = result.transferWalkingDistance;
    json["accessTravelTimeMinutes"] = Toolbox::convertSecondsToMinutes(result.accessTravelTime);
    json["accessTravelTimeSeconds"] = result.accessTravelTime;
    json["accessDistanceMeters"] = result.accessDistance;
    json["egressTravelTimeMinutes"] = Toolbox::convertSecondsToMinutes(result.egressTravelTime);
    json["egressTravelTimeSeconds"] = result.egressTravelTime;
    json["egressDistanceMeters"] = result.egressDistance;
    json["transferWaitingTimeMinutes"] = Toolbox::convertSecondsToMinutes(result.transferWaitingTime);
    json["transferWaitingTimeSeconds"] = result.transferWaitingTime;
    json["firstWaitingTimeMinutes"] = Toolbox::convertSecondsToMinutes(result.firstWaitingTime);
    json["firstWaitingTimeSeconds"] = result.firstWaitingTime;
    json["totalWaitingTimeMinutes"] = Toolbox::convertSecondsToMinutes(result.totalWaitingTime);
    json["totalWaitingTimeSeconds"] = result.totalWaitingTime;
    json["steps"] = nlohmann::json::array();

    // convert the steps
    StepToV1Visitor stepVisitor = StepToV1Visitor();
    for (auto &step : result.steps) {
      json["steps"].push_back(step.get()->accept(stepVisitor));
    }
    response = json;
  }

  void ResultToV1Visitor::visitAlternativesResult(const AlternativesResult& result)
  {
    nlohmann::json json;
    json["status"] = STATUS_SUCCESS;
    json["alternatives"] = nlohmann::json::array();
    // Sequence is there as parts of results that will be displayed to humans, in csv files, so start at 1
    int alternativeSequence = 1;
    for (auto &alternative : result.alternatives) {
      nlohmann::json alternativeJson = alternative.get()->accept(*this);
      alternativeJson["alternativeSequence"] = alternativeSequence;
      json["alternatives"].push_back(alternativeJson);
      alternativeSequence++;
    }
    json["alternativesTotal"] = result.totalAlternativesCalculated;
    response = json;
  }

  void ResultToV1Visitor::visitAllNodesResult(const AllNodesResult& result)
  {
    nlohmann::json json;
    json["status"] = STATUS_SUCCESS;
    json["nodes"] = nlohmann::json::array();
    for (auto &node : result.nodes) {
      nlohmann::json nodeJson;
      nodeJson["id"]                     = boost::uuids::to_string(node.node.uuid);
      nodeJson["arrivalTime"]            = Toolbox::convertSecondsToFormattedTime(node.arrivalTime);
      nodeJson["arrivalTimeSeconds"]     = node.arrivalTime;
      nodeJson["totalTravelTimeSeconds"] = node.totalTravelTime;
      nodeJson["numberOfTransfers"]      = node.numberOfTransfers;
      json["nodes"].push_back(nodeJson);
    }
    json["numberOfReachableNodes"] = result.numberOfReachableNodes;
    // Get a number with 2 decimals.
    json["percentOfReachableNodes"] = round(10000 * (float)(result.numberOfReachableNodes) / (float)(result.totalNodeCount))/100.0;
    response = json;
  }

  nlohmann::json ResultToV1Response::noRoutingFoundResponse(RouteParameters& params, NoRoutingReason noRoutingReason)
  {
    nlohmann::json json;
    json["status"]                     = STATUS_NO_ROUTING_FOUND;
    json["origin"]                     = { params.getOrigin()->latitude, params.getOrigin()->longitude };
    json["destination"]                = { params.getDestination()->latitude, params.getDestination()->longitude };
    json["departureTime"]              = Toolbox::convertSecondsToFormattedTime(params.getTimeOfTrip());
    json["departureTimeSeconds"]       = params.getTimeOfTrip();
    std::string reason;
    switch(noRoutingReason) {
      case NoRoutingReason::NO_ROUTING_FOUND:
        reason = NO_ROUTING_REASON_DEFAULT;
        break;
      case NoRoutingReason::NO_ACCESS_AT_ORIGIN:
        reason = NO_ROUTING_REASON_NO_ACCESS_AT_ORIGIN;
        break;
      case NoRoutingReason::NO_ACCESS_AT_DESTINATION:
        reason = NO_ROUTING_REASON_NO_ACCESS_AT_DESTINATION;
        break;
      case NoRoutingReason::NO_SERVICE_FROM_ORIGIN:
        reason = NO_ROUTING_REASON_NO_SERVICE_FROM_ORIGIN;
        break;
      case NoRoutingReason::NO_SERVICE_TO_DESTINATION:
        reason = NO_ROUTING_REASON_NO_SERVICE_TO_DESTINATION;
        break;
      case NoRoutingReason::NO_ACCESS_AT_ORIGIN_AND_DESTINATION:
        reason = NO_ROUTING_REASON_NO_ACCESS_AT_ORIGIN_AND_DESTINATION;
        break;
      default:
        reason = NO_ROUTING_REASON_DEFAULT;
        break;
    }
    json["reason"] = reason;
    return json;
  }

  nlohmann::json ResultToV1Response::resultToJsonString(RoutingResult& result, RouteParameters& params)
  {
    ResultToV1Visitor visitor = ResultToV1Visitor(params);
    return result.accept(visitor);
  }
}
