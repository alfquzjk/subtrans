#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#define MAX_LEN 120
#define BUFFER_SIZE 512
#define DEFAULT_VALUE 1
#define USAGE "usage:\n    -p=source.json\n    -s=[order config.json]\n"

typedef struct {
	unsigned order;
	unsigned ater_id;
	unsigned port;
	char *protocol;
	char *id;
	char *address;
	char *remark;
} service;

typedef struct {
	service service[MAX_LEN];
	unsigned len;
} services;

static char *json_source;
static char *json_target;

void json_error(json_object *object);
service *initial_ser();
services *initial_sers();
void push_service(service *ser, services *sers);
void free_services(services *sers);
char *alloc_string(const char *string);
services *json_convert_to_object(FILE *fp);
unsigned int string_convert_to_int(char *string);
unsigned int is_decimal(char *decimal);
void print_all_services();
void produce_config(unsigned int selection);
services *get_sers();
void die(char *info, const char *error);
void help();

void help()
{
	printf(USAGE);
}

void die(char *info, const char *error)
{
	if (info != NULL) {
		if (error != NULL) {
		fprintf(stderr, info, error);
			exit(1);
		} else {
			fprintf(stderr, info);
			exit(1);			
		}

	}
}

services *get_sers()
{
	services *sers;
	FILE *fp = fopen(json_source, "r");
	if (fp == NULL)
		die("Unable to open this file: %s", json_source);
	sers = json_convert_to_object(fp);
	return sers;
}

void produce_config(unsigned int selection) {
	
	services *sers = get_sers();
	service ser = sers->service[selection];

	json_object *outbounds, *protocol, *settings, *vnext,
		    *users, *id, *aterId, *port, *address;
	
	json_object *root = json_object_from_file(json_target);
	json_error(root);
	
	json_object_object_get_ex(root, "outbounds", &outbounds);
	json_error(outbounds);

	json_object_object_get_ex(json_object_array_get_idx(outbounds, 0), "settings", &settings);
	json_error(settings);

	json_object_object_get_ex(settings, "vnext", &vnext);
	json_error(vnext);

	json_object_object_get_ex(json_object_array_get_idx(vnext, 0), "users", &users);
	json_error(users);

	json_object_object_get_ex(json_object_array_get_idx(users, 0), "id", &id);
	json_error(id);

	json_object_object_get_ex(json_object_array_get_idx(vnext, 0), "port", &port);
	json_error(port);

	json_object_object_get_ex(json_object_array_get_idx(vnext, 0), "address", &address);
	json_error(address);

	/* editing the config.json object's value */
	json_object_set_string(id, ser.id);
	json_object_set_int(port, ser.port);
	json_object_set_string(address, ser.address);

	/* save to file */
	if (json_object_to_file_ext(json_target, root, JSON_C_TO_STRING_PRETTY)) {
		free_services(sers);	
		json_object_put(root);
		die("save to file failed!\n", NULL);		
	}

	free_services(sers);	
	json_object_put(root);					
}

void print_all_services()
{
	services *sers = get_sers();
	if (sers->len > 0) {
		for (int i = sers->len - 1; i >= 0; --i) {
			printf("order: %d ", sers->service[i].order);
			printf("remark: %s\n", sers->service[i].remark);
		}
	}

	free_services(sers);	
}

unsigned int is_decimal(char *decimal)
{
	for (int i = 0; i < strlen(decimal); ++i) {
		if (decimal[i] >= '1' && decimal[i] <= '9')
			break;
		else
			return 1;
	}
	return 0;
}

unsigned int string_convert_to_int(char *string)
{
	int result = 0;
	const unsigned int con = 48;
	for (int i = 0; i < strlen(string); ++i) {
		result = result * 10 + (string[i] - con);
	}
	return result;
}


services *json_convert_to_object(FILE *fp)
{
	json_object *root;
	char chunk[BUFFER_SIZE];
	int i = 1;
	json_object *add, *remark, *id, *port, *aid;
	services *sers = initial_sers();

	while (fgets(chunk, sizeof(chunk), fp) != NULL) {
		
		service *ser = initial_ser();
		root = json_tokener_parse(chunk);
		json_error(root);
		
		json_object_object_get_ex(root, "add", &add);
		json_error(add);
		json_object_object_get_ex(root, "remark", &remark);
		json_error(remark);
		json_object_object_get_ex(root, "id", &id);
		json_error(id);		
		json_object_object_get_ex(root, "port", &port);
		json_error(id);
		json_object_object_get_ex(root, "aid", &aid);
		json_error(aid);
		
		ser->order = i;
		ser->ater_id = json_object_get_int(aid);

		ser->id = alloc_string(json_object_get_string(id));
		strcpy(ser->id, json_object_get_string(id));

		ser->address = alloc_string(json_object_get_string(add));
		strcpy(ser->address, json_object_get_string(add));

		ser->remark = alloc_string(json_object_get_string(remark));
		strcpy(ser->remark, json_object_get_string(remark));

		push_service(ser, sers);
		++i;
		
		json_object_put(root);				
	}
	return sers;
}

char *alloc_string(const char *string)
{
	char *alloc_mem = (char *)malloc(strlen(string) + 1);
	if (alloc_mem == NULL)
		die("Allocating memory failed!\n", NULL);

	return alloc_mem;
}

void free_services(services *sers)
{
	if (sers == NULL) {
		return;
	} else {
		for (int i = 0; i < sers->len; ++i) {
			free(sers->service[i].protocol);
			free(sers->service[i].id);
			free(sers->service[i].address);
			free(sers->service[i].remark);		
		}
		free(sers);
	}
}
	
void push_service(service *ser, services *sers)
{
	if ((ser != NULL && sers != NULL) && sers->len < MAX_LEN) {
		sers->service[sers->len] = *ser;
		sers->len++;
	} else {
		die("Arguments are NULL\n", NULL);
	}
}

service *initial_ser()
{
	service *ser = (service *)malloc(sizeof(service));
	if (ser == NULL)
		die("Initial service failed!\n", NULL);

	ser->order = 0;
	ser->ater_id = 0;
	ser->port = 0;
	ser->protocol = NULL;
	ser->id = NULL;
	ser->address = NULL;
	ser->remark = NULL;

	return ser;
}

services *initial_sers()
{
	services *sers = (services *)malloc(sizeof(services));
	if (sers == NULL)
		die("Initial services failed!\n", NULL);
	sers->len = 0;
	return sers; 
}

void json_error(json_object *object)
{
	if (object == NULL) {
		die("JSON ERROR!: %s\n", json_util_get_last_err());
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		help();
		die("Invalid arguments!\n", NULL);		
	}
	if (!strcmp(argv[1], "-h")) {
		help();
		return 0;
	} else if (!strcmp(argv[1], "-p")) {
		json_source = argv[2];
		print_all_services();
	} else if (!strcmp(argv[1], "-s")) {
		if (!is_decimal(argv[2])) {
			json_source = argv[3];
			json_target = argv[4];
			produce_config(string_convert_to_int(argv[2]));
		} else {
			die("Please input a valid number for \"-s\" option!\n", NULL);
		}
	} else {
		help();
		die("Please giving an option!: [-s, -p]\n", NULL);
	}
		
	return 0;
}
