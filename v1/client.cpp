#include <iostream>
#include <boost/asio.hpp>
#include <boost/process.hpp>

int main()
{
    const uint32_t buff_size = 8192;
    const wchar_t* pipe_name = L"\\\\.\\pipe\\SamplePipe";
    boost::asio::io_context ios;

    boost::asio::detail::native_pipe_handle pipe_handle = 
        ::CreateFileW(pipe_name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

    boost::asio::readable_pipe pipe(ios);
    pipe.assign(pipe_handle);

    std::string buf(buff_size, '\0');
    while (true) {
        pipe.async_read_some(boost::asio::buffer(buf),
            [&buf](auto ec, auto size) {
                if (ec)
                    std::cout << ec << std::endl;
                if (!ec)
                    std::cout << buf << std::endl;
            });
        ios.restart();
        ios.run();
    }
}