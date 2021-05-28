#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);
    int lenght = message.length();
    if (lenght < 80)
        message.append(80 - lenght, '\0');
    lenght = nick.length();
    if (lenght < 8)
        message.append(8 - lenght, '\0');
    char *aux = _data;
    memcpy(aux, &type, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    memcpy(aux, message.c_str(), sizeof(char) * 80);
    aux += sizeof(char) * 80;
    memcpy(aux, nick.c_str(), sizeof(char) * 8);

    //Serializar los campos type, nick y message en el buffer _data
}

int ChatMessage::from_bin(char *bobj)
{
    if (bobj == 0)
    {
        std::cout << "Error on deserialization, empty object received\n";
        return -1;
    }
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char *aux = _data;
    memcpy(&type, aux, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    char auxMessage[80];
    memcpy(&auxMessage, aux, sizeof(char) * 80);
    message = auxMessage;
    aux += 80 * sizeof(char);
    char auxNick[8];
    memcpy(&auxNick, aux, sizeof(char) * 8);
    nick = auxNick;
    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
        Socket *client;
        ChatMessage msg;
        int returnCode = socket.recv(msg, client);
        if (returnCode == -1)
            std::cout << "error on socket recv: " << strerror(errno) << '\n';
        if (msg.type == ChatMessage::LOGIN)
            clients.push_back(std::unique_ptr<Socket>(std::move(client)));
        else if (msg.type == ChatMessage::LOGOUT)
        {
            auto it = clients.begin();
            while (it != clients.end() && (*it).get() != client)
            {
                it++;
            }
            if (it != clients.end())
            {
                clients.erase(it);
            }
        }
        else
        {
            for (auto i = clients.begin(); i != clients.end(); i++)
                if (!(*(*i) == *client))
                    socket.send(msg, *(*i));
        }

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        std::string message;
        std::getline(std::cin, message);
        if (message == "q")
        {
            logout();
            break;
        }
        else
        {
            char *chatmsg = new char[80];
            message.copy(chatmsg, 80);
            // Enviar al servidor usando socket
            ChatMessage em(nick, chatmsg);
            em.type = ChatMessage::MESSAGE;
            socket.send(em, socket);
        }
    }
}

void ChatClient::net_thread()
{
    while (true)
    {
        //Recibir Mensajes de red
        ChatMessage msg;
        socket.recv(msg);
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::cout << msg.nick << ": " << msg.message << '\n';
    }
}
