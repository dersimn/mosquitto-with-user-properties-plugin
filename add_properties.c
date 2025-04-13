/*
Copyright (c) 2020 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.
 
The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.
 
SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
   Simon Christmann - use timestamp in unix epoch milliseconds; add client properties
*/

/*
 * Adds MQTT v5 user-properties to incoming messages:
 *  - $timestamp: unix epoch in milliseconds
 *  - $client_id: client id of the publishing client
 *  - $client_username: username that the publishing client used to authenticate
 *
 * Compile with:
 *   gcc -I<path to mosquitto-repo/include> -fPIC -shared add_properties.c -o add_properties.so
 *
 * Use in config with:
 *
 *   plugin /path/to/add_properties.so
 *
 * Note that this only works on Mosquitto 2.0 or later.
 */
#include "config.h"

#include <stdio.h>
#include <time.h>

#include "mosquitto_broker.h"
#include "mosquitto_plugin.h"
#include "mosquitto.h"
#include "mqtt_protocol.h"

#define TS_BUF_LEN (20+1)  // 20 characters in unix epoch (ns) is ≈16 Nov 5138

static mosquitto_plugin_id_t *mosq_pid = NULL;

static int callback_message(int event, void *event_data, void *userdata)
{
	struct mosquitto_evt_message *ed = event_data;

	UNUSED(event);
	UNUSED(userdata);

	int result;

	// Add timestamp in unix epoch (ms)
	struct timespec ts;
	
	char ts_buf_ms[TS_BUF_LEN];
	char ts_buf_ns[TS_BUF_LEN];
	
	clock_gettime(CLOCK_REALTIME, &ts);
	
	// Milliseconds
	snprintf(ts_buf_ms, TS_BUF_LEN, "%li%03lu", ts.tv_sec, ts.tv_nsec / 1000000);
	result = mosquitto_property_add_string_pair(
		&ed->properties,
		MQTT_PROP_USER_PROPERTY,
		"$TIMESTAMP",
		ts_buf_ms);
	if (result != MOSQ_ERR_SUCCESS) return result;

	// Nanoseconds
	snprintf(ts_buf_ns, TS_BUF_LEN, "%li%09lu", ts.tv_sec, ts.tv_nsec);
	result = mosquitto_property_add_string_pair(
		&ed->properties,
		MQTT_PROP_USER_PROPERTY,
		"$TIMESTAMP_NS",
		ts_buf_ns);
	if (result != MOSQ_ERR_SUCCESS) return result;

	// Add client id
	const char *client_id = mosquitto_client_id(ed->client);
	if (client_id && client_id[0] != '\0') {
		result = mosquitto_property_add_string_pair(
			&ed->properties,
			MQTT_PROP_USER_PROPERTY,
			"$CLIENT_ID",
			client_id);
		if (result != MOSQ_ERR_SUCCESS) return result;
	}

	// Add client username
	const char *username = mosquitto_client_username(ed->client);
	if (username && username[0] != '\0') {
		result = mosquitto_property_add_string_pair(
			&ed->properties,
			MQTT_PROP_USER_PROPERTY,
			"$CLIENT_USERNAME",
			username);
		if (result != MOSQ_ERR_SUCCESS) return result;
	}

	// If no return occurred up to this point, we were successful
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_plugin_version(int supported_version_count, const int *supported_versions)
{
	int i;

	for(i=0; i<supported_version_count; i++){
		if(supported_versions[i] == 5){
			return 5;
		}
	}
	return -1;
}

int mosquitto_plugin_init(mosquitto_plugin_id_t *identifier, void **user_data, struct mosquitto_opt *opts, int opt_count)
{
	UNUSED(user_data);
	UNUSED(opts);
	UNUSED(opt_count);

	mosq_pid = identifier;
	return mosquitto_callback_register(mosq_pid, MOSQ_EVT_MESSAGE, callback_message, NULL, NULL);
}

int mosquitto_plugin_cleanup(void *user_data, struct mosquitto_opt *opts, int opt_count)
{
	UNUSED(user_data);
	UNUSED(opts);
	UNUSED(opt_count);

	return mosquitto_callback_unregister(mosq_pid, MOSQ_EVT_MESSAGE, callback_message, NULL);
}
