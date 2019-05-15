/**
 * @file dummy_struct.h
 *
 * Because C doesn't allow empty structs, this provides a placeholder for
 * libraries which allow optional exernal structs to be injected via macros.
 */

#ifndef _DUMMY_STRUCT_H_
#define _DUMMY_STRUCT_H_

/**
 * A placeholder struct, not intended to actually be used.
 */
typedef struct DummyStruct {
  uint8_t dummy;
} DummyStruct;

#endif
