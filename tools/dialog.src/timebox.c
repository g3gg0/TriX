/*
 * $Id: timebox.c 1521 2006-06-28 13:53:39Z geggo $
 *
 *  timebox.c -- implements the timebox dialog
 *
 * Copyright 2001-2005,2006   Thomas E. Dickey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 2.1 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to
 *	Free Software Foundation, Inc.
 *	51 Franklin St., Fifth Floor
 *	Boston, MA 02110, USA.
 */

#include <dialog.h>
#include <dlg_keys.h>

#include <time.h>

#define ONE_HIGH 1
#define ONE_WIDE 2
#define BTN_HIGH 2

#define MIN_HIGH (ONE_HIGH + BTN_HIGH + (4 * MARGIN))
#define MIN_WIDE ((3 * (ONE_WIDE + 2 * MARGIN)) + 2 + (2 * MARGIN))

typedef enum {
    sHR = -3
    ,sMN = -2
    ,sSC = -1
} STATES;

struct _box;

typedef int (*BOX_DRAW) (struct _box *, struct tm *);

typedef struct _box {
    WINDOW *parent;
    WINDOW *window;
    int x;
    int y;
    int width;
    int height;
    int period;
    int value;
} BOX;

static int
next_or_previous(int key)
{
    int result = 0;

    switch (key) {
    case DLGK_ITEM_PREV:
	result = -1;
	break;
    case DLGK_ITEM_NEXT:
	result = 1;
	break;
    default:
	beep();
	break;
    }
    return result;
}
/*
 * Draw the hour-of-month selection box
 */
static int
draw_cell(BOX * data)
{
    werase(data->window);
    dlg_draw_box(data->parent,
		 data->y - MARGIN, data->x - MARGIN,
		 data->height + (2 * MARGIN), data->width + (2 * MARGIN),
		 menubox_border_attr, menubox_attr);

    wattrset(data->window, item_attr);
    wprintw(data->window, "%02d", data->value);
    return 0;
}

static int
init_object(BOX * data,
	    WINDOW *parent,
	    int x, int y,
	    int width, int height,
	    int period, int value,
	    int code)
{
    data->parent = parent;
    data->x = x;
    data->y = y;
    data->width = width;
    data->height = height;
    data->period = period;
    data->value = value % period;

    data->window = derwin(data->parent,
			  data->height, data->width,
			  data->y, data->x);
    if (data->window == 0)
	return -1;
    (void) keypad(data->window, TRUE);

    dlg_mouse_setbase(getbegx(parent), getbegy(parent));
    dlg_mouse_mkregion(y, x, height, width, code);

    return 0;
}

#define DrawObject(data) draw_cell(data)

/*
 * Display a dialog box for entering a date
 */
int
dialog_timebox(const char *title,
	       const char *subtitle,
	       int height,
	       int width,
	       int hour,
	       int minute,
	       int second)
{
    /* *INDENT-OFF* */
    static DLG_KEYS_BINDING binding[] = {
	DLG_KEYS_DATA( DLGK_DELETE_RIGHT,KEY_DC ),
	ENTERKEY_BINDINGS,
	DLG_KEYS_DATA( DLGK_ENTER,	' ' ),
	DLG_KEYS_DATA( DLGK_FIELD_FIRST,KEY_HOME ),
	DLG_KEYS_DATA( DLGK_FIELD_LAST, KEY_END ),
	DLG_KEYS_DATA( DLGK_FIELD_LAST, KEY_LL ),
	DLG_KEYS_DATA( DLGK_FIELD_NEXT, CHR_NEXT ),
	DLG_KEYS_DATA( DLGK_FIELD_NEXT, KEY_RIGHT ),
	DLG_KEYS_DATA( DLGK_FIELD_NEXT, TAB ),
	DLG_KEYS_DATA( DLGK_FIELD_PREV, CHR_BACKSPACE ),
	DLG_KEYS_DATA( DLGK_FIELD_PREV, CHR_PREVIOUS ),
	DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_BTAB ),
	DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_LEFT ),
	DLG_KEYS_DATA( DLGK_ITEM_NEXT,  KEY_DOWN),
	DLG_KEYS_DATA( DLGK_ITEM_NEXT,  KEY_NEXT),
	DLG_KEYS_DATA( DLGK_ITEM_NEXT,  KEY_NPAGE),
	DLG_KEYS_DATA( DLGK_ITEM_PREV,  KEY_PPAGE ),
	DLG_KEYS_DATA( DLGK_ITEM_PREV,  KEY_PREVIOUS ),
	DLG_KEYS_DATA( DLGK_ITEM_PREV,  KEY_UP ),
	END_KEYS_BINDING
    };
    /* *INDENT-ON* */

#ifdef KEY_RESIZE
    int old_height = height;
    int old_width = width;
#endif
    BOX hr_box, mn_box, sc_box;
    int key = 0, key2, fkey;
    int button;
    int result = DLG_EXIT_UNKNOWN;
    WINDOW *dialog;
    time_t now_time = time((time_t *) 0);
    struct tm current;
    STATES state = dlg_defaultno_button();
    const char **buttons = dlg_ok_labels();
    char *prompt = dlg_strclone(subtitle);
    char buffer[MAX_LEN];

    now_time = time((time_t *) 0);
    current = *localtime(&now_time);

    dlg_does_output();

#ifdef KEY_RESIZE
  retry:
#endif

    dlg_auto_size(title, prompt, &height, &width, 0, 0);
    height += MIN_HIGH;
    if (width < MIN_WIDE)
	width = MIN_WIDE;
    dlg_button_layout(buttons, &width);
    dlg_print_size(height, width);
    dlg_ctl_size(height, width);

    dialog = dlg_new_window(height, width,
			    dlg_box_y_ordinate(height),
			    dlg_box_x_ordinate(width));
    dlg_register_window(dialog, "timebox", binding);
    dlg_register_buttons(dialog, "timebox", buttons);

    dlg_draw_box(dialog, 0, 0, height, width, dialog_attr, border_attr);
    dlg_draw_bottom_box(dialog);
    dlg_draw_title(dialog, title);

    wattrset(dialog, dialog_attr);
    dlg_print_autowrap(dialog, prompt, height, width);

    /* compute positions of hour, month and year boxes */
    memset(&hr_box, 0, sizeof(hr_box));
    memset(&mn_box, 0, sizeof(mn_box));
    memset(&sc_box, 0, sizeof(sc_box));

    if (init_object(&hr_box,
		    dialog,
		    (width - MIN_WIDE + 1) / 2 + MARGIN,
		    (height - MIN_HIGH + MARGIN),
		    ONE_WIDE,
		    ONE_HIGH,
		    24,
		    hour >= 0 ? hour : current.tm_hour,
		    'H') < 0
	|| DrawObject(&hr_box) < 0)
	return -1;

    mvwprintw(dialog, hr_box.y, hr_box.x + ONE_WIDE + MARGIN, ":");
    if (init_object(&mn_box,
		    dialog,
		    hr_box.x + (ONE_WIDE + 2 * MARGIN + 1),
		    hr_box.y,
		    hr_box.width,
		    hr_box.height,
		    60,
		    minute >= 0 ? minute : current.tm_min,
		    'M') < 0
	|| DrawObject(&mn_box) < 0)
	return -1;

    mvwprintw(dialog, mn_box.y, mn_box.x + ONE_WIDE + MARGIN, ":");
    if (init_object(&sc_box,
		    dialog,
		    mn_box.x + (ONE_WIDE + 2 * MARGIN + 1),
		    mn_box.y,
		    mn_box.width,
		    mn_box.height,
		    60,
		    second >= 0 ? second : current.tm_sec,
		    'S') < 0
	|| DrawObject(&sc_box) < 0)
	return -1;

    while (result == DLG_EXIT_UNKNOWN) {
	BOX *obj = (state == sHR ? &hr_box
		    : (state == sMN ? &mn_box :
		       (state == sSC ? &sc_box : 0)));

	button = (state < 0) ? 0 : state;
	dlg_draw_buttons(dialog, height - 2, 0, buttons, button, FALSE, width);
	if (obj != 0)
	    dlg_set_focus(dialog, obj->window);

	key = dlg_mouse_wgetch(dialog, &fkey);
	if (dlg_result_key(key, fkey, &result))
	    break;

	if ((key2 = dlg_char_to_button(key, buttons)) >= 0) {
	    result = key2;
	} else {
	    /* handle function-keys */
	    if (fkey) {
		switch (key) {
		case DLGK_MOUSE(0):
		    result = DLG_EXIT_OK;
		    break;
		case DLGK_MOUSE(1):
		    result = DLG_EXIT_CANCEL;
		    break;
		case DLGK_MOUSE('H'):
		    state = sHR;
		    break;
		case DLGK_MOUSE('M'):
		    state = sMN;
		    break;
		case DLGK_MOUSE('S'):
		    state = sSC;
		    break;
		case DLGK_ENTER:
		    result = button;
		    break;
		case DLGK_FIELD_PREV:
		    state = dlg_prev_ok_buttonindex(state, sHR);
		    break;
		case DLGK_FIELD_NEXT:
		    state = dlg_next_ok_buttonindex(state, sHR);
		    break;
		case DLGK_FIELD_FIRST:
		    if (obj != 0) {
			obj->value = 0;
			(void) DrawObject(obj);
		    }
		    break;
		case DLGK_FIELD_LAST:
		    if (obj != 0) {
			switch (state) {
			case sHR:
			    obj->value = 23;
			    break;
			case sMN:
			case sSC:
			    obj->value = 59;
			    break;
			}
			(void) DrawObject(obj);
		    }
		    break;
		case DLGK_DELETE_RIGHT:
		    if (obj != 0) {
			obj->value /= 10;
			(void) DrawObject(obj);
		    }
		    break;
#ifdef KEY_RESIZE
		case KEY_RESIZE:
		    /* reset data */
		    height = old_height;
		    width = old_width;
		    hour = hr_box.value;
		    minute = mn_box.value;
		    second = sc_box.value;
		    /* repaint */
		    dlg_clear();
		    dlg_del_window(dialog);
		    refresh();
		    dlg_mouse_free_regions();
		    goto retry;
#endif
		default:
		    if (obj != 0) {
			int step = next_or_previous(key);
			if (step != 0) {
			    obj->value += step;
			    while (obj->value < 0)
				obj->value += obj->period;
			    obj->value %= obj->period;
			    (void) DrawObject(obj);
			}
		    }
		    break;
		}
	    } else if (isdigit(key)) {
		if (obj != 0) {
		    int digit = (key - '0');
		    int value = (obj->value * 10) + digit;
		    if (value < obj->period) {
			obj->value = value;
			(void) DrawObject(obj);
		    } else {
			beep();
		    }
		}
	    } else {
		beep();
	    }
	}
    }

    dlg_del_window(dialog);
    sprintf(buffer, "%02d:%02d:%02d\n",
	    hr_box.value, mn_box.value, sc_box.value);
    dlg_add_result(buffer);
    dlg_mouse_free_regions();
    free(prompt);
    return result;
}
