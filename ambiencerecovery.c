#include <math.h>
#include <stdlib.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>

#define PLUGIN_URI "http://lv2plug.in/plugins/ambiencerecovery"

// gain in dB to coefficient converter
#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)

typedef struct {
    float* input_port[2];
    float* output_port[2];
    float* gain_port;
    float* gain_m_port;
    float* gain_s_port;
    float* wet_port;
    float* delay_buffer[2];
    uint32_t buffer_size;
    uint32_t write_index;
    uint32_t delay_time;
} AmbienceRecovery;

static LV2_Handle instantiate(const LV2_Descriptor* descriptor, 
                              double sample_rate, const char* bundle_path, 
                              const LV2_Feature* const* features) {
    AmbienceRecovery* delay = (AmbienceRecovery*)malloc(sizeof(AmbienceRecovery));
    delay->buffer_size = (uint32_t)((sample_rate * 0.030) * 2);
    delay->write_index = 0;
    delay->delay_time = (uint32_t)(sample_rate * 0.030);
    delay->delay_buffer[0] = (float*)calloc(delay->buffer_size, sizeof(float));
    delay->delay_buffer[1] = (float*)calloc(delay->buffer_size, sizeof(float));
    return (LV2_Handle)delay;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
    AmbienceRecovery* delay = (AmbienceRecovery*)instance;
    switch (port) {
        case 0:
            delay->input_port[0] = (float*)data;
            break;
        case 1:
            delay->input_port[1] = (float*)data;
            break;
        case 2:
            delay->output_port[0] = (float*)data;
            break;
        case 3:
            delay->output_port[1] = (float*)data;
            break;
        case 4:
            delay->gain_port = (float*)data;
            break;
        case 5:
            delay->gain_m_port = (float*)data;
            break;
        case 6:
            delay->gain_s_port = (float*)data;
            break;
        case 7:
            delay->wet_port = (float*)data;
            break;
    }
}

static void activate(LV2_Handle instance) {}

static void deactivate(LV2_Handle instance) {}

static void cleanup(LV2_Handle instance) {
    AmbienceRecovery* delay = (AmbienceRecovery*)instance;
    free(delay->delay_buffer[0]);
    free(delay->delay_buffer[1]);
    free(delay);
}

static void run(LV2_Handle instance, uint32_t n_samples) {
    AmbienceRecovery* delay = (AmbienceRecovery*)instance;
    float* in_l = delay->input_port[0];
    float* in_r = delay->input_port[1];
    float* out_l = delay->output_port[0];
    float* out_r = delay->output_port[1];
    float* buffer_l = delay->delay_buffer[0];
    float* buffer_r = delay->delay_buffer[1];
    float gain = *delay->gain_port;
    float coef_gain = DB_CO(gain);
    float gain_m = *delay->gain_m_port;
    float coef_gain_m = DB_CO(gain_m);
    float gain_s = *delay->gain_s_port;
    float coef_gain_s = DB_CO(gain_s);
    float wet = *delay->wet_port;
    float coef_wet = DB_CO(wet);
    
    uint32_t delay_time = delay->delay_time;
    uint32_t write_index = delay->write_index;
    uint32_t buffer_size = delay->buffer_size;

    for (uint32_t i = 0; i < n_samples; ++i) {
		float ch_m = coef_gain_m * (.5 * (in_l[i] + in_r[i]));
		float ch_s = coef_gain_s * (.5 * (in_l[i] - in_r[i]));
		float dry_l = ch_m + ch_s;
		float dry_r = ch_m - ch_s;
        float delayed_l = buffer_l[(write_index + delay_time) % buffer_size];
        float delayed_r = buffer_r[(write_index + delay_time) % buffer_size];
        buffer_l[write_index] = dry_l;
        buffer_r[write_index] = dry_r;
        out_l[i] = (coef_gain * dry_l) + (coef_wet * delayed_r);
        out_r[i] = (coef_gain * dry_r) + (coef_wet * delayed_l);
        write_index = (write_index + 1) % buffer_size;
    }

    delay->write_index = write_index;
}

static const void* extension_data(const char* uri) {
    return NULL;
}

static const LV2_Descriptor descriptor = {
    PLUGIN_URI,
    instantiate,
    connect_port,
    activate,
    run,
    deactivate,
    cleanup,
    extension_data
};


LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
	switch (index) {
	case 0:  return &descriptor;
	default: return NULL;
	}
}
