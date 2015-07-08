/*
 * CommandExecutor.h
 *
 *  Created on: 2 jul. 2015
 *      Author: Herman
 */

#ifndef SERVICES_COMMANDPROCESSING_COMMANDEXECUTOR_H_
#define SERVICES_COMMANDPROCESSING_COMMANDEXECUTOR_H_

#include "WiringFrameworkIncludes.h"
#include "Network/TcpClient.h"
#include "CommandHandler.h"
#include "CommandOutput.h"

#define MAX_COMMANDSIZE 64

class CommandExecutor
{
public:
	CommandExecutor(TcpClient* cmdClient);
	CommandExecutor(Stream* reqStream);
	~CommandExecutor();

	int executorReceive(char *recvData, int recvSize);
	int executorReceive(char recvChar);
	void setCommandPrompt(String reqPrompt);
	void setCommandEOL(char reqEOL);

private :
	void processCommandLine(String cmdString);
	char commandBuf [MAX_COMMANDSIZE+1];
	uint16_t commandIndex = 0;
	CommandOutput* commandOutput;
	String prompt = "Sming>";
	char eolChar = '\r';

};

#endif /* SERVICES_COMMANDPROCESSING_COMMANDEXECUTOR_H_ */
