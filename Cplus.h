/**
 ******************************************************************************
 * @file Cplus.h
 * @author Paolo Sassi
 * @date 22 January 2016
 * @brief Macros for using class-like semantics in C.
 ******************************************************************************
 */

#ifndef CPLUS_h
#define CPLUS_h

/**
 * @brief Macros for declaring classes.
 */
#define CLASS(name_)          \
   typedef struct name_ name_;\
   struct name_ {
#define METHODS };
#define END_CLASS

/**
 * @brief Macros for declaring subclasses.
 */
#define SUBCLASS(class_, superclass_) \
   CLASS(class_)                      \
      superclass_ super_;

#endif
