/*
 * =====================================================================================
 *
 *       Filename:  zombieway-client.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/12/12 14:26:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zex (zex), top_zlynch@yahoo.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <dbus/dbus.h>
#include "zombieway-basic.h"

#define  MAX_TIME_TO_WAIT 10

static dbus_bool_t working = 1;

dbus_bool_t call_zombieway_yami_hello(DBusConnection *connection)
{
	DBusMessage *message = NULL, *reply = NULL;
	char **reply_content_string;
	int fail = 0;
	DBusError error = DBUS_ERROR_INIT;

        message = dbus_message_new_method_call (ZOMBIEWAY_SERVER_NAME,
	                                   ZOMBIEWAY_PATH,
					   ZOMBIEWAY_INTERFACE_YAMI,
					   "Hello");

        reply = dbus_connection_send_with_reply_and_block(connection, message, MAX_TIME_TO_WAIT, &error);
	if (!reply) {
		error_handler(__FILE__, "Failed on dbus_connection_send_with_reply_and_block()", __LINE__, &error);
		fail = 1;
		goto hello_out;
	}
	dbus_message_get_args(reply, &error, 
			DBUS_TYPE_STRING, &reply_content_string,
			DBUS_TYPE_INVALID);

	if (dbus_error_is_set(&error)) {
		error_handler(__FILE__, "Failed on dbus_message_get_args()", __LINE__, &error);
		fail = 1;
		goto hello_out;
	}
	printf("Reply Message:%s\n", reply_content_string);

hello_out:
	if (message) dbus_message_unref(message);
	if (reply) dbus_message_unref(reply);
	dbus_error_free(&error);

	return fail ? EXIT_FAILURE : EXIT_SUCCESS;
}

dbus_bool_t speak_as_client_steven(DBusConnection *connection)
{

	DBusMessage *message = NULL;
	DBusError error = DBUS_ERROR_INIT;

	dbus_bus_request_name(connection, CLIENT_STEVEN, DBUS_NAME_FLAG_DO_NOT_QUEUE, &error);
        if (dbus_error_is_set(&error)) {
		error_handler(__FILE__, "Error on dbus_bus_request_name()", __LINE__, &error);
		dbus_error_free(&error);
		return EXIT_FAILURE;
        }

	/* Unicast as client Steven */
	message = dbus_message_new_signal(ZOMBIEWAY_PATH, ZOMBIEWAY_INTERFACE_YAMI, "Sleep5");

	dbus_message_set_destination(message, ZOMBIEWAY_SERVER_NAME);
	dbus_message_set_sender(message, CLIENT_STEVEN);

	speaker_announce(message, FALSE);

	if (!dbus_connection_send(connection, message, NULL)) {
		error_handler(__FILE__, "Failed on dbus_connection_send()", __LINE__, NULL);
		dbus_message_unref(message);
		dbus_bus_release_name(connection, CLIENT_STEVEN, NULL);
		return EXIT_FAILURE;
	}
	dbus_message_unref(message);

	/* Broadcast as client Steven */
	message = dbus_message_new_signal(CLIENT_JENNY_PATH_HOORAY, CLIENT_JENNY_INTERFACE_YOGONA, "ShowYourFace");
	
	dbus_message_set_sender(message, CLIENT_STEVEN);
	speaker_announce(message, TRUE);

	if (!dbus_connection_send(connection, message, NULL)) {
		error_handler(__FILE__, "Failed on dbus_connection_send()", __LINE__, NULL);
		dbus_message_unref(message);
		dbus_bus_release_name(connection, CLIENT_STEVEN, NULL);
		return EXIT_FAILURE;
	}

	dbus_message_unref(message);

	dbus_bus_release_name(connection, CLIENT_STEVEN, NULL);
	return EXIT_SUCCESS;
}

dbus_bool_t speak_as_client_wanderland(DBusConnection *connection)
{
	DBusMessage *message = NULL;
	DBusError error = DBUS_ERROR_INIT;

	dbus_bus_request_name(connection, CLIENT_WANDERLAND, DBUS_NAME_FLAG_DO_NOT_QUEUE, &error);
        if (dbus_error_is_set(&error)) {
		error_handler(__FILE__, "Error on dbus_bus_request_name()", __LINE__, &error);
		dbus_error_free(&error);
		return EXIT_FAILURE;
        }
	/* Unicast */
	message = dbus_message_new_signal(ZOMBIEWAY_PATH, ZOMBIEWAY_INTERFACE_YAMI, "Sleep3");

	dbus_message_set_destination(message, ZOMBIEWAY_SERVER_NAME);
	speaker_announce(message, FALSE);

	if (!dbus_connection_send(connection, message, NULL)) {
		error_handler(__FILE__, "Failed on dbus_connection()", __LINE__, NULL);
		dbus_message_unref(message);
		return EXIT_FAILURE;
	}
	dbus_message_unref(message);

	dbus_bool_t broacast(DBusConnection *connection, DBusMessage *message) {

		speaker_announce(message, TRUE);

		if (!dbus_connection_send(connection, message, NULL)) {
			error_handler(__FILE__, "Failed on dbus_connection_send()", __LINE__, NULL);
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	/* Broadcast for Quit */
	message = dbus_message_new_signal(CLIENT_JENNY_PATH_HOORAY, CLIENT_JENNY_INTERFACE_YOGONA, "Quit");
	dbus_message_set_sender(message, CLIENT_WANDERLAND);
	broacast(connection, message);
	printf("It seems no one hear me, whatever, I'll try again\n");
	dbus_message_unref(message);

	/* signal someone to add a match rule for Quit signal */
	message = dbus_message_new_signal(CLIENT_JENNY_PATH_HOORAY, CLIENT_JENNY_INTERFACE_YOGONA, "AddQuitMatch");
 	dbus_message_set_sender(message, CLIENT_WANDERLAND);
	broacast(connection, message);
	dbus_message_unref(message);
	
	/* Waiting for service to finish the previous job */
	ZOMBIEWAY_SLEEP_PROCESSER(7);

	/* Broadcast to remove match rule for ShowYourFace member */
	message = dbus_message_new_signal(CLIENT_JENNY_PATH_HOORAY, CLIENT_JENNY_INTERFACE_YOGONA, "RemoveShowFaceMatch");
	dbus_message_set_sender(message, CLIENT_WANDERLAND);
	broacast(connection, message);
	dbus_message_unref(message);

	/* Call Steven to broadcast to ShowYourFace member again, nothing shall happens :P */
	speak_as_client_steven(connection);

	/* Now, back to my job, broadcast for Quit again*/
	message = dbus_message_new_signal(CLIENT_JENNY_PATH_HOORAY, CLIENT_JENNY_INTERFACE_YOGONA, "Quit");
	dbus_message_set_sender(message, CLIENT_WANDERLAND);
	broacast(connection, message);
	dbus_message_unref(message);

	ZOMBIEWAY_SLEEP_PROCESSER(5);
	dbus_bus_release_name(connection, CLIENT_WANDERLAND, NULL);

	return EXIT_SUCCESS;
}

dbus_bool_t shutdown_zombieway(DBusConnection *connection)
{
	DBusMessage *message;

	printf("Shuting down zombieway...\n");

	message = dbus_message_new_signal(ZOMBIEWAY_PATH_WALION, ZOMBIEWAY_INTERFACE_YAMI, "Quit");

	dbus_message_set_destination(message, ZOMBIEWAY_SERVER_NAME);
	speaker_announce(message, FALSE);

	if (!dbus_connection_send(connection, message, NULL)) {
		error_handler(__FILE__, "Failed on dbus_connection_send()", __LINE__, NULL);
		dbus_message_unref(message);
		return EXIT_FAILURE;
	}
	dbus_message_unref(message);
	return EXIT_SUCCESS;
}

DBusHandlerResult interesting_message_handler(
	DBusConnection *connection, DBusMessage *message, void *user_data)
{
//	DBusError error = DBUS_ERROR_INIT;

	zombieway_print_message_info(message);

	if (dbus_message_is_signal(message, ZOMBIEWAY_INTERFACE_GEEDOSY, "JennyIsOff")) {
		printf("Jenny is off\n");
		working = 0;
		return DBUS_HANDLER_RESULT_HANDLED;
	} else if (dbus_message_is_signal(message, DBUS_INTERFACE_DBUS, "NameAcquired")) {
		char** msg_content;
		dbus_message_get_args(message, NULL, 
			DBUS_TYPE_STRING, &msg_content,
			DBUS_TYPE_INVALID);
		printf("Message from %s.%s: [%s]\n", DBUS_INTERFACE_DBUS, "NameAcquired", msg_content);
		return DBUS_HANDLER_RESULT_HANDLED;
	} else {
		error_handler(__FILE__, "Unregconized message caught", __LINE__, NULL);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
}

dbus_bool_t set_match_rule(DBusConnection *connection)
{
	DBusError error = DBUS_ERROR_INIT;

	dbus_bus_add_match(connection, SIGNAL_WANDERLAND_CARE_JENNY_IS_OFF_MR, &error);
	if (dbus_error_is_set(&error)) {
                 error_handler(__FILE__, "Error on dbus_bus_add_match()", __LINE__, &error);
		 dbus_error_free(&error);
		 return FALSE;
	}

	if (!dbus_connection_add_filter (connection, interesting_message_handler, NULL, NULL)) {
                 error_handler(__FILE__, "Error on dbus_connection_add_filter()", __LINE__, NULL);
		 dbus_error_free(&error);
		 return FALSE;
	}

	return TRUE;
}

int main(int argc, const char *argv[])
{

	DBusConnection *connection;
	DBusError error = DBUS_ERROR_INIT;

	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if (dbus_error_is_set(&error)) {
		error_handler(__FILE__, "Failed on dbus_bus_get()", __LINE__, &error);
		goto fail_and_out;
	}
	
	set_match_rule(connection);

//	while (dbus_connection_dispatch(connection) && working){
	while (dbus_connection_read_write_dispatch(connection, -1) && working){
	/* Access member Hello on server org.zombieway */
	call_zombieway_yami_hello(connection);
	/* Speak as client Steven */
	speak_as_client_steven(connection);
	/* Speak as wanderland client */
	speak_as_client_wanderland(connection);
}
//	shutdown_zombieway(connection);
	
	dbus_connection_unref(connection);

	return EXIT_SUCCESS;

fail_and_out:
	dbus_connection_unref(connection);	
	dbus_error_free(&error);

	return EXIT_FAILURE;
}
