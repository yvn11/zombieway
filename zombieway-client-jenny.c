/*
 * =====================================================================================
 *
 *       Filename:  zombieway-client-jenny.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/12/12 11:07:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zex (zex), top_zlynch@yahoo.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "zombieway-basic.h"

static int online = 0;

DBusHandlerResult jenny_message_handler(DBusConnection *connection, DBusMessage *message, void *user_data)
{
  zombieway_print_message_info(message);

	if (dbus_message_is_signal(message, CLIENT_JENNY_INTERFACE_YOGONA, "ShowYourFace")) {
		printf("Showing my face\n");
		return DBUS_HANDLER_RESULT_HANDLED;
	} else if (dbus_message_is_signal(message, CLIENT_JENNY_INTERFACE_YOGONA, "Quit")) {
		online = 0;
		printf("Bye-bye!\n");

		DBusMessage *message = dbus_message_new_signal(ZOMBIEWAY_PATH_HOLLY, ZOMBIEWAY_INTERFACE_GEEDOSY, "JennyIsOff");
		dbus_message_set_sender(message, CLIENT_JENNY);
		speaker_announce(message, TRUE);

		if (!dbus_connection_send(connection, message, NULL)) {
			error_handler(__FILE__, "Failed on dbus_connection_send()", __LINE__, NULL);
			dbus_message_unref(message);
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
		}
		dbus_message_unref(message);

		return DBUS_HANDLER_RESULT_HANDLED;
	} else if (dbus_message_is_signal(message, CLIENT_JENNY_INTERFACE_YOGONA, "AddQuitMatch")) {
		DBusError error = DBUS_ERROR_INIT;

		dbus_bus_add_match(connection, SIGNAL_JENNY_CARE_QUIT_MR, &error);
		if (dbus_error_is_set(&error)) {
                	error_handler(__FILE__, "Error on dbus_bus_add_match()", __LINE__, &error);
			dbus_error_free(&error);
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
		}
	
		if (!dbus_connection_add_filter (connection, jenny_message_handler, NULL, NULL)) {
                	error_handler(__FILE__, "Error on dbus_connection_add_filter()", __LINE__, &error);
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
		}
		printf("Added match rule of *Quit* member...\n%s\n", SIGNAL_JENNY_CARE_QUIT_MR);
		return DBUS_HANDLER_RESULT_HANDLED;
	} else if (dbus_message_is_signal(message, CLIENT_JENNY_INTERFACE_YOGONA, "RemoveShowYourFaceMatch")) {
		DBusError error = DBUS_ERROR_INIT;

		dbus_bus_remove_match(connection, SIGNAL_JENNY_CARE_SHOW_FACE_MR, &error);
		if (dbus_error_is_set(&error)) {
                	error_handler(__FILE__, "Error on dbus_bus_add_match()", __LINE__, &error);
			dbus_error_free(&error);
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
		}
	
		printf("Removed match rule of *ShowYourFace* member...\n%s\n", SIGNAL_JENNY_CARE_SHOW_FACE_MR);
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

int main(int argc, const char *argv[])
{
	DBusMessage *message = NULL, *reply = NULL;
	DBusConnection *connection;
	DBusError error = DBUS_ERROR_INIT;

	online = 1;

	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if (dbus_error_is_set(&error)) {
		error_handler(__FILE__, "Failed on dbus_bus_get()", __LINE__, &error);
		goto fail_and_out;
	}

	dbus_bus_request_name(connection, CLIENT_JENNY, DBUS_NAME_FLAG_DO_NOT_QUEUE, &error);
        if (dbus_error_is_set(&error)) {
		error_handler(__FILE__, "Error on dbus_bus_request_name()", __LINE__, &error);
		return EXIT_FAILURE;
        }

	dbus_bus_add_match(connection, SIGNAL_JENNY_CARE_SHOW_FACE_MR, &error);
	if (dbus_error_is_set(&error)) {
                 error_handler(__FILE__, "Error on dbus_bus_add_match()", __LINE__, &error);
		 goto fail_and_out;
	}

	dbus_bus_add_match(connection, SIGNAL_JENNY_CARE_ADD_QUIT_MR, &error);
	if (dbus_error_is_set(&error)) {
                 error_handler(__FILE__, "Error on dbus_bus_add_match()", __LINE__, &error);
		 goto fail_and_out;
	}

	dbus_bus_add_match(connection, SIGNAL_JENNY_CARE_REMOVE_SHOW_FACE_MR, &error);
	if (dbus_error_is_set(&error)) {
                 error_handler(__FILE__, "Error on dbus_bus_add_match()", __LINE__, &error);
		 goto fail_and_out;
	}

	if (!dbus_connection_add_filter (connection, jenny_message_handler, NULL, NULL)) {
                 error_handler(__FILE__, "Error on dbus_connection_add_filter()", __LINE__, NULL);
		 goto fail_and_out;
	}

	printf("Jenny is %s line\n", online ? "on" : "off");
	while(dbus_connection_read_write_dispatch(connection, -1) && online);

	printf("Jenny is %s line\n", online ? "on" : "off");
	dbus_bus_release_name(connection, CLIENT_JENNY, NULL);
	dbus_connection_unref(connection);
	dbus_shutdown();
	return EXIT_SUCCESS;

fail_and_out:
	if (message)
		dbus_message_unref(message);
	if (reply)
		dbus_message_unref(reply);
	if (connection)
		dbus_connection_unref(connection);
	dbus_error_free(&error);

	return EXIT_FAILURE;
}
