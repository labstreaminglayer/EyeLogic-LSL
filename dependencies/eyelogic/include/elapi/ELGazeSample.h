// -----------------------------------------------------------------------
// Copyright (C) 2019-2022, EyeLogic GmbH
//
// Permission is hereby granted, free of charge, to any person or
// organization obtaining a copy of the software and accompanying
// documentation covered by this license (the "Software") to use,
// reproduce, display, distribute, execute, and transmit the Software,
// and to prepare derivative works of the Software, and to permit
// third-parties to whom the Software is furnished to do so.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
// NON-INFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE
// DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER
// LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
// OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// -----------------------------------------------------------------------

/**
 * @file ELGazeSample.h
 *
 * @brief The file specifies the C++ container for a gaze sample.
 */

#pragma once

#include "ELExports.hpp"

#include <cinttypes>

#ifdef _WIN32
#define STDCALL __stdcall
#else
#define STDCALL
#endif

/** @brief namespace for C++ API calls */
namespace elapi
{
/** @brief marker for an invalid double value */
extern EL_EXPORT const double ELInvalidValue;

/**
 * @brief contains all information about the state of the eyes at a specific time
 */
struct ELGazeSample {
    /** @brief timepoint when data was acquired in microseconds after EPOCH */
    int64_t timestampMicroSec;

    /** @brief increasing GazeSample index */
    int32_t index;

    /**
     * @brief X coordinate of binocular point of regard on the stimulus plane, check porRawX !=
     * InvalidValue before using it.
     */
    double porRawX;

    /**
     * @brief Y coordinate of binocular point of regard on the stimulus plane, check porRawX !=
     * InvalidValue also before using porRawY.
     */
    double porRawY;

    /**
     * @brief X coordinate of filtered binocular point of regard on the stimulus plane, check
     * porFilteredX != InvalidValue before using it.
     */
    double porFilteredX;

    /**
     * @brief Y coordinate of filtered binocular point of regard on the stimulus plane, also check
     * porFilteredX != InvalidValue before using porFilteredY.
     */
    double porFilteredY;

    /**
     * @brief X coordinate of monocular point of regard of the left eye, check porLeftX !=
     * InvalidValue before using it.
     */
    double porLeftX;
    /**
     * @brief Y coordinate of monocular point of regard of the left eye, also check porLeftX !=
     * InvalidValue before using porLeftY.
     */
    double porLeftY;

    /**
     * @brief position of the left eye in device coordinates, unit is mm
     * - (0, 0, 0) is in the center of the device
     * - x-coordinate: positive towards the right side of the screen
     *
     * check eyePositionLeftX != InvalidValue before using it
     */
    double eyePositionLeftX;

    /**
     * @brief position of the left eye in device coordinates, unit is mm
     * - (0, 0, 0) is in the center of the device
     * - y-coordinate: positive towards the top of the screen
     *
     * check eyePositionLeftX != InvalidValue before using eyePositionLeftY
     */
    double eyePositionLeftY;

    /**
     * @brief position of the left eye in device coordinates, unit is mm
     * - (0, 0, 0) is in the center of the device
     * - z-coordinate: distance in front of the screen
     *
     * check eyePositionLeftX != InvalidValue before using eyePositionLeftZ
     */
    double eyePositionLeftZ;

    /** @brief radius of the left pupil in mm */
    double pupilRadiusLeft;

    /**
     * @brief X coordinate of monocular point of regard of the right eye, check porRightX !=
     * InvalidValue before using it.
     */
    double porRightX;

    /**
     * @brief Y coordinate of monocular point of regard of the right eye, also check porRightX !=
     * InvalidValue before using porRightY.
     */
    double porRightY;

    /**
     * @brief position of the right eye in device coordinates, unit is mm:
     * - (0, 0, 0) is in the center of the device
     * - x-coordinate: positive towards the right side of the screen
     *
     * check eyePositionRightX != InvalidValue before using it
     */
    double eyePositionRightX;

    /**
     * @brief position of the right eye in device coordinates, unit is mm:
     * - (0, 0, 0) is in the center of the device
     * - y-coordinate: positive towards the top of the screen
     *
     * check eyePositionRightX != InvalidValue before using eyePositionRightY
     */
    double eyePositionRightY;

    /**
     * @brief position of the right eye in device coordinates, unit is mm:
     * - (0, 0, 0) is in the center of the device
     * - z-coordinate: distance in front of the screen
     *
     * check eyePositionRightX != InvalidValue before using eyePositionRightZ
     */
    double eyePositionRightZ;

    /** @brief radius of the right pupil in mm */
    double pupilRadiusRight;
};
}  // namespace elapi
