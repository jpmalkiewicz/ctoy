#ifndef _TV_H_
#define _TV_H_

struct timeval;

void tvdiff(struct timeval *td, const struct timeval *t1, const struct timeval *t2);

#endif /* _TV_H_ */
