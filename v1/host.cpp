#include <iostream>
#include <boost/asio.hpp>
#include <boost/process.hpp>

int main()
{
    const uint32_t buff_size = 8192;
    const wchar_t* pipe_name = L"\\\\.\\pipe\\SamplePipe";
    boost::asio::io_context ios;

    boost::asio::detail::native_pipe_handle pipe_handle = 
        ::CreateNamedPipeW(pipe_name,
            PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_WAIT, 1, buff_size, buff_size, NMPWAIT_USE_DEFAULT_WAIT, nullptr);

    boost::asio::writable_pipe pipe(ios, pipe_handle);

    BOOL result = ::ConnectNamedPipe(pipe_handle, NULL);

    std::string buf(buff_size, '\0');
    auto buff = boost::asio::buffer(buf);
    while (true)
    {
        std::getline(std::cin, buf);
        pipe.write_some(buff);
    }
    return 0;
}