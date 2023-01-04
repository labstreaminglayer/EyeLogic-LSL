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
 * @file ELApi.h
 *
 * @brief The file contains the C++ prototype declaration for all functions which are
 * neccessary to control the EyeLogic software from an API client.
 */

#pragma once

#include "ELGazeSample.h"

#include <memory>

/** @brief namespace for C++ API calls */
namespace elapi
{
/**
 * @brief main class for communication with the EyeLogic server
 */
class ELApi final
{
public:
    /**
     * @brief constructor
     * @param   clientName          string identifier of the client (shown in the server tool
     *                              window), may be null
     */
    EL_EXPORT STDCALL ELApi( const char* clientName );
    /** @brief destructor */
    EL_EXPORT STDCALL ~ELApi( );

    ELApi( const ELApi& ) = delete;
    ELApi& operator=( const ELApi& ) = delete;
    ELApi( ELApi&& )                 = delete;
    ELApi& operator=( ELApi&& ) = delete;

    /** @brief eye tracking event */
    enum class Event {
        /** @brief active screen or resolution has changed */
        SCREEN_CHANGED,
        /** @brief connection to server has closed */
        CONNECTION_CLOSED,
        /** @brief a new device has connected */
        DEVICE_CONNECTED,
        /** @brief actual device has disconnected */
        DEVICE_DISCONNECTED,
        /** @brief tracking has stopped */
        TRACKING_STOPPED,
    };

    /** @brief Callback interface for events related to the eye tracker */
    class ELEventCallback
    {
    public:
        virtual STDCALL ~ELEventCallback( ) = default;
        /**
         * @brief Callback function for new events
         * @param event The occurred event
         */
        virtual void STDCALL onEvent( ELApi::Event event ) = 0;
    };
    /**
     * @brief Registers the event listener. An existing listener will be overwritten.
     * @param callback   this instance will be notified of all events published by the ELApi.
     *                   If null, the current callback is removed/unregistered.
     *                   Ensure that the listener is unregistered before its destruction.
     */
    EL_EXPORT void STDCALL registerEventListener( ELEventCallback* callback );

    /** @brief Callback interface for GazeSamples */
    class ELGazeSampleCallback
    {
    public:
        virtual STDCALL ~ELGazeSampleCallback( ) = default;
        /**
         * @brief Callback function for new GazeSamples
         * @param gazeSample incoming GazeSample
         */
        virtual void STDCALL onGazeSample( const ELGazeSample& gazeSample ) = 0;
    };
    /**
     * @brief Registers the gaze sample listener. An existing listener will be overwritten.
     * @param callback   this instance will be notified of all gaze samples published by the ELApi.
     *                   If null, the current callback is removed/unregistered.
     *                   Ensure that the listener is unregistered before its destruction.
     */
    EL_EXPORT void STDCALL registerGazeSampleListener( ELGazeSampleCallback* callback );

    /** @brief return values of connect( ) */
    enum class ReturnConnect {
        /** @brief connection successully established */
        SUCCESS,
        /** @brief connection could not be established: the server can not be found or is not
           responding */
        FAILURE,
        /** @brief connection could not be established: API is build on a newer version than the
         * server. Update the EyeLogicServer to the newest version.
         */
        VERSION_MISMATCH
    };

    /**
     * @brief initialize connection to the server (method is blocking until connection
     * established). The connection is only established for a local server (running on this
     * machine). For connections to a remote server, @see connectRemote( ).
     */
    EL_EXPORT ReturnConnect STDCALL connect( );

    /** @brief connection information for an EyeLogic server */
    struct ServerInfo {
        /** @brief IP address of server as 0-terminated string */
        char ip[ 16 ];
        /** @brief port of server */
        uint16_t port;
    };

    /**
     * @brief initialize connection to a remote server (method is blocking until connection
     * established)
     *
     * @param   server  Server to connect to
     *
     * @see acquireServerList( ) to obtain IP address and port of a remote server
     */
    EL_EXPORT ReturnConnect STDCALL connectRemote( ServerInfo server );

    /**
     * @brief Ping all running EyeLogic servers in the local network and wait some time for their
     * response.
     *
     * @param   blockingDurationMS  waiting duration in milliseconds. Method returns after this
     *                              time, or if 'serverListLength' many servers responded.
     * @param   serverList          pre-allocated array of length 'serverListLength'. Will be filled
     *                              with responding EyeLogic servers.
     * @param   serverListLength    Lenght of pre-allocated serverList array
     *
     * @return  number of entries, written to the server list
     */
    EL_EXPORT int32_t STDCALL requestServerList( int32_t blockingDurationMS, ServerInfo* serverList,
                                                 int32_t serverListLength );

    /** @brief closes connection to the server */
    EL_EXPORT void STDCALL disconnect( );

    /** @brief whether a connection to the server is established */
    EL_EXPORT bool STDCALL isConnected( ) const;

    /** @brief Screen configuration */
    struct ScreenConfig {
        /** @brief whether the screen is connected to the this machine */
        bool localMachine;
        /** @brief identifier name of the screen (0-terminated string) */
        char id[ 32 ];
        /** @brief descriptive name of the screen (0-terminated string) */
        char name[ 32 ];
        /** @brief raw screen X resolution [px] */
        int32_t resolutionX;
        /** @brief raw screen Y resolution [px] */
        int32_t resolutionY;
        /** @brief horizontal physical dimension of the screen [mm] */
        double physicalSizeX_mm;
        /** @brief vertical physical dimension of the screen [mm] */
        double physicalSizeY_mm;
    };

    /** @brief obtain configuration of active screen */
    EL_EXPORT void STDCALL getActiveScreen( ScreenConfig& screenConfig ) const;

    /**
     * @brief Get a list of screens connected to the local machine. If there are more screens than
     * 'numScreenConfigs' found, then only the first 'numScreenConfigs' ones are filled.
     *
     * @param   screenConfig        pre-allocated array, will be filled with screen configurations
     * @param   numScreenConfigs    number of entries of screenConfig
     * @return  number of filled screen configurations. will be <= numScreenConfigs
     */
    EL_EXPORT int32_t STDCALL getAvailableScreens( ScreenConfig* screenConfig,
                                                   int32_t       numScreenConfigs ) const;

    /** @brief Geometric position of the device related to the active monitor */
    struct DeviceGeometry {
        /**
         * @brief vertical distance between the lowest pixel on the display and the upper edge of
         * the eye tracker
         */
        double mmBelowScreen;
        /**
         * @brief horizontal distance between the front of the screen and the front edge of the eye
         * tracker
         */
        double mmTrackerInFrontOfScreen;
    };

    /** @brief return values of setActiveScreen( ) */
    enum class ReturnSetActiveScreen {
        /** active screen was set */
        SUCCESS,
        /** specified screen name was not found as a name of an available monitor */
        NOT_FOUND,
        /** active screen could not be changed */
        FAILURE,
    };

    /**
     * @brief Make a screen connected to this machine to the active screen.
     *
     * Recording is from now on performed on the new active screen. Remember to perform a
     * calibration on the new screen, otherwise it remains in an uncalibrated state.
     *
     * @param   screenID  ID of the new active screen on _this_ machine (even works if the
     *                    connection to the server is remote).
     *                    If null, the primary screen of this machine is set as active.
     * @param   deviceGeometry  Geometry of the device which is mounted to the screen.
     * @return  success/error code
     */
    EL_EXPORT ReturnSetActiveScreen STDCALL setActiveScreen( const char*    screenID,
                                                             DeviceGeometry deviceGeometry );

    /** @brief Device configuration */
    struct DeviceConfig {
        /** @brief serial number of the device as unsigned 64-bit int for a verbose format, print it
         * as 8-digit hex number
         */
        uint64_t deviceSerial;
        /** @brief number of available framerates */
        int32_t numFrameRates;
        /** @brief array of available framerates [Hz], use only the entries frameRates[0] ...
         * frameRates[numFrameRates-1]
         */
        uint8_t frameRates[ 16 ];
        /** @brief number of available calibration methods */
        int32_t numCalibrationMethods;
        /** @brief array of available calibration methods [number of calibration points], use only
         * the entries calibrationMethods[0] ... calibrationMethods[numCalibrationMethods-1]
         */
        uint8_t calibrationMethods[ 16 ];
    };
    /** @brief obtain configuration of active device */
    EL_EXPORT void STDCALL getDeviceConfig( DeviceConfig& deviceConfig ) const;

    /** @brief Return values of the getNextEvent/getNextGazeSample functions. */
    enum ReturnNextData {
        /** @brief new event or new GazeSample received */
        SUCCESS,
        /** @brief timeout reached, no new event/GazeSample received */
        TIMEOUT,
        /** @brief connection to server closed, no new event/GazeSample received */
        CONNECTION_CLOSED
    };

    /**
     * @brief Obtains the next unread event or blocks until a new event occurs or the given timeout
     * is reached.
     *
     * The last incoming event is buffered internally and can be obtained by calling this method in
     * a consecutive order. If there is no new event, the method blocks until an event occurs or the
     * given timeout is reached. The method returns SUCCESS if and only if a new event is provided
     * which was not returned before. Therefore, by checking the return value, you can assure to not
     * handle any event twice.
     *
     * If you want to catch events in a loop, be careful to not wait too long between the calls to
     * this method. Otherwise, you may miss events. If you want to be 100% sure to not miss any
     * event, consider to use the ELEventCallback mechanism. @see registerEventListener
     *
     * @param   event           If this method returns SUCCESS, this data structure is filled with
     *                          the new (yet unhandled) event. In all other cases, this data
     *                          structure is filled with the event which was returned last.
     * @param   timeoutMillis   duration in milliseconds, method returns at the latest after this
     *                          time. May be 0 if the method should return immediatly.
     * @return  whether an event was received (SUCCESS) or the method terminated without a new event
     */
    EL_EXPORT ReturnNextData STDCALL getNextEvent( Event& event, int32_t timeoutMillis );

    /**
     * @brief Obtains the next unread gazeSample or blocks until a new GazeSample is received or the
     * given timeout is reached.
     *
     * The last incoming GazeSample is buffered internally and can be obtained by calling this
     * method in a consecutive order. If there is no new GazeSample, the method blocks until a
     * GazeSample arrives or the given timeout is reached. The method returns SUCCESS if and only if
     * a new GazeSample is provided which was not returned before. Therefore, by checking the return
     * value, you can assure to not handle any GazeSample twice.
     *
     * If you want to catch GazeSamples in a loop, be careful to not wait too long between the calls
     * to this method (at least once per frame). Otherwise, you may miss GazeSamples. If you want to
     * be 100% sure to not miss any GazeSample, consider to use the ELGazeSampleCallback mechanism.
     * @see registerGazeSampleListener
     *
     * @param   gazeSample      If this method returns SUCCESS, this data structure is filled with
     *                          the new (yet unhandled) GazeSample. In all other cases, this data
     *                          structure is filled with the GazeSample which was returned last.
     * @param   timeoutMillis   duration in milliseconds, method returns at the latest after this
     *                          time. May be 0 if the method should return immediatly.
     * @return  whether a GazeSample was received (SUCCESS) or the method terminated without a new
     *          GazeSample
     */
    EL_EXPORT ReturnNextData STDCALL getNextGazeSample( ELGazeSample& gazeSample,
                                                        int32_t       timeoutMillis );

    /** @brief return values of requestTracking( ) */
    enum class ReturnStart {
        /** @brief start tracking successful */
        SUCCESS,
        /** @brief not connected to the server */
        NOT_CONNECTED,
        /** @brief cannot start tracking: no device found */
        DEVICE_MISSING,
        /** @brief cannot start tracking: framerate mode is invalid or not supported */
        INVALID_FRAMERATE_MODE,
        /** @brief tracking already ongoing, but frame rate mode is different */
        ALREADY_RUNNING_DIFFERENT_FRAMERATE,
        /** @brief some general failure occurred */
        FAILURE
    };

    /**
     * @brief request tracking
     *
     * If tracking is not yet ongoing, tracking is started in the device. If tracking is already
     * running (e.g. started from another client) with the same frame-rate as requested, all gaze
     * samples are reported to this client as well.
     *
     * @param   frameRateModeInd    index of the requested frame rate mode (0 ...
     * \#frameRateModes-1)
     */
    EL_EXPORT ReturnStart STDCALL requestTracking( int32_t frameRateModeInd );

    /**
     * @brief unrequest tracking
     *
     * Note that the tracking device may continue if other processes still request tracking. Check
     * the EyeLogic server window to observe the actual state.
     */
    EL_EXPORT void STDCALL unrequestTracking( );

    /** @brief  return values of calibrate( ) */
    enum class ReturnCalibrate {
        /** @brief start calibration successful */
        SUCCESS,
        /** @brief cannot calibrate: not connected to the server */
        NOT_CONNECTED,
        /** @brief cannot calibrate: no device found or tracking not started */
        NOT_TRACKING,
        /** @brief cannot start calibration: calibration mode is invalid or not supported */
        INVALID_CALIBRATION_MODE,
        /** @brief cannot start calibration: calibration or validation is already in progress */
        ALREADY_BUSY,
        /** @brief calibration failure */
        FAILURE
    };

    /** @brief perform calibration (method is blocking until calibration finished)
     * @param   calibrationModeInd  index of the requested calibration method
     *                              (0 ... \#calibrationMethods-1)
     */
    EL_EXPORT ReturnCalibrate STDCALL calibrate( int32_t calibrationModeInd );

    /** @brief abort a running calibration */
    EL_EXPORT void STDCALL abortCalibValidation( );

    /** @brief  return values of validate( ) */
    enum class ReturnValidate {
        /** start validation successful */
        SUCCESS,
        /** cannot validate: not connected to the server */
        NOT_CONNECTED,
        /** cannot validate: no device found or tracking not started */
        NOT_TRACKING,
        /** cannot start validation: validation mode is invalid or not supported */
        NOT_CALIBRATED,
        /** cannot start validation: calibration or validation is already in progress */
        ALREADY_BUSY,
        /** validation failure */
        FAILURE
    };

    /** @brief ValidationPointResult holds the results of the validation ( total deviation between
     * true point position and calculated POR of the left and right eye POR in [px] and [deg] ) of
     * the validation point at position ( validationPointPxX, validationPointPxY ) [px].
     *
     * The stimulus point position and deviation [px] are given in the 2D stimulus coordinate
     * system originating in the top left corner of the stimulus.
     *
     * The deviation [deg] corresponds to the total angular deviation between the measured gaze
     * direction from the ground truth gaze direction as determined according to the measured eye
     * position.
     *
     * Note: All data fields may be ELInvalidValue. All pairs validationPointPxX/-Y,
     * meanDeviationLeftDeg/-Px and meanDeviationRightDeg-/Px are always either both valid or both
     * ELInvalidValue.
     */
    struct ELValidationPointResult {
        /** @brief ELInvalidValue or x-coordinate of stimulus point position */
        double validationPointPxX;
        /** @brief ELInvalidValue or y-coordinate of stimulus point position */
        double validationPointPxY;
        /** @brief ELInvalidValue or mean deviation between left eye POR and stimulus position in
         * [px] in the stimulus plane */
        double meanDeviationLeftPx;
        /** @brief ELInvalidValue or mean deviation of left eye gaze direction in [deg] in the 3-D
         * world system */
        double meanDeviationLeftDeg;
        /** @brief ELInvalidValue or mean deviation between right eye POR and stimulus position in
         * [px] in the stimulus plane */
        double meanDeviationRightPx;
        /** @brief ELInvalidValue or mean deviation of right eye gaze direction in [deg] in the 3-D
         * world system */
        double meanDeviationRightDeg;
    };

    /** @brief ValidationResult contains one ValidationPointResult struct per validation stimulus
     * point of the performed valdation. */
    struct ELValidationResult {
        ELValidationPointResult pointsData[ 4 ];
    };

    /** @brief perform validation (method is blocking until validation finished)
     *
     *@param validationResult upon ReturnValidate::SUCCESS this struct will be filled with the
     * validation results - may contain ELInvalidValues. Contains all ELInvalidValues for all other
     * return values.
     */
    EL_EXPORT ReturnValidate STDCALL validate( ELValidationResult& validationResult );

private:
    class Impl;
    std::unique_ptr< Impl > m_private;
};
}  // namespace elapi
