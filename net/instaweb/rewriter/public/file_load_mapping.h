/*
 * Copyright 2012 Google Inc.
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

// Author: jefftk@google.com (Jeff Kaufman)

#ifndef NET_INSTAWEB_REWRITER_PUBLIC_FILE_LOAD_MAPPING_H_
#define NET_INSTAWEB_REWRITER_PUBLIC_FILE_LOAD_MAPPING_H_

#include "pagespeed/kernel/base/basictypes.h"
#include "pagespeed/kernel/base/manually_ref_counted.h"
#include "pagespeed/kernel/base/string.h"
#include "pagespeed/kernel/base/string_util.h"
#include "pagespeed/kernel/util/re2.h"

namespace net_instaweb {

// Class for storing a mapping from a URL to a filesystem path, for use by
// FileLoadPolicy.
class FileLoadMapping : public ManuallyRefCounted {
 public:
  virtual ~FileLoadMapping();

  // If this mapping applies to this url, put the mapped path into filename and
  // return true.  Otherwise return false.
  virtual bool Substitute(StringPiece url, GoogleString* filename) const = 0;
};

// A simple mapping from a prefix in url-space to a prefix in filesystem-space.
// For example, if we had:
//   FileLoadMappingLiteral("http://example.com/foo/bar/", "/foobar/")
// that would mean http://example.com/foo/bar/baz would be found on the
// filesystem at /foobar/baz.
class FileLoadMappingLiteral : public FileLoadMapping {
 public:
  FileLoadMappingLiteral(const GoogleString& url_prefix,
                         const GoogleString& filename_prefix)
      : url_prefix_(url_prefix),
        filename_prefix_(filename_prefix) {}

  virtual bool Substitute(StringPiece url, GoogleString* filename) const;

 private:
  const GoogleString url_prefix_;
  const GoogleString filename_prefix_;

  DISALLOW_COPY_AND_ASSIGN(FileLoadMappingLiteral);
};

// If a mapping is too complicated to represent with a simple literal with
// FileLoadMappingLiteral, you can use a regexp mapper.  For example, if we had:
//   FileLoadMappingRegexp("http://example.com/([^/*])/bar/", "/var/bar/\\1/")
// that would mean http://example.com/foo/bar/baz would be found on the
// filesystem at /var/bar/foo/baz.
class FileLoadMappingRegexp : public FileLoadMapping {
 public:
  FileLoadMappingRegexp(const GoogleString& url_regexp,
                        const GoogleString& filename_prefix)
      : url_regexp_(url_regexp),
        url_regexp_str_(url_regexp),
        filename_prefix_(filename_prefix) {}

  virtual bool Substitute(StringPiece url, GoogleString* filename) const;

 private:
  const RE2 url_regexp_;
  // RE2s can't be copied, so we need to keep the string around.
  const GoogleString url_regexp_str_;
  const GoogleString filename_prefix_;

  DISALLOW_COPY_AND_ASSIGN(FileLoadMappingRegexp);
};

}  // namespace net_instaweb

#endif  // NET_INSTAWEB_REWRITER_PUBLIC_FILE_LOAD_MAPPING_H_

