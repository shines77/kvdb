
#ifndef KVDB_REQUEST_HANDLER_H
#define KVDB_REQUEST_HANDLER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <string>
#include <boost/noncopyable.hpp>

#include <kvdb/jstd/StringRef.h>
#include <kvdb/core/Message.h>
#include <kvdb/core/Request.h>
#include <kvdb/core/Response.h>
#include <kvdb/stream/InputStream.h>
#include <kvdb/stream/OutputStream.h>
#include <kvdb/stream/PackagedInputStream.h>
#include <kvdb/stream/PackagedOutputStream.h>

#include "server/ConnectionContext.h"

using namespace kvdb;

namespace kvdb {
namespace server {

/// The common handler for all incoming requests.
class RequestHandler : private boost::noncopyable
{
public:
    /// Construct with a directory containing files to be served.
    explicit RequestHandler(const std::string & doc_root);
    ~RequestHandler();

    /// Handle a request and produce a response.
    
    int handleRequest(ConnectionContext & context, const IRequest & request, OutputStream & os);
    int handleRequest(ConnectionContext & context, const IRequest & request, PackagedOutputStream & os);

private:
    template <typename OutputStreamTy>
    int handleHandShakeRequest(ConnectionContext & context, InputStream & is, OutputStreamTy & os);

    template <typename OutputStreamTy>
    int handleLoginRequest(ConnectionContext & context, InputStream & is, OutputStreamTy & os);

    template <typename OutputStreamTy>
    int handleLogoutRequest(ConnectionContext & context, InputStream & is, OutputStreamTy & os);

    template <typename OutputStreamTy>
    int handleQueryRequest(ConnectionContext & context, InputStream & is, OutputStreamTy & os);

    int parseQueryRequestFirstCmd(jstd::StringRef & cmd, const jstd::StringRef & qurey);

    int handleRequestData(const char * data, size_t size);
};

} // namespace server
} // namespace kvdb

#endif // KVDB_REQUEST_HANDLER_H
