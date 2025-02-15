/*
 * Objectively: Ultra-lightweight object oriented framework for GNU C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "Config.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "MutableArray.h"

#if defined(__APPLE__)

/**
 * @brief BSD qsort_r.
 */
static int _quicksort(void *data, const void *a, const void *b) {
	return ((Comparator) data)(*((const ident *) a), *((const ident *) b));
}

void quicksort(ident base, size_t count, size_t size, Comparator comparator, ident data) {
	qsort_r(base, count, size, comparator, _quicksort);
}

#elif defined(_WIN32)

/**
 * @brief WIN32 qsort_s.
 */
static int _quicksort(void *data, const void *a, const void *b) {
	return ((Comparator) data)(*((const ident *) a), *((const ident *) b));
}

void quicksort(ident base, size_t count, size_t size, Comparator comparator, ident data) {
	qsort_s(base, count, size, _quicksort, comparator);
}

#else

/**
 * @brief GNU qsort_r.
 */
static int _quicksort(const void *a, const void *b, void *data) {
	return ((Comparator) data)(*((const ident *) a), *((const ident *) b));
}

void quicksort(ident base, size_t count, size_t size, Comparator comparator, ident data) {
	qsort_r(base, count, size, _quicksort, comparator);
}

#endif

#define _Class _MutableArray

#define ARRAY_CHUNK_SIZE 64

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Array *this = (Array *)  self;

	MutableArray *copy = $(alloc(MutableArray), initWithCapacity, this->count);

	$(copy, addObjectsFromArray, this);

	return (Object *) copy;
}

#pragma mark - MutableArray

/**
 * @fn void MutableArray::addObject(MutableArray *self, const ident obj)
 * @memberof MutableArray
 */
static void addObject(MutableArray *self, const ident obj) {

	Array *array = (Array *) self;
	if (array->count == self->capacity) {

		self->capacity += ARRAY_CHUNK_SIZE;

		if (array->elements) {
			array->elements = realloc(array->elements, self->capacity * sizeof(ident));
		} else {
			array->elements = calloc(self->capacity, sizeof(ident));
		}

		assert(array->elements);
	}

	array->elements[array->count++] = retain(obj);
}

/**
 * @fn void MutableArray::addObjects(MutableArray *self, const ident obj, ...)
 * @memberof MutableArray
 */
static void addObjects(MutableArray *self, const ident obj, ...) {

	va_list args;
	va_start(args, obj);

	ident object = obj;
	while (object) {
		$(self, addObject, object);
		object = va_arg(args, ident);
	}

	va_end(args);
}

/**
 * @fn void MutableArray::addObjectsFromArray(MutableArray *self, const Array *array)
 * @memberof MutableArray
 */
static void addObjectsFromArray(MutableArray *self, const Array *array) {

	if (array) {
		for (size_t i = 0; i < array->count; i++) {
			$(self, addObject, array->elements[i]);
		}
	}
}

/**
 * @fn MutableArray *MutableArray::array(void)
 * @memberof MutableArray
 */
static MutableArray *array(void) {

	return $(alloc(MutableArray), init);
}

/**
 * @fn MutableArray *MutableArray::arrayWithCapacity(size_t capacity)
 * @memberof MutableArray
 */
static MutableArray *arrayWithCapacity(size_t capacity) {

	return $(alloc(MutableArray), initWithCapacity, capacity);
}

/**
 * @fn void MutableArray::filter(MutableArray *self, Predicate predicate, ident data)
 * @memberof MutableArray
 */
static void filter(MutableArray *self, Predicate predicate, ident data) {

	assert(predicate);

	for (size_t i = 0; i < self->array.count; i++) {
		if (predicate(self->array.elements[i], data) == false) {
			$(self, removeObjectAtIndex, i--);
		}
	}
}

/**
 * @fn MutableArray *MutableArray::init(MutableArray *self)
 * @memberof MutableArray
 */
static MutableArray *init(MutableArray *self) {

	return $(self, initWithCapacity, 0);
}

/**
 * @fn MutableArray *MutableArray::initWithCapacity(MutableArray *self, size_t capacity)
 * @memberof MutableArray
 */
static MutableArray *initWithCapacity(MutableArray *self, size_t capacity) {

	self = (MutableArray *) super(Object, self, init);
	if (self) {

		self->capacity = capacity;
		if (self->capacity) {

			self->array.elements = calloc(self->capacity, sizeof(ident));
			assert(self->array.elements);
		}
	}

	return self;
}

/**
 * @fn void MutableArray::insertObjectAtIndex(MutableArray *self, ident obj, size_t index)
 * @memberof MutableArray
 */
static void insertObjectAtIndex(MutableArray *self, ident obj, size_t index) {

	assert(index <= self->array.count);

	$(self, addObject, obj);

	for (size_t i = self->array.count - 1; i > index; i--) {
		self->array.elements[i] = self->array.elements[i - 1];
	}

	self->array.elements[index] = obj;
}

/**
 * @fn void MutableArray::removeAllObjects(MutableArray *self)
 * @memberof MutableArray
 */
static void removeAllObjects(MutableArray *self) {

	while (self->array.count) {
		$(self, removeLastObject);
	}
}

/**
 * @fn void MutableArray::removeAllObjectsWithEnumerator(MutableArray *self, ArrayEnumerator enumerator, ident data)
 * @memberof MutableArray
 */
static void removeAllObjectsWithEnumerator(MutableArray *self, ArrayEnumerator enumerator, ident data) {

	assert(enumerator);

	while (self->array.count) {

		enumerator((Array *) self, $((Array *) self, lastObject), data);

		$(self, removeLastObject);
	}
}

/**
 * @fn void MutableArray::removeLastObject(MutableArray *self)
 * @memberof MutableArray
 */
static void removeLastObject(MutableArray *self) {

	if (self->array.count) {
		$(self, removeObjectAtIndex, self->array.count - 1);
	}
}

/**
 * @fn void MutableArray::removeObject(MutableArray *self, const ident obj)
 * @memberof MutableArray
 */
static void removeObject(MutableArray *self, const ident obj) {

	const ssize_t index = $((Array *) self, indexOfObject, obj);
	if (index > -1) {
		$(self, removeObjectAtIndex, index);
	}
}

/**
 * @fn void MutableArray::removeObjectAtIndex(MutableArray *self, size_t index)
 * @memberof MutableArray
 */
static void removeObjectAtIndex(MutableArray *self, size_t index) {

	assert(index < self->array.count);

	release(self->array.elements[index]);

	for (size_t i = index; i < self->array.count - 1; i++) {
		self->array.elements[i] = self->array.elements[i + 1];
	}

	self->array.count--;
}

/**
 * @fn void MutableArray::setObjectAtIndex(MutableArray *self, const ident obj, size_t index)
 * @memberof MutableArray
 */
static void setObjectAtIndex(MutableArray *self, const ident obj, size_t index) {

	assert(index < self->array.count);

	retain(obj);

	release(self->array.elements[index]);

	self->array.elements[index] = obj;
}

/**
 * @fn void MutableArray::sort(MutableArray *self, Comparator comparator)
 * @memberof MutableArray
 */
static void sort(MutableArray *self, Comparator comparator) {
	quicksort(self->array.elements, self->array.count, sizeof(ident), comparator, NULL);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->copy = copy;

	((MutableArrayInterface *) clazz->interface)->addObject = addObject;
	((MutableArrayInterface *) clazz->interface)->addObjects = addObjects;
	((MutableArrayInterface *) clazz->interface)->addObjectsFromArray = addObjectsFromArray;
	((MutableArrayInterface *) clazz->interface)->array = array;
	((MutableArrayInterface *) clazz->interface)->arrayWithCapacity = arrayWithCapacity;
	((MutableArrayInterface *) clazz->interface)->filter = filter;
	((MutableArrayInterface *) clazz->interface)->init = init;
	((MutableArrayInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
	((MutableArrayInterface *) clazz->interface)->insertObjectAtIndex = insertObjectAtIndex;
	((MutableArrayInterface *) clazz->interface)->removeAllObjects = removeAllObjects;
	((MutableArrayInterface *) clazz->interface)->removeAllObjectsWithEnumerator = removeAllObjectsWithEnumerator;
	((MutableArrayInterface *) clazz->interface)->removeLastObject = removeLastObject;
	((MutableArrayInterface *) clazz->interface)->removeObject = removeObject;
	((MutableArrayInterface *) clazz->interface)->removeObjectAtIndex = removeObjectAtIndex;
	((MutableArrayInterface *) clazz->interface)->setObjectAtIndex = setObjectAtIndex;
	((MutableArrayInterface *) clazz->interface)->sort = sort;
}

/**
 * @fn Class *MutableArray::_MutableArray(void)
 * @memberof MutableArray
 */
Class *_MutableArray(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "MutableArray",
			.superclass = _Array(),
			.instanceSize = sizeof(MutableArray),
			.interfaceOffset = offsetof(MutableArray, interface),
			.interfaceSize = sizeof(MutableArrayInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
