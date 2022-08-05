#include "sensor_msgpack.h"
#include "mpack/mpack.h"


// Generic keys
const char *PACKET_ID_KEY = "packet_id";
const char *SENSOR_ID_KEY = "sensor_id";
const char *COMMAND_ID_KEY = "command_id";

const char *READING_TYPE_KEY = "type";
const char *READING_VALUE_KEY = "value";

// Header packet keys
const char *RESPONSE_CODE_KEY = "response_code";
const char *SENSOR_DATA_COUNT_KEY = "sensor_data_count";
const char *TERMINATOR_CODE = "terminator_code";

// Reading description keys
const char *READING_DESCRIPTION_KEY = "reading_description";
const char *READING_DESCRIPTION_NAME_KEY = "name";
const char *READING_DESCRIPTION_MIN_VALUE_KEY = "min_value";
const char *READING_DESCRIPTION_MAX_VALUE_KEY = "max_value";

// Sensor data keys
const char *SENSOR_DATA_NAME_KEY = "name";
const char *SENSOR_DATA_STATUS_KEY = "sensor_status";
const char *SENSOR_DATA_READINGS_KEY = "sensor_readings";


PackResponse pack_heartbeat_packet(char* outBuf, size_t outBufSize) {
    HeaderPacket controllerReadyHeader = {
        NO_COMMAND,
        HEARTBEAT,
    };

    return pack_header_data(controllerReadyHeader, outBuf, outBufSize);
}

PackResponse pack_controller_ready_packet(char* outBuf, size_t outBufSize) {
    HeaderPacket controllerReadyHeader = {
        NO_COMMAND,
        CONTROLLER_READY,
    };

    return pack_header_data(controllerReadyHeader, outBuf, outBufSize);
}

PackResponse pack_header_data(HeaderPacket headerPacket, char* outBuf, size_t outBufSize) {
    // Initialize writer
    PackResponse response;
    mpack_writer_t writer;
    mpack_writer_init(&writer, outBuf, outBufSize);

    // Write out packet data
    mpack_start_map(&writer, 3);

    // Pack packet ID
    mpack_write_cstr(&writer, PACKET_ID_KEY);
    mpack_write_u8(&writer, HEADER_PACKET);

    // Pack command ID
    mpack_write_cstr(&writer, COMMAND_ID_KEY);
    mpack_write_u8(&writer, headerPacket.mCommandID);
    
    // Pack response code
    mpack_write_cstr(&writer, RESPONSE_CODE_KEY);
    mpack_write_u8(&writer, headerPacket.mResponseCode);
    
    // Finish building the map
    mpack_finish_map(&writer);

    // Get the amount of bytes used
    response.mBytesUsed = mpack_writer_buffer_used(&writer);

    // Finish writing the data
    response.mErrorCode = mpack_writer_destroy(&writer);

    return response;
}

PackResponse pack_terminator_packet(uint8_t terminatorCode, char* outBuf, size_t outBufSize) {
    // Initialize writer
    PackResponse response;
    mpack_writer_t writer;
    mpack_writer_init(&writer, outBuf, outBufSize);

    // Write out packet data
    mpack_start_map(&writer, 2);

    // Pack packet ID
    mpack_write_cstr(&writer, PACKET_ID_KEY);
    mpack_write_u8(&writer, TERMINATOR_PACKET);

    // Pack terminator code
    mpack_write_cstr(&writer, TERMINATOR_CODE);
    mpack_write_u8(&writer, terminatorCode);

    // Finish building the map
    mpack_finish_map(&writer);

    // Get the amount of bytes used
    response.mBytesUsed = mpack_writer_buffer_used(&writer);

    // Finish writing the data
    response.mErrorCode = mpack_writer_destroy(&writer);

    return response;
}

void pack_reading_value(MsgPackReadingType type, MsgPackReadingValue value, mpack_writer_t *writer) {
    switch(type) {
        case INT_READING:
            mpack_write_u16(writer, value.mIntValue);
            break;

        case FLOAT_READING:
            mpack_write_float(writer, value.mFloatValue);
            break;

        case BOOL_READING:
            mpack_write_u8(writer, value.mBoolValue);
            break;
    }
}

void pack_reading_description(const MsgPackSensorReadingDescription* const description, mpack_writer_t *writer) {
    // Begin
    mpack_start_map(writer, 4);

    // Pack the name
    mpack_write_cstr(writer, READING_DESCRIPTION_NAME_KEY);
    mpack_write_cstr(writer, description->mReadingName);

    // Pack the type
    mpack_write_cstr(writer, READING_TYPE_KEY);
    mpack_write_u8(writer, description->mType);

    // Pack the min value
    mpack_write_cstr(writer, READING_DESCRIPTION_MIN_VALUE_KEY);
    pack_reading_value(description->mType, description->mMinValue, writer);

    // Pack the max value
    mpack_write_cstr(writer, READING_DESCRIPTION_MAX_VALUE_KEY);
    pack_reading_value(description->mType, description->mMaxValue, writer);
    
    // Done
    mpack_finish_map(writer);
}

void pack_sensor_reading(const MsgPackSensorReading* const reading, mpack_writer_t *writer) {
    // Begin
    mpack_start_map(writer, 2);

    // Pack reading description    
    mpack_write_cstr(writer, READING_DESCRIPTION_KEY);
    pack_reading_description(reading->mDescription, writer);
    
    // Pack reading value
    mpack_write_cstr(writer, READING_VALUE_KEY);
    pack_reading_value(reading->mDescription->mType, reading->mValue, writer);

    // Done
    mpack_finish_map(writer);
}

PackResponse pack_sensor_data(const MsgPackSensorData * const sensorData, char* outBuf, size_t outBufSize) {
    // Initialize writer
    PackResponse response;
    mpack_writer_t writer;
    mpack_writer_init(&writer, outBuf, outBufSize);

    // Write out sensor data
    mpack_start_map(&writer, 5);

    // Pack packet ID
    mpack_write_cstr(&writer, PACKET_ID_KEY);
    mpack_write_u8(&writer, SENSOR_DATA_PACKET);

    // Pack sensor ID
    mpack_write_cstr(&writer, SENSOR_ID_KEY);
    mpack_write_u8(&writer, sensorData->mSensorID);

    // Pack sensor name
    mpack_write_cstr(&writer, SENSOR_DATA_NAME_KEY);
    mpack_write_cstr(&writer, sensorData->mSensorName);

    // Pack sensor status
    mpack_write_cstr(&writer, SENSOR_DATA_STATUS_KEY);
    mpack_write_u8(&writer, sensorData->mStatus);

    // Pack sensor readings
    mpack_write_cstr(&writer, SENSOR_DATA_READINGS_KEY);
    mpack_start_array(&writer, sensorData->mNumReadings);
    for(int i = 0; i < sensorData->mNumReadings; ++i) {
        pack_sensor_reading(&(sensorData->mSensorReadings[i]), &writer);
    }
    mpack_finish_array(&writer);

    // Finish building the map
    mpack_finish_map(&writer);

    // Get the amount of bytes used
    response.mBytesUsed = mpack_writer_buffer_used(&writer);

    // Finish writing the data
    response.mErrorCode = mpack_writer_destroy(&writer);

    return response;
}

const char * error_to_string(mpack_error_t error) {
    switch(error) {
        case mpack_ok:
            return 0;
        case mpack_error_io:
            return "The reader or writer failed to fill or flush, or some other file or socket error occurred";
        case mpack_error_invalid:
            return "The data read is not valid MessagePack";
        case mpack_error_unsupported:
            return "The data read is not supported by this configuration of MPack. (See @ref MPACK_EXTENSIONS.)";
        case mpack_error_type:
            return "The type or value range did not match what was expected by the caller";
        case mpack_error_too_big:
            return "A read or write was bigger than the maximum size allowed for that operation";
        case mpack_error_memory:
            return "An allocation failure occurred";
        case mpack_error_bug:
            return "The MPack API was used incorrectly. (This will always assert in debug mode.)";
        case mpack_error_data:
            return "The contained data is not valid";
        case mpack_error_eof:
            return "The reader failed to read because of file or socket EOF";
        default:
            return "Unknown error";
    }
}
