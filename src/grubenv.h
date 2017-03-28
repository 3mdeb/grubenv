/*
 * Author: Maciej Pijanowski, maciej.pijanowski@3mdeb.com
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

#define GRUBENV_SIZE 1024 /* bytes */
#define GRUBENV_HEADER "# GRUB Environment Block\n"

#define ERROR(M, ...) fprintf(stderr, "ERROR (%d): %s:%d: " M ": \n",\
errno, __FILE__, __LINE__, ##__VA_ARGS__)


/* Internal functions */
/* Exposed here for unit tests during development */
char *grubenv_open(char *grubenv_file);
char *grubenv_find(char *grubenv, char *name);
void grubenv_remove(char *grubenv, char *ptrline, int space);
void grubenv_close(char *grubenv);
int grubenv_append(char *grubenv, char *name, char *value, int space);
int grubenv_space(char *grubenv);
int grubenv_write(char *grubenv_file, char *grub_env);
int grubenv_llen(char *grubenv, char *ptrline);

/* only these should be called from external */
int grubenv_set(char *grubenv_file, char *name, char *value);
int grubenv_unset(char *grubenv_file, char *name);
