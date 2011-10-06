/*  SSNES - A Super Nintendo Entertainment System (SNES) Emulator frontend for libsnes.
 *  Copyright (C) 2010-2011 - Hans-Kristian Arntzen
 *
 *  Some code herein may be based on code found in BSNES.
 * 
 *  SSNES is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  SSNES is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with SSNES.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "thread.h"
#include <stdlib.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#endif

#ifdef _WIN32

struct slock
{
   CRITICAL_SECTION crit;
};

slock_t *slock_new(void)
{
   slock_t *lock = calloc(1, sizeof(*lock));
   if (!lock)
      return NULL;

   Initiali
}


#else

struct thread_data
{
   void (*func)(void*);
   void *userdata;
};

struct sthread
{
   pthread_t id;
};

static void *thread_wrap(void *data_)
{
   struct thread_data *data = data_;
   data->func(data->userdata);
   free(data);
   pthread_exit(NULL);
}

sthread_t *sthread_create(void (*thread_func)(void*), void *userdata)
{
   sthread_t *thr = calloc(1, sizeof(*thr));
   if (!thr)
      return NULL;

   struct thread_data *data = calloc(1, sizeof(*data));
   if (!data)
   {
      free(thr);
      return NULL;
   }

   data->func = thread_func;
   data->userdata = userdata;

   if (pthread_create(&thr->id, NULL, thread_wrap, data) < 0)
   {
      free(data);
      free(thr);
      return NULL;
   }

   return thr;
}

void sthread_join(sthread_t *thread)
{
   pthread_join(thread->id, NULL);
   free(thread);
}

struct slock
{
   pthread_mutex_t lock;
};

slock_t *slock_new(void)
{
   slock_t *lock = calloc(1, sizeof(*lock));
   if (!lock)
      return NULL;

   if (pthread_mutex_init(&lock->lock, NULL) < 0)
   {
      free(lock);
      return NULL;
   }

   return lock;
}

void slock_free(slock_t *lock)
{
   pthread_mutex_destroy(&lock->lock);
   free(lock);
}

void slock_lock(slock_t *lock)
{
   pthread_mutex_lock(&lock->lock);
}

void slock_unlock(slock_t *lock)
{
   pthread_mutex_unlock(&lock->lock);
}

struct scond
{
   pthread_cond_t cond;
};

scond_t *scond_new(void)
{
   scond_t *cond = calloc(1, sizeof(*cond));
   if (!cond)
      return NULL;

   if (pthread_cond_init(&cond->cond, NULL) < 0)
   {
      free(cond);
      return NULL;
   }

   return cond;
}

void scond_free(scond_t *cond)
{
   pthread_cond_destroy(&cond->cond);
   free(cond);
}

void scond_wait(scond_t *cond, slock_t *lock)
{
   pthread_cond_wait(&cond->cond, &lock->lock);
}

bool scond_wait_timeout(scond_t *cond, slock_t *lock, unsigned timeout_ms)
{
   struct timespec now;
   clock_gettime(CLOCK_REALTIME, &now);

   now.tv_sec += timeout_ms / 1000;
   now.tv_nsec += timeout_ms * 1000000L;

   now.tv_sec += now.tv_nsec / 1000000000L;
   now.tv_nsec = now.tv_nsec % 1000000000L;

   int ret = pthread_cond_timedwait(&cond->cond, &lock->lock, &now);
   return ret == 0;
}

void scond_signal(scond_t *cond)
{
   pthread_cond_signal(&cond->cond);
}

#endif
