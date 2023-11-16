#include <iostream>
#include <boost/asio.hpp>
#include <boost/process.hpp>

int main()
{
    const uint32_t buff_size = 8192;
    const wchar_t* pipe_name = L"\\\\.\\pipe\\SamplePipe";
    boost::asio::io_context ios;

    boost::asio::detail::native_pipe_handle pipe_handle = 
        ::CreateFileW(pipe_name, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

    if (pipe_handle == INVALID_HANDLE_VALUE) {
        std::cout << "Error: " << GetLastError() << std::endl;
        return -1;
    }

    boost::asio::writable_pipe pipe(ios);
    pipe.assign(pipe_handle);

    std::string buf;
    while (true) {
        std::cout << "send: ";
        std::getline(std::cin, buf);
        if (not buf.empty())
            pipe.async_write_some(boost::asio::buffer(buf),
                [&buf](auto ec, auto size) {
                    std::cout << ec.what() << std::endl;
                });
    }
}