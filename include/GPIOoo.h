/*
 * GPIOoo.h
 *
 *  Created on: Jun 8, 2013
 *      Author: jacek
 */

#ifndef GPIOOO_H_
#define GPIOOO_H_

#include "GPIOpin.h"

class BeagleGoo;

/**
 * @brief Object-oriented implementation of GPIO
 * Class defines interface for object-oriented handling of GPIO operations.
 * Should be used as parent class for platform-specific implementations.
 */
class GPIOoo
{
	protected:
		friend class GPIOpin;
		static class BeagleGoo inst();
	public:
		/**
		 * Options flags for GPIO pin allocation.
		 */
		enum gpioFlags
		{
			gpioFlagsNone = 0, //!< gpioFlagsNone - No flags
			gpioExclusive = 1 //!< gpioExclusive - GPIOs allocated exclusively. Allocating with this flag disables sharing with other blocks.
		};

		/**
		 * Enum defines semantics of write operation to GPIOs.
		 */
		enum gpioWriteSemantics
		{
			//pin write semantics
			gpioWrite = 1, //!< gpioWrite - Simple write to the port. Prone to race conditions, offers no multi-process safety.
						   //!< State of the port can be affected by writes to the pins on the same GPIO port.
			gpioWriteAtomic, //!< gpioWriteAtomic - Atomic write to the port. Write to the port must be guaranteed to be successful and effective.
			gpioWriteSetBeforeClear, //!< gpioWriteSetBeforeClear - In two-step implementation of writing to the pins,
									 //!< pins with value '1' are set before pins with value '0' are cleared.
									 //!< For a short period of time the state of the pins in the GPIO block will
									 //!< be equal to bitwise OR of the previous and next states.
			gpioWriteClearBeforeSet //!< gpioWriteClearBeforeSet - In two-step implementation of writing to the pins,
									//!< pins with value '0' are cleared before pins with value '1' are set.
									//!< For a short period of time the state of the pins in the GPIO block will
									//!< be equal to bitwise AND of the previous and next states.
		};

		virtual ~GPIOoo();

		static GPIOoo *getInstance();

		/**
		 * @brief Simplified version of GPIOpin::claim()
		 * Simplified version of GPIOpin::claim(). Assumes \a gpioWrite semantics and no options.
		 * @param names - an array of system names of pins in the block. Pin names are implementation-dependent.
		 *                The array should have \a num entries.
		 * @param num - number of pins in the block.
		 * @return
		 */
		virtual GPIOpin *claim(char *names[], int num)
		{
			return claim(names, num, gpioWrite, gpioFlagsNone);
		}
		;
		/**
		 * @brief Method allocates GPIO pins and returns a GPIOPin object.
		 * Method allocates a block of pins specified by names passed in \a names argument.
		 * Number of pins in the block is determined by \a num argument. If flag \a gpioExclusive
		 * is present, only non-allocated pins can be allocated, the pins are marked as exclusive
		 * and can not be shared with other blocks. If the \a gpioExclusive flag has not been specified,
		 * pins can be shared with other blocks. If sharing conflict has been detected, no pins must be
		 * allocated and the method must return NULL.
		 * Argument \a semantics determines how write operations should be handled. If the requested write
		 * semantics is not supported by the hardware platfoom, no pins must be allocated and method must
		 * return NULL.
		 * @param names - an array of system names of pins in the block. Pin names are implementation-dependent.
		 *                The array should have \a num entries.
		 * @param num - number of pins in the block.
		 * @param semantics - write semantics. Uses constants defined by \a gpioWriteSemantics enum.
		 * @param flags - Flags governing pin allocation. Defined by \a gpioFlags enum. Optional parameter. Default value is no flags.
		 * @return
		 */
		virtual GPIOpin *claim(char *names[], int num,
				gpioWriteSemantics semantics,
				gpioFlags flags = gpioFlagsNone) = 0;

		/**
		 * @brief Releases a block of GPIO pins.
		 * Method releases allocated block of GPIO pins. Methods releases memory allocated for the block, destroys the object
		 * and assigns NULL to the referencing variable.
		 * @param gpio - pointer to a variable with reference to an object describing a block of GPIO pins.
		 */
		virtual void release(GPIOpin **gpio)=0;
};

#endif /* GPIOOO_H_ */
