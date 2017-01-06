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

#include <assert.h>

#include <Objectively/Error.h>
#include <Objectively/Hash.h>
#include <Objectively/MutableString.h>

#define _Class _Error

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Error *this = (Error *) self;
	Error *that = $(alloc(Error), initWithDomain, this->domain, this->code, this->message);

	return (Object *) that;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Error *this = (Error *) self;

	release(this->domain);
	release(this->message);

	super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	Error *this = (Error *) self;

	MutableString *desc = $(alloc(MutableString), init);

	$(desc, appendFormat, "%ls: %d", this->domain->chars, this->code);

	if (this->message) {
		$(desc, appendFormat, ": %ls", this->message->chars);
	}

	return (String *) desc;
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	Error *this = (Error *) self;

	int hash = HASH_SEED;

	hash = HashForInteger(hash, this->code);
	hash = HashForObject(hash, this->domain);
	hash = HashForObject(hash, this->message);

	return hash;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && (self->clazz == other->clazz)) {

		const Error *this = (Error *) self;
		const Error *that = (Error *) other;

		if (this->code == that->code) {

			if ($((Object * ) this->domain, isEqual, (Object * ) that->domain)) {

				if (this->message) {
					return $((Object * ) this->message, isEqual, (Object * ) that->message);
				}

				return that->message == NULL;
			}
		}
	}

	return false;
}

#pragma mark - Error

/**
 * @fn Error *Error::initWithDomain(Error *self, String *domain, int code, String *message)
 * @memberof Error
 */
static Error *initWithDomain(Error *self, String *domain, int code, String *message) {

	assert(domain);

	self = (Error *) super(Object, self, init);
	if (self) {

		self->domain = retain(domain);
		self->code = code;

		if (message) {
			self->message = retain(message);
		}
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->def->interface;

	object->copy = copy;
	object->dealloc = dealloc;
	object->description = description;
	object->hash = hash;
	object->isEqual = isEqual;

	ErrorInterface *error = (ErrorInterface *) clazz->def->interface;

	error->initWithDomain = initWithDomain;
}

Class *_Error(void) {
	static Class clazz;
	static Once once;
	
	do_once(&once, {
		clazz.name = "Error";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Error);
		clazz.interfaceOffset = offsetof(Error, interface);
		clazz.interfaceSize = sizeof(ErrorInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
