#include "RadioShack_Camera.h"

uint32_t frame_length = 0;
uint32_t vc_frame_address = 0;
uint32_t last_data_length = 0;

RadioShack_Camera::RadioShack_Camera() {}

void RadioShack_Camera::capture_photo(File image) {
    Serial.begin(115200);
    VC0706_compression_ratio(63);
    delay(100);

    VC0706_frame_control(3);
    delay(10);

    VC0706_frame_control(0);
    delay(10);
    rx_ready = false;
    rx_counter = 0;

    Serial.end();			// clear all rx buffer
    delay(5);

    Serial.begin(115200);

    VC0706_get_framebuffer_length(0);
    delay(10);
    buffer_read();

    // store frame buffer length for coming reading
    frame_length = (VC0706_rx_buffer[5] << 8) + VC0706_rx_buffer[6];
    frame_length = frame_length << 16;
    frame_length = frame_length + (0x0ff00 & (VC0706_rx_buffer[7] << 8)) + VC0706_rx_buffer[8];

    vc_frame_address = READ_DATA_BLOCK_NO;

    while (vc_frame_address < frame_length) {
         VC0706_read_frame_buffer(vc_frame_address - READ_DATA_BLOCK_NO, READ_DATA_BLOCK_NO);
         delay(9);

        //get the data with length=READ_DATA_BLOCK_NObytes
        rx_ready = false;
        rx_counter = 0;
        buffer_read();

        image.write(VC0706_rx_buffer + 5, READ_DATA_BLOCK_NO);
        vc_frame_address = vc_frame_address + READ_DATA_BLOCK_NO;
    }

    vc_frame_address = vc_frame_address - READ_DATA_BLOCK_NO;

    last_data_length = frame_length - vc_frame_address;


    VC0706_read_frame_buffer(vc_frame_address, last_data_length);
    delay(9);
    //get the data
    rx_ready = false;
    rx_counter = 0;
    buffer_read();

    image.write(VC0706_rx_buffer + 5, last_data_length);

    image.close();
    delay(100);
    VC0706_frame_control(3);
    delay(10);
    Serial.end();			// clear all rx buffer
    rx_ready = false;
}

String RadioShack_Camera::get_version() {
    Serial.begin(115200);
    VC0706_get_version();
    delay(10);
    buffer_read();
    String version;
    for (unsigned char i = 5; i < rx_counter; i++) {
        version = version + (char)VC0706_rx_buffer[i];
    }
    Serial.end();
    return version;
}

void RadioShack_Camera::freeze() {
    Serial.begin(115200);
    VC0706_frame_control(0);
    delay(10);
    Serial.end();
}

void RadioShack_Camera::resume() {
    Serial.begin(115200);
    VC0706_frame_control(3);
    delay(10);
    Serial.end();
}

void RadioShack_Camera::turn_tv_out_off() {
    Serial.begin(115200);
    VC0706_tv_out_control(0);
    delay(10);
    Serial.end();
}

void RadioShack_Camera::turn_tv_out_on() {
    Serial.begin(115200);
    VC0706_tv_out_control(1);
    delay(10);
    Serial.end();
}

void RadioShack_Camera::VC0706_reset() {
    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_RESET;
    tx_vcbuffer[3] = 0x00;
    tx_counter = 4;
    buffer_send();
}

void RadioShack_Camera::VC0706_get_version() {
    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_GEN_VERSION;
    tx_vcbuffer[3] = 0x00;
    tx_counter = 4;
    buffer_send();
}

void RadioShack_Camera::VC0706_tv_out_control(int on) {
    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_TV_OUT_CTRL;
    tx_vcbuffer[3] = 0x01;
    tx_vcbuffer[4] = on;
    tx_counter = 5;
    buffer_send();
}

void RadioShack_Camera::VC0706_w_h_downsize(int scale_width, int scale_height) {
    int scale;

    if (scale_width >= 2) scale_width = 2;
    if (scale_height > scale_width)	scale_height = scale_width;
        scale = (unsigned char)(scale_height << 2 | scale_width);


    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_DOWNSIZE_SIZE;
    tx_vcbuffer[3] = 0x01;

    tx_vcbuffer[4] = scale;		//bit[1:0] width zooming proportion
    //bit[3:2] height zooming proportion

    tx_counter = 5;

    buffer_send();
}

void RadioShack_Camera::VC0706_read_frame_buffer(unsigned long buffer_address, unsigned long buffer_length) {

    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_READ_FBUF;
    tx_vcbuffer[3] = 0x0c;
    tx_vcbuffer[4] = FBUF_CURRENT_FRAME;
    tx_vcbuffer[5] = 0x0a;		// 0x0a=data transfer by MCU mode; 0x0f=data transfer by SPI interface
    tx_vcbuffer[6] = buffer_address >> 24;			//starting address
    tx_vcbuffer[7] = buffer_address >> 16;
    tx_vcbuffer[8] = buffer_address >> 8;
    tx_vcbuffer[9] = buffer_address & 0x0ff;
    tx_vcbuffer[10] = buffer_length >> 24;		// data length
    tx_vcbuffer[11] = buffer_length >> 16;
    tx_vcbuffer[12] = buffer_length >> 8;
    tx_vcbuffer[13] = buffer_length & 0x0ff;
    tx_vcbuffer[14] = 0x00;		// delay time
    tx_vcbuffer[15] = 0x0a;
    tx_counter = 16;

    buffer_send();
}

void RadioShack_Camera::VC0706_frame_control(byte frame_control) {
    if (frame_control > 3) frame_control = 3;
    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_FBUF_CTRL;
    tx_vcbuffer[3] = 0x01;
    tx_vcbuffer[4] = frame_control;
    tx_counter = 5;

    buffer_send();
}

void RadioShack_Camera::VC0706_motion_detection(int control_flag) {
    if (control_flag > 1) control_flag = 1;
    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_COMM_MOTION_CTRL;
    tx_vcbuffer[3] = 0x01;
    tx_vcbuffer[4] = control_flag;
    tx_counter = 5;

    buffer_send();
}

void RadioShack_Camera::VC0706_motion_control(int control_flag) {
    if (control_flag > 1) control_flag = 1;
    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_MOTION_CTRL;
    tx_vcbuffer[3] = 0x03;
    tx_vcbuffer[4] = 0x00;			//motion control attribute
    tx_vcbuffer[5] = 0x01;			//mcu uart control
    tx_vcbuffer[6] = control_flag;
    tx_counter = 7;

    buffer_send();
}

void RadioShack_Camera::VC0706_get_framebuffer_length(byte fbuf_type) {
    if (fbuf_type > 1) fbuf_type = 1;
    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_GET_FBUF_LEN;
    tx_vcbuffer[3] = 0x01;
    tx_vcbuffer[4] = fbuf_type;
    tx_counter = 5;

    buffer_send();
}

void RadioShack_Camera::VC0706_uart_power_save(byte power_save_on) {
    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_POWER_SAVE_CTRL;
    tx_vcbuffer[3] = 0x03;
    tx_vcbuffer[4] = 00;			//power save control mode
    tx_vcbuffer[5] = 01;			// control by UART
    tx_vcbuffer[6] = power_save_on;			//start power save
    tx_counter = 7;

    buffer_send();
}

void RadioShack_Camera::VC0706_uart_color_control(byte show_mode) {
    if (show_mode > 2) show_mode = 2;
    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_COLOR_CTRL;
    tx_vcbuffer[3] = 0x02;
    tx_vcbuffer[4] = 01;		//control by UART
    tx_vcbuffer[5] = show_mode;		// automatically step black-white and colour
    tx_counter = 6;

    buffer_send();
}

void RadioShack_Camera::VC0706_compression_ratio(int ratio) {
    if (ratio > 63) ratio = 63;
    if (ratio < 13) ratio = 13;
    int vc_comp_ratio = (ratio - 13) * 4 + 53;
    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_WRITE_DATA;
    tx_vcbuffer[3] = 0x05;
    tx_vcbuffer[4] = 01;		//chip register
    tx_vcbuffer[5] = 0x01;	//data num ready to write
    tx_vcbuffer[6] = 0x12;	//register address
    tx_vcbuffer[7] = 0x04;
    tx_vcbuffer[8] = vc_comp_ratio; //data

    tx_counter = 9;

    buffer_send();
}

void RadioShack_Camera::VC0706_motion_windows_setting(unsigned int register_address, unsigned long data) {
    tx_vcbuffer[0] = VC0706_PROTOCOL_SIGN;
    tx_vcbuffer[1] = VC0706_SERIAL_NUMBER;
    tx_vcbuffer[2] = VC0706_COMMAND_WRITE_DATA;
    tx_vcbuffer[3] = 0x08;
    tx_vcbuffer[4] = 01;		//chip register
    tx_vcbuffer[5] = 0x04;	//data num ready to write
    tx_vcbuffer[6] = register_address >> 8;	//register address
    tx_vcbuffer[7] = register_address & 0x0ff;;

    tx_vcbuffer[8] = data >> 24;		// data ready to write
    tx_vcbuffer[9] = data >> 16;
    tx_vcbuffer[10] = data >> 8;
    tx_vcbuffer[11] = data & 0x0ff;

    tx_counter = 12;

    buffer_send();
}

void RadioShack_Camera::debug_send() {
    int i = 0;

    for (i = 0; i < tx_counter; i++) {
        Serial.print(tx_vcbuffer[i], HEX);
        Serial.print(", ");
    }

    Serial.println("");
}

void RadioShack_Camera::buffer_send() {
    int i = 0;
    for (i = 0; i < tx_counter; i++)
        Serial.write(tx_vcbuffer[i]);
    tx_ready = true;
}

void RadioShack_Camera::buffer_read() {
    bool validity = true;

    if (rx_ready)			// if something unread in buffer, just quit
        return;

    rx_counter = 0;
    VC0706_rx_buffer[0] = 0;
    while (Serial.available() > 0) {
        VC0706_rx_buffer[rx_counter++] = Serial.read();
    }

    if (VC0706_rx_buffer[0] != 0x76)
        validity = false;
    if (VC0706_rx_buffer[1] != VC0706_SERIAL_NUMBER)
        validity = false;

    if (validity) rx_ready = true;
}
