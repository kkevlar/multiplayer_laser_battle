#include <stddef.h>
#include <string.h>

#include <iostream>
#include <list>
#include <string>

#include "outgoing_message.h"
#include "packet.h"

typedef enum
{
    CLIENT_COMMAND_MALFORMED,
    CLIENT_COMMAND_MESSAGE,
    CLIENT_COMMAND_BROADCAST,
    CLIENT_COMMAND_LIST,
    CLIENT_COMMAND_EXIT,
} ClientCommandType;

typedef struct
{
    std::string myhandle;
    ClientCommandType type;
    std::list<std::string> arguments;
} ClientCommand;

static __attribute__((warn_unused_result)) bool clientCommand(
    int fd, ClientCommand& command);
static __attribute__((warn_unused_result)) int readFromStdin(char* buffer,
                                                             size_t buf_size);
static ClientCommandType tokenToCommandType(std::string token);

bool keyboardProcessForClient(const std::string myhandle, int socketNum)
{
    using namespace std;

    char buf[20000];

    size_t read_count = readFromStdin(buf, sizeof(buf));

    const char delim[] = " ";
    ClientCommand command;
    command.myhandle = myhandle;
    command.arguments = list<string>{};

    char* start = buf;
    char* p;
    while ((p = strpbrk(start, delim)) != NULL)
    {
        *p = 0;
        string token = string{start};
        command.arguments.push_back(token);
        start = ++p;
#if DEBUG_FLAG == 1
        cerr << "Got a token " << token << endl;
#endif
    }

    if (MAKE_POINTER_AT_OFFSET(start, 0) <
        MAKE_POINTER_AT_OFFSET(buf, read_count))
    {
        string token = string{start};
        command.arguments.push_back(token);
#if DEBUG_FLAG == 1
        cerr << "Got a token " << token << endl;
#endif
    }

    command.type = tokenToCommandType(command.arguments.front());
    command.arguments.pop_front();

    if (!clientCommand(socketNum, command))
    {
        cerr << "Failed to process the command in some way." << endl;
        return false;
    }

    return true;
}

static bool clientCommand(int fd, ClientCommand& command)
{
    switch (command.type)
    {
        case CLIENT_COMMAND_MESSAGE:
        {
            return outgoingMessage(fd, command.myhandle, command.arguments);
            break;
        }
        case CLIENT_COMMAND_LIST:
        {
            return packetFillSendHeaderOnlyPacket(
                fd, FLAG_CLIENT_REQUST_HANDLE_LIST);
            break;
        }
        case CLIENT_COMMAND_EXIT:
        {
            return packetFillSendHeaderOnlyPacket(fd, FLAG_CLIENT_EXIT_REQUEST);
            break;
        }
        case CLIENT_COMMAND_BROADCAST:
        {
            return outgoingBroadcast(fd, command.myhandle, command.arguments);
            break;
        }
        default:
        {
            using namespace std;
            cerr << "Malformed command. Ignoring. " << endl;
            return false;
        }
    }
}

static int readFromStdin(char* buffer, size_t buf_size)
{
    char aChar = 0;
    int inputLen = 0;

    // Important you don't input more characters than you have space
    buffer[0] = '\0';
    while (inputLen < (((int)buf_size) - 1) && aChar != '\n')
    {
        aChar = getchar();
        if (aChar != '\n')
        {
            buffer[inputLen] = aChar;
            inputLen++;
        }
    }

    // Null terminate the string
    buffer[inputLen] = '\0';
    inputLen++;

    return inputLen;
}

static ClientCommandType tokenToCommandType(std::string token)
{
    if ('%' != token[0])
    {
        return CLIENT_COMMAND_MALFORMED;
    }
    else if (token[1] == 'm' || token[1] == 'M')
    {
        return CLIENT_COMMAND_MESSAGE;
    }
    else if (token[1] == 'b' || token[1] == 'B')
    {
        return CLIENT_COMMAND_BROADCAST;
    }
    outelse if (token[1] == 'l' || token[1] == 'L')
    {
        return CLIENT_COMMAND_LIST;
    }
    else if (token[1] == 'e' || token[1] == 'E')
    {
        return CLIENT_COMMAND_EXIT;
    }
    else
    {
        return CLIENT_COMMAND_MALFORMED;
    }
}

void keyboardPrompt()
{
    fprintf(stdout, "\n");
    fflush(stdout);
    fprintf(stdout, "$: ");
    fflush(stdout);
}