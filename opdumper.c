/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Eric Zhang <ericzhang.buaa@gmail.com>                        |
  +----------------------------------------------------------------------+
*/

/* $Id: header 321634 2012-01-01 13:15:04Z felipe $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_opdumper.h"
#include "opcodes.h"
#include "dumper.h"

static zend_op_array* (*native_compile_file)(zend_file_handle* file_handle, int type TSRMLS_DC);
static zend_op_array* od_compile_file(zend_file_handle*, int type TSRMLS_DC);

static zend_op_array* (*native_compile_string)(zval *source_string, char *filename TSRMLS_DC);
static zend_op_array* od_compile_string(zval *source_string, char *filename TSRMLS_DC);

ZEND_DECLARE_MODULE_GLOBALS(opdumper)

/* True global resources - no need for thread safety here */
static int le_opdumper;

/* {{{ PHP_FUNCTION
 */
PHP_FUNCTION(od_dump_opcodes_string) {
	char *php_script;
	int php_script_len, i;
	zval *zv, *opcodes_array;
	zend_op_array *op_array;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &php_script, &php_script_len) == FAILURE) {
		return;
	}

	MAKE_STD_ZVAL(zv);
	ZVAL_STRINGL(zv, php_script, php_script_len, 0);

	MAKE_STD_ZVAL(opcodes_array);
	array_init(opcodes_array);

	op_array = zend_compile_string (zv, "" TSRMLS_CC);
	for (i = 0; i < op_array->last; i++) {
		zend_op op = op_array->opcodes[i];
		add_index_zval(opcodes_array, i, od_dump_op_array(op));
	}

	*return_value = *opcodes_array;
}

PHP_FUNCTION(od_dump_opcodes_file) {
	int i;
	zval *file, *opcodes_array;
	zend_op_array *op_array;
	zend_file_handle file_handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &file) == FAILURE) {
		return;
	}

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = Z_STRVAL_P(file);
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;

	MAKE_STD_ZVAL(opcodes_array);
	array_init(opcodes_array);

	op_array = zend_compile_file(&file_handle, ZEND_INCLUDE TSRMLS_CC);
	for (i = 0; i < op_array->last; i++) {
		zend_op op = op_array->opcodes[i];
		add_index_zval(opcodes_array, i, od_dump_op_array(op));
	}

	zend_destroy_file_handle(&file_handle TSRMLS_CC);

	*return_value = *opcodes_array;
}

ZEND_BEGIN_ARG_INFO(arginfo_od_dump_opcodes_string, 0)
	ZEND_ARG_INFO(0, php_script)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_od_dump_opcodes_file, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ opdumper_functions[]
 *
 * Every user visible function must have an entry in opdumper_functions[].
 */
const zend_function_entry opdumper_functions[] = {
	PHP_FE(od_dump_opcodes_string, arginfo_od_dump_opcodes_string)
	PHP_FE(od_dump_opcodes_file, arginfo_od_dump_opcodes_file)
	PHP_FE_END	/* Must be the last line in opdumper_functions[] */
};
/* }}} */

/* {{{ opdumper_module_entry
 */
zend_module_entry opdumper_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"opdumper",
	opdumper_functions,
	PHP_MINIT(opdumper),
	PHP_MSHUTDOWN(opdumper),
	PHP_RINIT(opdumper),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(opdumper),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(opdumper),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_OPDUMPER
ZEND_GET_MODULE(opdumper)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("opdumper.active", "0", PHP_INI_SYSTEM, OnUpdateLong, active, zend_opdumper_globals, opdumper_globals)
    STD_PHP_INI_ENTRY("opdumper.raw",    "0", PHP_INI_SYSTEM, OnUpdateLong, raw,    zend_opdumper_globals, opdumper_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_opdumper_init_globals
 */
static void php_opdumper_init_globals(zend_opdumper_globals *opdumper_globals)
{
	opdumper_globals->active = 0;
	opdumper_globals->raw = 0;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(opdumper)
{
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(opdumper)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(opdumper)
{
	native_compile_file = zend_compile_file;
#if (PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 2)
	native_compile_string = zend_compile_string;
#endif

	if (OPDUMPER_G(active)) {
		zend_compile_file = od_compile_file;
#if (PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 2)
		zend_compile_string = od_compile_string;
#endif
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(opdumper)
{
	zend_compile_file = native_compile_file;
#if (PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 2)
	zend_compile_string = native_compile_string;
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(opdumper)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "opdumper support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ od_compile_file
 */
static zend_op_array *od_compile_file(zend_file_handle* file_handle, int type TSRMLS_DC) {
	zend_op_array *op_array;
	op_array = native_compile_file (file_handle, type TSRMLS_CC);

	if (OPDUMPER_G(active)) {
		int i;
		char *opcode = (char *)emalloc(BUFF_SIZE);
		for (i = 0; i < op_array->last; i++) {
			zend_op op = op_array->opcodes[i];
			if (OPDUMPER_G(raw)) {
				od_raw_dump_op(op, opcode);
				printf("%s", opcode);
			} else {
				if (i > 0) {
					printf("==============================\n");
				}
				od_dump_op(op, opcode);
				printf("%s", opcode);
			}
		}
		efree(opcode);
	}
	return NULL;
}
/* }}} */

/* {{{ od_compile_string
 */
static zend_op_array *od_compile_string(zval *source_string, char *filename TSRMLS_DC) {
	zend_op_array *op_array;
	op_array = native_compile_string (source_string, filename TSRMLS_CC);

	if (OPDUMPER_G(active)) {
		int i;
		char *opcode = (char *)emalloc(BUFF_SIZE);
		for (i = 0; i < op_array->last; i++) {
			zend_op op = op_array->opcodes[i];
			if (OPDUMPER_G(raw)) {
				od_raw_dump_op(op, opcode);
				printf("%s", opcode);
			} else {
				if (i > 0) {
					printf("==============================\n");
				}
				od_dump_op(op, opcode);
				printf("%s", opcode);
			}
		}
		efree(opcode);
	}
	return NULL;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
