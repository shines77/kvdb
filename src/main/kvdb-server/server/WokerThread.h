
#ifndef KVDB_WORKER_THREAD_H
#define KVDB_WORKER_THREAD_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>

#include <vector>
#include <atomic>
#include <thread>
#include <memory>
#include <mutex>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <kvdb/core/Request.h>
#include <kvdb/core/Response.h>

#include "kvdb/asio/IoContext.h"
#include "kvdb/asio/DeadlineTimer.h"

using namespace boost::asio::ip;
using boost::asio::ip::tcp;

//
// Since boost.asio 1.66, it has changed from asio::io_service to asio::io_context.
//

namespace kvdb {
namespace server {

class WorkerThread {
private:
    typedef std::vector<std::shared_ptr<tcp::socket>> socket_container_t;

    std::unique_ptr<std::thread> thread_;

    std::atomic<int32_t>        connections_;
    std::atomic<bool>           startting_;
    std::atomic<bool>           running_;
    std::atomic<bool>           stopping_;

    socket_container_t          socketContainer_;

    std::mutex                  socketsLock_;
    socket_container_t          newSockets_;

    kvdb::Asio::IoContext       ioContext_;
    tcp::socket                 acceptSocket_;
    kvdb::Asio::DeadlineTimer   updateTimer_;

public:
    WorkerThread() : connections_(0), startting_(false), running_(false), stopping_(false),
                     ioContext_(1), acceptSocket_(ioContext_), updateTimer_(ioContext_) {
        //
    }

    virtual ~WorkerThread() {
        this->stop();
        this->wait();
    }

    bool start()
    {
        if (this->startting_ || this->running_)
            return false;

        if (this->thread_.get() != nullptr)
            return false;

        this->startting_ = true;

        this->thread_.reset(new std::thread(&WorkerThread::run, this));
        return true;
    }

    void stop()
    {
        if (this->running_ && !this->stopping_) {
            this->stopping_ = true;

            this->ioContext_.stop();
        }
    }

    void wait()
    {
        if (this->startting_ || this->running_) {
            assert(this->thread_.get() != nullptr);

            if (this->thread_.get() != nullptr) {
                if (this->thread_->joinable()) {
                    this->thread_->join();
                }
                this->thread_.reset();
            }

            this->stopping_ = false;
            this->running_ = false;
        }
    }

protected:
    void run()
    {
        if (this->startting_ && !this->running_) {

            this->ioContext_.run();

            this->startting_ = false;
            this->running_ = true;
            this->stopping_ = false;
        }
    }

};

} // namespace server
} // namespace kvdb

#endif // KVDB_WORKER_THREAD_H
