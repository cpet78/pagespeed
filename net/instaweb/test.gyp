# Copyright 2010-2011 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

{
  'variables': {
    # chromium_code indicates that the code is not
    # third-party code and should be subjected to strict compiler
    # warnings/errors in order to catch programming mistakes.
    'chromium_code': 1,
  },

  'targets': [
    {
      'variables': {
        # Although OpenCV has been removed, there are still compile
        # warnings about signed and unsigned value comparison, so strict
        # checking continues to be off.
        #
        # TODO(jmarantz): disable the specific warning rather than
        # turning off all warnings, and also scope this down to a
        # minimal wrapper around the offending header file.
        #
        # TODO(jmarantz): figure out how to test for this failure in
        # checkin tests, as it passes in gcc 4.2 and fails in gcc 4.1.
        'chromium_code': 0,
      },
      'target_name': 'pagespeed_automatic_test',
      'type': 'executable',
      'dependencies': [
        'test_util',
        'instaweb.gyp:instaweb_automatic',
        'instaweb.gyp:instaweb_javascript',
        'instaweb.gyp:instaweb_spriter_test',
        'instaweb.gyp:instaweb_system',
        '<(DEPTH)/pagespeed/opt.gyp:pagespeed_ads_util',
        '<(DEPTH)/pagespeed/controller.gyp:pagespeed_grpc_test_proto',
        '<(DEPTH)/pagespeed/kernel.gyp:brotli',
        '<(DEPTH)/pagespeed/kernel.gyp:mem_lock',
        '<(DEPTH)/pagespeed/kernel.gyp:pagespeed_base_core',
        '<(DEPTH)/pagespeed/kernel.gyp:pagespeed_cache',
        '<(DEPTH)/pagespeed/kernel.gyp:pagespeed_http',
        '<(DEPTH)/pagespeed/kernel.gyp:pagespeed_image_test_util',
        '<(DEPTH)/pagespeed/kernel.gyp:pthread_system',
        '<(DEPTH)/pagespeed/kernel.gyp:pagespeed_proto_matcher_test_proto',
        '<(DEPTH)/pagespeed/kernel.gyp:redis_cache_cluster_setup_lib',
        '<(DEPTH)/pagespeed/kernel.gyp:tcp_connection_for_testing',
        '<(DEPTH)/pagespeed/kernel.gyp:tcp_server_thread_for_testing',
        '<(DEPTH)/testing/gmock.gyp:gmock',
        '<(DEPTH)/testing/gtest.gyp:gtest_main',
        '<(DEPTH)/third_party/apr/apr.gyp:apr',
        '<(DEPTH)/third_party/aprutil/aprutil.gyp:aprutil',
        '<(DEPTH)/third_party/css_parser/css_parser.gyp:css_parser',
        '<(DEPTH)/third_party/libpng/libpng.gyp:libpng',
        '<(DEPTH)/third_party/protobuf/protobuf.gyp:protobuf_message_differencer',
        '<(DEPTH)/third_party/re2/re2.gyp:re2',
      ],
      'include_dirs': [
        '<(DEPTH)/third_party/protobuf/src',
        '<(SHARED_INTERMEDIATE_DIR)/protoc_out/instaweb',
        '<(DEPTH)',
        '<(DEPTH)/third_party/css_parser/src',
        '<(DEPTH)/third_party/libwebp/src',
      ],
      'sources': [
        '<(DEPTH)/pagespeed/automatic/html_detector_test.cc',
        '<(DEPTH)/pagespeed/automatic/proxy_fetch_test.cc',
        '<(DEPTH)/pagespeed/automatic/proxy_interface_test.cc',
        '<(DEPTH)/pagespeed/automatic/proxy_interface_test_base.cc',
        # TODO(jefftk): get this test to build.
        # '<(DEPTH)/pagespeed/automatic/rewriter_speed_test.cc',
        'config/measurement_proxy_rewrite_options_manager_test.cc',
        'config/rewrite_options_manager_test.cc',
        'http/async_fetch_test.cc',
        'http/cache_url_async_fetcher_test.cc',
        'http/fetcher_test.cc',
        'http/headers_cookie_util_test.cc',
        'http/http_cache_test.cc',
        'http/http_dump_url_async_writer_test.cc',
        'http/http_dump_url_fetcher_test.cc',
        'http/http_response_parser_test.cc',
        'http/http_value_test.cc',
        'http/inflating_fetch_test.cc',
        'http/mock_url_fetcher_test.cc',
        'http/rate_controlling_url_async_fetcher_test.cc',
        'http/redirect_following_url_async_fetcher_test.cc',
        'http/reflecting_test_fetcher_test.cc',
        # SimulatedDelayFetcher isn't currently used in {mod_,ngx_}pagespeed,
        # so we only build it into the test binary.
        'http/simulated_delay_fetcher.cc',
        'http/simulated_delay_fetcher_test.cc',
        'http/sync_fetcher_adapter_test.cc',
        'http/ua_sensitive_test_fetcher.cc',
        'http/ua_sensitive_test_fetcher_test.cc',
        'http/url_async_fetcher_stats_test.cc',
        'http/wait_url_async_fetcher_test.cc',
        'rewriter/add_ids_filter_test.cc',
        'rewriter/add_instrumentation_filter_test.cc',
        'rewriter/association_transformer_test.cc',
        'rewriter/base_tag_filter_test.cc',
        'rewriter/beacon_critical_images_finder_test.cc',
        'rewriter/cache_extender_test.cc',
        'rewriter/cacheable_resource_base_test.cc',
        'rewriter/collect_dependencies_filter_test.cc',
        'rewriter/common_filter_test.cc',
        'rewriter/critical_css_beacon_filter_test.cc',
        'rewriter/critical_finder_support_util_test.cc',
        'rewriter/critical_images_beacon_filter_test.cc',
        'rewriter/critical_images_finder_test.cc',
        'rewriter/critical_images_finder_test_base.cc',
        'rewriter/critical_selector_filter_test.cc',
        'rewriter/critical_selector_finder_test.cc',
        'rewriter/css_combine_filter_test.cc',
        'rewriter/css_embedded_config_test.cc',
        'rewriter/css_filter_test.cc',
        'rewriter/css_flatten_imports_test.cc',
        'rewriter/css_hierarchy_test.cc',
        'rewriter/css_image_rewriter_test.cc',
        'rewriter/css_inline_filter_test.cc',
        'rewriter/css_inline_import_to_link_filter_test.cc',
        'rewriter/css_minify_test.cc',
        'rewriter/css_move_to_head_filter_test.cc',
        'rewriter/css_outline_filter_test.cc',
        'rewriter/css_rewrite_test_base.cc',
        'rewriter/css_summarizer_base_test.cc',
        'rewriter/css_tag_scanner_test.cc',
        'rewriter/css_url_encoder_test.cc',
        'rewriter/css_util_test.cc',
        'rewriter/debug_filter_test.cc',
        'rewriter/decode_rewritten_urls_filter_test.cc',
        'rewriter/dedup_inlined_images_filter_test.cc',
        'rewriter/defer_iframe_filter_test.cc',
        'rewriter/delay_images_filter_test.cc',
        'rewriter/dependency_tracker_test.cc',
        'rewriter/deterministic_js_filter_test.cc',
        'rewriter/device_properties_test.cc',
        'rewriter/dom_stats_filter_test.cc',
        'rewriter/domain_lawyer_test.cc',
        'rewriter/domain_rewrite_filter_test.cc',
        'rewriter/downstream_cache_purger_test.cc',
        'rewriter/downstream_caching_directives_test.cc',
        'rewriter/experiment_matcher_test.cc',
        'rewriter/experiment_util_test.cc',
        'rewriter/file_load_policy_test.cc',
        'rewriter/fix_reflow_filter_test.cc',
        'rewriter/flush_html_filter_test.cc',
        'rewriter/google_analytics_filter_test.cc',
        'rewriter/google_font_css_inline_filter_test.cc',
        'rewriter/google_font_service_input_resource_test.cc',
        'rewriter/handle_noscript_redirect_filter_test.cc',
        'rewriter/image_combine_filter_test.cc',
        'rewriter/image_endian_test.cc',
        'rewriter/image_rewrite_filter_test.cc',
        'rewriter/image_test.cc',
        'rewriter/image_test_base.cc',
        'rewriter/image_url_encoder_test.cc',
        'rewriter/in_place_rewrite_context_test.cc',
        'rewriter/insert_amp_link_filter_test.cc',
        'rewriter/insert_dns_prefetch_filter_test.cc',
        'rewriter/insert_ga_filter_test.cc',
        'rewriter/javascript_code_block_test.cc',
        'rewriter/javascript_filter_test.cc',
        'rewriter/js_combine_filter_test.cc',
        'rewriter/js_defer_disabled_filter_test.cc',
        'rewriter/js_disable_filter_test.cc',
        'rewriter/js_inline_filter_test.cc',
        'rewriter/js_outline_filter_test.cc',
        'rewriter/js_replacer_test.cc',
        'rewriter/lazyload_images_filter_test.cc',
        'rewriter/local_storage_cache_filter_test.cc',
        'rewriter/make_show_ads_async_filter_test.cc',
        'rewriter/measurement_proxy_url_namer_test.cc',
        'rewriter/meta_tag_filter_test.cc',
        'rewriter/mock_critical_images_finder.cc',
        'rewriter/mock_resource_callback.cc',
        'rewriter/pedantic_filter_test.cc',
        'rewriter/property_cache_util_test.cc',
        'rewriter/push_preload_filter_test.cc',
        'rewriter/redirect_on_size_limit_filter_test.cc',
        'rewriter/render_blocking_html_computation_test.cc',
        'rewriter/request_properties_test.cc',
        'rewriter/resource_combiner_test.cc',
        'rewriter/resource_fetch_test.cc',
        'rewriter/resource_namer_test.cc',
        'rewriter/resource_slot_test.cc',
        'rewriter/resource_tag_scanner_test.cc',
        'rewriter/resource_update_test.cc',
        'rewriter/responsive_image_filter_test.cc',
        'rewriter/rewrite_context_test.cc',
        'rewriter/rewrite_context_test_base.cc',
        'rewriter/rewrite_driver_test.cc',
        'rewriter/rewrite_options_test.cc',
        'rewriter/rewrite_query_test.cc',
        'rewriter/rewrite_single_resource_filter_test.cc',
        'rewriter/rewrite_test_base.cc',
        'rewriter/rewriter_test.cc',
        'rewriter/rewritten_content_scanning_filter_test.cc',
        'rewriter/scan_filter_test.cc',
        'rewriter/script_tag_scanner_test.cc',
        'rewriter/server_context_test.cc',
        'rewriter/shared_cache_test.cc',
        'rewriter/srcset_slot_test.cc',
        'rewriter/static_asserts_test.cc',
        'rewriter/static_asset_manager_test.cc',
        'rewriter/strip_scripts_filter_test.cc',
        'rewriter/strip_subresource_hints_filter_test.cc',
        'rewriter/support_noscript_filter_test.cc',
        'rewriter/two_level_cache_test.cc',
        'rewriter/url_input_resource_test.cc',
        'rewriter/url_left_trim_filter_test.cc',
        'rewriter/url_namer_test.cc',
        'rewriter/url_partnership_test.cc',
        'rewriter/webp_optimizer_test.cc',
        'spriter/image_spriter_test.cc',
        'spriter/libpng_image_library_test.cc',
        '<(DEPTH)/pagespeed/system/apr_mem_cache_test.cc',
        '<(DEPTH)/pagespeed/system/redis_cache_test.cc',
        '<(DEPTH)/pagespeed/system/redis_cache_cluster_test.cc',
        '<(DEPTH)/pagespeed/system/admin_site_test.cc',
        '<(DEPTH)/pagespeed/system/system_message_handler_test.cc',
        '<(DEPTH)/pagespeed/controller/central_controller_callback_test.cc',
        '<(DEPTH)/pagespeed/controller/context_registry_test.cc',
        '<(DEPTH)/pagespeed/controller/expensive_operation_rpc_context_test.cc',
        '<(DEPTH)/pagespeed/controller/expensive_operation_rpc_handler_test.cc',
        '<(DEPTH)/pagespeed/controller/grpc_server_test.cc',
        '<(DEPTH)/pagespeed/controller/named_lock_schedule_rewrite_controller_test.cc',
        '<(DEPTH)/pagespeed/controller/popularity_contest_schedule_rewrite_controller_test.cc',
        '<(DEPTH)/pagespeed/controller/priority_queue_test.cc',
        '<(DEPTH)/pagespeed/controller/rpc_handler_test.cc',
        '<(DEPTH)/pagespeed/controller/schedule_rewrite_rpc_context_test.cc',
        '<(DEPTH)/pagespeed/controller/schedule_rewrite_rpc_handler_test.cc',
        '<(DEPTH)/pagespeed/controller/queued_expensive_operation_controller_test.cc',
        '<(DEPTH)/pagespeed/controller/work_bound_expensive_operation_controller_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/annotated_message_handler_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/arena_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/base64_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/callback_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/charset_util_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/chunking_writer_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/circular_buffer_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/countdown_timer_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/escaping_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/fast_wildcard_group_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/function_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/hasher_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/hostname_util_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/inline_slist_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/md5_hasher_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/mem_debug.cc',
        '<(DEPTH)/pagespeed/kernel/base/mem_file_system_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/message_handler_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/mock_message_handler_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/mock_timer_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/null_statistics_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/pool_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/proto_matcher_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/ref_counted_ptr_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/sha1_signature_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/shared_string_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/source_map_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/split_statistics_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/split_writer_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/stdio_file_system_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/string_multi_map_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/string_util_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/symbol_table_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/time_util_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/vector_deque_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/waveform_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/wildcard_group.cc',
        '<(DEPTH)/pagespeed/kernel/base/wildcard_group_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/wildcard_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/async_cache_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/cache_batcher_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/cache_key_prepender.cc',
        '<(DEPTH)/pagespeed/kernel/cache/cache_key_prepender_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/cache_stats_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/compressed_cache_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/delay_cache_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/fallback_cache_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/file_cache_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/in_memory_cache_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/key_value_codec_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/lru_cache_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/mock_time_cache_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/purge_context_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/purge_set_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/threadsafe_cache_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/write_through_cache_test.cc',
        '<(DEPTH)/pagespeed/kernel/html/amp_document_filter_test.cc',
        '<(DEPTH)/pagespeed/kernel/html/canonical_attributes_test.cc',
        '<(DEPTH)/pagespeed/kernel/html/collapse_whitespace_filter_test.cc',
        '<(DEPTH)/pagespeed/kernel/html/doctype_test.cc',
        '<(DEPTH)/pagespeed/kernel/html/elide_attributes_filter_test.cc',
        '<(DEPTH)/pagespeed/kernel/html/html_attribute_quote_removal_test.cc',
        '<(DEPTH)/pagespeed/kernel/html/html_keywords_test.cc',
        '<(DEPTH)/pagespeed/kernel/html/html_name_test.cc',
        '<(DEPTH)/pagespeed/kernel/html/html_parse_test.cc',
        '<(DEPTH)/pagespeed/kernel/html/remove_comments_filter_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/bot_checker_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/caching_headers_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/content_type_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/data_url_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/domain_registry_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/google_url_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/query_params_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/request_headers_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/response_headers_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/semantic_type_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/user_agent_matcher_test.cc',
        '<(DEPTH)/pagespeed/kernel/http/user_agent_matcher_test_base.cc',
        '<(DEPTH)/pagespeed/kernel/http/user_agent_normalizer_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/frame_interface_integration_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/frame_interface_optimizer_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/gif_reader_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/image_analysis_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/image_converter_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/image_optimizer_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/image_resizer_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/image_util_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/jpeg_optimizer_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/jpeg_reader_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/jpeg_utils_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/pixel_format_optimizer_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/png_optimizer_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/scanline_interface_frame_adapter_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/scanline_status_test.cc',
        '<(DEPTH)/pagespeed/kernel/image/webp_optimizer_test.cc',
        '<(DEPTH)/pagespeed/kernel/js/js_minify_test.cc',
        '<(DEPTH)/pagespeed/kernel/js/js_tokenizer_test.cc',
        '<(DEPTH)/pagespeed/kernel/sharedmem/inprocess_shared_mem_test.cc',
        '<(DEPTH)/pagespeed/kernel/sharedmem/shared_mem_cache_spammer_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/mock_scheduler_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/pthread_condvar_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/pthread_thread_system_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/queued_alarm_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/queued_worker_pool_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/queued_worker_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/scheduler_based_abstract_lock_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/scheduler_sequence_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/scheduler_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/scheduler_thread_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/slow_worker_test.cc',
        '<(DEPTH)/pagespeed/kernel/thread/thread_synchronizer_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/brotli_inflater_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/categorized_refcount_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/copy_on_write_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/file_system_lock_manager_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/gzip_inflater_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/hashed_nonce_generator_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/input_file_nonce_generator_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/mem_lock_manager_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/nonce_generator_test_base.cc',
        '<(DEPTH)/pagespeed/kernel/util/re2_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/simple_stats_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/statistics_logger_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/threadsafe_lock_manager_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/url_escaper_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/url_multipart_encoder_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/url_to_filename_encoder_test.cc',
        '<(DEPTH)/pagespeed/opt/ads/ads_attribute_test.cc',
        '<(DEPTH)/pagespeed/opt/ads/ads_util_test.cc',
        '<(DEPTH)/pagespeed/opt/ads/show_ads_snippet_parser_test.cc',
        '<(DEPTH)/pagespeed/opt/logging/log_record_test.cc',
        '<(DEPTH)/pagespeed/opt/logging/log_record_test_helper.cc',
        '<(DEPTH)/pagespeed/opt/logging/request_timing_info_test.cc',
        '<(DEPTH)/pagespeed/opt/http/cache_property_store_test.cc',
        '<(DEPTH)/pagespeed/opt/http/fallback_property_page_test.cc',
        '<(DEPTH)/pagespeed/opt/http/mock_property_page.cc',
        '<(DEPTH)/pagespeed/opt/http/property_cache_test.cc',
        '<(DEPTH)/pagespeed/opt/http/property_store_test.cc',
        '<(DEPTH)/pagespeed/opt/http/request_context_test.cc',
        '<(DEPTH)/pagespeed/opt/http/two_level_property_store_test.cc',


# Rolling hash test fails to build in 32-bit g++ 4.1
#        '<(DEPTH)/pagespeed/kernel/base/rolling_hash_test.cc',
#        'util/split_writer_test.cc',               # not currently needed
      ],
      'conditions': [
        ['support_posix_shared_mem != 1', {
          'sources!' : [
            '<(DEPTH)/pagespeed/kernel/sharedmem/pthread_shared_mem_test.cc',
          ],
        }]
      ],
    },
    {
      'variables': {
        # Although OpenCV has been removed, there are still compile
        # warnings about signed and unsigned value comparison, so strict
        # checking continues to be off.
        #
        # TODO(jmarantz): disable the specific warning rather than
        # turning off all warnings, and also scope this down to a
        # minimal wrapper around the offending header file.
        #
        # TODO(jmarantz): figure out how to test for this failure in
        # checkin tests, as it passes in gcc 4.2 and fails in gcc 4.1.
        'chromium_code': 0,
      },
      'target_name': 'mod_pagespeed_test',
      'type': 'executable',
      'dependencies': [
        'test_util',
        'instaweb_apr.gyp:instaweb_apr',
        '<(DEPTH)/testing/gmock.gyp:gmock',
        '<(DEPTH)/testing/gtest.gyp:gtest_main',
        '<(DEPTH)/third_party/apr/apr.gyp:apr',
        '<(DEPTH)/third_party/aprutil/aprutil.gyp:aprutil',
        '<(DEPTH)/third_party/httpd/httpd.gyp:include',
        '<(DEPTH)/pagespeed/kernel.gyp:tcp_server_thread_for_testing',
      ],
      'include_dirs': [
        '<(DEPTH)/third_party/protobuf/src',
        '<(SHARED_INTERMEDIATE_DIR)/protoc_out/instaweb',
        '<(DEPTH)',
      ],
      'sources': [
        '<(DEPTH)/pagespeed/apache/apache_config_test.cc',
        '<(DEPTH)/pagespeed/apache/apache_fetch_test.cc',
        '<(DEPTH)/pagespeed/apache/apache_writer.cc',
        '<(DEPTH)/pagespeed/apache/apache_writer_test.cc',
        # header_util.cc is dependent on the version of httpd, so it
        # is not included in 'instaweb_apr' which is httpd-version independent.
        # Note that the unit tests are only run against Apache 2.2.  In module
        # builds it is pulled in mod_pagespeed.gypi. FIXME
        '<(DEPTH)/pagespeed/apache/header_util.cc',
        '<(DEPTH)/pagespeed/apache/header_util_test.cc',
        '<(DEPTH)/pagespeed/apache/mock_apache.cc',
        '<(DEPTH)/pagespeed/apache/simple_buffered_apache_fetch_test.cc',
        '<(DEPTH)/pagespeed/system/add_headers_fetcher_test.cc',
        '<(DEPTH)/pagespeed/system/external_server_spec_test.cc',
        '<(DEPTH)/pagespeed/system/in_place_resource_recorder_test.cc',
        '<(DEPTH)/pagespeed/system/loopback_route_fetcher_test.cc',
        '<(DEPTH)/pagespeed/system/serf_url_async_fetcher_test.cc',
        '<(DEPTH)/pagespeed/system/system_caches_test.cc',
        '<(DEPTH)/pagespeed/system/system_rewrite_options_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/mem_debug.cc',
      ],
    },
    {
      'target_name': 'test_infrastructure',
      'type': '<(library)',
      'dependencies': [
        'instaweb.gyp:instaweb_rewriter',
        'instaweb.gyp:instaweb_http_test',
        'instaweb.gyp:process_context',
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/build/build_util.gyp:mod_pagespeed_version_header',
        '<(DEPTH)/pagespeed/kernel.gyp:pagespeed_base_test_infrastructure',
        '<(DEPTH)/pagespeed/kernel.gyp:util_gflags',
      ],
      'include_dirs': [
        '<(DEPTH)/third_party/protobuf/src',
        '<(SHARED_INTERMEDIATE_DIR)/protoc_out/instaweb',
        '<(DEPTH)',
      ],
      'sources': [
        'rewriter/css_url_extractor.cc',
      ],
    },
    {
      'target_name': 'test_util',
      'type': '<(library)',
      'dependencies': [
        # OpenSSL is transitively included by various deps below, as is
        # ICU. However, system ICU puts -L/usr/lib into LDFLAGS. If we want
        # to use non-system OpenSSL, we need to make sure the openssl GYP
        # file is "seen" first.
        '<(DEPTH)/third_party/serf/select_openssl.gyp:select_openssl',
        'test_infrastructure',
        '<(DEPTH)/testing/gmock.gyp:gmock',
        '<(DEPTH)/pagespeed/kernel.gyp:kernel_test_util',
      ],
      'include_dirs': [
        '<(DEPTH)/third_party/protobuf/src',
        '<(DEPTH)',
      ],
      'sources': [
        '<(DEPTH)/pagespeed/kernel/html/html_parse_test_base.cc',
        'http/mock_url_fetcher.cc',
        'rewriter/fake_filter.cc',
        'rewriter/notifying_fetch.cc',
        'rewriter/rewrite_test_base.cc',
        'rewriter/test_rewrite_driver_factory.cc',
        'rewriter/test_url_namer.cc',
      ],
    },
    {
      'target_name': 'mod_pagespeed_speed_test',
      'type': 'executable',
      'dependencies': [
        'test_util',
        '<(DEPTH)/net/instaweb/instaweb.gyp:instaweb_console_css_data2c',
        '<(DEPTH)/net/instaweb/instaweb.gyp:instaweb_console_js_data2c',
        '<(DEPTH)/pagespeed/kernel.gyp:pthread_system',
        '<(DEPTH)/pagespeed/kernel.gyp:pagespeed_base_core',
        '<(DEPTH)/pagespeed/kernel.gyp:pagespeed_http',
        '<(DEPTH)/pagespeed/kernel.gyp:proto_util',
        '<(DEPTH)/third_party/css_parser/css_parser.gyp:css_parser',
        '<(DEPTH)/third_party/re2/re2.gyp:re2_bench_util',
      ],
      'include_dirs': [
        '<(DEPTH)',
        '<(DEPTH)/third_party/css_parser/src',
      ],
      'sources': [
        'rewriter/css_minify_speed_test.cc',
        'rewriter/domain_lawyer_speed_test.cc',
        'rewriter/image_speed_test.cc',
        'rewriter/javascript_minify_speed_test.cc',
        'rewriter/rewrite_driver_speed_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/fast_wildcard_group_speed_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/file_system_speed_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/string_multi_map_speed_test.cc',
        '<(DEPTH)/pagespeed/kernel/base/wildcard_group.cc',
        '<(DEPTH)/pagespeed/kernel/cache/compressed_cache_speed_test.cc',
        '<(DEPTH)/pagespeed/kernel/cache/lru_cache_speed_test.cc',
        '<(DEPTH)/pagespeed/kernel/html/html_parse_speed_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/deque_speed_test.cc',
        '<(DEPTH)/pagespeed/kernel/util/url_escaper_speed_test.cc',
      ],
    },
    {
      'target_name': 'css_minify_main',
      'type': 'executable',
      'sources': [
        'rewriter/css_minify_main.cc',
      ],
      'dependencies': [
        'instaweb.gyp:automatic_util',
        'instaweb.gyp:instaweb_rewriter',
        'instaweb.gyp:instaweb_rewriter_css',
        'instaweb.gyp:instaweb_util',
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/pagespeed/kernel.gyp:pthread_system',
        '<(DEPTH)/pagespeed/kernel.gyp:util_gflags',
        '<(DEPTH)/third_party/css_parser/css_parser.gyp:css_parser',
      ],
      'include_dirs': [
        '<(DEPTH)',
        '<(DEPTH)/third_party/css_parser/src',
      ],
    },
  ],
}
