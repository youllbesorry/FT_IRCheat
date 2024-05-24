#include "Server.hpp"
#include "Cmd.hpp"

int Server::getFd() const {return this->_fd;}

std::string Server::getAddrIp() const {return this->_addrIp;}

void Server::setFd(int fd) {this->_fd = fd;}

void Server::setAddrIp(std::string addrIp) {this->_addrIp = addrIp;}

std::map<std::string, Channels>& Server::getChannels() {return (_channels);}

std::map<int, Clients>& Server::getClients() {return (_clients);}

void Server::setChannels(std::map<std::string, Channels> channels) {this->_channels = channels;}

void Server::setClient(std::map<int, Clients> clients) {this->_clients = clients;}

void Server::setPassword(std::string password) {this->_password = password;}

std::string Server::getPassword() const {return (this->_password);}

bool startWith(const std::string &line, const char *cmd) {return (line.find(cmd) == 0);}

void Server::cmdHandler(std::string cmd, Clients& client)
{
	// std::string::size_type pos = cmd.find("\r\n");
	// if (pos != std::string::npos)
	// {
	// 	cmd = cmd.erase(pos);
	// }
	// std::cout << "cmd char: " << cmd[cmd.length()] + 48 << std::endl;
	std::cout << "cmd: " << cmd << std::endl;
    const char *lstCmd[] = {"JOIN", "KICK", "PRIVMSG", "PING", "MODE"};
    // , "NAMES", "NICK", "INVITE", "TOPIC", "PRIVMSG", "QUIT", "PART", "KICK", "MODE"
    void (*lstFunc[])(std::string, Clients&, Server&) = {Join, Kick, Privmsg, Pong, Mode};
    for (int i = 0; i < 5; i++)
    {
        if (startWith(cmd, lstCmd[i]))
        {
            lstFunc[i](cmd, client, *this);
            return;
        }
    }
}

Server::Server(std::string av, std::string av2)
{
	struct sockaddr_in address;
	int opt = 1;
	char buffer[512];

	// Création de la socket serveur

	setAddrIp("127.0.0.1");
	setPassword(av2);
	_fd = -1;
	
	setFd(socket(AF_INET, SOCK_STREAM, 0));
	if (getFd() < 0)
	{
		perror("Error Socket: ");
		throw std::invalid_argument("");
	}

	if (setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		perror("Error setsockopt: ");
		throw std::invalid_argument("");
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(av.c_str()));

	if (bind(getFd(), (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("Error bind: ");
		throw std::invalid_argument("");
	}

	if (listen(getFd(), 3) < 0)
	{
		perror("Error listen: ");
		throw std::invalid_argument("");
	}
	
	struct pollfd pollServ;
	pollServ.fd = getFd();
	pollServ.events = POLLIN;
	_lstPollFd.push_back(pollServ);
	bool init = false;
	while (true)
	{
		if (poll(&_lstPollFd[0], _lstPollFd.size(), -1) >= 0)
		{
			if (_lstPollFd[0].revents & POLLIN)
			{
				Clients newClient;
				socklen_t sock_info_len = sizeof(address);
   				newClient.setFd(accept(getFd(), (struct sockaddr*)&address, &sock_info_len));
				struct pollfd pollClienTmp;
				pollClienTmp.fd = newClient.getFd();
				pollClienTmp.events = POLLIN;
				_lstPollFd.push_back(pollClienTmp);
				_clients.insert(std::make_pair(newClient.getFd(), newClient));
			}
			for (size_t i = 1; i < _lstPollFd.size(); i++)
			{
				if (_lstPollFd[i].revents & POLLIN)
				{
					bzero(buffer, 512);
					std::map<int, Clients>::iterator itClients = getClients().find(_lstPollFd[i].fd);
					ssize_t bytes = recv(itClients->first, buffer, 511, MSG_DONTWAIT);
					// std::cout << "buffer : |" << buffer << "| BYTES: " << bytes<< std::endl;
					if (bytes < 0)
						std::cerr << "ERROR rcve !" << std::endl;
					else if ( bytes == 0)
					{
						std::cout << "connexion closed " << std::endl;
						throw std::invalid_argument("tg");
					}
					if (startWith(buffer, "CAP LS 302") || !init)
					{
						init = itClients->second.initClients(buffer, *this);
						if (init)
						{
							std::cout << "init client" << std::endl;
							itClients->second.printInfo();
						}
					}
					else
					{
						cmdHandler(buffer, itClients->second);
						// itClients->second.printChannels();
					}
					itClients++;
				}
			}
		}
		else // ERROR DE POLL
		{
			throw std::invalid_argument("lol");
		}

	}
}

Server::~Server()
{

}