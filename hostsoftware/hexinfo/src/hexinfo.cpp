// TODO:
// 1. Validation
// 2. JSON Export
// 3. Testing

#include <iostream>
#include <libhexabus/packet.hpp>
#include <libhexabus/network.hpp>
#include "../../../shared/hexabus_packet.h"
#include <string>
#include "DeviceInfo.hpp"

void usage() {
	std::cout << "Usage: hexinfo <IPv6 Address of Device> <Options>" << std::endl;
	std::cout << "Possible options are: None so far" << std::endl;
	// std::cout << "-j \t output json instead of the normal output" << std::endl;
}

int main(int argc, char **argv) {
	
	if(argc != 2) {
		usage();
		//exit(1);
		return 1;
	}
	
	std::string ipAddress = argv[1];
	std::vector<EndpointInfo> eps;

	DeviceInfo devInfo(argv[1]);	
	
	// Get information on all the available endpoints
	devInfo.getAllEndpointInfo();	
	
	// Why C++ needs for each...
	for (int i = 0;i < (int)eps.size();i++) {
 		std::cout << eps[i].toString();
	}
	std::cout << std::endl;

	return 0;
}
