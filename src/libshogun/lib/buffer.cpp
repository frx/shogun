/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2011 Shashwat Lal Das
 * Copyright (C) 2011 Berlin Institute of Technology and Max-Planck-Society
 */

#include "lib/buffer.h"

using namespace shogun;

template <class T>
ParseBuffer<T>::ParseBuffer(int32_t size)
{
	int32_t buffer_size_features=size*1024*1024;
	buffer_size=100;		// HACK!
	ex_buff=new example<T>[buffer_size];
	printf("initializing fv_buff with size: %d\n", buffer_size_features);
	fv_buff=new T[buffer_size_features/sizeof(T)];
	ex_used=new E_IS_EXAMPLE_USED[buffer_size];
	
	ex_in_use_mutex=new pthread_mutex_t[buffer_size];
	ex_in_use_cond=new pthread_cond_t[buffer_size];
	
	ex_write_index=0;
	ex_read_index=-1;

	fv_write_index=0;

	for (int32_t i=0; i<buffer_size; i++)
	{
		ex_used[i] = E_EMPTY;
		pthread_cond_init(&ex_in_use_cond[i], NULL);
		pthread_mutex_init(&ex_in_use_mutex[i], NULL);
	}
}

template <class T>
ParseBuffer<T>::~ParseBuffer()
{
	delete[] ex_buff;
	delete[] fv_buff;
	delete[] ex_used;

	for (int32_t i=0; i<buffer_size; i++)
	{
		pthread_mutex_destroy(&ex_in_use_mutex[i]);
		pthread_cond_destroy(&ex_in_use_cond[i]);
	}
}

template <class T>
void ParseBuffer<T>::inc_read_index()
{
	ex_read_index=(ex_read_index + 1) % buffer_size;
}

template <class T>
void ParseBuffer<T>::inc_write_index(int32_t len)
{
	ex_write_index=(ex_write_index + 1) % buffer_size;
	fv_write_index=fv_write_index + len;
}

template <class T>
int32_t ParseBuffer<T>::write_example(example<T> *ex)
{
	ex_buff[ex_write_index].label = ex->label;
	ex_buff[ex_write_index].fv.vector = &fv_buff[fv_write_index];
	ex_buff[ex_write_index].fv.length = ex->fv.length;
		
	//Write feature vector into the fv buffer
	//First we should check if the remaining length is enough to accommodate the fv
	//Then realloc/expand if necessary

	for (int i=0; i<ex->fv.length; i++)
	{
		fv_buff[fv_write_index+i] = ex->fv.vector[i];
	}

	ex_used[ex_write_index] = E_NOT_USED;
	inc_write_index(ex->fv.length);

	return 1;					// Should check for size and return 0 if insufficient
}

template <class T>
example<T>* ParseBuffer<T>::get_example()
{
	example<T>* ex;
	
	if (ex_read_index >= 0)
	{
		ex = &ex_buff[ex_read_index];
		return ex;
	}
	else
		return NULL;
}

template <class T>
example<T>* ParseBuffer<T>::fetch_example()
{
	example<T> *ex;
	int32_t current_index = ex_read_index;
	// Because read index will change after get_example

	pthread_mutex_lock(&ex_in_use_mutex[current_index]);
		
	if (ex_used[current_index] == E_NOT_USED)
		ex = get_example();
	else
		ex = NULL;
	
	pthread_mutex_unlock(&ex_in_use_mutex[current_index]);
	return ex;
}

template <class T>
int32_t ParseBuffer<T>::copy_example(example<T> *ex)
{
	// Check this mutex call.. It should probably be locked regardless of ex in use

	int32_t ret;
	int32_t current_index = ex_write_index;

	pthread_mutex_lock(&ex_in_use_mutex[current_index]);
	while (ex_used[ex_write_index] == E_NOT_USED)
	{
		pthread_cond_wait(&ex_in_use_cond[ex_write_index], &ex_in_use_mutex[ex_write_index]);
	}
	
	ret=write_example(ex);

	if (ex_read_index < 0)
		ex_read_index = 0;
	
	pthread_mutex_unlock(&ex_in_use_mutex[current_index]);
	return ret;
}

template <class T>
void ParseBuffer<T>::finalize_example()
{
	pthread_mutex_lock(&ex_in_use_mutex[ex_read_index]);
	ex_used[ex_read_index] = E_USED;
	pthread_cond_signal(&ex_in_use_cond[ex_read_index]);
	pthread_mutex_unlock(&ex_in_use_mutex[ex_read_index]);
	
	inc_read_index();

}
