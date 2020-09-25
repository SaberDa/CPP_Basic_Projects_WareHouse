
#include <iostream>
#include "docker.hpp"

int main(int argc, char** argv) {
	std::cout << "...start container" << std::endl;
	docker::container_config config;

	// Container config
	// ...
	config.host_name = "mydocker";
	config.root_dir = "./image";
	config.ip = "192.168.0.100";	// Container IP
	config.bridge_name = "docker0";
	config.bridge_ip = "192.168.0.1";

	docker::container container(config);	// Generate container according to config
	container.start();
	std::cout << "stop container..." << std::endl;

	return 0;
}



