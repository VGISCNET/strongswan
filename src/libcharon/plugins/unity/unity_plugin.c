/*
 * Copyright (C) 2012 Martin Willi
 * Copyright (C) 2012 revosec AG
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include "unity_plugin.h"
#include "unity_handler.h"
#include "unity_narrow.h"

#include <daemon.h>
#include <hydra.h>

typedef struct private_unity_plugin_t private_unity_plugin_t;

/**
 * private data of unity_plugin
 */
struct private_unity_plugin_t {

	/**
	 * public functions
	 */
	unity_plugin_t public;

	/**
	 * Handler for UNITY configuration attributes
	 */
	unity_handler_t *handler;

	/**
	 * Traffic selector narrower, for Unity Split-Includes
	 */
	unity_narrow_t *narrower;
};

METHOD(plugin_t, get_name, char*,
	private_unity_plugin_t *this)
{
	return "unity";
}

METHOD(plugin_t, destroy, void,
	private_unity_plugin_t *this)
{
	charon->bus->remove_listener(charon->bus, &this->narrower->listener);
	this->narrower->destroy(this->narrower);
	hydra->attributes->remove_handler(hydra->attributes, &this->handler->handler);
	this->handler->destroy(this->handler);
	free(this);
}

/*
 * see header file
 */
plugin_t *unity_plugin_create()
{
	private_unity_plugin_t *this;

	INIT(this,
		.public = {
			.plugin = {
				.get_name = _get_name,
				.reload = (void*)return_false,
				.destroy = _destroy,
			},
		},
		.handler = unity_handler_create(),
	);
	hydra->attributes->add_handler(hydra->attributes, &this->handler->handler);

	this->narrower = unity_narrow_create(this->handler),
	charon->bus->add_listener(charon->bus, &this->narrower->listener);

	return &this->public.plugin;
}
