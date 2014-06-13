/*
 * Copyright (c) 2014 Juniper Networks, Inc. All rights reserved.
 */

#include "bgp/inet6/inet6_route.h"

#include "base/logging.h"
#include "base/task.h"
#include "bgp/bgp_log.h"
#include "bgp/inet6/inet6_table.h"
#include "control-node/control_node.h"
#include "testing/gunit.h"

class Inet6RouteTest : public ::testing::Test {
    virtual void SetUp() {
        // Needed primarily for IsMoreSpecific
        Inet6Masks::Init();
    }
    virtual void TearDown() {
        Inet6Masks::Clear();
    }
};

TEST_F(Inet6RouteTest, IsMoreSpecificTest) {
    std::string prefix_str = "2001:db8:85a3:aaaa::b:c:d/128";
    Inet6Route route1(Inet6Prefix::FromString(prefix_str));
    EXPECT_EQ(route1.IsMoreSpecific(prefix_str), true);

    // Compare with prefixlen 64
    prefix_str = "2001:db8:85a3:aaaa::b:c:d/64";
    EXPECT_EQ(route1.IsMoreSpecific(prefix_str), true);

    prefix_str = "2001:db8:85a3:aaaa::/64";
    Inet6Route route2(Inet6Prefix::FromString(prefix_str));
    EXPECT_EQ(route1.IsMoreSpecific(prefix_str), true);

    // Compare with prefixlen 48
    prefix_str = "2001:db8:85a3::/48";
    EXPECT_EQ(route2.IsMoreSpecific(prefix_str), true);

    prefix_str = "2001:db8:85a3:aaaa::/48";
    EXPECT_EQ(route2.IsMoreSpecific(prefix_str), true);
}

TEST_F(Inet6RouteTest, SetKeyTest) {
    Inet6Prefix null_prefix;
    Inet6Route route(null_prefix);

    std::string prefix_str = "2001:db8:85a3::d:e:a/128";
    Inet6Prefix prefix(Inet6Prefix::FromString(prefix_str));
    boost::scoped_ptr<Inet6Table::RequestKey> key(
        new Inet6Table::RequestKey(prefix, NULL));

    route.SetKey(key.get());
    EXPECT_EQ(prefix, key->prefix);
    EXPECT_EQ(prefix, route.GetPrefix());
}

TEST_F(Inet6RouteTest, GetDBRequestKeyTest) {
    std::string prefix_str = "2001:db8:85a3::d:e:a/128";
    Inet6Prefix prefix(Inet6Prefix::FromString(prefix_str));
    Inet6Route route(prefix);

    DBEntryBase::KeyPtr keyptr = route.GetDBRequestKey();
    const Inet6Table::RequestKey *key =
        static_cast<Inet6Table::RequestKey *>(keyptr.get());

    EXPECT_EQ(prefix, key->prefix);
}

int main(int argc, char **argv) {
    bgp_log_test::init();
    ::testing::InitGoogleTest(&argc, argv);
    ControlNode::SetDefaultSchedulingPolicy();
    int result = RUN_ALL_TESTS();
    TaskScheduler::GetInstance()->Terminate();
    return result;
}
