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

static inline void od_dump_zval(zval val) {
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

static inline void od_dump_op(zend_op op) {
	printf("line: %u, opcode: %u, op1_type: %u, op2_type: %u op1: ", 
			op.lineno, op.opcode, op.OD_TYPE(op1), op.OD_TYPE(op2));

	switch (op.OD_TYPE(op1)) {
		case IS_UNUSED:
			printf("UNUSED");
			break;
		case IS_CONST:
#if PHP_VERSION_ID >= 50399
			od_dump_zval(*op.op1.zv);
#else
			od_dump_zval(op.op1.u.constant);
#endif
			break;
		case IS_VAR:
		case IS_TMP_VAR:
		case IS_CV:
			printf("%u", OD_ZNODE_ELEM(op.op1,var));
			break;
		default:
			break;
	}

	printf(", op2: ");

	switch (op.OD_TYPE(op2)) {
		case IS_UNUSED:
			printf("UNUSED");
			break;
		case IS_VAR:
		case IS_TMP_VAR:
		case IS_CV:
			printf("%u", OD_ZNODE_ELEM(op.op2,var));
			break;
		case IS_CONST:
#if PHP_VERSION_ID >= 50399
			od_dump_zval(*op.op2.zv);
#else
			od_dump_zval(op.op2.u.constant);
#endif
			break;
		default:
			break;
	}

	printf("\n");
}
