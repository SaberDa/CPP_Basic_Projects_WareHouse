


#include <sys/wait.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <cstring>

#include <string>

#include "network.h"

#define STACK_SIZE (512 * 512)		// Define child process space

namespace docker {

/*
 * First add readable variables
 * */
typedef int proc_statu;
proc_statu proc_err = -1;
proc_statu proc_exit = 0;
proc_statu proc_wait = 1;

/*
 * Docker container start config
 * */
typedef struct container_config {
    std::string host_name;		// Host name
    std::string root_dir;		// Container root dir
	std::string ip;				// Container IP address
	std::string bridge_name;	// Bridge name
	std::string bridge_ip;		// Bridge IP
} container_config;

/*
 * Class container
 * */
class container {
private:

	void set_network() {
		int ifindex = if_nametoindex("eth0");
		struct in_addr ipv4;
		struct in_addr bcast;
		struct in_addr gateway;

		// IP address convert function
		inet_pton(AF_INET, this->config.ip.c_str(), &ipv4);
		inet_pton(AF_INET, "255.255.255.0", &bcast);
		inet_pton(AF_INET, this->config.bridge_ip.c_str(), &gateway);

		// Set eth0 IP address
		lxc_ipv4_addr_add(ifindex, &ipv4, &bcast, 16);

		// Active lo
		lxc_netdev_up("lo");

		// Active eth0
		lxc_netdev_up("eth0");

		// Set gateway
		lxc_ipv4_gateway_add(ifindex, &gateway);

		// Set eth0 MAC address
		char mac[18];
		new_hwaddr(mac);
		setup_hw_addr(mac, "eth0");
	
	}

	// Store the container network devide
	char *veth1, *veth2;

	// Set up a separate process space
	void set_procsys() {
		// Mount proc file system
		mount("none", "/proc", "proc", 0, nullptr);
		mount("none", "/sys", "sysfs", 0, nullptr);
	}

	void set_rootdir() {
		// Switch to a directory
		chdir(this->config.root_dir.c_str());
		
		// Set the root directory
		// Because the current directory has been switched 
		// You can directly use the current directory as the root directory
		chroot(".");
	}

	void set_hostname() {
		sethostname(this->config.host_name.c_str(), this->config.host_name.length());
	}

	void start_bash() {
		// Safely convert C++ std::string to C-style string char*
		// Starting from C++14, the C++ compiler will prohibit this writing 'char *str = "test";'
		std::string bash = "/bin/bash";
		char *c_bash = new char[bash.length() + 1];
		strcpy(c_bash, bash.c_str());

		char *const child_args[] = { c_bash, NULL};
		execv(child_args[0], child_args);
		delete []c_bash;
	}

	// Improved readability
	typedef int process_pid;

	// Child process stack
	char child_stack[STACK_SIZE];

	// Container config
	container_config config;

public: 
	container(container_config &config) {
		this->config = config;
	}


	void start() {
		char veth1buf[IFNAMSIZ] = "docker0X";
		char veth2buf[IFNAMSIZ] = "docker0X";

		// Create a pair of network devices
		// One for loading into the host, the other one for transferring to the child process container
		
		// The API 'lxc_mkifname()' needs to add at least one 'X' after the network device name 
		// to support random creation of virtual network devices
		veth1 = lxc_mkifname(veth1buf);
		veth2 = lxc_mkifname(veth2buf);
		lxc_veth_create(veth1, veth2);

		// Set the veth1 MAC address
		setup_private_host_hw_addr(veth1);

		// Add veth1 to the bridge
		lxc_bridge_attach(config.bridge_name.c_str(), veth1);

		// Active the veth1
		lxc_netdev_up(veth1);

		auto setup = [](void *args) -> int {
			auto _this = reinterpret_cast<container *>(args);
			
			// Do config
			//
			_this->set_hostname();
			_this->set_rootdir();
			_this->start_bash();
			_this->set_procsys(); 
			_this->set_network();
			return proc_wait;
		};

		process_pid child_pid = clone(setup, child_stack + STACK_SIZE, 		// move to bottom of stack
							CLONE_NEWUTS|			// UTS namespace
							CLONE_NEWNS|			// Mount namespace 
							CLONE_NEWPID|			// PID namespace 
							CLONE_NEWNET|			// Net namespace
							SIGCHLD,				// When the child process exits, a signal is sent to the parent process 
							this);

		// Move the eth2 into the container and rename it to eth0
		lxc_netdev_move_by_name(veth2, child_pid, "eth0");

		waitpid(child_pid, nullptr, 0);				// Wait for child process exiting
	}

	~container() {
		// When exit, remember to delete the virtual network device
		lxc_netdev_delete_by_name(veth1);
		lxc_netdev_delete_by_name(veth2);
	}

}; // class container





} // namespace docker








