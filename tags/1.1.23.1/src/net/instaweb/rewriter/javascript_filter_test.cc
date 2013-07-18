/*
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

// Unit-test the javascript filter

#include "net/instaweb/rewriter/public/javascript_filter.h"

#include "net/instaweb/htmlparse/public/html_parse_test_base.h"
#include "net/instaweb/http/public/content_type.h"
#include "net/instaweb/http/public/log_record.h"
#include "net/instaweb/rewriter/public/javascript_code_block.h"
#include "net/instaweb/rewriter/public/rewrite_test_base.h"
#include "net/instaweb/rewriter/public/rewrite_driver.h"
#include "net/instaweb/rewriter/public/rewrite_options.h"
#include "net/instaweb/util/public/basictypes.h"
#include "net/instaweb/util/public/gtest.h"
#include "net/instaweb/util/public/lru_cache.h"
#include "net/instaweb/util/public/md5_hasher.h"
#include "net/instaweb/util/public/statistics.h"
#include "net/instaweb/util/public/string.h"
#include "net/instaweb/util/public/string_util.h"

namespace {

const char kHtmlFormat[] =
    "<script type='text/javascript' src='%s'></script>\n";
const char kInlineScriptFormat[] =
    "<script type='text/javascript'>"
    "%s"
    "</script>";
const char kEndInlineScript[] = "<script type='text/javascript'>";

const char kCdataWrapper[] = "//<![CDATA[\n%s\n//]]>";
const char kCdataAltWrapper[] = "//<![CDATA[\r%s\r//]]>";

const char kInlineJs[] =
    "<script type='text/javascript'>%s</script>\n";

const char kJsData[] =
    "alert     (    'hello, world!'    ) "
    " /* removed */ <!-- removed --> "
    " // single-line-comment";
const char kJsMinData[] = "alert('hello, world!')";
const char kFilterId[] = "jm";
const char kOrigJsName[] = "hello.js";
const char kRewrittenJsName[] = "hello.js";
const char kLibraryUrl[] = "https://www.example.com/hello/1.0/hello.js";

}  // namespace

namespace net_instaweb {

class JavascriptFilterTest : public RewriteTestBase {
 protected:
  virtual void SetUp() {
    RewriteTestBase::SetUp();
    expected_rewritten_path_ = Encode(kTestDomain, kFilterId, "0",
                                      kRewrittenJsName, "js");

    blocks_minified_ = statistics()->GetVariable(
        JavascriptRewriteConfig::kBlocksMinified);
    libraries_identified_ = statistics()->GetVariable(
        JavascriptRewriteConfig::kLibrariesIdentified);
    minification_failures_ = statistics()->GetVariable(
        JavascriptRewriteConfig::kMinificationFailures);
    total_bytes_saved_ = statistics()->GetVariable(
        JavascriptRewriteConfig::kTotalBytesSaved);
    total_original_bytes_ = statistics()->GetVariable(
        JavascriptRewriteConfig::kTotalOriginalBytes);
    num_uses_ = statistics()->GetVariable(
        JavascriptRewriteConfig::kMinifyUses);
  }

  void InitFilters() {
    options()->EnableFilter(RewriteOptions::kRewriteJavascript);
    options()->EnableFilter(RewriteOptions::kCanonicalizeJavascriptLibraries);
    rewrite_driver_->AddFilters();
  }

  void InitTest(int64 ttl) {
    SetResponseWithDefaultHeaders(kOrigJsName, kContentTypeJavascript,
                                  kJsData, ttl);
  }

  void InitFiltersAndTest(int64 ttl) {
    InitFilters();
    InitTest(ttl);
  }

  void RegisterLibrary() {
    MD5Hasher hasher;
    GoogleString hash = hasher.Hash(kJsMinData);
    EXPECT_TRUE(
        options()->RegisterLibrary(
            STATIC_STRLEN(kJsMinData), hash, kLibraryUrl));
  }

  // Generate HTML loading 3 resources with the specified URLs
  GoogleString GenerateHtml(const char* a) {
    return StringPrintf(kHtmlFormat, a);
  }

  void TestCorruptUrl(const char* new_suffix) {
    // Do a normal rewrite test
    InitFiltersAndTest(100);
    ValidateExpected("no_ext_corruption",
                    GenerateHtml(kOrigJsName),
                    GenerateHtml(expected_rewritten_path_.c_str()));

    // Fetch messed up URL.
    ASSERT_TRUE(StringCaseEndsWith(expected_rewritten_path_, ".js"));
    GoogleString munged_url =
        ChangeSuffix(expected_rewritten_path_, false /* replace */,
                     ".js", new_suffix);

    GoogleString out;
    EXPECT_TRUE(FetchResourceUrl(munged_url, &out));

    // Rewrite again; should still get normal URL
    ValidateExpected("no_ext_corruption",
                    GenerateHtml(kOrigJsName),
                    GenerateHtml(expected_rewritten_path_.c_str()));
  }

  GoogleString expected_rewritten_path_;

  // Stats
  Variable* blocks_minified_;
  Variable* libraries_identified_;
  Variable* minification_failures_;
  Variable* total_bytes_saved_;
  Variable* total_original_bytes_;
  Variable* num_uses_;
};

TEST_F(JavascriptFilterTest, DoRewrite) {
  LoggingInfo logging_info;
  LogRecord log_record(&logging_info);
  rewrite_driver()->set_log_record(&log_record);
  InitFiltersAndTest(100);
  ValidateExpected("do_rewrite",
                   GenerateHtml(kOrigJsName),
                   GenerateHtml(expected_rewritten_path_.c_str()));

  EXPECT_EQ(1, blocks_minified_->Get());
  EXPECT_EQ(0, minification_failures_->Get());
  EXPECT_EQ(STATIC_STRLEN(kJsData) - STATIC_STRLEN(kJsMinData),
            total_bytes_saved_->Get());
  EXPECT_EQ(STATIC_STRLEN(kJsData), total_original_bytes_->Get());
  EXPECT_EQ(1, num_uses_->Get());
  EXPECT_STREQ("jm", logging_info.applied_rewriters());
}

TEST_F(JavascriptFilterTest, DoRewriteUnhealthy) {
  lru_cache()->set_is_healthy(false);

  LoggingInfo logging_info;
  LogRecord log_record(&logging_info);
  rewrite_driver()->set_log_record(&log_record);
  InitFiltersAndTest(100);
  ValidateNoChanges("do_rewrite", GenerateHtml(kOrigJsName));
  EXPECT_STREQ("", logging_info.applied_rewriters());
}

TEST_F(JavascriptFilterTest, RewriteAlreadyCachedProperly) {
  InitFiltersAndTest(100000000);  // cached for a long time to begin with
  // But we will rewrite because we can make the data smaller.
  ValidateExpected("rewrite_despite_being_cached_properly",
                   GenerateHtml(kOrigJsName),
                   GenerateHtml(expected_rewritten_path_.c_str()));
}

TEST_F(JavascriptFilterTest, NoRewriteOriginUncacheable) {
  InitFiltersAndTest(0);  // origin not cacheable
  ValidateExpected("no_extend_origin_not_cacheable",
                   GenerateHtml(kOrigJsName),
                   GenerateHtml(kOrigJsName));

  EXPECT_EQ(0, blocks_minified_->Get());
  EXPECT_EQ(0, minification_failures_->Get());
  EXPECT_EQ(0, total_bytes_saved_->Get());
  EXPECT_EQ(0, total_original_bytes_->Get());
  EXPECT_EQ(0, num_uses_->Get());
}

TEST_F(JavascriptFilterTest, IdentifyLibrary) {
  RegisterLibrary();
  InitFiltersAndTest(100);
  ValidateExpected("identify_library",
                   GenerateHtml(kOrigJsName),
                   GenerateHtml(kLibraryUrl));

  EXPECT_EQ(1, libraries_identified_->Get());
  EXPECT_EQ(1, blocks_minified_->Get());
  EXPECT_EQ(0, minification_failures_->Get());
}

TEST_F(JavascriptFilterTest, IdentifyLibraryTwice) {
  // Make sure cached recognition is handled properly.
  RegisterLibrary();
  InitFiltersAndTest(100);
  GoogleString orig = StrCat(GenerateHtml(kOrigJsName),
                             GenerateHtml(kOrigJsName));
  GoogleString expect = StrCat(GenerateHtml(kLibraryUrl),
                               GenerateHtml(kLibraryUrl));
  ValidateExpected("identify_library_twice", orig, expect);
  // The second rewrite uses cached data from the first rewrite.
  EXPECT_EQ(1, libraries_identified_->Get());
  EXPECT_EQ(1, blocks_minified_->Get());
  EXPECT_EQ(0, minification_failures_->Get());
}

TEST_F(JavascriptFilterTest, IdentifyLibraryNoMinification) {
  // Don't enable kRewriteJavascript.  This should still identify the library.
  RegisterLibrary();
  options()->EnableFilter(RewriteOptions::kCanonicalizeJavascriptLibraries);
  rewrite_driver_->AddFilters();
  InitTest(100);
  ValidateExpected("identify_library_no_minification",
                   GenerateHtml(kOrigJsName),
                   GenerateHtml(kLibraryUrl));

  EXPECT_EQ(1, libraries_identified_->Get());
  EXPECT_EQ(1, blocks_minified_->Get());
  EXPECT_EQ(0, minification_failures_->Get());
  EXPECT_EQ(0, total_bytes_saved_->Get());
}

TEST_F(JavascriptFilterTest, IdentifyFailureNoMinification) {
  // Don't enable kRewriteJavascript.  We should attempt library identification,
  // fail, and not modify the code even though it can be minified.
  options()->EnableFilter(RewriteOptions::kCanonicalizeJavascriptLibraries);
  rewrite_driver_->AddFilters();
  InitTest(100);
  // We didn't register any libraries, so we should see that minification
  // happened but that nothing changed on the page.
  ValidateExpected("identify_failure_no_minification",
                   GenerateHtml(kOrigJsName),
                   GenerateHtml(kOrigJsName));

  EXPECT_EQ(0, libraries_identified_->Get());
  EXPECT_EQ(1, blocks_minified_->Get());
  EXPECT_EQ(0, minification_failures_->Get());
}

TEST_F(JavascriptFilterTest, IgnoreLibraryNoIdentification) {
  RegisterLibrary();
  // We register the library but don't enable library redirection.
  options()->EnableFilter(RewriteOptions::kRewriteJavascript);
  rewrite_driver_->AddFilters();
  InitTest(100);
  ValidateExpected("ignore_library",
                   GenerateHtml(kOrigJsName),
                   GenerateHtml(expected_rewritten_path_.c_str()));

  EXPECT_EQ(0, libraries_identified_->Get());
  EXPECT_EQ(1, blocks_minified_->Get());
  EXPECT_EQ(0, minification_failures_->Get());
}

TEST_F(JavascriptFilterTest, ServeFiles) {
  InitFilters();
  TestServeFiles(&kContentTypeJavascript, kFilterId, "js",
                 kOrigJsName, kJsData,
                 kRewrittenJsName, kJsMinData);

  EXPECT_EQ(1, blocks_minified_->Get());
  EXPECT_EQ(0, minification_failures_->Get());
  EXPECT_EQ(STATIC_STRLEN(kJsData) - STATIC_STRLEN(kJsMinData),
            total_bytes_saved_->Get());
  EXPECT_EQ(STATIC_STRLEN(kJsData), total_original_bytes_->Get());
  // Note: We do not count any uses, because we did not write the URL into
  // an HTML file, just served it on request.
  EXPECT_EQ(0, num_uses_->Get());

  // Finally, serve from a completely separate server.
  ServeResourceFromManyContexts(expected_rewritten_path_, kJsMinData);
}

TEST_F(JavascriptFilterTest, ServeFilesUnhealthy) {
  lru_cache()->set_is_healthy(false);

  InitFilters();
  InitTest(100);
  TestServeFiles(&kContentTypeJavascript, kFilterId, "js",
                 kOrigJsName, kJsData,
                 kRewrittenJsName, kJsMinData);
}

TEST_F(JavascriptFilterTest, InvalidInputMimetype) {
  InitFilters();
  // Make sure we can rewrite properly even when input has corrupt mimetype.
  ContentType not_java_script = kContentTypeJavascript;
  not_java_script.mime_type_ = "text/semicolon-inserted";
  const char* kNotJsFile = "script.notjs";

  SetResponseWithDefaultHeaders(kNotJsFile, not_java_script, kJsData, 100);
  ValidateExpected("wrong_mime",
                   GenerateHtml(kNotJsFile),
                   GenerateHtml(Encode(kTestDomain, "jm", "0",
                                       kNotJsFile, "js").c_str()));
}

TEST_F(JavascriptFilterTest, RewriteJs404) {
  InitFilters();
  // Test to make sure that a missing input is handled well.
  SetFetchResponse404("404.js");
  ValidateNoChanges("404", "<script src='404.js'></script>");
  EXPECT_EQ(0, blocks_minified_->Get());
  EXPECT_EQ(0, minification_failures_->Get());
  EXPECT_EQ(0, num_uses_->Get());

  // Second time, to make sure caching doesn't break it.
  ValidateNoChanges("404", "<script src='404.js'></script>");
  EXPECT_EQ(0, blocks_minified_->Get());
  EXPECT_EQ(0, minification_failures_->Get());
  EXPECT_EQ(0, num_uses_->Get());
}

// Make sure bad requests do not corrupt our extension.
TEST_F(JavascriptFilterTest, NoExtensionCorruption) {
  TestCorruptUrl(".js%22");
}

TEST_F(JavascriptFilterTest, NoQueryCorruption) {
  TestCorruptUrl(".js?query");
}

TEST_F(JavascriptFilterTest, NoWrongExtCorruption) {
  TestCorruptUrl(".html");
}

TEST_F(JavascriptFilterTest, InlineJavascript) {
  // Test minification of a simple inline script
  InitFiltersAndTest(100);
  ValidateExpected("inline javascript",
                   StringPrintf(kInlineJs, kJsData),
                   StringPrintf(kInlineJs, kJsMinData));

  EXPECT_EQ(1, blocks_minified_->Get());
  EXPECT_EQ(0, minification_failures_->Get());
  EXPECT_EQ(STATIC_STRLEN(kJsData) - STATIC_STRLEN(kJsMinData),
            total_bytes_saved_->Get());
  EXPECT_EQ(STATIC_STRLEN(kJsData), total_original_bytes_->Get());
  EXPECT_EQ(1, num_uses_->Get());
}

TEST_F(JavascriptFilterTest, StripInlineWhitespace) {
  // Make sure we strip inline whitespace when minifying external scripts.
  InitFiltersAndTest(100);
  ValidateExpected(
      "StripInlineWhitespace",
      StrCat("<script src='", kOrigJsName, "'>   \t\n   </script>"),
      StrCat("<script src='",
             Encode(kTestDomain, "jm", "0", kOrigJsName, "js"),
             "'></script>"));
}

TEST_F(JavascriptFilterTest, RetainInlineData) {
  // Test to make sure we keep inline data when minifying external scripts.
  InitFiltersAndTest(100);
  ValidateExpected("StripInlineWhitespace",
                   StrCat("<script src='", kOrigJsName, "'> data </script>"),
                   StrCat("<script src='",
                          Encode(kTestDomain, "jm", "0", kOrigJsName, "js"),
                          "'> data </script>"));
}

// Test minification of a simple inline script in markup with no
// mimetype, where the script is wrapped in a commented-out CDATA.
//
// Note that javascript_filter never adds CDATA.  It only removes it
// if it's sure the mimetype is HTML.
TEST_F(JavascriptFilterTest, CdataJavascriptNoMimetype) {
  InitFiltersAndTest(100);
  ValidateExpected(
      "cdata javascript no mimetype",
      StringPrintf(kInlineJs, StringPrintf(kCdataWrapper, kJsData).c_str()),
      StringPrintf(kInlineJs, StringPrintf(kCdataWrapper, kJsMinData).c_str()));
  ValidateExpected(
      "cdata javascript no mimetype with \\r",
      StringPrintf(kInlineJs, StringPrintf(kCdataAltWrapper, kJsData).c_str()),
      StringPrintf(kInlineJs, StringPrintf(kCdataWrapper, kJsMinData).c_str()));
}

// Same as CdataJavascriptNoMimetype, but with explicit HTML mimetype.
TEST_F(JavascriptFilterTest, CdataJavascriptHtmlMimetype) {
  SetHtmlMimetype();
  InitFiltersAndTest(100);
  ValidateExpected(
      "cdata javascript with explicit HTML mimetype",
      StringPrintf(kInlineJs, StringPrintf(kCdataWrapper, kJsData).c_str()),
      StringPrintf(kInlineJs, kJsMinData));
  ValidateExpected(
      "cdata javascript with explicit HTML mimetype and \\r",
      StringPrintf(kInlineJs, StringPrintf(kCdataAltWrapper, kJsData).c_str()),
      StringPrintf(kInlineJs, kJsMinData));
}

// Same as CdataJavascriptNoMimetype, but with explicit XHTML mimetype.
TEST_F(JavascriptFilterTest, CdataJavascriptXhtmlMimetype) {
  SetXhtmlMimetype();
  InitFiltersAndTest(100);
  ValidateExpected(
      "cdata javascript with explicit XHTML mimetype",
      StringPrintf(kInlineJs, StringPrintf(kCdataWrapper, kJsData).c_str()),
      StringPrintf(kInlineJs, StringPrintf(kCdataWrapper, kJsMinData).c_str()));
  ValidateExpected(
      "cdata javascript with explicit XHTML mimetype and \\r",
      StringPrintf(kInlineJs, StringPrintf(kCdataAltWrapper, kJsData).c_str()),
      StringPrintf(kInlineJs, StringPrintf(kCdataWrapper, kJsMinData).c_str()));
}

TEST_F(JavascriptFilterTest, XHtmlInlineJavascript) {
  // Test minification of a simple inline script in xhtml
  // where it must be wrapped in CDATA.
  InitFiltersAndTest(100);
  const GoogleString xhtml_script_format =
      StrCat(kXhtmlDtd, StringPrintf(kInlineJs, kCdataWrapper));
  ValidateExpected("xhtml inline javascript",
                   StringPrintf(xhtml_script_format.c_str(), kJsData),
                   StringPrintf(xhtml_script_format.c_str(), kJsMinData));
  const GoogleString xhtml_script_alt_format =
      StrCat(kXhtmlDtd, StringPrintf(kInlineJs, kCdataAltWrapper));
  ValidateExpected("xhtml inline javascript",
                   StringPrintf(xhtml_script_alt_format.c_str(), kJsData),
                   StringPrintf(xhtml_script_format.c_str(), kJsMinData));
}

// http://code.google.com/p/modpagespeed/issues/detail?id=324
TEST_F(JavascriptFilterTest, RetainExtraHeaders) {
  InitFilters();
  GoogleString url = StrCat(kTestDomain, kOrigJsName);
  SetResponseWithDefaultHeaders(url, kContentTypeJavascript, kJsData, 300);
  TestRetainExtraHeaders(kOrigJsName, "jm", "js");
}

// http://code.google.com/p/modpagespeed/issues/detail?id=327 -- we were
// previously busting regexps with backslashes in them.
TEST_F(JavascriptFilterTest, BackslashInRegexp) {
  InitFilters();
  GoogleString input = StringPrintf(kInlineJs, "/http:\\/\\/[^/]+\\//");
  ValidateNoChanges("backslash_in_regexp", input);
}

TEST_F(JavascriptFilterTest, WeirdSrcCrash) {
  InitFilters();
  // These used to crash due to bugs in the lexer breaking invariants some
  // filters relied on.
  //
  // Note that the attribute-value "foo<bar" gets converted into "foo%3Cbar"
  // by this line:
  //   const GoogleUrl resource_url(base_url(), input_url);
  // in CommonFilter::CreateInputResource.  Following that, resource_url.Spec()
  // has the %3C in it.  I guess that's probably the right thing to do, but
  // I was a little surprised.
  static const char kUrl[] = "foo%3Cbar";
  SetResponseWithDefaultHeaders(kUrl, kContentTypeJavascript, kJsData, 300);
  ValidateExpected("weird_attr", "<script src=foo<bar>Content",
                   StrCat("<script src=",
                          Encode(kTestDomain, "jm", "0", kUrl, "js"),
                          ">Content"));
  ValidateNoChanges("weird_tag", "<script<foo>");
}

TEST_F(JavascriptFilterTest, MinificationFailure) {
  InitFilters();
  SetResponseWithDefaultHeaders("foo.js", kContentTypeJavascript,
                                "/* truncated comment", 100);
  ValidateNoChanges("fail", "<script src=foo.js></script>");

  EXPECT_EQ(0, blocks_minified_->Get());
  EXPECT_EQ(1, minification_failures_->Get());
  EXPECT_EQ(0, num_uses_->Get());
}

TEST_F(JavascriptFilterTest, ReuseRewrite) {
  InitFiltersAndTest(100);

  ValidateExpected("reuse_rewrite1",
                   GenerateHtml(kOrigJsName),
                   GenerateHtml(expected_rewritten_path_.c_str()));
  // First time: We minify JS and use the minified version.
  EXPECT_EQ(1, blocks_minified_->Get());
  EXPECT_EQ(1, num_uses_->Get());

  ClearStats();
  ValidateExpected("reuse_rewrite2",
                   GenerateHtml(kOrigJsName),
                   GenerateHtml(expected_rewritten_path_.c_str()));
  // Second time: We reuse the original rewrite.
  EXPECT_EQ(0, blocks_minified_->Get());
  EXPECT_EQ(1, num_uses_->Get());
}

TEST_F(JavascriptFilterTest, NoReuseInline) {
  InitFiltersAndTest(100);

  ValidateExpected("reuse_inline1",
                   StringPrintf(kInlineJs, kJsData),
                   StringPrintf(kInlineJs, kJsMinData));
  // First time: We minify JS and use the minified version.
  EXPECT_EQ(1, blocks_minified_->Get());
  EXPECT_EQ(1, num_uses_->Get());

  ClearStats();
  ValidateExpected("reuse_inline2",
                   StringPrintf(kInlineJs, kJsData),
                   StringPrintf(kInlineJs, kJsMinData));
  // Second time: Apparently we minify it again.
  // NOTE: This test is here to document current behavior. It should be fine
  // to change this behavior so that the rewrite is cached (although it may
  // not be worth it).
  EXPECT_EQ(1, blocks_minified_->Get());
  EXPECT_EQ(1, num_uses_->Get());
}

// See http://code.google.com/p/modpagespeed/issues/detail?id=542
TEST_F(JavascriptFilterTest, ExtraCdataOnMalformedInput) {
  InitFiltersAndTest(100);

  // This is an entirely bogus thing to have in a script tag, but that was
  // what was reported  by a user.  We were wrapping this an an extra CDATA
  // tag, so this test proves we are no longer doing that.
  static const char kIssue542LinkInScript[] =
      "<![CDATA[<link href='http://fonts.googleapis.com/css'>]]>";

  const GoogleString kHtmlInput = StringPrintf(
      kInlineScriptFormat,
      StrCat("\n", kIssue542LinkInScript, "\n").c_str());
  const GoogleString kHtmlOutput = StringPrintf(
      kInlineScriptFormat,
      kIssue542LinkInScript);
  ValidateExpected("broken_cdata", kHtmlInput, kHtmlOutput);
}

TEST_F(JavascriptFilterTest, ValidCdata) {
  InitFiltersAndTest(100);

  const GoogleString kHtmlInput = StringPrintf(
      kInlineScriptFormat,
      StringPrintf(kCdataWrapper, "alert ( 'foo' ) ; \n").c_str());
  const GoogleString kHtmlOutput = StringPrintf(
      kInlineScriptFormat,
      StringPrintf(kCdataWrapper, "alert('foo');").c_str());
  ValidateExpected("valid_cdata", kHtmlInput, kHtmlOutput);
}

}  // namespace net_instaweb