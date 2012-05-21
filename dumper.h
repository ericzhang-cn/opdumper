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

#ifndef OD_DUMPER_H
#define OD_DUMPER_H

#if PHP_VERSION_ID >= 50399
# define OD_ZNODE znode_op
# define OD_TYPE(z) z##_type
# define OD_ZNODE_ELEM(z,v) z.v
#else
# define OD_ZNODE znode
# define OD_TYPE(z) z.op_type
# define OD_ZNODE_ELEM(z,v) z.u.v
#endif

#define BUFF_SIZE 4096

static inline char* od_dump_type(zend_uchar type) {
	switch (type) {
		case IS_UNUSED:
			return "UNUSED";
		case IS_CONST:
			return "CONST";
		case IS_VAR:
			return "VAR";
		case IS_TMP_VAR:
			return "TMP_VAR";
#if (PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 1)
		case IS_CV:
			return "CV";
#endif
		default:
			return "";
	}
}

static inline void od_dump_zval(zval val, char *output) {
	switch (val.type) {
		case IS_NULL:
			sprintf(output, "NULL");
			break;
		case IS_LONG:
			sprintf(output, "%ld", val.value.lval);
			break;
		case IS_DOUBLE:
			sprintf(output, "%g", val.value.lval);
			break;
		case IS_STRING:
			sprintf(output, "%s", val.value.str.val);
			break;
		case IS_BOOL:
			sprintf(output, val.value.lval != 0 ? "TRUE" : "FALSE");
			break;
		case IS_RESOURCE:
			sprintf(output, "res#%ld", val.value.lval);
			break;
		case IS_ARRAY:
			sprintf(output, "[ARRAY]");
			break;
		case IS_OBJECT:
			sprintf(output, "[OBJECT]");
			break;
		case IS_CONSTANT:
			sprintf(output, "[CONSTANT]");
			break;
		case IS_CONSTANT_ARRAY:
			sprintf(output, "[CONSTANT_ARRAY");
			break;
		default:
			sprintf(output, "[UNKNOWN]");
			break;
	}
}

static inline void od_raw_dump_zval(zval val, char *output) {
	switch (val.type) {
		case IS_NULL:
		case IS_LONG:
		case IS_BOOL:
		case IS_RESOURCE:
			sprintf(output, "%ld", val.value.lval);
			break;
		case IS_DOUBLE:
			sprintf(output, "%g", val.value.lval);
			break;
		case IS_STRING:
			sprintf(output, "%s", val.value.str.val);
			break;
		case IS_ARRAY:
		case IS_OBJECT:
		case IS_CONSTANT:
		case IS_CONSTANT_ARRAY:
		default:
			sprintf(output, "");
			break;
	}
}

static inline void od_dump_znode(OD_ZNODE node, zend_uchar type, char *output) {
	switch (type) {
		case IS_UNUSED:
			sprintf(output, "UNUSED");
			break;
		case IS_CONST:
#if PHP_VERSION_ID >= 50399
			od_dump_zval(*node.zv, output);
#else
			od_dump_zval(node.u.constant, output);
#endif
			break;
		case IS_VAR:
			sprintf(output, "$%u", OD_ZNODE_ELEM(node,var));
			break;
		case IS_TMP_VAR:
			sprintf(output, "#%u", OD_ZNODE_ELEM(node,var));
			break;
#if (PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 1)
		case IS_CV:
			sprintf(output, "~%u", OD_ZNODE_ELEM(node,var));
			break;
#endif
		default:
			sprintf(output, "");
			break;
	}
}

static inline void od_raw_dump_znode(OD_ZNODE node, zend_uchar type, char *output) {
	switch (type) {
		case IS_UNUSED:
			sprintf(output, "");
			break;
		case IS_CONST:
#if PHP_VERSION_ID >= 50399
			od_raw_dump_zval(*node.zv, output);
#else
			od_raw_dump_zval(node.u.constant, output);
#endif
			break;
		case IS_VAR:
		case IS_TMP_VAR:
#if (PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 1)
		case IS_CV:
#endif
			sprintf(output, "%u", OD_ZNODE_ELEM(node,var));
			break;
		default:
			sprintf(output, "");
			break;
	}
}

static inline zval* od_dump_op_array(zend_op op) {
	zval *opcode;
	char *op1 = (char *)emalloc(BUFF_SIZE);
	char *op2 = (char *)emalloc(BUFF_SIZE);
	char *result = (char *)emalloc(BUFF_SIZE);

	od_dump_znode(op.op1, op.OD_TYPE(op1), op1);
	od_dump_znode(op.op2, op.OD_TYPE(op2), op2);
	od_dump_znode(op.result, op.OD_TYPE(result), result);

	MAKE_STD_ZVAL(opcode);
	array_init(opcode);
	add_assoc_long(opcode, "lineno", op.lineno);
	add_assoc_string(opcode, "opcode", (char *)od_opcodes[op.opcode], 1);
	add_assoc_string(opcode, "op1_type", od_dump_type(op.OD_TYPE(op1)), 1);
	add_assoc_string(opcode, "op2_type", od_dump_type(op.OD_TYPE(op2)), 1);
	add_assoc_string(opcode, "result_type", od_dump_type(op.OD_TYPE(result)), 1);
	add_assoc_string(opcode, "op1", op1, 1);
	add_assoc_string(opcode, "op2", op2, 1);
	add_assoc_string(opcode, "result", result, 1);

	efree(op1);
	efree(op2);
	efree(result);

	return opcode;
}

static inline void od_dump_op(zend_op op, char *opcode) {
	char *op1 = (char *)emalloc(BUFF_SIZE);
	char *op2 = (char *)emalloc(BUFF_SIZE);
	char *result = (char *)emalloc(BUFF_SIZE);

	od_dump_znode(op.op1, op.OD_TYPE(op1), op1);
	od_dump_znode(op.op2, op.OD_TYPE(op2), op2);
	od_dump_znode(op.result, op.OD_TYPE(result), result);

	sprintf(opcode, "line: %u\nopcode: %s\nop1_type: %s\nop2_type: %s\nresult_type: %s\nop1: %s\nop2: %s\nresult: %s\n", 
			op.lineno, 
			od_opcodes[op.opcode], 
			od_dump_type(op.OD_TYPE(op1)), 
			od_dump_type(op.OD_TYPE(op2)), 
			od_dump_type(op.OD_TYPE(result)), 
			op1, 
			op2, 
			result);

	efree(op1);
	efree(op2);
	efree(result);
}

static inline void od_raw_dump_op(zend_op op, char *opcode) {
	char *op1 = (char *)emalloc(BUFF_SIZE);
	char *op2 = (char *)emalloc(BUFF_SIZE);
	char *result = (char *)emalloc(BUFF_SIZE);

	od_dump_znode(op.op1, op.OD_TYPE(op1), op1);
	od_dump_znode(op.op2, op.OD_TYPE(op2), op2);
	od_dump_znode(op.result, op.OD_TYPE(result), result);

	sprintf(opcode, "line: %u\topcode: %u\top1_type: %u\top2_type: %u\tresult_type: %u\top1: %s\top2: %s\tresult: %s\n", 
			op.lineno, 
			op.opcode, 
			op.OD_TYPE(op1), 
			op.OD_TYPE(op2), 
			op.OD_TYPE(result), 
			op1, 
			op2, 
			result);

	efree(op1);
	efree(op2);
	efree(result);
}

#endif
