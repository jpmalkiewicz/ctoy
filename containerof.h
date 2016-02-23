#ifndef _CONTAINEROF_H_
#define _CONTAINEROF_H_

#define containerof(ptr, type, member)     (type*)((char*)(1?(ptr): &((type*)0)->member)-offsetof(type, member))

#endif /* _CONTAINEROF_H_ */
