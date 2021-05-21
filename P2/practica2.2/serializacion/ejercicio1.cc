#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_NAME_LENGHT 80

class Jugador : public Serializable
{
public:
    Jugador(const char *_n, int16_t _x, int16_t _y) : x(_x), y(_y)
    {
        strncpy(name, _n, MAX_NAME_LENGHT);
    };

    virtual ~Jugador(){};

    void to_bin()
    {

        int lenght = strnlen(name, MAX_NAME_LENGHT);
        //los datos serán la longitud del nombre, el propio nombre y las coordenadas
        int size = sizeof(int) + sizeof(char) * lenght + 2 * sizeof(int16_t);
        alloc_data(size);
        char *aux = _data;
        memcpy(aux, &lenght, sizeof(int));
        aux += sizeof(int);
        memcpy(aux, &name, sizeof(char) * lenght);
        aux += lenght * sizeof(char);
        memcpy(aux, &x, sizeof(int16_t));
        aux += sizeof(int16_t);
        memcpy(aux, &y, sizeof(int16_t));
    }

    int from_bin(char *data)
    {
        //Si está vacío no se puede serializar
        if (data == 0)
            return -1;
        int lenght = 0;
        memcpy(&lenght, data, sizeof(int));
        data += sizeof(int);
        memcpy(&name, data, lenght);
        data += sizeof(char) * lenght;
        memcpy(&x, data, sizeof(int16_t));
        data += sizeof(int16_t);

        memcpy(&y, data, sizeof(int16_t));
        _size = lenght * sizeof(char) + 2 * sizeof(int16_t);
        return 0;
    }

public:
    char name[80];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();
    // 2. Escribir la serialización en un fichero
    int fileW = open("serialW", O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (fileW == -1)
    {
        std::cout << strerror(errno);
        return -1;
    }
    write(fileW, one_w.data(), one_w.size());
    close(fileW);
    // 3. Leer el fichero
    fileW = open("serialW", O_RDONLY, 0666);
    char *fileData = new char[one_w.size()];
    read(fileW,fileData,one_w.size());

    // 4. "Deserializar" en one_r

    if (one_r.from_bin(fileData) < 0)
    {
        std::cout << "data esta vacia\n";
        return -1;
    }

    // 5. Mostrar el contenido de one_r
    std::cout << one_r.name << '\n';
    std::cout << one_r.x << '\n';
    std::cout << one_r.y << '\n';
    delete fileData;
    close(fileW);
    return 0;
}
