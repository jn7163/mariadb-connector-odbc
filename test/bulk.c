/*
  Copyright (c) 2001, 2012, Oracle and/or its affiliates. All rights reserved.
                2013 MontyProgram AB

  The MySQL Connector/ODBC is licensed under the terms of the GPLv2
  <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
  MySQL Connectors. There are special exceptions to the terms and
  conditions of the GPLv2 as it is applied to this software, see the
  FLOSS License Exception
  <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; version 2 of the License.
  
  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.
  
  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "tap.h"

#define MAX_INSERT_COUNT 800

ODBC_TEST(t_bulk_insert_nts)
{
  char a[2][30]= {"Name 1", "Name 23"};
  SQLLEN indicator[2]= {SQL_NTS, SQL_NTS};

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS t_bulk_insert");
  OK_SIMPLE_STMT(Stmt, "CREATE TABLE t_bulk_insert (a VARCHAR(20))");
         
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_CURSOR_TYPE,
                                (SQLPOINTER)SQL_CURSOR_FORWARD_ONLY, 0));
  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE,
                                (SQLPOINTER)2, 0));
  
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 1, SQL_C_CHAR, &a[0], 30, indicator));

  OK_SIMPLE_STMT(Stmt, "SELECT a FROM t_bulk_insert LIMIT 1");

 
  CHECK_STMT_RC(Stmt, SQLBulkOperations(Stmt, SQL_ADD));

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_UNBIND));
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE,
                                (SQLPOINTER)1, 0));
 
  OK_SIMPLE_STMT(Stmt, "SELECT * FROM t_bulk_insert");
  is_num(myrowcount(Stmt), 2);

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS t_bulk_insert");

  return OK;
}

/* TODO: As of now this test is useless */
ODBC_TEST(t_bulk_insert_test)
{
  char a[2][30]= {"Name 1", "Name 23"};
  double b[2]= {5.78, 6.78};
  double d[2]= {1.23, 3.45};
  SQLLEN indicator[2]= {SQL_NTS, SQL_NTS};
  SQLLEN b_indicator[2]= {0,0};
  SQLLEN d_indicator[2]= {0,0};

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS t_bulk_insert");
  OK_SIMPLE_STMT(Stmt, "CREATE TABLE t_bulk_insert (a VARCHAR(20), b bigint(20), c decimal(15,2))");
         
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_CURSOR_TYPE,
                                (SQLPOINTER)SQL_CURSOR_FORWARD_ONLY, 0));
  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE,
                                (SQLPOINTER)2, 0));

  CHECK_STMT_RC(Stmt, SQLPrepare(Stmt, "INSERT INTO t_bulk_insert VALUES (?,?,?)", SQL_NTS));
  
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 1, SQL_C_CHAR, &a[0], 30, indicator));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 2, SQL_DOUBLE, &b[0], 8, b_indicator));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 3, SQL_DOUBLE, &d[0], 8, d_indicator));

  OK_SIMPLE_STMT(Stmt, "SELECT a,b,c FROM t_bulk_insert LIMIT 1");

 
//  CHECK_STMT_RC(Stmt, SQLBulkOperations(Stmt, SQL_ADD));

  CHECK_DBC_RC(Stmt, SQLEndTran(SQL_HANDLE_DBC, Connection, 0));

  SQLFreeHandle(SQL_HANDLE_STMT, Stmt);

  CHECK_DBC_RC(Connection, SQLAllocHandle(SQL_HANDLE_STMT, Connection, &Stmt));
  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_CURSOR_TYPE,
                                (SQLPOINTER)SQL_CURSOR_FORWARD_ONLY, 0));
  //SQLFreeHandle(SQL_HANDLE_STMT, Stmt);
  CHECK_DBC_RC(Stmt, SQLEndTran(SQL_HANDLE_DBC, Connection, 0));

  //CHECK_DBC_RC(Connection, SQLDisconnect(Connection));

  return OK;
}

ODBC_TEST(t_bulk_insert)
{
  SQLINTEGER i, id[MAX_INSERT_COUNT+1];
  SQLCHAR    name[MAX_INSERT_COUNT][40],
             txt[MAX_INSERT_COUNT][60],
             ltxt[MAX_INSERT_COUNT][70];
  SQLDOUBLE  dt, dbl[MAX_INSERT_COUNT];

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS t_bulk_insert");
  OK_SIMPLE_STMT(Stmt, "CREATE TABLE t_bulk_insert (id INT, v VARCHAR(100),"
         "txt TEXT, ft FLOAT(10), ltxt LONG VARCHAR)");

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  dt= 0.23456;

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_CURSOR_TYPE,
                                (SQLPOINTER)SQL_CURSOR_FORWARD_ONLY, 0));
  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE,
                                (SQLPOINTER)MAX_INSERT_COUNT, 0));
  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_CONCURRENCY,
                                (SQLPOINTER)SQL_CONCUR_ROWVER, 0));

  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 1, SQL_C_LONG, id, 0, NULL));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 2, SQL_C_CHAR, name, sizeof(name[0]), NULL));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 3, SQL_C_CHAR, txt, sizeof(txt[0]), NULL));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 4, SQL_C_DOUBLE, dbl, 0, NULL));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 5, SQL_C_CHAR, ltxt, sizeof(ltxt[0]), NULL));

  OK_SIMPLE_STMT(Stmt, "SELECT id, v, txt, ft, ltxt FROM t_bulk_insert");

  FAIL_IF(SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0)!= SQL_NO_DATA_FOUND, "SQL_NO_DATA_FOUND expected");

  for (i= 0; i < MAX_INSERT_COUNT; i++)
  {
    id[i]= i;
    dbl[i]= i + dt;
    sprintf((char *)name[i], "Varchar%d", i);
    sprintf((char *)txt[i],  "Text%d", i);
    sprintf((char *)ltxt[i], "LongText, id row:%d", i);
  }

  CHECK_STMT_RC(Stmt, SQLBulkOperations(Stmt, SQL_ADD));
  CHECK_STMT_RC(Stmt, SQLBulkOperations(Stmt, SQL_ADD));

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_UNBIND));
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE,
                                (SQLPOINTER)1, 0));

  OK_SIMPLE_STMT(Stmt, "SELECT * FROM t_bulk_insert");
  is_num(myrowcount(Stmt), MAX_INSERT_COUNT * 2);

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS t_bulk_insert");

  return OK;
}


ODBC_TEST(t_mul_pkdel)
{
  SQLINTEGER nData;
  SQLLEN nlen;
  SQLULEN pcrow;

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS t_mul_pkdel");
  OK_SIMPLE_STMT(Stmt, "CREATE TABLE t_mul_pkdel (a INT NOT NULL, b INT,"
         "c VARCHAR(30) NOT NULL, PRIMARY KEY(a, c))");
  OK_SIMPLE_STMT(Stmt, "INSERT INTO t_mul_pkdel VALUES (100,10,'MySQL1'),"
         "(200,20,'MySQL2'),(300,20,'MySQL3'),(400,20,'MySQL4')");

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  CHECK_STMT_RC(Stmt, SQLSetCursorName(Stmt, (SQLCHAR *)"venu", SQL_NTS));

  OK_SIMPLE_STMT(Stmt, "SELECT a, c FROM t_mul_pkdel");

  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 1, SQL_C_LONG, &nData, 0, NULL));

  CHECK_STMT_RC(Stmt, SQLExtendedFetch(Stmt, SQL_FETCH_NEXT, 1, &pcrow, NULL));
  CHECK_STMT_RC(Stmt, SQLSetPos(Stmt, 1, SQL_POSITION, SQL_LOCK_NO_CHANGE));

  CHECK_STMT_RC(Stmt, SQLSetPos(Stmt, 1, SQL_DELETE, SQL_LOCK_NO_CHANGE));
  CHECK_STMT_RC(Stmt, SQLRowCount(Stmt, &nlen));
  is_num(nlen, 1);

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_UNBIND));
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  OK_SIMPLE_STMT(Stmt, "SELECT * FROM t_mul_pkdel");

  is_num(myrowcount(Stmt), 3);

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS t_mul_pkdel");

  return OK;
}


/**
  Bug #24306: SQLBulkOperations always uses indicator varables' values from
  the first record
*/
ODBC_TEST(t_bulk_insert_indicator)
{
  SQLINTEGER id[4], nData;
  SQLLEN     indicator[4], nLen;

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS my_bulk");
  OK_SIMPLE_STMT(Stmt, "CREATE TABLE my_bulk (id int default 5)");

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_CURSOR_TYPE,
                                (SQLPOINTER)SQL_CURSOR_STATIC, 0));

  CHECK_STMT_RC(Stmt,
          SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)3, 0));

  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 1, SQL_C_LONG, id, 0, indicator));

  OK_SIMPLE_STMT(Stmt, "SELECT id FROM my_bulk");

  FAIL_IF(SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0)!=SQL_NO_DATA_FOUND, "SQL_NO_DATA_FOUND expected");

  id[0]= 1; indicator[0]= SQL_COLUMN_IGNORE;
  id[1]= 2; indicator[1]= SQL_NULL_DATA;
  id[2]= 3; indicator[2]= 0;

  CHECK_STMT_RC(Stmt, SQLBulkOperations(Stmt, SQL_ADD));

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_UNBIND));
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE,
                                (SQLPOINTER)1, 0));

  OK_SIMPLE_STMT(Stmt, "SELECT id FROM my_bulk");

  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 1, SQL_C_LONG, &nData, 0, &nLen));

  CHECK_STMT_RC(Stmt, SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0));
  is_num(nData, 5);
  IS(nLen != SQL_NULL_DATA);

  CHECK_STMT_RC(Stmt, SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0));
  is_num(nLen, SQL_NULL_DATA);

  CHECK_STMT_RC(Stmt, SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0));
  is_num(nData, 3);
  IS(nLen != SQL_NULL_DATA);

  FAIL_IF(SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0)!=SQL_NO_DATA_FOUND, "SQL_NO_DATA_FOUND expected");

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_UNBIND));
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS my_bulk");

  return OK;
 }


/**
  Simple structure for a row (just one element) plus an indicator column.
*/
typedef struct {
  SQLINTEGER val;
  SQLLEN     ind;
} row;

/**
  This is related to the fix for Bug #24306 -- handling of row-wise binding,
  plus handling of SQL_ATTR_ROW_BIND_OFFSET_PTR, within the context of
  SQLBulkOperations(Stmt, SQL_ADD).
*/
ODBC_TEST(t_bulk_insert_rows)
{
  row        rows[3];
  SQLINTEGER nData;
  SQLLEN     nLen, offset;

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS my_bulk");
  OK_SIMPLE_STMT(Stmt, "CREATE TABLE my_bulk (id int default 5)");

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_CURSOR_TYPE,
                                (SQLPOINTER)SQL_CURSOR_STATIC, 0));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE,
                                (SQLPOINTER)3, 0));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_BIND_TYPE,
                                (SQLPOINTER)sizeof(row), 0));

  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 1, SQL_C_LONG, &rows[0].val, 0,
                            &rows[0].ind));

  OK_SIMPLE_STMT(Stmt, "SELECT id FROM my_bulk");

  FAIL_IF(SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0)!=SQL_NO_DATA_FOUND, "SQL_NO_DATA_FOUND expected");

  rows[0].val= 1; rows[0].ind= SQL_COLUMN_IGNORE;
  rows[1].val= 2; rows[1].ind= SQL_NULL_DATA;
  rows[2].val= 3; rows[2].ind= 0;

  CHECK_STMT_RC(Stmt, SQLBulkOperations(Stmt, SQL_ADD));

  /* Now re-insert the last row using SQL_ATTR_ROW_BIND_OFFSET_PTR */
  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE,
                                (SQLPOINTER)1, 0));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_BIND_OFFSET_PTR,
                                (SQLPOINTER)&offset, 0));

  offset= 2 * sizeof(row);

  CHECK_STMT_RC(Stmt, SQLBulkOperations(Stmt, SQL_ADD));

  /* Remove SQL_ATTR_ROW_BIND_OFFSET_PTR */
  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_BIND_OFFSET_PTR,
                                NULL, 0));

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_UNBIND));
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  OK_SIMPLE_STMT(Stmt, "SELECT id FROM my_bulk");

  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 1, SQL_C_LONG, &nData, 0, &nLen));

  CHECK_STMT_RC(Stmt, SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0));
  is_num(nData, 5);
  IS(nLen != SQL_NULL_DATA);

  CHECK_STMT_RC(Stmt, SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0));
  is_num(nLen, SQL_NULL_DATA);

  CHECK_STMT_RC(Stmt, SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0));
  is_num(nData, 3);
  IS(nLen != SQL_NULL_DATA);

  CHECK_STMT_RC(Stmt, SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0));
  is_num(nData, 3);
  IS(nLen != SQL_NULL_DATA);

  FAIL_IF(SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0)!=SQL_NO_DATA_FOUND, "SQL_NO_DATA_FOUND expected");

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_UNBIND));
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS my_bulk");

  return OK;
}

#define MAODBC_ROWS 2
ODBC_TEST(t_odbc90)
{
  SQLHANDLE  henv1;
  SQLHANDLE  Connection1;
  SQLHANDLE  Stmt1;
  SQLCHAR    conn[512], sval[MAODBC_ROWS][32]={"Record 1", "Record 21"};
  SQLLEN     ind1[MAODBC_ROWS]= {SQL_COLUMN_IGNORE, 0}, ind2[MAODBC_ROWS]= {sizeof(int), sizeof(int)},
             ind3[MAODBC_ROWS]= {8, 9}, ind4[MAODBC_ROWS]={SQL_COLUMN_IGNORE, SQL_COLUMN_IGNORE};
  SQLINTEGER nval[MAODBC_ROWS]={100, 500}, id[MAODBC_ROWS]={2, 7};
 
  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS odbc90");
  OK_SIMPLE_STMT(Stmt, "CREATE TABLE odbc90 (id int not null primary key auto_increment, \
                          nval int not null, sval varchar(32) not null, ts timestamp)");

  /* odbc 3 */
  /* This cursor closing is required, otherwise DM(on Windows) freaks out */
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));
  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_CURSOR_TYPE,
    (SQLPOINTER)SQL_CURSOR_STATIC, 0));

  CHECK_STMT_RC(Stmt,
    SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)MAODBC_ROWS, 0));

  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 1, SQL_C_LONG, id, 0, ind1));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 2, SQL_C_LONG, nval, 0, ind2));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 3, SQL_C_CHAR, sval, sizeof(sval[0]), ind3));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 4, SQL_C_CHAR, NULL, 0, ind4));

  OK_SIMPLE_STMT(Stmt, "SELECT id, nval, sval, ts FROM odbc90");

  FAIL_IF(SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0)!=SQL_NO_DATA_FOUND, "SQL_NO_DATA_FOUND expected");

  CHECK_STMT_RC(Stmt, SQLBulkOperations(Stmt, SQL_ADD));

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_UNBIND));
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE,
    (SQLPOINTER)1, 0));

  OK_SIMPLE_STMT(Stmt, "SELECT id, nval, sval, ts FROM odbc90");

  ind4[0]= 0;
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 1, SQL_C_LONG, id, 0, NULL));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 2, SQL_C_LONG, nval, 0, NULL));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 3, SQL_C_CHAR, sval[0], sizeof(sval[0]), NULL));
  CHECK_STMT_RC(Stmt, SQLBindCol(Stmt, 4, SQL_C_CHAR, sval[1], sizeof(sval[0]), ind4));

  CHECK_STMT_RC(Stmt, SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0));
  is_num(id[0], 1);
  is_num(nval[0], 100);
  IS_STR(sval[0], "Record 1", ind3[0] + 1);
  is_num(ind4[0], 19);

  CHECK_STMT_RC(Stmt, SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0));
  is_num(id[0], 7);
  is_num(nval[0], 500);
  IS_STR(sval[0], "Record 21", ind3[1] + 1);
  is_num(ind4[0], 19);

  FAIL_IF(SQLFetchScroll(Stmt, SQL_FETCH_NEXT, 0)!=SQL_NO_DATA_FOUND, "SQL_NO_DATA_FOUND expected");

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  /* odbc 2. Not sure if it's really needed internaly one call is mapped to another as well. But won't hurt. */
  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS odbc90");
  OK_SIMPLE_STMT(Stmt, "CREATE TABLE odbc90 (id int not null primary key auto_increment, \
                                            nval int not null, sval varchar(32) not null, ts timestamp)");
  id[0]= 2;
  ind4[0]= SQL_COLUMN_IGNORE;
  strcpy(sval[0], "Record 1");
  strcpy(sval[1], "Record 21");
  nval[0]= 100;

  sprintf((char *)conn, "DRIVER=%s;SERVER=%s;UID=%s;PASSWORD=%s;DATABASE=%s%s;",
    my_drivername, my_servername, my_uid, my_pwd, my_schema, ma_strport);

  CHECK_ENV_RC(henv1, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv1));
  CHECK_ENV_RC(henv1, SQLSetEnvAttr(henv1, SQL_ATTR_ODBC_VERSION,
    (SQLPOINTER)SQL_OV_ODBC2, SQL_IS_INTEGER));
  CHECK_ENV_RC(henv1, SQLAllocHandle(SQL_HANDLE_DBC, henv1, &Connection1));
  CHECK_DBC_RC(Connection1, SQLDriverConnect(Connection1, NULL, conn, (SQLSMALLINT)strlen(conn), NULL, 0,
    NULL, SQL_DRIVER_NOPROMPT));
  CHECK_DBC_RC(Connection1, SQLAllocHandle(SQL_HANDLE_STMT, Connection1, &Stmt1));

  /* This cursor closing is required, otherwise DM(on Windows) freaks out */
  CHECK_STMT_RC(Stmt1, SQLFreeStmt(Stmt1, SQL_CLOSE));
  CHECK_STMT_RC(Stmt1, SQLSetStmtAttr(Stmt1, SQL_ATTR_CURSOR_TYPE,
    (SQLPOINTER)SQL_CURSOR_STATIC, 0));

  CHECK_STMT_RC(Stmt1,
    SQLSetStmtAttr(Stmt1, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)MAODBC_ROWS, 0));

  CHECK_STMT_RC(Stmt1, SQLBindCol(Stmt1, 1, SQL_C_LONG, id, 0, ind1));
  CHECK_STMT_RC(Stmt1, SQLBindCol(Stmt1, 2, SQL_C_LONG, nval, 0, ind2));
  CHECK_STMT_RC(Stmt1, SQLBindCol(Stmt1, 3, SQL_C_CHAR, sval, sizeof(sval[0]), ind3));
  CHECK_STMT_RC(Stmt1, SQLBindCol(Stmt1, 4, SQL_C_CHAR, NULL, 0, ind4));

  OK_SIMPLE_STMT(Stmt1, "SELECT id, nval, sval, ts FROM odbc90");

  FAIL_IF(SQLExtendedFetch(Stmt1, SQL_FETCH_FIRST, 2, NULL, NULL)!=SQL_NO_DATA_FOUND, "SQL_NO_DATA_FOUND expected");

  CHECK_STMT_RC(Stmt1, SQLSetPos(Stmt1, 0, SQL_ADD, SQL_LOCK_NO_CHANGE));

  CHECK_STMT_RC(Stmt1, SQLFreeStmt(Stmt1, SQL_UNBIND));
  CHECK_STMT_RC(Stmt1, SQLFreeStmt(Stmt1, SQL_CLOSE));

  CHECK_STMT_RC(Stmt1, SQLSetStmtAttr(Stmt1, SQL_ATTR_ROW_ARRAY_SIZE,
    (SQLPOINTER)1, 0));

  OK_SIMPLE_STMT(Stmt1, "SELECT id, nval, sval, ts FROM odbc90");

  ind4[0]= 0;
  CHECK_STMT_RC(Stmt1, SQLBindCol(Stmt1, 1, SQL_C_LONG, id, 0, NULL));
  CHECK_STMT_RC(Stmt1, SQLBindCol(Stmt1, 2, SQL_C_LONG, nval, 0, NULL));
  CHECK_STMT_RC(Stmt1, SQLBindCol(Stmt1, 3, SQL_C_CHAR, sval[0], sizeof(sval[0]), NULL));
  CHECK_STMT_RC(Stmt1, SQLBindCol(Stmt1, 4, SQL_C_CHAR, sval[1], sizeof(sval[0]), ind4));

  CHECK_STMT_RC(Stmt1, SQLFetch(Stmt1));
  is_num(id[0], 1);
  is_num(nval[0], 100);
  IS_STR(sval[0], "Record 1", ind3[0] + 1);
  is_num(ind4[0], 19);

  CHECK_STMT_RC(Stmt1, SQLFetch(Stmt1));
  is_num(id[0], 7);
  is_num(nval[0], 500);
  IS_STR(sval[0], "Record 21", ind3[1] + 1);
  is_num(ind4[0], 19);

  FAIL_IF(SQLFetch(Stmt1)!=SQL_NO_DATA_FOUND, "SQL_NO_DATA_FOUND expected");

  CHECK_STMT_RC(Stmt1, SQLFreeHandle(SQL_HANDLE_STMT, Stmt1));
  CHECK_DBC_RC(Connection1, SQLDisconnect(Connection1));
  CHECK_DBC_RC(Connection1, SQLFreeHandle(SQL_HANDLE_DBC, Connection1));
  CHECK_ENV_RC(henv1, SQLFreeHandle(SQL_HANDLE_ENV, henv1));

  OK_SIMPLE_STMT(Stmt, "DROP TABLE odbc90");

  return OK;
}
#undef MAODBC_ROWS

/* This is test of DELETE statemten with param array. The primary goal is to test MariaDB bulk operation,
   thus putting it into "bulk". ODBC-117 is corresponding bug report */
ODBC_TEST(t_bulk_delete)
{
  SQLINTEGER a[3]= {1, 3, 2};
  SQLCHAR val[7], insert_val[][7]= {"first", "third", "second"};
  SQLLEN val_indicator[]= {SQL_NTS, SQL_NTS, SQL_NTS}, id_ind[]= {4, 4, 4};

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS t_bulk_delete");
  OK_SIMPLE_STMT(Stmt, "CREATE TABLE t_bulk_delete (id INT unsigned not null primary key, val VARCHAR(20) not null)");
  /*OK_SIMPLE_STMT(Stmt, "INSERT INTO t_bulk_delete VALUES(1, 'first'), (2, 'second'), (3, 'third')");*/
         
  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_PARAMSET_SIZE,
    (SQLPOINTER)3, 0));
  CHECK_STMT_RC(Stmt, SQLBindParameter(Stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, a, 0, id_ind));
  CHECK_STMT_RC(Stmt, SQLBindParameter(Stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, insert_val, 7, val_indicator));

  OK_SIMPLE_STMT(Stmt, "INSERT INTO t_bulk_delete VALUES(?, ?)");

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_PARAMSET_SIZE,
                                (SQLPOINTER)2, 0));
  OK_SIMPLE_STMT(Stmt, "DELETE FROM t_bulk_delete WHERE id=?");

  OK_SIMPLE_STMT(Stmt, "SELECT id, val FROM t_bulk_delete");

  CHECK_STMT_RC(Stmt, SQLFetch(Stmt));
  is_num(my_fetch_int(Stmt, 1), 2);
  IS_STR(my_fetch_str(Stmt, val, 2), "second", 7);

  EXPECT_STMT(Stmt, SQLFetch(Stmt), SQL_NO_DATA);

  CHECK_STMT_RC(Stmt, SQLFreeStmt(Stmt, SQL_CLOSE));

  CHECK_STMT_RC(Stmt, SQLSetStmtAttr(Stmt, SQL_ATTR_ROW_ARRAY_SIZE,
                                (SQLPOINTER)1, 0));

  OK_SIMPLE_STMT(Stmt, "DROP TABLE IF EXISTS t_bulk_delete");

  return OK;
}


MA_ODBC_TESTS my_tests[]=
{
  {t_bulk_insert_nts, "t_bulk_insert_nts"},
  {t_bulk_insert_test, "t_bulk_insert_test"},
  {t_bulk_insert, "t_bulk_insert"},
  {t_mul_pkdel, "t_mul_pkdel"}, 
  {t_bulk_insert_indicator, "t_bulk_insert_indicator"},
  {t_bulk_insert_rows, "t_bulk_insert_rows"},
  {t_odbc90, "odbc90_insert_with_ts_col"},
  {t_bulk_delete, "t_bulk_delete"},
  {NULL, NULL}
};

int main(int argc, char **argv)
{
  int tests= sizeof(my_tests)/sizeof(MA_ODBC_TESTS) - 1;
  get_options(argc, argv);
  plan(tests);
  mark_all_tests_normal(my_tests);
  return run_tests(my_tests);
}
