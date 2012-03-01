#include "rwlock.h"

#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

struct rwlock {
	pthread_mutex_t wrlock;
	pthread_mutex_t rdlock;
	pthread_cond_t rdcond;
	size_t nreader;
};

// FIXME Error Check!!!

int
rwlock_create(struct rwlock **lop) {
	struct rwlock *lo = malloc(sizeof(*lo));
	pthread_mutex_init(&lo->wrlock, NULL);
	pthread_mutex_init(&lo->rdlock, NULL);
	pthread_cond_init(&lo->rdcond, NULL);
	lo->nreader = 0;
	*lop = lo;
	return 0;
}

int
rwlock_delete(struct rwlock *lo) {
	pthread_cond_destroy(&lo->rdcond);
	pthread_mutex_destroy(&lo->rdlock);
	pthread_mutex_destroy(&lo->wrlock);
	free(lo);
	return 0;
}

static void
_mutex_unlock(void *ptr) {
	pthread_mutex_t *mx = ptr;
	pthread_mutex_unlock(mx);
}

int
rwlock_rdlock(struct rwlock *lo) {
	pthread_mutex_lock(&lo->rdlock);
	pthread_cleanup_push(_mutex_unlock, &lo->rdlock);
	while (lo->nreader == 0 && pthread_mutex_trylock(&lo->wrlock) == EBUSY) {
		pthread_cond_wait(&lo->rdcond, &lo->rdlock);
	}
	pthread_cleanup_pop(0);
	lo->nreader++;
	pthread_mutex_unlock(&lo->rdlock);
	return 0;
}

int
rwlock_wrlock(struct rwlock *lo) {
	return pthread_mutex_lock(&lo->wrlock);
}

int
rwlock_unlock(struct rwlock *lo) {
	pthread_mutex_lock(&lo->rdlock);
	if (lo->nreader == 0 || lo->nreader == 1) {
		lo->nreader = 0;
		pthread_mutex_unlock(&lo->wrlock);
	} else {
		lo->nreader--;
	}
	pthread_cond_broadcast(&lo->rdcond);
	pthread_mutex_unlock(&lo->rdlock);
	return 0;
}
