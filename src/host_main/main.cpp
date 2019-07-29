/*
 * main.cpp
 *
 *  Created on: Jun 2, 2019
 *      Author: mikaelr
 */


#include "SignalChain.h"

#include <errno.h>
#include <fmt/format.h>
#include <string.h>
#include <chrono>
#include <unistd.h>

#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>

namespace po = boost::program_options;

void
setupOptions(po::options_description& desc)
{
    desc.add_options()                                    //
        ("help", "produce help message")                  //
        ("usage", "Show summary of command line options") //
        ("version", "Show git version of the program.")   //
#if 0
        ("serial-device,d", po::value<std::string>(),
         "Name of serial device for byte interface.") //
        ("serial-options", po::value<std::string>()->default_value(""),
         "Colon sep. list of options for serial device. Ex: "
         "<none>|<pulldown>|<rs485_te>") //
        ("mode", po::value<std::string>(),
         "Mode the program should work in. Allowed: std_io, raw_pty,"
         "socat_tun, socat_tap, tun, tap, setup_tap, remove_tap.") //
        ("address", po::value<int>()->default_value(0),
         "Local address on the serial net. Implies static address. "
         "If not given, implies dynamic address for client only or static "
         "address 1 for master client. Value between 1 - 32") //
        ("peer_address", po::value<int>()->default_value(0),
         "Address for the opposite end in the raw format.") //
        ("master,m",
         "Start the master part. Exactly one master should be active for each "
         "serial_net.") //
        ("mtimeout", po::value<int>()->default_value(-1),
         "Stop the master after given amount of time (sec)") //
        ("log,l", po::value<int>()->default_value(2),
         "Log level. (0=trace, 4=error)") //
        ("wsdump", po::value<std::string>(),
         "Dump all serial packets to a named pipe, suitable for test2pcap") //
        ("endwithmaster",
         "Quit the client if we receive a 'master_stop' message.") //
        ("on-if-up", po::value<std::string>()->default_value(""),
         "Give a command to run using 'system' call when the interface comes "
         "up.") //
        ("on-if-down", po::value<std::string>()->default_value(""),
         "Give a command to run using 'system' call when the interface goes "
         "down.") //
        ("user", po::value<std::string>()->default_value(""),
         "Owning user when setting up a tap interface.") //
        ("group", po::value<std::string>()->default_value(""),
         "Owning group when setting up a tap interface.") //
        ("tap-name", po::value<std::string>()->default_value("tap0"),
         "Name to use for the tap/tun interface.") //
#endif
        ;
}


static std::vector<std::array<uint32_t, 4>> readInput()
{
    std::array<uint32_t, 4> line;
    std::vector<std::array<uint32_t, 4>> data;
    int x;
    int y = 0;
    while ((x = scanf("%x %x %x %x", &line[0], &line[2], &line[1], &line[3])) ==
           4)
    {
        ++y;
        data.push_back(line);
    }
    fmt::print("errcode:{} errno:{}, y:{}\n", x, strerror(errno), y);
    return data;
}

struct El1
{
    State state;
    RawSignalCondition::Result res;
    State medState;
    uint32_t median;
    bool isAirVent;
};

void processSample(uint32_t index, SignalChain& sc)
{
	El1 el;
	el.isAirVent = sc.m_median.isTopAirvent();
	el.medState = sc.m_median.m_state;
	el.median = sc.m_median.m_median;
	el.res = sc.m_rawCond.m_result;
    el.state = sc.m_rawCond.m_state;

    int angle = sc.m_distanceCalc.wheelAngle();
    int speedi = sc.m_distanceCalc.spokeSpeed();
    double speed = el.medState == State::VALID
                       ? 3.6 * DistanceCalc::wheelDiameter * 0.0001 * angle / 65536 * 1000000.0 / el.res.m_deltaRelease
                       : 0.0;

    fmt::print("{}\t{}\t{}\t{}\t{}\t{}\t{:>6}\t{:>6} {:>3}\t{}\t{}\t{:>7}\t{:>.4}\t{:.7}\t{:.4}\t{:.4}\n",
    		index,
			   el.state, el.res.m_count, el.res.m_systick,
               el.res.m_deltaAssert, el.res.m_deltaRelease,
               el.res.m_timeAsserted, el.median, el.medState,
			   sc.m_slotTracker.index(),
               el.isAirVent, angle, speed, sc.m_slotTracker.m_state,
			   speedi*3.6*0.001, speed - speedi*3.6*0.001);
}

int
main(int argc, const char* argv[])
{
    if (argc < 1)
    {
        return -1;
    }
    if (argc == 1)
    {
        std::cout << "\n";
        std::cout << "Use: 'host_main --help' for overview of the tool.\n";
        std::cout << std::endl;
        return 0;
    }

    // Declare the supported options.
    po::options_description desc("Allowed options");
    setupOptions(desc);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") > 0)
    {
    	// std::cout << helpMessage() << "\n";
        return 1;
    }
    if (vm.count("usage") > 0)
    {
        std::cout << desc << std::endl;
        return 1;
    }

    std::vector<std::array<uint32_t, 4>> data = readInput();

    SignalChain signalChain;

    std::vector<El1> data2;
    bool realTime = false; //true;

    auto baseTime = std::chrono::system_clock::now();
    auto now = baseTime;

    auto baseTick = data[0][3];
    auto endTick = data.back()[3] + 1;

    uint32_t dataIndex=0;

    for (uint32_t tick = baseTick; tick < endTick; tick++)
    {
    	signalChain.addSysTick(tick);

        auto d_p = &data[dataIndex];
        while (realTime && (std::chrono::system_clock::now() - baseTime <
                            std::chrono::milliseconds(data[dataIndex][3] - baseTick)))
        {
            usleep(1000);
        }

        for (; dataIndex < data.size() && data[dataIndex][3] <= tick;
        		++dataIndex)
        {
        	const auto& d = data[dataIndex];
        	signalChain.addTickPoint(TickPoint(d[0], d[1], d[2]));
        	processSample(dataIndex, signalChain);
        }
    }

    for (auto i : signalChain.m_slotTracker.m_permanent)
        fmt::print("     {}\n", double(i)/65536.0);
    fmt::print("{}\n", argc);
}
