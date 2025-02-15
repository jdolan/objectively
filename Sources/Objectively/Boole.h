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

#pragma once

#include <Objectively/Object.h>

/**
 * @file
 * @brief A wrapper for placing boolean primitives into collections, etc.
 */

typedef struct Boole Boole;
typedef struct BooleInterface BooleInterface;

/**
 * @brief A wrapper for placing boolean primitives into collections, etc.
 * @extends Object
 * @ingroup JSON
 */
struct Boole {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	BooleInterface *interface;

	/**
	 * @brief The backing bool.
	 */
	bool value;
};

/**
 * @brief The Boole interface.
 */
struct BooleInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 * @fn Boole *Boole::False(void)
	 * @return The `false` Boole.
	 * @memberof Boole
	 */
	Boole *(*False)(void);

	/**
	 * @static
	 * @fn Boole *Boole::True(void)
	 * @return The `true` Boole.
	 * @memberof Boole
	 */
	Boole *(*True)(void);

	/**
	 * @static
	 * @fn Boole *Boole::valueof(bool value)
	 * @param value The boolean.
	 * @return The Boole representation of `value`.
	 * @memberof Boole
	 */
	Boole *(*valueof)(bool value);
};

/**
 * @fn Class *Boole::boole(void)
 * @brief The Boole archetype.
 * @return The Boole Class.
 * @memberof Boole
 */
OBJECTIVELY_EXPORT Class *_Boole(void);
