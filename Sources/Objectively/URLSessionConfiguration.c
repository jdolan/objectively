/*
 * URLSessionConfiguration.c
 *
 *  Created on: Nov 30, 2014
 *      Author: jdolan
 */


#include <assert.h>

#include <Objectively/URLSessionConfiguration.h>

#define __class __URLSessionConfiguration

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSessionConfiguration *this = (URLSessionConfiguration *) self;

	release(this->credentials.username);
	release(this->credentials.password);
	release(this->httpHeaders);

	super(Object, self, dealloc);
}

#pragma mark - URLSessionConfigurationInterface

/**
 * @see URLSessionConfigurationInterface::init(URLSessionConfiguration *)
 */
static URLSessionConfiguration *init(URLSessionConfiguration *self) {

	self = (URLSessionConfiguration *) super(Object, self, init);
	if (self) {
		// wut
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((URLSessionConfigurationInterface *) clazz->interface)->init = init;
}

Class __URLSessionConfiguration = {
	.name = "URLSessionConfiguration",
	.superclass = &__Object,
	.instanceSize = sizeof(URLSessionConfiguration),
	.interfaceOffset = offsetof(URLSessionConfiguration, interface),
	.interfaceSize = sizeof(URLSessionConfigurationInterface),
	.initialize = initialize,
};

#undef __class
