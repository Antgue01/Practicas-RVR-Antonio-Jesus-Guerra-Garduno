#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    char *aux = _data;
    memcpy(aux, &type, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    memcpy(aux, message.data(), sizeof(char) * 80);
    aux += sizeof(char) * 80;
    memcpy(aux, nick.data(), sizeof(char) * 80);

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
    memcpy(&message, aux, sizeof(char) * 80);
    aux += 80 * sizeof(char);
    memcpy(&nick, aux, sizeof(char) * 8);
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
        struct sockaddr clientaddr;
        socklen_t size = sizeof(struct sockaddr);
        Socket *client = new Socket(&clientaddr, size);
        ChatMessage msg;
        socket.recv(msg, client);

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
            {
                if ((*i).get() != client)
                    socket.send(msg, *(*i).get());
            }
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
        char *chatmsg = new char[80];
        message.copy(chatmsg, 80);
        // Enviar al servidor usando socket
        ChatMessage em(nick, chatmsg);
        em.type = ChatMessage::MESSAGE;
        socket.send(em, socket);
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
