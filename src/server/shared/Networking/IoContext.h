#ifndef IoContext_h__
#define IoContext_h__

#include <boost/asio/io_context.hpp>
#include <boost/asio/deadline_timer.hpp>

using IoContext = boost::asio::io_context;
using DeadlineTimer = boost::asio::deadline_timer;

#endif // IoContext_h__
