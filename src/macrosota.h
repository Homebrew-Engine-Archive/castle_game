#ifndef MACROSOTA_H_
#define MACROSOTA_H_

#define NAMESPACE_BEGIN(ns) namespace ns {
#define NAMESPACE_END(ns) }

#define UNUSED(name) (void)name;

#define MAKE_COMPILER_HAPPY(name) (void)name;

#define DISALLOW_COPY(name) name(const name &) { }
#define DISALLOW_ASSIGN(name) name &operator=(const name &) { }

#endif
