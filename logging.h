#ifndef LOGGING_H_
#define LOGGING_H_

/* logging functionality */

enum cpwn_loglevel {
	CPWN_LOGLEVEL_CRITICAL,
	CPWN_LOGLEVEL_ERROR,
	CPWN_LOGLEVEL_WARNING,
	CPWN_LOGLEVEL_INFO,
	CPWN_LOGLEVEL_DEBUG,
	CPWN_LOGLEVEL__TOTAL_LEVELS
};

#endif