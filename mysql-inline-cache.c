#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "memcache.h"

typedef struct {
	struct memcache *mc;
	
	char   *str;
	size_t  str_len;
} udf_memcache;

my_bool memcache_get_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	udf_memcache *udf;
	
	initid->maybe_null = 1;
	initid->max_length = 64 * 1024; /* assume a blob */
	
	/**
	 * we need a IP of the mc-host and a key
	 * both should be strings
	 */
	
	if (args->arg_count != 2) {
		strcpy(message, "memcache_get() needs host-address and key");
		
		return 1;
	}
	
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	
	udf = (udf_memcache *)calloc(1, sizeof(udf_memcache));
	
	udf->mc = mc_new();
	
	if (0 != mc_server_add4(udf->mc, args->args[0])) {
		strcpy(message, "connect to memcached failed");
		
		mc_free(udf->mc);
		free(udf);
		
		return 1;
	}
	
	initid->ptr = (char *)udf;
	
	return 0;
}

char * memcache_get(UDF_INIT *initid, UDF_ARGS *args,
		    char *result, unsigned long *length,
		    char *is_null, char *error) {
	udf_memcache *udf = (udf_memcache *)initid->ptr;
	char *r;
	
	*is_null = 0;
	*error = 0;
	
	if (args->arg_count != 2) {
		*error = 1;
		
		return result;
	}
	
	if (args->arg_type[0] != STRING_RESULT ||
	    args->arg_type[1] != STRING_RESULT) {
		*error = 1;
		
		return result;
	}
	
	if (NULL == (r = (char *)mc_aget(udf->mc, 
				 args->args[1], args->lengths[1]))) {
		
		/* not found */
		
		*is_null = 1;
		
		return result;
	}
	
	if ((*length = strlen(r)) < 255) {
		strcpy(result, r);
		free(r);
		
		return result;
	} else {
		if (udf->str_len == 0) {
			udf->str_len = *length;
			
			udf->str = (char *)malloc(udf->str_len);
		} else if (*length > udf->str_len) {
			udf->str = (char *)realloc(udf->str, udf->str_len);
		}
		memcpy(udf->str, r, *length);
		free(r);
		
		return udf->str;
	}
}

my_bool memcache_get_deinit(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	udf_memcache *udf = (udf_memcache *)initid->ptr;
	
	if (udf) {
		if (udf->mc) mc_free(udf->mc);
		if (udf->str) free(udf->str);
		free(udf);
	}
	
	return 0;
}


my_bool memcache_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	udf_memcache *udf;
	
	initid->maybe_null = 1;
	initid->max_length = 64 * 1024; /* assume a blob */
	
	/**
	 * we need a IP of the mc-host and a key
	 * both should be strings
	 */
	
	if (args->arg_count != 4) {
		strcpy(message, "memcache_set() needs host-address, key and content");
		
		return 1;
	}
	
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	args->arg_type[2] = STRING_RESULT;
	
	udf = (udf_memcache *)calloc(1, sizeof(udf_memcache));
	
	udf->mc = mc_new();
	
	if (0 != mc_server_add4(udf->mc, args->args[0])) {
		strcpy(message, "connect to memcached failed");
		
		mc_free(udf->mc);
		free(udf);
		
		return 1;
	}
	
	initid->ptr = (char *)udf;
	
	return 0;
}

long long memcache_set(UDF_INIT *initid, UDF_ARGS *args,
		       char *is_null, char *error) {
	udf_memcache *udf = (udf_memcache *)initid->ptr;
	
	*is_null = 0;
	*error = 0;
	
	if (args->arg_count != 4) {
		*error = 1;
		
		return 0;
	}
	
	if (args->arg_type[0] != STRING_RESULT ||
	    args->arg_type[1] != STRING_RESULT ||
	    args->arg_type[2] != STRING_RESULT) {
		*error = 1;
		
		return 0;
	}
	
	if (0 != mc_set(udf->mc, 
			args->args[1], args->lengths[1],
			args->args[2], args->lengths[2],
			0, 0)) {
		
		/* not found */
				
		return 0;
	}
	
	return 0;
}

my_bool memcache_set_deinit(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	udf_memcache *udf = (udf_memcache *)initid->ptr;
	
	if (udf) {
		if (udf->mc) mc_free(udf->mc);
		if (udf->str) free(udf->str);
		free(udf);
	}
	
	return 0;
}


my_bool memcache_flush_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	udf_memcache *udf;
	
	initid->maybe_null = 1;
	initid->max_length = 64 * 1024; /* assume a blob */
	
	/**
	 * we need a IP of the mc-host and a key
	 * both should be strings
	 */
	
	if (args->arg_count != 2) {
		strcpy(message, "memcache_get() needs host-address and key");
		
		return 1;
	}
	
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	
	udf = (udf_memcache *)calloc(1, sizeof(udf_memcache));
	
	udf->mc = mc_new();
	
	if (0 != mc_server_add4(udf->mc, args->args[0])) {
		strcpy(message, "connect to memcached failed");
		
		mc_flush_all(udf->mc);
		free(udf);
		
		return 1;
	}
	
	initid->ptr = (char *)udf;
	
	return 0;
}


my_bool memcache_flush_deinit(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	udf_memcache *udf = (udf_memcache *)initid->ptr;
	
	if (udf) {
		if (udf->mc) mc_free(udf->mc);
		if (udf->str) free(udf->str);
		free(udf);
	}
	
	return 0;
}


char * memcache_flush(UDF_INIT *initid, UDF_ARGS *args,
		    char *result, unsigned long *length,
		    char *is_null, char *error) {
	udf_memcache *udf = (udf_memcache *)initid->ptr;
	char *r;
	
	*is_null = 0;
	*error = 0;
	
	if (args->arg_count != 2) {
		*error = 1;
		
		return result;
	}
	
	if (args->arg_type[0] != STRING_RESULT ||
	    args->arg_type[1] != STRING_RESULT) {
		*error = 1;
		
		return result;
	}
	
	if (NULL == (r = (char *)mc_aget(udf->mc, 
				 args->args[1], args->lengths[1]))) {
		
		/* not found */
		
		*is_null = 1;
		
		return result;
	}
	
	if ((*length = strlen(r)) < 255) {
		strcpy(result, r);
		free(r);
		
		return result;
	} else {
		if (udf->str_len == 0) {
			udf->str_len = *length;
			
			udf->str = (char *)malloc(udf->str_len);
		} else if (*length > udf->str_len) {
			udf->str = (char *)realloc(udf->str, udf->str_len);
		}
		memcpy(udf->str, r, *length);
		free(r);
		
		return udf->str;
	}
}

my_bool memcache_delete_deinit(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	udf_memcache *udf = (udf_memcache *)initid->ptr;
	
	if (udf) {
		if (udf->mc) mc_free(udf->mc);
		if (udf->str) free(udf->str);
		free(udf);
	}
	
	return 0;
}

my_bool memcache_exists_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	udf_memcache *udf;
	
	initid->maybe_null = 0;
	initid->max_length = 64 * 1024; /* assume a blob */
	
	/**
	 * we need a IP of the mc-host and a key
	 * both should be strings
	 */
	
	if (args->arg_count != 2) {
		strcpy(message, "memcache_exists() needs host-address and key");
		
		return 1;
	}
	
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	
	udf = (udf_memcache *)calloc(1, sizeof(udf_memcache));
	
	udf->mc = mc_new();
	
	if (0 != mc_server_add4(udf->mc, args->args[0])) {
		strcpy(message, "connect to memcached failed");
		
		mc_free(udf->mc);
		free(udf);
		
		return 1;
	}
	
	initid->ptr = (char *)udf;
	
	return 0;
}

long long memcache_exists(UDF_INIT *initid, UDF_ARGS *args,
			  char *is_null, char *error) {
	udf_memcache *udf = (udf_memcache *)initid->ptr;
	char *r;
	
	*is_null = 0;
	*error = 0;

	if (args->arg_type[0] != STRING_RESULT ||
	    args->arg_type[1] != STRING_RESULT) {
		*error = 1;
		
		return 0;
	}
	
	if (NULL == (r = mc_aget(udf->mc, 
				 (char *)args->args[1], (size_t)args->lengths[1]))) {
		
		/* not found */
		
		return 0;
	}
	
	free(r);
	return 1;
}

my_bool memcache_exists_deinit(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	udf_memcache *udf = (udf_memcache *)initid->ptr;
	
	if (udf) {
		if (udf->mc) mc_free(udf->mc);
		if (udf->str) free(udf->str);
		free(udf);
	}
	
	return 0;
}
