#include <iostream>
#include <string>

int main()
{
    const uint32_t buff_size = 8192;

    std::string buf;
    while (true) {
        //std::cout << "send: ";
        std::getline(std::cin, buf);
        std::cout << buf << std::flush;
    }
}