#ifndef RadioShack_Camera_h
#define RadioShack_Camera_h

#include "Arduino.h"
#include <SD.h>

#define VC0706_PROTOCOL_SIGN 0x56
#define VC0706_SERIAL_NUMBER 0x00
#define VC0706_COMMAND_RESET 0x26
#define VC0706_COMMAND_GEN_VERSION 0x11
#define VC0706_COMMAND_TV_OUT_CTRL 0x44
#define VC0706_COMMAND_OSD_ADD_CHAR 0x45
#define VC0706_COMMAND_DOWNSIZE_SIZE 0x53
#define VC0706_COMMAND_READ_FBUF 0x32
#define FBUF_CURRENT_FRAME 0
#define FBUF_NEXT_FRAME 0
#define VC0706_COMMAND_FBUF_CTRL 0x36
#define VC0706_COMMAND_COMM_MOTION_CTRL 0x37
#define VC0706_COMMAND_COMM_MOTION_DETECTED 0x39
#define VC0706_COMMAND_POWER_SAVE_CTRL 0x3E
#define VC0706_COMMAND_COLOR_CTRL 0x3C
#define VC0706_COMMAND_MOTION_CTRL 0x42
#define VC0706_COMMAND_WRITE_DATA 0x31
#define VC0706_COMMAND_GET_FBUF_LEN 0x34

#define READ_DATA_BLOCK_NO 56
class RadioShack_Camera {
    public:
        RadioShack_Camera();
        void capture_photo(File myFile);
    private:
        unsigned int sdSelect;
        unsigned char tx_counter;
        unsigned char tx_vcbuffer[20];
        bool tx_ready;
        bool rx_ready;
        unsigned char rx_counter;
        unsigned char VC0706_rx_buffer[80];

        uint32_t frame_length;
        uint32_t vc_frame_address;
        uint32_t last_data_length;

        void VC0706_reset();
        void VC0706_get_version();
        void VC0706_tv_out_control(int on);
        void VC0706_osd_add_char(int col, int row, String osd_string);
        void VC0706_w_h_downsize(int scale_width, int scale_height);
        void VC0706_read_frame_buffer(unsigned long buffer_address, unsigned long buffer_length);
        void VC0706_frame_control(byte frame_control);
        void VC0706_motion_detection(int control_flag);
        void VC0706_motion_control(int control_flag);
        void VC0706_get_framebuffer_length(byte fbuf_type);
        void VC0706_uart_power_save(byte power_save_on);
        void VC0706_uart_color_control(byte show_mode);
        void VC0706_compression_ratio(int ratio);
        void VC0706_motion_windows_setting(unsigned int register_address, unsigned long data);
        void debug_send();
        void buffer_send();
        void buffer_read();
};

#endif