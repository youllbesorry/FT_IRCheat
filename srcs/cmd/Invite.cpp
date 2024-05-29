
#include "Cmd.hpp"

bool	parsForInvite(Clients client, std::string &nickname, std::string &channelName, std::string cmd);
bool 	checkIfUserAlreadyInviteOrInChannel(std::map<std::string, Channels>::iterator it, std::string nickname, std::string channelName, Clients client, std::map<int, Clients>::iterator ite);
bool	checkChannelExistAndUserLegitimateToInvite(std::map<std::string, Channels> serv, std::map<std::string, Channels>::iterator it, Clients client, std::string channelName);


void    Invite(std::string cmd, Clients& client, Server& server)
{
	std::string nickname = &cmd[7];
	std::string channelName;

	if (parsForInvite(client, nickname, channelName, cmd) == 1)
		return ;
	std::map<std::string, Channels> &serv = server.getChannels();
	std::map<std::string, Channels>::iterator it = serv.find(channelName);
	if (checkChannelExistAndUserLegitimateToInvite(serv, it, client, channelName))
		return ;

	std::map<int, Clients>::iterator ite;
	std::map<int, Clients> allClient = server.getClients();
	for (ite = allClient.begin(); ite != allClient.end(); ite++)
	{
		if (nickname == ite->second.getNickname())
		{
			if (checkIfUserAlreadyInviteOrInChannel(it, nickname, channelName, client, ite))
					return ;
			server.getChannels().find(it->first)->second.getClientInvited().insert(std::make_pair(ite->second.getNickname(), ite->second));
			sendCmd(RPL_INVITING(nickname, channelName), client);
			sendCmd(RPL_INVITING_NOTICE( client.getNickname(), channelName), ite->second);
			return ;
		}
	}
	sendCmd(ERR_NOSUCHNICK(nickname, client.getUsername()), client); // vraiment pas sur des infos que je lui transmet
}

bool	Channels::checkIfOpeUserForInvite(Clients client)
{
	size_t i = 0;
	while(i < _operator.size())
	{
		if (_operator[i].getNickname() == client.getNickname())
			break;
		i++;
	}
	if (i == _operator.size())
	{
		sendCmd(ERR_CHANOPRIVSNEEDED(client.getNickname(), _name), client);
		return 1;
	}
	return 0;
}


bool	parsForInvite(Clients client, std::string &nickname, std::string &channelName, std::string cmd)
{
	size_t spacefind;

	spacefind = nickname.find_first_of(" ");
	nickname = nickname.substr(0, spacefind);
	channelName = &cmd[7 + nickname.size() + 1];
	channelName = channelName.substr(0, channelName.size() - 2);

	if (channelName == client.getUsername())
	{
		sendCmd(ERR_NEEDMOREPARAMS(nickname, "INVITE"), client);
		return 1;
	}
	return 0;
}

bool	checkChannelExistAndUserLegitimateToInvite(std::map<std::string, Channels> serv, std::map<std::string, Channels>::iterator it, Clients client, std::string channelName)
{
	if (it == serv.end() || serv.size() == 0)
	{
		sendCmd(ERR_NOSUCHCHANNEL(client.getNickname(), channelName), client);
		return true;
	}
	if (it->second.checkIfOpeUserForInvite(client) == 1)
		return true;
	if (it->second.getClientMap().end() == it->second.getClientMap().find(client.getNickname()))
	{
		sendCmd(ERR_NOTONCHANNEL(channelName, it->second.getName()), client);
		return true;
	}
	return false;
}

bool checkIfUserAlreadyInviteOrInChannel(std::map<std::string, Channels>::iterator it, std::string nickname, std::string channelName, Clients client, std::map<int, Clients>::iterator ite)
{
	std::map<std::string, Clients> clientChannel = it->second.getClientMap();
	if (clientChannel.find(nickname) != clientChannel.end())
	{
		sendCmd(ERR_USERONCHANNEL(client.getNickname(), nickname, channelName), client);
		return true;
	}

	std::map<std::string, Clients> &verif = it->second.getClientInvited();
	if (verif.find(nickname) != verif.end())
	{
		sendCmd(RPL_INVITING(nickname, channelName), client);
		sendCmd(RPL_INVITING_NOTICE( client.getNickname(), channelName), ite->second);
		return true;
	}
	return false;
}


