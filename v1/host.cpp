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
            PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_WAIT, 1, buff_size, buff_size, NMPWAIT_USE_DEFAULT_WAIT, nullptr);

    boost::asio::writable_pipe pipe(ios, pipe_handle);
    // run client
    auto program_path = boost::dll::program_location().parent_path();
    auto client_pathname = bp::search_path("client", { program_path });
    bp::child client(client_pathname, new_window());

    while (client.running())
    {
        BOOL result = ::ConnectNamedPipe(pipe_handle, nullptr);
        std::string buf;
        while (true)
        {
            try {
                std::cout << "send: ";
                std::getline(std::cin, buf);
                if (not buf.empty())
                    pipe.write_some(boost::asio::buffer(buf));
            }
            catch (std::exception & e) {
                std::cout << e.what() << std::endl;
                break;
            }
        }
    }

    // wait for the client to exit
    client.wait();
    int result = client.exit_code();

    return 0;
}