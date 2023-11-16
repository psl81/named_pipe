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
    boost::asio::io_context ios;
    bp::async_pipe pipe(ios, "\\\\.\\pipe\\SamplePipe");

    // run client
    auto program_path = boost::dll::program_location().parent_path();
    auto client_pathname = bp::search_path("client", { program_path });
    bp::child client(client_pathname, bp::std_in < stdin, bp::std_out > pipe, new_window());

    std::string buf(buff_size, '\0');
    auto boost_buff = boost::asio::buffer(buf);
    while (client.running())
    {
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