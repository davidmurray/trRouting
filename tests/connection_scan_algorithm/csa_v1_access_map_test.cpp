#include <errno.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "gtest/gtest.h"
#include "calculator.hpp"
#include "csa_test_base.hpp"
#include "csa_v1_simple_calculation_test.hpp"
#include "constants.hpp"
#include "mode.hpp"
#include "data_source.hpp"
#include "household.hpp"
#include "person.hpp"
#include "place.hpp"
#include "agency.hpp"
#include "service.hpp"
#include "line.hpp"
#include "path.hpp"
#include "trip.hpp"
#include "od_trip.hpp"
#include "node.hpp"

/**
 * This file covers accessibility map use cases, where the value of all_nodes is
 * set to 1
 * */

// TODO:
// Do more than simple single use case, but at least we make sure the query path always works

class RouteAccessMapFixtureTests : public RouteCalculationFixtureTests
{

public:
    // Asserts the successful result fields, given some easy to provide expected test data
    void assertResults(TrRouting::RoutingResult& result,
        int nbReachableNodes);
    // Return a parameters vector with the scenario and minimum waiting time, which is repetitive and mandatory
    std::vector<std::string> initializeParameters();
};

// Test from an origin near the south2 node
TEST_F(RouteAccessMapFixtureTests, SimpleAllNodesQuery)
{
    int departureTime = getTimeInSeconds(9, 45);

    std::vector<std::string> parametersWithValues = initializeParameters();
    parametersWithValues.push_back("origin=45.5242,-73.5817");
    parametersWithValues.push_back("departure_time_seconds=" + std::to_string(departureTime));

    std::unique_ptr<TrRouting::RoutingResult> result = calculateOd(parametersWithValues);
    assertResults(*result.get(), 5);

}

// Test for an destination near the north2 node
TEST_F(RouteAccessMapFixtureTests, SimpleAllNodesQueryBackward)
{
    int arrivalTime = getTimeInSeconds(11, 15);

    std::vector<std::string> parametersWithValues = initializeParameters();
    parametersWithValues.push_back("destination=45.54,-73.6146");
    parametersWithValues.push_back("arrival_time_seconds=" + std::to_string(arrivalTime));

    std::unique_ptr<TrRouting::RoutingResult> result = calculateOd(parametersWithValues);
    assertResults(*result.get(), 6);

}

// Test all nodes query from an origin far from the network (origin offsetted)
TEST_F(RouteAccessMapFixtureTests, SimpleAllNodesQueryNoNodeAtOrigin)
{
    int departureTime = getTimeInSeconds(9, 45);

    std::vector<std::string> parametersWithValues = initializeParameters();
    parametersWithValues.push_back("origin=44.5242,-73.5817");
    parametersWithValues.push_back("departure_time_seconds=" + std::to_string(departureTime));

    try {
        calculateOd(parametersWithValues);
        FAIL() << "Expected TrRouting::NoRoutingFoundException, no exception thrown";
    } catch (TrRouting::NoRoutingFoundException const & e) {
        assertNoRouting(e, TrRouting::NoRoutingReason::NO_ACCESS_AT_ORIGIN);
    } catch(...) {
        FAIL() << "Expected TrRouting::NoRoutingFoundException, another type was thrown";
    }

}

// Test all nodes query to a destination far from the network (destination offsetted)
TEST_F(RouteAccessMapFixtureTests, SimpleAllNodesQueryNoNodeAtDestination)
{
    int arrivalTime = getTimeInSeconds(9, 45);

    std::vector<std::string> parametersWithValues = initializeParameters();
    parametersWithValues.push_back("destination=44.5242,-73.5817");
    parametersWithValues.push_back("arrival_time_seconds=" + std::to_string(arrivalTime));

    try {
        calculateOd(parametersWithValues);
        FAIL() << "Expected TrRouting::NoRoutingFoundException, no exception thrown";
    } catch (TrRouting::NoRoutingFoundException const & e) {
        assertNoRouting(e, TrRouting::NoRoutingReason::NO_ACCESS_AT_DESTINATION);
    } catch(...) {
        FAIL() << "Expected TrRouting::NoRoutingFoundException, another type was thrown";
    }

}

void RouteAccessMapFixtureTests::assertResults(TrRouting::RoutingResult& result,
    int nbReachableNodes)
{
    ASSERT_EQ(TrRouting::result_type::ALL_NODES, result.resType);
    TrRouting::AllNodesResult& routingResult = dynamic_cast<TrRouting::AllNodesResult&>(result);
    ASSERT_EQ(nbReachableNodes, routingResult.numberOfReachableNodes);
}

std::vector<std::string> RouteAccessMapFixtureTests::initializeParameters()
{
    std::vector<std::string> parametersWithValues = RouteCalculationFixtureTests::initializeParameters();
    parametersWithValues.push_back("all_nodes=1");
    return parametersWithValues;
}
