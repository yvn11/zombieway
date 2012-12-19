#ifndef ZOMBIEWAY_BASIC_H
#define ZOMBIEWAY_BASIC_H

#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
  dbus_uint32_t id;
}ConnectData;

ConnectData server_data = {-1};

typedef struct IntrospectString IntrospectString;
struct IntrospectString
{
  char		 *str;           /**< String data, plus nul termination */
  int            len;            /**< Length without nul */
  int            allocated;      /**< Allocated size of data */
};

#define ZOMBIEWAY_SERVER_NAME "org.zombieway"
#define ZOMBIEWAY_PATH "/org/zombieway"
#define ZOMBIEWAY_PATH_HOLLY "/org/zombieway/Holly"
#define ZOMBIEWAY_PATH_WALION "/org/zombieway/Walion"
#define ZOMBIEWAY_INTERFACE_YAMI "org.zombieway.walion.yami"
#define ZOMBIEWAY_INTERFACE_GEEDOSY "org.zombieway.holly.geedosy"
#define ZOMBIEWAY_SERVER_TIMEOUT -1 //1000000

#define STRING_ALLOCATION_PADDING 2

const char* zombieway_walion_yami = 
   "   <interface name=\"org.zombieway.walion.yami\">\n"
   "     <method name=\"Hello\">\n"
   "       <arg name=\"words\" type=\"s\" direction=\"out\"/>\n"
   "     </method>\n"
   "     <method name=\"Plus\">\n"
   "       <arg name=\"num0\" type=\"i\" direction=\"in\"/>\n"
   "       <arg name=\"num1\" type=\"i\" direction=\"in\"/>\n"
   "       <arg name=\"ret\" type=\"i\" direction=\"out\"/>\n"
   "     </method>\n"
   "     <signal name=\"Quit\"/>\n"
   "   </interface>\n";

#define ZOMBIEWAY_SLEEP_PROCESSER(s) { printf("Fell asleep...wake up in %d seconds\n", s);	\
				sleep(s);					\
				printf("Woke up...\n");	}
#define CLIENT_STEVEN "client.Steven"
#define CLIENT_WANDERLAND "client.Wanderland"
#define CLIENT_JENNY "client.Jenny"
#define CLIENT_JENNY_PATH_HOORAY "/client/jenny/hooray"
#define	CLIENT_JENNY_INTERFACE_YOGONA "client.Jenny.yogona"
#define SIGNAL_JENNY_CARE_SHOW_FACE_MR \
		"sender='"CLIENT_STEVEN"',"\
		"interface='"CLIENT_JENNY_INTERFACE_YOGONA"',"\
		"type='signal',member='ShowYourFace'"
#define SIGNAL_JENNY_CARE_QUIT_MR \
		"sender='"CLIENT_WANDERLAND"',"\
		"interface='"CLIENT_JENNY_INTERFACE_YOGONA"',"\
		"type='signal',member='Quit'"
#define SIGNAL_JENNY_CARE_ADD_QUIT_MR \
		"sender='"CLIENT_WANDERLAND"',"\
		"interface='"CLIENT_JENNY_INTERFACE_YOGONA"',"\
		"type='signal',member='AddQuitMatch'"
#define SIGNAL_JENNY_CARE_REMOVE_SHOW_FACE_MR \
		"sender='"CLIENT_WANDERLAND"',"\
		"interface='"CLIENT_JENNY_INTERFACE_YOGONA"',"\
		"type='signal',member='RemoveShowYouFaceMatch'"
#define SIGNAL_WANDERLAND_CARE_JENNY_IS_OFF_MR \
		"sender='"CLIENT_JENNY"',"\
		"interface='"ZOMBIEWAY_INTERFACE_GEEDOSY"',"\
		"type='signal',member='JennyIsOff'"
		
void error_handler(char* file, char* umsg, int line, DBusError *error)
{
	if (error){
		fprintf(stderr, "%s Line [ %d ], %s, ename:[ %s ], emsg:[ %s ]\n",
			file, line, umsg, error->name, error->message);
		dbus_error_free(error);
	} else {
		fprintf(stderr, "%s Line [ %d ], %s\n",
			file, line, umsg);
	}
}

void handler_speakup(const char* func)
{
	fprintf(stdout, "%s() is processing incomming message...\n", func);
}

void handler_shutup(const char* func)
{
	fprintf(stdout, "Leaving %s()...\n", func);
}

void zombieway_print_message_info(DBusMessage *message)
{
	printf("-----------------------Message Infomation------------------------\n");
	printf("Message Sender: [%s]\n", dbus_message_get_sender(message));
	printf("Message Signature: [%s]\n", dbus_message_get_signature(message));
	printf("Message Type: [%s]\n", dbus_message_type_to_string(dbus_message_get_type(message)));
	printf("Message Sending Path: [%s]\n", dbus_message_get_path(message));
	printf("Message Sending Destination: [%s]\n", dbus_message_get_destination(message));
	printf("Message Sending Interface: [%s]\n", dbus_message_get_interface(message));
	printf("Message Invoking Member: [%s]\n", dbus_message_get_member(message));
}

void speaker_announce(DBusMessage *message, dbus_bool_t isBroacast)
{
	printf("------------------------Speaker Announce-------------------------\n");
	printf("This's [%s] speaking.\n", dbus_message_get_sender(message));
	printf("Signal will be sent to [%s]\n", dbus_message_get_destination(message));
	if (isBroacast)
		printf("Broacasting...wish someone could hear me...\n");
}

#endif /* ZOMBIEWAY_BASIC_H */
