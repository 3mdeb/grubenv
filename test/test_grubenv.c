/*
 * Author: Maciej Pijanowski maciej.pijanowski@3mdeb.com
 * Copyright (C) 2017, 3mdeb
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include "unity.h"
#include "grubenv.h"
#include <string.h>
#include <stdlib.h>

#define PREFIX "/home/user/projects/grubenv/test/"

#define GRUBENV_PATH PREFIX "grubenv"
#define GRUBENV_SMALL PREFIX "grubenv_small"
#define GRUBENV_BIG PREFIX "grubenv_big"
#define GRUBENV_CORRUPTED_HEADER PREFIX "grubenv_corrupted_header"
#define GRUBENV_NOT_EXISTING PREFIX "grubenv_1jjadus1u23"
#define GRUBENV_FULL PREFIX "grubenv_full"

#define GRUBENV_BASE PREFIX "grubenv_base"
#define GRUBENV_WRITE PREFIX "grubenv_write"
#define GRUBENV_SET_EXISTING_VAR PREFIX "grubenv_set_existing_var"
#define GRUBENV_SET_NOT_EXISTING_VAR PREFIX "grubenv_set_not_existing_var"
#define GRUBENV_UNSET_EXISTING_VAR PREFIX "grubenv_unset_existing_var"
#define GRUBENV_UNSET_NOT_EXISTING_VAR PREFIX "grubenv_unset_not_existing_var"

#define EXISTING_VAR_NAME "abc"
#define EXISTING_VAR_VALUE "value_ex_set"
#define EXISTING_VAR_STRING EXISTING_VAR_NAME "=" EXISTING_VAR_VALUE "\n"
#define NOT_EXISTING_VAR_NAME "name_ne_set"
#define NOT_EXISTING_VAR_VALUE "value_ne_set"
#define NOT_EXISTING_VAR_STRING NOT_EXISTING_VAR_NAME "=" NOT_EXISTING_VAR_VALUE "\n"

#define TEST_REPORT(M, ...) fprintf(stderr, "REPORT: %s:%d: " M "\n",\
__FILE__, __LINE__, ##__VA_ARGS__)


/* copy file1 content into file2 */
/* used to set up some tests */
int fcopy(char *file1, char *file2)
{
    char buffer[1024];
    FILE *f1, *f2;

    f1 = fopen(file1, "rb");
    if (!f1) {
        TEST_REPORT("Failed to open %s\n", file1);
        TEST_FAIL();
        return -1;
    }
    f2 = fopen(file2, "wb");
    if (!f2) {
        TEST_REPORT("Failed to open %s\n", file2);
        TEST_FAIL();
        return -1;
    }

    if (fread(buffer, 1, 1024, f1) != 1024)
    {
        TEST_REPORT("Failed to read %s", file1);
        goto fail;
    }
    if (fwrite(buffer, 1, 1024, f2) != 1024)
    {

        TEST_REPORT("Failed to copy %s into %s\n", file1, file2);
        goto fail;
    }

    fclose(f1);
    fclose(f2);
    return 0;

    fail:
        fclose(f1);
        fclose(f2);
        TEST_FAIL();
        return -1;
}

char *grubenv;
int space_global;

// setUp() is called by the generated runner before each test case function
void setUp(void)
{
    grubenv = grubenv_open(GRUBENV_PATH);
    space_global = grubenv_space(grubenv);
}

// tearDown() is called by the generated runner before each test case function
void tearDown(void)
{
     grubenv_close(grubenv);
}

void test_OpenCorrectEnvblkFile(void)
{
    char *ret = grubenv_open(GRUBENV_PATH);
    TEST_ASSERT_NOT_NULL(ret);
}

void test_OpenTooSmallEnvblkFile(void)
{
    char *ret = grubenv_open(GRUBENV_SMALL);
    TEST_ASSERT_NULL(ret);
}

void test_OpenTooBigEnvblkFile(void)
{
    char *ret = grubenv_open(GRUBENV_BIG);
    TEST_ASSERT_NULL(ret);
}

void test_OpenNotExistingEnvblkFile(void)
{
    char *ret = grubenv_open(GRUBENV_NOT_EXISTING);
    TEST_ASSERT_NULL(ret);
}

void test_OpenCorruptedHeaderEnvblkFile(void)
{
    char *ret = grubenv_open(GRUBENV_CORRUPTED_HEADER);
    TEST_ASSERT_NULL(ret);
}

void test_OpenFullEnvblkFile(void)
{
    char *ret = grubenv_open(GRUBENV_FULL);
    TEST_ASSERT_NOT_NULL(ret);
}

void test_FindExistingVariableInBuffer(void)
{
    char *ret = grubenv_find(grubenv, EXISTING_VAR_NAME);
    TEST_ASSERT_NOT_NULL(ret);
}

void test_FindNotExistingVariableInBuffer(void)
{
    char *ret = grubenv_find(grubenv, NOT_EXISTING_VAR_NAME);
    TEST_ASSERT_NULL(ret);
}

void test_GetLineLength(void)
{
    char *ptrline = grubenv_find(grubenv, EXISTING_VAR_NAME);
    int ret = grubenv_llen(grubenv, ptrline);
    /* string is: "abc=def\n" */
    TEST_ASSERT_EQUAL_INT(8, ret);
}

void test_GetFreeSpace(void)
{
    int space = grubenv_space(grubenv);
    TEST_ASSERT_EQUAL_INT(936, space);
}

void test_GetFreeSpaceFullEnv(void)
{
    char *env = grubenv_open(GRUBENV_FULL);
    int space = grubenv_space(env);
    TEST_ASSERT_EQUAL_INT(0, space);
}

void test_RemoveVarFromBuffer(void)
{
    /* remove line with variable named 'ala' */
    char *ptrline = grubenv_find(grubenv, "ala");
    int space = grubenv_space(grubenv);

    //TEST_REPORT("TEST: grubenv before remove = \n %s\n", grubenv);
    grubenv_remove(grubenv, ptrline, space);

    int hashChars = 0;
    int nonHashChars = 0;
    int sum;
    char *ptr;

    /* set pointer at the beginning of the first variable */
    /* header is 25-bytes long */
    ptr = grubenv + sizeof(GRUBENV_HEADER);

    /* move through non-# characters */
    while (*ptr != '#') {
        ptr++;
    }

    /* number on non-# characters from grubenv file (except for header) */
    nonHashChars = 6+9+5+11+11+10;

    /* count # characters */
    while (*ptr == '#') {
        hashChars++;
        ptr++;
    }

    /* 25 for header */
    sum = GRUBENV_SIZE - nonHashChars - 25;

    TEST_ASSERT_EQUAL_INT(sum, hashChars);
}

void test_RemoveVarFromFullBuffer(void)
{
    /* open full envblk file instead of the one from setUp */
    char *env = grubenv_open(GRUBENV_FULL);
    /* remove line with variable named 'ala' */
    char *ptrline = grubenv_find(env, "ala");
    int space = grubenv_space(env);
    grubenv_remove(env, ptrline, space);

    int hashChars = 0;
    int nonHashChars = 0;
    int sum;
    char *ptr;

    /* set pointer at the beginning of the first variable */
    /* header is 25-bytes long */
    ptr = env + sizeof(GRUBENV_HEADER);

    /* move through non-# characters */
    while (*ptr != '#') {
        ptr++;
    }

    /* count # characters */
    while (*ptr == '#') {
        hashChars++;
        ptr++;
    }

    /* it was full file and 11-character length line was removed */

    TEST_ASSERT_EQUAL_INT(11, hashChars);
}

/* append not existing variable into buffer */
void test_AppendVariableInBuffer(void)
{
    grubenv_append(grubenv, "name", "value", space_global);
    char *ret = strstr(grubenv, "name=value\n");
    TEST_ASSERT_NOT_NULL(ret);
}

/* append not existing variable into buffer */
/* then write to file */
void test_GrubWrite(void)
{
    FILE *fp;
    char *buf;
    int ret;
    ret = grubenv_append(grubenv, "abc", "1234567", space_global);
    ret = grubenv_write(GRUBENV_WRITE, grubenv);

    fp = fopen(GRUBENV_WRITE, "rb");
    if (!fp) {
        TEST_REPORT("Failed to open %s file", GRUBENV_WRITE);
        TEST_FAIL();
    }

    buf = malloc(GRUBENV_SIZE);
    if (!buf) {
        TEST_REPORT("out of memory error");
        TEST_FAIL();
    }
    ret = fread(buf, 1, GRUBENV_SIZE, fp);
    if (ret != GRUBENV_SIZE) {
        TEST_REPORT("Failed read. Bytes read: %d", ret);
        TEST_FAIL();
    }
    fclose(fp);

    /* compare buffer and file content */
    ret = memcmp(grubenv, buf, GRUBENV_SIZE);
    free(buf);

    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_GrubSetNotExistingVariableInFile(void)
{
    FILE *fp;
    char *buf, *ptr;
    int ret;

    fcopy(GRUBENV_BASE, GRUBENV_SET_NOT_EXISTING_VAR);

    ret = grubenv_set(GRUBENV_SET_NOT_EXISTING_VAR, NOT_EXISTING_VAR_NAME,
            NOT_EXISTING_VAR_VALUE);

    if (ret) {
        TEST_REPORT("grubenv_set ret = %d\n",ret);
        TEST_FAIL();
    }

    buf = malloc(GRUBENV_SIZE);
    if (!buf) {
        TEST_REPORT("out of memory error\n");
        TEST_FAIL();
    }

    fp = fopen(GRUBENV_SET_NOT_EXISTING_VAR, "rb");
    if (!fp) {
        TEST_REPORT("Failed to open %s file\n", GRUBENV_SET_NOT_EXISTING_VAR);
        TEST_FAIL();
    }

    ret = fread(buf, 1, GRUBENV_SIZE, fp);
    if (ret != GRUBENV_SIZE) {
        TEST_REPORT("Failed read. Bytes read: %d]n", ret);
        TEST_FAIL();
    }
    fclose(fp);

    ptr = strstr(buf, NOT_EXISTING_VAR_STRING);
    free(buf);

    TEST_ASSERT_NOT_NULL(ptr);
}


void test_GrubSetExistingVariableInFile(void)
{
    FILE *fp;
    char *buf, *ptr;
    int ret;

    fcopy(GRUBENV_BASE, GRUBENV_SET_EXISTING_VAR);

    ret = grubenv_set(GRUBENV_SET_EXISTING_VAR, EXISTING_VAR_NAME,
            EXISTING_VAR_VALUE);
    if (ret) {
        TEST_REPORT("grubenv_set ret = %d\n",ret);
        TEST_FAIL();
    }

    buf = malloc(GRUBENV_SIZE);
    if (!buf) {
        TEST_REPORT("out of memory error\n");
        TEST_FAIL();
    }

    fp = fopen(GRUBENV_SET_EXISTING_VAR, "rb");
    if (!fp) {
        TEST_REPORT("Failed to open %s file\n", GRUBENV_SET_EXISTING_VAR);
        TEST_FAIL();
    }

    ret = fread(buf, 1, GRUBENV_SIZE, fp);
    if (ret != GRUBENV_SIZE) {
        TEST_REPORT("Failed to  read file %s. Bytes read: %d\n",
                GRUBENV_SET_EXISTING_VAR, ret);
        TEST_FAIL();
    }
    fclose(fp);

    ptr = strstr(buf, EXISTING_VAR_STRING);
    free(buf);

    TEST_ASSERT_NOT_NULL(ptr);
}

void test_GrubUnsetExistingVariableInFile(void)
{
    FILE *fp;
    char *buf, *ptr;
    int ret;

    fcopy(GRUBENV_BASE, GRUBENV_UNSET_EXISTING_VAR);

    ret = grubenv_unset(GRUBENV_UNSET_EXISTING_VAR, EXISTING_VAR_NAME);
    if (ret) {
        TEST_REPORT("grubenv_unset ret = %d",ret);
        TEST_FAIL();
    }

    buf = malloc(GRUBENV_SIZE);
    if (!buf) {
        TEST_REPORT("out of memory error");
        TEST_FAIL();
    }

    fp = fopen(GRUBENV_UNSET_EXISTING_VAR, "rb");
    if (!fp) {
        TEST_REPORT("Failed to open %s file\n", GRUBENV_UNSET_EXISTING_VAR);
        TEST_FAIL();
    }

    ret = fread(buf, 1, GRUBENV_SIZE, fp);
    if (ret != GRUBENV_SIZE) {
        TEST_REPORT("Failed read. Bytes read: %d", ret);
        TEST_FAIL();
    }
    fclose(fp);

    ptr = strstr(buf, EXISTING_VAR_NAME "=");
    free(buf);

    TEST_ASSERT_NULL(ptr);
}

void test_GrubUnsetNotExistingVariableInFile(void)
{
    FILE *fp;
    char *buf1, *buf2, *ptr;
    int ret;

    fcopy(GRUBENV_BASE, GRUBENV_UNSET_NOT_EXISTING_VAR);

    buf1 = malloc(GRUBENV_SIZE);
    if (!buf1) {
        TEST_REPORT("out of memory error");
        TEST_FAIL();
    }

    /* get file content before test */
    fp = fopen(GRUBENV_UNSET_NOT_EXISTING_VAR, "rb");
    if (!fp) {
        TEST_REPORT("Failed to open %s file\n",
                GRUBENV_UNSET_NOT_EXISTING_VAR);
        TEST_FAIL();
    }

    ret = fread(buf1, 1, GRUBENV_SIZE, fp);
    if (ret != GRUBENV_SIZE) {
        TEST_REPORT("Failed read %s file. Bytes read: %d",
                GRUBENV_UNSET_NOT_EXISTING_VAR, ret);
        TEST_FAIL();
    }
    fclose(fp);

    ret = grubenv_unset(GRUBENV_UNSET_NOT_EXISTING_VAR, NOT_EXISTING_VAR_NAME);
    if (ret) {
        TEST_REPORT("grubenv_unset ret = %d",ret);
        TEST_FAIL();
    }

    /* get file content after test */
    buf2 = malloc(GRUBENV_SIZE);
    if (!buf2) {
        TEST_REPORT("out of memory error");
        TEST_FAIL();
    }

    fp = fopen(GRUBENV_UNSET_NOT_EXISTING_VAR, "rb");
    if (!fp) {
        TEST_REPORT("Failed to open %s file\n", GRUBENV_WRITE);
        TEST_FAIL();
    }

    ret = fread(buf2, 1, GRUBENV_SIZE, fp);
    if (ret != GRUBENV_SIZE) {
        TEST_REPORT("Failed read. Bytes read: %d", ret);
        TEST_FAIL();
    }
    fclose(fp);

    /* file content should not change */
    ptr = memcmp(buf1, buf2, GRUBENV_SIZE);
    free(buf1);
    free(buf2);

    TEST_ASSERT_NULL(ptr);
}
