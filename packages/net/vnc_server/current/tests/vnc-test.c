//==========================================================================
//
//      vnc-test.c
//
//====================================================================
// Copyright (C) 2003 Sweeney Design Ltd
//
//  This software is provided 'as-is', without any express or implied
//  warranty. Permission is granted to anyone to use this software for
//  any purpose, including commercial applications, and to alter it and
//  redistribute it freely.
//
// http://www.sweeneydesign.co.uk
//
//====================================================================
//
//  Test app for eCos VNC-server
//
//==========================================================================


#include <vnc-server.h>
#include <cyg/kernel/kapi.h>  /* Kernel API */
#include <cyg/infra/diag.h>   /* Required for diag_printf */
#include <cyg/error/codes.h>  /* Cyg_ErrNo, ENOENT */
#include <cyg/io/io.h>        /* cyg_io_handle_t */

int main()
{
    Cyg_ErrNo  mouse_err, kbd_err;
    cyg_io_handle_t mouse_handle, kbd_handle;
    cyg_uint8 mouse_data[8], kbd_data[4];
    cyg_uint8 last_mouse_button = 0;
    cyg_uint32 mouse_len = 0;
    cyg_uint32 kbd_len = 0;
    vnc_printf_return_t print_area;
    cyg_uint16 text_y_pos;

    VncInit(VNC_WHITE);

    mouse_err = cyg_io_lookup("/dev/vnc_mouse", &mouse_handle);  /* Open mouse device */
    if (mouse_err == -ENOENT)
    {
        diag_printf("Could not open mouse device\n");
    }

    kbd_err = cyg_io_lookup("/dev/vnc_kbd", &kbd_handle);  /* Open keyboard device */
    if (kbd_err == -ENOENT)
    {
        diag_printf("Could not open kbd device\n");
    }

    VncFillRect(0, 0, 50, 50, VNC_BLACK);
    VncFillRect(50, 0, 100, 50, VNC_BLUE);
    VncFillRect(100, 0, 150, 50, VNC_GREEN);
    VncFillRect(150, 0, 200, 50, VNC_CYAN);

    VncFillRect(0, 50, 50, 100, VNC_RED);
    VncFillRect(50, 50, 100, 100, VNC_MAGENTA);
    VncFillRect(100, 50, 150, 100, VNC_BROWN);
    VncFillRect(150, 50, 200, 100, VNC_GRAY);

    VncFillRect(0, 100, 50, 150, VNC_LTGRAY);
    VncFillRect(50, 100, 100, 150, VNC_LTBLUE);
    VncFillRect(100, 100, 150, 150, VNC_LTGREEN);
    VncFillRect(150, 100, 200, 150, VNC_LTCYAN);

    VncFillRect(0, 150, 50, 200, VNC_LTRED);
    VncFillRect(50, 150, 100, 200, VNC_LTMAGENTA);
    VncFillRect(100, 150, 150, 200, VNC_YELLOW);
    VncFillRect(150, 150, 200, 200, VNC_WHITE);

    text_y_pos = 0;
    print_area = VncPrintf(&font_rom8x8, 1, VNC_BLACK, 250, text_y_pos, "Hello World!\nUsing rom8x8 font");
    text_y_pos += (print_area.height * 1.5);
    print_area = VncPrintf(&font_rom8x16, 1, VNC_BLUE, 250, text_y_pos, "Hello World!\nUsing rom8x16 font");
    text_y_pos += (print_area.height * 1.5);
    print_area = VncPrintf(&font_winFreeSansSerif11x13, 1, VNC_CYAN, 250, text_y_pos, "Hello World!\nUsing winFreeSansSerif11x13 font");
    text_y_pos += (print_area.height * 1.5);
    print_area = VncPrintf(0, 1, VNC_RED, 250, text_y_pos, "Hello World!\nUsing default (winFreeSystem14x16) font");
    text_y_pos += (print_area.height * 1.5);
    print_area = VncPrintf(&font_helvB10, 1, VNC_MAGENTA, 250, text_y_pos, "Hello World!\nUsing helvB10 font");
    text_y_pos += (print_area.height * 1.5);
    print_area = VncPrintf(&font_helvB12, 1, VNC_BROWN, 250, text_y_pos, "Hello World!\nUsing helvB12 font");
    text_y_pos += (print_area.height * 1.5);
    print_area = VncPrintf(&font_helvR10, 1, VNC_GREEN, 250, text_y_pos, "Hello World!\nUsing helvR10 font");
    text_y_pos += (print_area.height * 1.5);
    print_area = VncPrintf(&font_X5x7, 1, VNC_BLACK, 250, text_y_pos, "Hello World!\nUsing X5x7 font");
    text_y_pos += (print_area.height * 1.5);
    print_area = VncPrintf(&font_X6x13, 1, VNC_LTRED, 250, text_y_pos, "Hello World!\nUsing X6x13 font");

    /* Get the details of the area the text will occupy */
    print_area = VncPrintf(0, 0, VNC_BLACK, 150, 400, "Click on a yellow pixel to sound the bell!!\n");
    /* Draw a background for the text */
    VncFillRect(150, 400, 150 + print_area.width, 400 + print_area.height, VNC_BLUE);
    /* Write the text on the background */
    print_area = VncPrintf(0, 1, VNC_YELLOW, 150, 400, "Click on a yellow pixel to sound the bell!!\n");


    diag_printf("mouse_handle: %d\n", mouse_handle);
    diag_printf("kbd_handle: %d\n", kbd_handle);

    while(1)
    {
        if (kbd_handle)
        {
            kbd_len = 4;  /* Try to read 4 bytes from keyboard */
            cyg_io_read(kbd_handle, kbd_data, &kbd_len );
        }

        if (mouse_handle)
        {
            mouse_len = 8;  /* Try to read 8 bytes from mouse */
            cyg_io_read(mouse_handle, mouse_data, &mouse_len );
        }

        if (!mouse_len && !kbd_len)
        {
            /* There was no mouse or keyboard data */
            cyg_thread_delay(5);
        }

        if (mouse_len)
        {
            diag_printf("Mouse data: (%d, %d) - Button = 0x%x\n",
                         mouse_data[2]*256 + mouse_data[3],
                         mouse_data[4]*256 + mouse_data[5],
                         mouse_data[1]);

            if (mouse_data[1] && !last_mouse_button)
            {
                /* Ring bell if mouse button is pressed on a yellow pixel */
                if (VncReadPixel(mouse_data[2]*256 + mouse_data[3],
                                 mouse_data[4]*256 + mouse_data[5]) == VNC_YELLOW)
                {
                    VncSoundBell();
                    print_area = VncPrintf(0, 0, VNC_LTBLUE, 150, 400, "Click on a yellow pixel to sound the bell!!\n");
                    VncFillRect(150, 400, 150 + print_area.width, 400 + print_area.height, VNC_YELLOW);
                    print_area = VncPrintf(0, 1, VNC_BLACK, 150, 400, "Click on a yellow pixel to sound the bell!!\n");
                }
            }

            last_mouse_button = mouse_data[1];
        }

        if (kbd_len)
        {
            if (kbd_data[1])
            {
                diag_printf("Keyboard data: keysym value 0x%x is pressed\n",
                             kbd_data[2]*256 + kbd_data[3]);
            }
            else
            {
                diag_printf("Keyboard data: keysym value 0x%x is released\n",
                             kbd_data[2]*256 + kbd_data[3]);
            }
        }
    }

    return 1;
}
