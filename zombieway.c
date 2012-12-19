/*
 * =====================================================================================
 *
 *       Filename:  zombieway.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/11/12 14:01:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zex (zex), top_zlynch@yahoo.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "zombieway-basic.h"

static int working = 0;

DBusHandlerResult zombieway_hello(DBusConnection *connection, DBusMessage *message)
{
  DBusError error = DBUS_ERROR_INIT;
	DBusMessage *reply;
	char* reply_content = (char*)dbus_malloc(256);

	handler_speakup(__func__);

	reply = dbus_message_new_method_return(message);
	if (!reply){
		error_handler(__FILE__, "Failed on dbus_message_new_method_call()", __LINE__, &error);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	sprintf(reply_content, "Dear sender [%s]:\n\tHello there!\n"
		"\tIt's our pleasure to hear from you :D\n"
		"\tLooking forward to receive messages from you again!\n"
		"\tYours sincerely.\n", 
		dbus_message_get_sender(message));

	if (!dbus_message_append_args (reply,
                            DBUS_TYPE_STRING, &reply_content,
                            DBUS_TYPE_INVALID)) {
		error_handler(__FILE__, "Failed on dbus_message_append_args()", __LINE__, &error);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	if (!dbus_connection_send(connection, reply, NULL)){ 
		error_handler(__FILE__, "Failed on dbus_connection_send()", __LINE__, &error);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	dbus_message_unref(reply);
	dbus_error_free(&error);
	dbus_free(reply_content);
	handler_shutup(__func__);

	return DBUS_HANDLER_RESULT_HANDLED;
}

dbus_bool_t introspect_init(IntrospectString* xml)
{
  xml->str = (char*)dbus_malloc(STRING_ALLOCATION_PADDING);
  if (!xml->str) return FALSE;  
  
  xml->allocated = STRING_ALLOCATION_PADDING;
  xml->len = 0;
  xml->str[xml->len] = '\0';
  
//  fixup_alignment (xml);
  
  return TRUE;
}

dbus_bool_t introspect_free(IntrospectString* xml)
{
	if (!xml->str) return FALSE;
	if (!xml->allocated) return FALSE;

	dbus_free(xml->str);
	xml->str = NULL;
	xml->allocated = 0;
	xml->len = 0;

	return TRUE;
}

dbus_bool_t introspect_string_append(IntrospectString* xml, const char* new_str)
{
	if (!new_str) return FALSE;

	char* old_str = xml->str;
	size_t nsize = 0;

	if (xml->len) {
		nsize = strlen(old_str)+strlen(new_str)+1;
		char *tmpstr =  malloc(nsize);
		xml->str = strcat(strcpy(tmpstr, old_str), new_str);
		xml->allocated = nsize;
		xml->len = nsize;
		free(old_str);
	} else {
		nsize = strlen(new_str)+1;
		char* tmpstr = malloc(nsize);
		xml->str = strcpy(tmpstr, new_str);
		xml->allocated = nsize;
		xml->len = nsize;
		free(old_str);
	}

	return TRUE;
}

DBusHandlerResult zombieway_introspect(DBusConnection *connection, DBusMessage *message)
{	
	handler_speakup(__func__);
	IntrospectString xml;
	DBusMessage *reply;
	DBusError error = DBUS_ERROR_INIT;
	char** children;
	char* tstr = dbus_malloc(100);
	int i;

	reply = NULL;

	if (! dbus_message_get_args (message, &error,
			       DBUS_TYPE_INVALID)) {
	    if (dbus_error_is_set(&error)) 
	    	error_handler(__FILE__, "Failed on dbus_message_get_args()", __LINE__, &error);
	    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	if (!introspect_init(&xml)) {
		error_handler(__FILE__, "Failed on introspect_init()", __LINE__, &error);
		goto oom;
	}
  
	if (!dbus_connection_list_registered (connection, 
             dbus_message_get_path (message), &children)) {
		error_handler(__FILE__, "Failed on dbus_connection_list_registered()", __LINE__, &error);
		goto oom;
	}

	introspect_string_append(&xml, DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE);
	introspect_string_append(&xml, "<node>\n");
	sprintf(tstr, "  <interface name=\"%s\">\n", DBUS_INTERFACE_INTROSPECTABLE);
	introspect_string_append(&xml, tstr);
	introspect_string_append(&xml, "    <method name=\"Introspect\">\n");
	sprintf(tstr, "      <arg name=\"data\" direction=\"out\" type=\"%s\"/>\n", DBUS_TYPE_STRING_AS_STRING);
	introspect_string_append(&xml, tstr);
	introspect_string_append(&xml, "    </method>\n");
	introspect_string_append(&xml, "  </interface>\n");

	sprintf(tstr, "  <interface name=\"%s\">\n", DBUS_INTERFACE_PROPERTIES);
	introspect_string_append(&xml, tstr);
	introspect_string_append(&xml, "    <method name=\"Get\">\n");
	sprintf(tstr, "      <arg name=\"interface\" direction=\"in\" type=\"%s\"/>\n", DBUS_TYPE_STRING_AS_STRING);
	introspect_string_append(&xml, tstr);
	sprintf(tstr, "      <arg name=\"propname\" direction=\"in\" type=\"%s\"/>\n", DBUS_TYPE_STRING_AS_STRING);
	introspect_string_append(&xml, tstr);
	sprintf(tstr, "      <arg name=\"value\" direction=\"out\" type=\"%s\"/>\n", DBUS_TYPE_VARIANT_AS_STRING);
	introspect_string_append(&xml, tstr);
	introspect_string_append(&xml, "    </method>\n");
	introspect_string_append(&xml, "    <method name=\"Set\">\n");
	sprintf(tstr, "      <arg name=\"interface\" direction=\"in\" type=\"%s\"/>\n", DBUS_TYPE_STRING_AS_STRING);
	introspect_string_append(&xml, tstr);
	sprintf(tstr, "      <arg name=\"propname\" direction=\"in\" type=\"%s\"/>\n", DBUS_TYPE_STRING_AS_STRING);
	introspect_string_append(&xml, tstr);
	sprintf(tstr, "      <arg name=\"value\" direction=\"in\" type=\"%s\"/>\n", DBUS_TYPE_VARIANT_AS_STRING);
	introspect_string_append(&xml, tstr);
	introspect_string_append(&xml, "    </method>\n");
	introspect_string_append(&xml, "    <method name=\"GetAll\">\n");
	sprintf(tstr, "      <arg name=\"interface\" direction=\"in\" type=\"%s\"/>\n", DBUS_TYPE_STRING_AS_STRING);
	introspect_string_append(&xml, tstr);
	sprintf(tstr, "      <arg name=\"props\" direction=\"out\" type=\"%s\"/>\n",
	                        DBUS_TYPE_ARRAY_AS_STRING 
	                        DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING 
	                          DBUS_TYPE_STRING_AS_STRING 
	                          DBUS_TYPE_VARIANT_AS_STRING 
	                        DBUS_DICT_ENTRY_END_CHAR_AS_STRING);
	introspect_string_append(&xml, tstr);
	introspect_string_append(&xml, "    </method>\n");
	introspect_string_append(&xml, "  </interface>\n");
  	
	for (i = 0; children[i]; i++) {
		if (strcmp(children[i], "Walion") == 0) {
	      		sprintf (tstr, "  <node name=\"%s\">\n", children[i]);
			introspect_string_append(&xml, tstr);
			introspect_string_append(&xml, zombieway_walion_yami);
      			introspect_string_append(&xml, "  </node>\n");
		} else {
	      		sprintf (tstr, "  <node name=\"%s\"/>\n", children[i]);
			introspect_string_append(&xml, tstr);
		}
	}
  
	introspect_string_append(&xml, "</node>\n");
	
  	reply = dbus_message_new_method_return(message);
	if (!reply) { 
		error_handler(__FILE__, "Failed on dbus_message_append_args()", __LINE__, NULL);
		goto oom;
	}

	if (!dbus_message_append_args (reply,
                            DBUS_TYPE_STRING, &xml.str,
                            DBUS_TYPE_INVALID)) {
		error_handler(__FILE__, "Failed on dbus_message_append_args()", __LINE__, NULL);
		goto oom;
	}

	if (!dbus_connection_send (connection, reply, NULL)) {
		error_handler(__FILE__, "Failed on dbus_connection_send()", __LINE__, NULL);
		goto oom;
	}

//	if (! bus_transaction_send_from_driver (transaction, connection, reply))
//	  goto oom;
	dbus_free(tstr);
	dbus_free_string_array (children);
	introspect_free(&xml);
	dbus_message_unref(reply);

	return DBUS_HANDLER_RESULT_HANDLED;

oom:
	if (xml.str)
		introspect_free(&xml);
	if (reply)
		dbus_message_unref (reply);

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

//static DBusHandlerResult
//handle_introspect (DBusConnection *connection,
//                   DBusMessage    *message,
//                   GObject        *object)

DBusHandlerResult zombieway_plus(DBusConnection *connection, DBusMessage *message)
{
	handler_speakup(__func__);

	DBusMessage* reply;
	DBusError error = DBUS_ERROR_INIT;
	int num0, num1, reply_content;

	if (! dbus_message_get_args (message, &error,
			       DBUS_TYPE_INT32, &num0,
			       DBUS_TYPE_INT32, &num1,
			       DBUS_TYPE_INVALID)) {
	    if (dbus_error_is_set(&error)) 
	    	error_handler(__FILE__, "Failed on dbus_message_get_args()", __LINE__, &error);
	    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

  	reply = dbus_message_new_method_return(message);
	if (!reply) {
		error_handler(__FILE__, "Failed on dbus_message_new_method_return()", __LINE__, NULL);
		dbus_message_unref(reply);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	reply_content = num0 + num1;
	if (!dbus_message_append_args (reply,
                            DBUS_TYPE_INT32, &reply_content,
                            DBUS_TYPE_INVALID)) {
		error_handler(__FILE__, "Failed on dbus_message_append_args()", __LINE__, NULL);
		dbus_message_unref(reply);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	if (!dbus_connection_send(connection, reply, NULL)) {
		error_handler(__FILE__, "Failed on dbus_dbus_connection_send()", __LINE__, NULL);
		dbus_message_unref(reply);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	dbus_message_unref(reply);
	return DBUS_HANDLER_RESULT_HANDLED;
}

DBusHandlerResult zombieway_message_handler(DBusConnection *connection, DBusMessage *message, void *user_data)
{
	handler_speakup(__func__);

	if (!message) {
		fprintf(stdout, "Incomming message empty\n");
		return  DBUS_HANDLER_RESULT_HANDLED;
	}

	zombieway_print_message_info(message);

	switch (dbus_message_get_type(message)) {
	 case DBUS_MESSAGE_TYPE_METHOD_CALL:
		if (dbus_message_is_method_call(message, ZOMBIEWAY_INTERFACE_YAMI, "Hello"))
			return zombieway_hello(connection, message);
		else if (dbus_message_is_method_call(message, ZOMBIEWAY_INTERFACE_YAMI, "Plus"))
			return zombieway_plus(connection, message);
		else if (dbus_message_is_method_call(message, DBUS_INTERFACE_INTROSPECTABLE, "Introspect"))
			return zombieway_introspect(connection, message);
		else {
			error_handler(__FILE__, "Unregconized message caught", __LINE__, NULL);
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
		}
	 case DBUS_MESSAGE_TYPE_SIGNAL:
		if (dbus_message_is_signal(message, ZOMBIEWAY_INTERFACE_YAMI, "Quit")){
			working = 0;
			printf("Weekends are coming, let's go to the beach!...Quiting\n");
			return  DBUS_HANDLER_RESULT_HANDLED;
		} else if (dbus_message_is_signal(message, ZOMBIEWAY_INTERFACE_YAMI, "Sleep5")){
			ZOMBIEWAY_SLEEP_PROCESSER(5);
			return  DBUS_HANDLER_RESULT_HANDLED;
		} else if (dbus_message_is_signal(message, ZOMBIEWAY_INTERFACE_YAMI, "Sleep3")){
			ZOMBIEWAY_SLEEP_PROCESSER(3);
			return  DBUS_HANDLER_RESULT_HANDLED;
		} else {
			error_handler(__FILE__, "Unregconized message caught", __LINE__, NULL);
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
		}
	 case DBUS_MESSAGE_TYPE_METHOD_RETURN:
		return  DBUS_HANDLER_RESULT_HANDLED;
	 case DBUS_MESSAGE_TYPE_INVALID:
		return  DBUS_HANDLER_RESULT_HANDLED;
	 default:
		return  DBUS_HANDLER_RESULT_HANDLED;
	}
}

void zombieway_unregister(DBusConnection *connection, void *user_data)
{
	fprintf(stdout, "Unregister zombieway service\n");
	dbus_free(user_data);
	handler_shutup(__func__);
}

static DBusObjectPathVTable server_path_vt = {
	.unregister_function = zombieway_unregister,
	.message_function = zombieway_message_handler,
	NULL,
};

DBusConnection* zombieway_service_init()
{
	DBusError error = DBUS_ERROR_INIT;

	DBusConnection *connection = dbus_bus_get(DBUS_BUS_STARTER, &error);
	if(dbus_error_is_set(&error)) {
		error_handler(__FILE__, "Error on getting bus", __LINE__, &error);
		return NULL;
	}
	dbus_bus_request_name(connection, ZOMBIEWAY_SERVER_NAME, DBUS_NAME_FLAG_DO_NOT_QUEUE, &error);
        if (dbus_error_is_set(&error)) {
		error_handler(__FILE__, "Error on dbus_bus_request_name()", __LINE__, &error);
		return NULL;
        }
	
	if (!dbus_connection_register_object_path(
		connection, ZOMBIEWAY_PATH, &server_path_vt, &server_data)) {
		error_handler(__FILE__, "Error on dbus-connection_register_object_path()", __LINE__, NULL);
		return NULL;
	}

	if (!dbus_connection_register_object_path(
		connection, ZOMBIEWAY_PATH_HOLLY, &server_path_vt, &server_data)) {
		error_handler(__FILE__, "Error on dbus-connection_register_object_path()", __LINE__, NULL);
		return NULL;
	}

	if (!dbus_connection_register_object_path(
		connection, ZOMBIEWAY_PATH_WALION, &server_path_vt, &server_data)) {
		error_handler(__FILE__, "Error on dbus-connection_register_object_path()", __LINE__, NULL);
		return NULL;
	}

//	if (!dbus_connection_register_fallback(conn, SERVER_OBJ_PATH, &server_path_vt, &server_data))
//		|| !dbus_connection_add_filter(m->bus, bus_message_filter, m, NULL)
//		fprintf(stdout, "Fail on dbus_connection_register_fallback");
//
	return connection;
}

void zombieway_cleanup(DBusConnection *connection)
{
	DBusError error = DBUS_ERROR_INIT;

	dbus_connection_unref(connection);
	dbus_bus_release_name(connection, ZOMBIEWAY_SERVER_NAME, &error);
	dbus_error_free(&error);
}

int main(int argc, char *argv[])
{
	DBusConnection *connection = zombieway_service_init();

	if (!connection) return EXIT_FAILURE;

	fprintf(stdout, "Entering zombieway service main loop...\n");
	working = 1;	

	/* main loop */
	while (dbus_connection_read_write_dispatch(connection, ZOMBIEWAY_SERVER_TIMEOUT) && working);

	fprintf(stdout, "Leaving zombieway service main loop...\n");
	zombieway_cleanup(connection);
	dbus_shutdown();

	handler_shutup(__func__);
	return EXIT_SUCCESS;
}
