#include "Clients.hpp"
#include "Server.hpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "ERROR arg" << std::endl;
        return -1;
    }
	try
	{
    	Server server(argv[1]);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
    return 0;
}