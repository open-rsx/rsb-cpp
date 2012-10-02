/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
 *
 * ============================================================ */

#include <stdexcept>
#include <fstream>

#include <boost/format.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsc/config/ConfigFileSource.h>

#include "rsb/ParticipantConfig.h"

#include "testconfig.h"

using namespace std;
using namespace boost;
using namespace testing;
using namespace rsb;

TEST(TransportTest, testCreation)
{

    EXPECT_THROW(ParticipantConfig::Transport(""), invalid_argument);
    EXPECT_EQ("a test", ParticipantConfig::Transport("a test").getName());
    EXPECT_TRUE(ParticipantConfig::Transport("a test").getOptions().empty());

}

TEST(TransportTest, testOptions)
{

    ParticipantConfig::Transport transport("a test");
    rsc::runtime::Properties options;
    options.insert(make_pair("key", "value"));
    EXPECT_FALSE(transport.getOptions().has("key"));
    transport.setOptions(options);
    EXPECT_TRUE(transport.getOptions().has("key"));

}

TEST(EventProcessingStrategyOptionTest, testMutableOptions)
{
    ParticipantConfig::EventProcessingStrategy ep("foo");
    const string key = "foo";
    const string value = "bar";
    ep.mutableOptions().insert(make_pair(key, value));
    EXPECT_EQ(value, ep.getOptions().get<string> (key));
}

TEST(ParticipantConfigTest, testCreation)
{

    ParticipantConfig config;
    EXPECT_TRUE(config.getTransports().empty());
    EXPECT_EQ(QualityOfServiceSpec(), config.getQualityOfServiceSpec());
    EXPECT_EQ(ParticipantConfig::LOG, config.getErrorStrategy());

}

TEST(ParticipantConfigTest, testQualityOfServiceSpec)
{

    ParticipantConfig config;

    QualityOfServiceSpec spec(QualityOfServiceSpec::ORDERED,
            QualityOfServiceSpec::UNRELIABLE);
    EXPECT_NE(spec, config.getQualityOfServiceSpec());
    config.setQualityOfServiceSpec(spec);
    EXPECT_EQ(spec, config.getQualityOfServiceSpec());

}

TEST(ParticipantConfigTest, testTransports)
{

    ParticipantConfig config;

    ParticipantConfig::Transport t1("t1");
    ParticipantConfig::Transport t2("t2");

    EXPECT_EQ(size_t(0), config.getTransports().size());
    config.addTransport(t1);
    EXPECT_EQ(size_t(1), config.getTransports().size());
    config.addTransport(t1);
    EXPECT_EQ(size_t(1), config.getTransports().size());
    config.addTransport(t2);
    EXPECT_EQ(size_t(2), config.getTransports().size());

    config.removeTransport(ParticipantConfig::Transport("unknown"));
    EXPECT_EQ(size_t(2), config.getTransports().size());
    config.removeTransport(ParticipantConfig::Transport(t1.getName()));
    EXPECT_EQ(size_t(1), config.getTransports().size());
    config.removeTransport(ParticipantConfig::Transport(t2));
    EXPECT_EQ(size_t(0), config.getTransports().size());

    set<ParticipantConfig::Transport> newSet;
    newSet.insert(ParticipantConfig::Transport("foo"));
    config.setTransports(newSet);
    EXPECT_EQ(newSet, config.getTransports());

}

TEST(ParticipantConfigTest, testMutableTransport)
{

    ParticipantConfig config;

    const string name = "t1";
    ParticipantConfig::Transport t1(name);
    t1.setEnabled(false);
    config.addTransport(t1);
    config.mutableTransport(name).setEnabled(true);
    EXPECT_TRUE(config.getTransport(name).isEnabled());

}

TEST(ParticipantConfigTest, testOptions)
{

    ParticipantConfig config;
    rsc::runtime::Properties options;
    options.insert(make_pair("key", "value"));
    EXPECT_FALSE(config.getOptions().has("key"));
    config.setOptions(options);
    EXPECT_TRUE(config.getOptions().has("key"));

}

TEST(ParticipantConfigTest, testMutableOptions)
{

    ParticipantConfig config;
    const string key = "foo";
    const string value = "bar";
    config.mutableOptions().insert(make_pair(key, value));
    EXPECT_EQ(value, config.getOptions().get<string> (key));

}

TEST(ParticipantConfigTest, testFromFile)
{
    ifstream stream(str(format("%1%/rsb.conf-for-smoke-test") % TEST_ROOT).c_str());
    rsc::config::ConfigFileSource source(stream);
    ParticipantConfig config;
    source.provideOptions(config);
    EXPECT_EQ(config.getOptions().getAs<unsigned int>("global"), 1u);

    EXPECT_EQ(config.getQualityOfServiceSpec().getReliability(),
            QualityOfServiceSpec::UNRELIABLE);
    EXPECT_EQ(config.getQualityOfServiceSpec().getOrdering(),
            QualityOfServiceSpec::UNORDERED);

    EXPECT_EQ(config.getErrorStrategy(), ParticipantConfig::EXIT);

#ifdef RSB_WITH_SPREAD_TRANSPORT
    ParticipantConfig::Transport spread = config.getTransport("spread");
    EXPECT_EQ(spread.getOptions().get<string>("host"), "localhost");
    EXPECT_EQ(spread.getOptions().getAs<unsigned int>("port"), 4803u);

    EXPECT_EQ(spread.getConverters().size(), 1u);
    EXPECT_EQ(spread.getConverters().begin()->first, "image");
    EXPECT_EQ(spread.getConverters().begin()->second, "IplImage");
#endif

    for (unsigned int i = 3; i <= 6; ++i) {
        ifstream stream(str(format("%1%/rsb.conf-semantic-errors-%2%") % TEST_ROOT % i).c_str());
        rsc::config::ConfigFileSource source(stream);
        ParticipantConfig config;
        EXPECT_THROW(source.provideOptions(config), invalid_argument);
    }
}

TEST(ParticipantConfigTest, testErrorStrategy)
{

    ParticipantConfig config;
    const ParticipantConfig::ErrorStrategy strategy = ParticipantConfig::EXIT;
    config.setErrorStrategy(strategy);
    EXPECT_EQ(strategy, config.getErrorStrategy());

}
