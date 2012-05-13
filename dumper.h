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

#if PHP_VERSION_ID >= 50399
# define OD_ZNODE znode_op
# define OD_TYPE(z) z##_type
# define OD_ZNODE_ELEM(z,v) z.v
#else
# define OD_ZNODE znode
# define OD_TYPE(z) z.op_type
# define OD_ZNODE_ELEM(z,v) z.u.v
#endif

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
			return "UNKNOWN";
	}
}

static inline void od_dump_zval(zval val) {
	switch (val.type) {
		case IS_NULL:
			printf("NULL");
			break;
		case IS_LONG:
			printf("%ld", val.value.lval);
			break;
		case IS_DOUBLE:
			printf("%g", val.value.lval);
			break;
		case IS_STRING:
			printf("%s", val.value.str.val);
			break;
		case IS_BOOL:
			printf(val.value.lval != 0l ? "TRUE" : "FALSE");
			break;
		case IS_RESOURCE:
			printf("#%ld", val.value.lval);
			break;
		case IS_ARRAY:
			printf("[ARRAY]");
			break;
		case IS_OBJECT:
			printf("[OBJECT]");
			break;
		case IS_CONSTANT:
			printf("[CONSTANT]");
			break;
		case IS_CONSTANT_ARRAY:
			printf("[CONSTANT_ARRAY]");
			break;
		default:
			printf("[UNKNOWN]");
			break;
	}
}

static inline void od_raw_dump_zval(zval val) {
	switch (val.type) {
		case IS_NULL:
		case IS_LONG:
		case IS_BOOL:
		case IS_RESOURCE:
			printf("%ld", val.value.lval);
			break;
		case IS_DOUBLE:
			printf("%g", val.value.lval);
			break;
		case IS_STRING:
			printf("%s", val.value.str.val);
			break;
		case IS_ARRAY:
		case IS_OBJECT:
		case IS_CONSTANT:
		case IS_CONSTANT_ARRAY:
		default:
			break;
	}
}

static inline void od_dump_znode(OD_ZNODE node, zend_uchar type) {
	switch (type) {
		case IS_UNUSED:
			printf("UNUSED");
			break;
		case IS_CONST:
#if PHP_VERSION_ID >= 50399
			od_dump_zval(*node.zv);
#else
			od_dump_zval(node.u.constant);
#endif
			break;
		case IS_VAR:
			printf("VAR~%u", OD_ZNODE_ELEM(node,var));
			break;
		case IS_TMP_VAR:
			printf("TMP_VAR~%u", OD_ZNODE_ELEM(node,var));
			break;
#if (PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 1)
		case IS_CV:
			printf("CV~%u", OD_ZNODE_ELEM(node,var));
			break;
#endif
		default:
			break;
	}
}

static inline void od_raw_dump_znode(OD_ZNODE node, zend_uchar type) {
	switch (type) {
		case IS_UNUSED:
			break;
		case IS_CONST:
#if PHP_VERSION_ID >= 50399
			od_raw_dump_zval(*node.zv);
#else
			od_raw_dump_zval(node.u.constant);
#endif
			break;
		case IS_VAR:
		case IS_TMP_VAR:
#if (PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 1)
		case IS_CV:
#endif
			printf("$%u", OD_ZNODE_ELEM(node,var));
			break;
		default:
			break;
	}
}

static inline void od_dump_op(zend_op op) {
	printf("line: %u,\topcode: %s,\top1_type: %s,\top2_type: %s op1: ", 
			op.lineno, od_opcodes[op.opcode], od_dump_type(op.OD_TYPE(op1)), od_dump_type(op.OD_TYPE(op2)));

	od_dump_znode(op.op1, op.OD_TYPE(op1));
	printf(",\top2: ");
	od_dump_znode(op.op2, op.OD_TYPE(op2));
	printf("\n");
}

static inline void od_raw_dump_op(zend_op op) {
	printf("line: %u,\topcode: %u,\top1_type: %u,\top2_type: %u op1: ", 
			op.lineno, op.opcode, op.OD_TYPE(op1), op.OD_TYPE(op2));

	od_raw_dump_znode(op.op1, op.OD_TYPE(op1));
	printf(",\top2: ");
	od_raw_dump_znode(op.op2, op.OD_TYPE(op2));
	printf("\n");
}
