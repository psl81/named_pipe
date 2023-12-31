#include <iostream>
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/dll.hpp>
#include <boost/winapi/process.hpp>

namespace bp = boost::process;

struct new_window : bp::detail::handler_base
{
    // this function will be invoked at child process constructor before spawning process
    template <class WindowsExecutor>
    void on_setup(WindowsExecutor& e) const
    {
        e.creation_flags = ::boost::winapi::CREATE_NEW_CONSOLE_;
    }
};

int main()
{
    const uint32_t buff_size = 8192;
    const wchar_t* pipe_name = L"\\\\.\\pipe\\SamplePipe";
    boost::asio::io_context ios;

    boost::asio::detail::native_pipe_handle pipe_handle = 
        ::CreateNamedPipeW(pipe_name,
            PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_WAIT, 1, buff_size, buff_size, NMPWAIT_USE_DEFAULT_WAIT, nullptr);

    if (pipe_handle == INVALID_HANDLE_VALUE) {
        std::cout << "Error: " << GetLastError() << std::endl;
        return -1;
    }

    boost::asio::readable_pipe pipe(ios, pipe_handle);
    // run client
    auto program_path = boost::dll::program_location().parent_path();
    auto client_pathname = bp::search_path("client", { program_path });
    bp::child client(client_pathname, new_window());

    if (client.running())
        ConnectNamedPipe(pipe_handle, nullptr);

    std::string buf(buff_size, '\0');
    auto boost_buff = boost::asio::buffer(buf);
    while (client.running()) {
        pipe.async_read_some(boost_buff,
            [&buf](auto ec, auto size) {
                if (ec)
                    std::cout << ec.what() << std::endl;
                if (!ec) {
                    buf[size] = '\0';
                    std::cout << "recv: " << buf.c_str() << std::endl;
                }
            });
        ios.restart();
        ios.run();
    }

    // wait for the client to exit
    client.wait();

    return 0;
}