#include "sensor_msgpack_data.h"
#include "mpack.h"


// Reading details keys
const char *READING_DETAILS_TYPE_KEY = "type";
const char *READING_DETAILS_MIN_VALUE_KEY = "min_value";
const char *READING_DETAILS_MAX_VALUE_KEY = "max_value";

// Sensor reading keys
const char *SENSOR_READING_NAME_KEY = "name";
const char *SENSOR_READING_VALUE_KEY = "value";
const char *SENSOR_READING_DETAILS_KEY = "details";

// Sensor data keys
const char *SENSOR_DATA_ID_KEY = "sensor_id";
const char *SENSOR_DATA_DESCRIPTION_KEY = "sensor_description";
const char *SENSOR_DATA_STATUS_KEY = "sensor_status";
const char *SENSOR_DATA_READINGS_KEY = "sensor_readings";


void pack_reading_value(ReadingType type, ReadingValue value, mpack_writer_t *writer) {
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

void pack_reading_details(ReadingDetails *details, mpack_writer_t *writer) {
    // Begin
    mpack_start_map(writer, 3);

    // Pack the type
    mpack_write_cstr(writer, READING_DETAILS_TYPE_KEY);
    mpack_write_u8(writer, details->mType);

    // Pack the min value
    mpack_write_cstr(writer, READING_DETAILS_MIN_VALUE_KEY);
    pack_reading_value(details->mType, details->mMinValue, writer);

    // Pack the max value
    mpack_write_cstr(writer, READING_DETAILS_MAX_VALUE_KEY);
    pack_reading_value(details->mType, details->mMaxValue, writer);
    
    // Done
    mpack_finish_map(writer);
}

void pack_sensor_reading(SensorReading *reading, mpack_writer_t *writer) {
    // Begin
    mpack_start_map(writer, 3);

    // Pack sensor name
    mpack_write_cstr(writer, SENSOR_READING_NAME_KEY);
    mpack_write_cstr(writer, reading->mSensorName);

    // Pack sensor value
    mpack_write_cstr(writer, SENSOR_READING_VALUE_KEY);
    pack_reading_value(reading->mDetails.mType, reading->mValue, writer);

    // Pack reading details
    mpack_write_cstr(writer, SENSOR_READING_DETAILS_KEY);
    pack_reading_details(&reading->mDetails, writer);

    // Done
    mpack_finish_map(writer);    
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
    }
}

PackResponse pack_sensor_data(SensorData *sensorData, uint8_t* outBuf, size_t outBufSize) {    
    // Initialize writer
    PackResponse response;
    size_t bytesUsed = 0;
    mpack_writer_t writer;
    mpack_writer_init(&writer, outBuf, outBufSize);

    // Write out sensor data
    mpack_start_map(&writer, 4);

    // Pack sensor ID
    mpack_write_cstr(&writer, SENSOR_DATA_ID_KEY);
    mpack_write_u8(&writer, sensorData->mSensorID);

    // Pack sensor description
    mpack_write_cstr(&writer, SENSOR_DATA_DESCRIPTION_KEY);
    mpack_write_cstr(&writer, sensorData->mSensorDescription);

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
