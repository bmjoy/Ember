/*
 * Copyright (c) 2015 Ember
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "LogConfig.h"
#include <logger/ConsoleSink.h>
#include <logger/FileSink.h>
#include <logger/SyslogSink.h>
#include <logger/Utility.h>
#include <string>
#include <stdexcept>
#include <utility>

namespace el = ember::log;
namespace po = boost::program_options;

namespace ember { namespace util {

namespace {

std::unique_ptr<el::Sink> init_remote_sink(const po::variables_map& args, el::Severity severity,
                                           el::Filter filter) {
	auto host = args["remote_log.host"].as<std::string>();
	auto service = args["remote_log.service_name"].as<std::string>();
	auto port = args["remote_log.port"].as<unsigned short>();
	auto facility = el::SyslogSink::Facility::LOCAL_USE_0;
	return std::make_unique<el::SyslogSink>(severity, filter, host, port, facility, service);
}

std::unique_ptr<el::Sink> init_file_sink(const po::variables_map& args, el::Severity severity,
                                         el::Filter filter) {
	auto mode_str = args["file_log.mode"].as<std::string>();
	auto path = args["file_log.path"].as<std::string>();

	if(mode_str != "append" && mode_str != "truncate") {
		throw std::runtime_error("Invalid file logging mode supplied");
	}

	el::FileSink::Mode mode = (mode_str == "append")? el::FileSink::Mode::APPEND :
	                                                  el::FileSink::Mode::TRUNCATE;

	auto file_sink = std::make_unique<el::FileSink>(severity, filter, path, mode);
	file_sink->size_limit( args["file_log.size_rotate"].as<std::uint32_t>());
	file_sink->log_severity(args["file_log.log_severity"].as<bool>());
	file_sink->log_date(args["file_log.log_timestamp"].as<bool>());
	file_sink->time_format(args["file_log.timestamp_format"].as<std::string>());
	file_sink->midnight_rotate(args["file_log.midnight_rotate"].as<bool>());
	return std::move(file_sink);
}

std::unique_ptr<el::Sink> init_console_sink(const po::variables_map& args, el::Severity severity,
                                            el::Filter filter) {
	return std::make_unique<el::ConsoleSink>(severity, filter);
}

} // unnamed

std::unique_ptr<ember::log::Logger> init_logging(const po::variables_map& args) {
	auto logger = std::make_unique<el::Logger>();
	el::Severity severity;
	el::Filter filter(-1);

	if((severity = el::severity_string(args["console_log.verbosity"].as<std::string>()))
		!= el::Severity::DISABLED) {
		logger->add_sink(init_console_sink(args, severity, filter));
	}

	if((severity = el::severity_string(args["file_log.verbosity"].as<std::string>()))
		!= el::Severity::DISABLED) {
		logger->add_sink(init_file_sink(args, severity, filter));
	}

	if((severity = el::severity_string(args["remote_log.verbosity"].as<std::string>()))
		!= el::Severity::DISABLED) {
		logger->add_sink(init_remote_sink(args, severity, filter));
	}

	return logger;
}

}} // util, ember