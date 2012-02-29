#ifndef __RWLOCK_H_
#define __RWLOCK_H_
struct rwlock;

int rwlock_create(struct rwlock **lo);
int rwlock_delete(struct rwlock *lo);

int rwlock_rdlock(struct rwlock *lo);
int rwlock_wrlock(struct rwlock *lo);
int rwlock_unlock(struct rwlock *lo);
#endif
