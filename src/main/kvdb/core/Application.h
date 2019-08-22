
#ifndef KVDB_APPLICATION_H
#define KVDB_APPLICATION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <boost/noncopyable.hpp>

namespace kvdb {

class Application : private boost::noncopyable
{
protected:
    int     argc_;
    char ** argv_;
    bool    is_daemon_;
    bool    is_inited_;

public:
    Application(int argc = 0, char * argv[] = nullptr)
        : argc_(argc), argv_(argv),
          is_daemon_(false), is_inited_(false) {
        this->create();
    }

    virtual ~Application() {
        this->destroy();
    }

    bool    is_daemon() const { return is_daemon_; }
    bool    is_inited() const { return is_inited_; }

    int     arg_count() const { return argc_; }
    char ** arg_vars()  const { return argv_; }

    void set_args(int argc, char * argv[]) {
        argc_ = argc;
        argv_ = argv_;
    }

    virtual void welcome() = 0;
    virtual void print_usage() = 0;
    virtual int  main(int argc, char * argv[]) = 0;

    virtual void initialize() {
        // Default is do nothing !
    }

    virtual void finalize() {
        // Default is do nothing !
    }

    void create() {
        if (!is_inited_) {
            this->initialize();
            is_inited_ = true;
        }
    }

    void destroy() {
        if (is_inited_) {
            this->finalize();
            is_inited_ = false;
        }
    }

    int run() {
        this->welcome();
        int result = this->main(argc_, argv_);
        return result;
    }
};

} // namespace kvdb

#endif // KVDB_APPLICATION_H
