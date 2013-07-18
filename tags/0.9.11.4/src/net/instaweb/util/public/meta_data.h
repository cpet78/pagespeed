/**
 * Copyright 2010 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Author: jmarantz@google.com (Joshua Marantz)
//
// Meta-data associated with a rewriting resource.  This is
// primarily a key-value store, but additionally we want to
// get easy access to the cache expiration time.

#ifndef NET_INSTAWEB_UTIL_PUBLIC_META_DATA_H_
#define NET_INSTAWEB_UTIL_PUBLIC_META_DATA_H_

#include <vector>
#include "base/basictypes.h"
#include "net/instaweb/util/public/string_util.h"

namespace net_instaweb {

class MessageHandler;
class Writer;

// Global constants for common HTML attribues names and values.
//
// TODO(jmarantz): proactively change all the occurences of the static strings
// to use these shared constants.
struct HttpAttributes {
  static const char kAcceptEncoding[];
  static const char kCacheControl[];
  static const char kContentEncoding[];
  static const char kContentLength[];
  static const char kContentType[];
  static const char kDate[];
  static const char kDeflate[];
  static const char kEtag[];
  static const char kExpires[];
  static const char kGzip[];
  static const char kHost[];
  static const char kIfModifiedSince[];
  static const char kLastModified[];
  static const char kLocation[];
  static const char kReferer[]; // sic
  static const char kServer[];
  static const char kSetCookie[];
  static const char kTransferEncoding[];
  static const char kUserAgent[];
  static const char kVary[];
};

namespace HttpStatus {
// Http status codes.
// Grokked from http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
enum Code {
  kContinue = 100,
  kSwitchingProtocols = 101,

  kOK = 200,
  kCreated = 201,
  kAccepted = 202,
  kNonAuthoritative = 203,
  kNoContent = 204,
  kResetContent = 205,
  kPartialContent = 206,

  kMultipleChoices = 300,
  kMovedPermanently = 301,
  kFound = 302,
  kSeeOther = 303,
  kNotModified = 304,
  kUseProxy = 305,
  kSwitchProxy = 306,  // In old spec; no longer used.
  kTemporaryRedirect = 307,

  kBadRequest = 400,
  kUnauthorized = 401,
  kPaymentRequired = 402,
  kForbidden = 403,
  kNotFound = 404,
  kMethodNotAllowed = 405,
  kNotAcceptable = 406,
  kProxyAuthRequired = 407,
  kRequestTimeout = 408,
  kConflict = 409,
  kGone = 410,
  kLengthRequired = 411,
  kPreconditionFailed = 412,
  kEntityTooLarge = 413,
  kUriTooLong = 414,
  kUnsupportedMediaType = 415,
  kRangeNotSatisfiable = 416,
  kExpectationFailed = 417,

  kInternalServerError = 500,
  kNotImplemented = 501,
  kBadGateway = 502,
  kUnavailable = 503,
  kGatewayTimeout = 504,
  kHttpVersionNotSupported = 505,

  // Instaweb-specific response codes: these are intentionally chosen to be
  // outside the normal HTTP range, but we consider these response codes
  // to be 'cacheable' in our own cache.
  kRememberNotFoundStatusCode = 10001,
};

// Transform a status code into the equivalent reason phrase.
const char* GetReasonPhrase(Code rc);

}  // namespace HttpStatus

// Container for required meta-data.  General HTTP headers can be added
// here as name/value pairs, and caching information can then be derived.
//
// TODO(jmarantz): consider rename to HTTPHeader.
// TODO(sligocki): This represents an HTTP response header. We need a request
// header class as well.
// TODO(sligocki): Stop using char* in interface.
class MetaData {
 public:
  MetaData() {}
  virtual ~MetaData();

  void CopyFrom(const MetaData& other);

  // Reset headers to initial state.
  virtual void Clear() = 0;

  // Raw access for random access to attribute name/value pairs.
  virtual int NumAttributes() const = 0;
  virtual const char* Name(int index) const = 0;
  virtual const char* Value(int index) const = 0;

  // Get the attribute values associated with this name.  Returns
  // false if the attribute is not found.  If it was found, then
  // the values vector is filled in.
  virtual bool Lookup(const char* name, CharStarVector* values) const = 0;

  // Add a new header.
  virtual void Add(const StringPiece& name, const StringPiece& value) = 0;

  // Remove all headers by name.
  virtual void RemoveAll(const char* name) = 0;

  // Serialize HTTP response header to a stream.
  virtual bool Write(Writer* writer, MessageHandler* handler) const = 0;
  // Serialize just the headers (not the version and response code line).
  virtual bool WriteHeaders(Writer* writer, MessageHandler* handler) const = 0;

  // Parse a chunk of HTTP response header.  Returns number of bytes consumed.
  virtual int ParseChunk(const StringPiece& text,  MessageHandler* handler) = 0;

  // Compute caching information.  The current time is used to compute
  // the absolute time when a cache resource will expire.  The timestamp
  // is in milliseconds since 1970.  It is an error to call any of the
  // accessors before ComputeCaching is called.
  virtual void ComputeCaching() = 0;
  virtual bool IsCacheable() const = 0;
  virtual bool IsProxyCacheable() const = 0;
  virtual int64 CacheExpirationTimeMs() const = 0;
  virtual void SetDate(int64 date_ms) = 0;
  virtual void SetLastModified(int64 last_modified_ms) = 0;

  virtual bool headers_complete() const = 0;
  virtual void set_headers_complete(bool x) = 0;

  virtual int major_version() const = 0;
  virtual int minor_version() const = 0;
  virtual int status_code() const = 0;
  virtual const char* reason_phrase() const = 0;
  virtual int64 timestamp_ms() const = 0;
  virtual bool has_timestamp_ms() const = 0;

  virtual void set_major_version(int major_version) = 0;
  virtual void set_minor_version(int minor_version) = 0;

  // Sets the status code and reason_phrase based on an internal table.
  void SetStatusAndReason(HttpStatus::Code code);

  virtual void set_status_code(int status_code) = 0;
  virtual void set_reason_phrase(const StringPiece& reason_phrase) = 0;
  // Set whole first line.
  virtual void set_first_line(int major_version,
                              int minor_version,
                              int status_code,
                              const StringPiece& reason_phrase) {
    set_major_version(major_version);
    set_minor_version(minor_version);
    set_status_code(status_code);
    set_reason_phrase(reason_phrase);
  }

  virtual std::string ToString() const = 0;
  void DebugPrint() const;

  // Parses an arbitrary string into milliseconds since 1970
  static bool ParseTime(const char* time_str, int64* time_ms);

  // Determines whether a response header is marked as gzipped.
  bool IsGzipped() const;

  // Determines whether a request header accepts gzipped content.
  bool AcceptsGzip() const;

  // Parses a date header such as HttpAttributes::kDate or
  // HttpAttributes::kExpires, returning the timestamp as
  // number of milliseconds since 1970.
  bool ParseDateHeader(const char* attr, int64* date_ms) const;

  // Updates a date header using time specified as a number of milliseconds
  // since 1970.
  void UpdateDateHeader(const char* attr, int64 date_ms);

 private:
  DISALLOW_COPY_AND_ASSIGN(MetaData);
};

}  // namespace net_instaweb

#endif  // NET_INSTAWEB_UTIL_PUBLIC_META_DATA_H_