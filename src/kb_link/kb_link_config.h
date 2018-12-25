/*
 * KB link configuration file.
 * Copyright (C) 2018 Kittipong Yothaithiang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _KB_LINK_CONIFG_H_
#define _KB_LINK_CONIFG_H_

// Priority for KB link event in SoftDevice.
#define KB_LINK_BLE_OBSERVER_PRIO 2

// Base UUID: 0D660000-AF06-44F6-A004-F8A8138518C0
#define KB_LINK_SERVICE_BASE_UUID {0xC0, 0x18, 0x85, 0x13, 0xA8, 0xF8, 0x04, 0xA0, 0xF6, 0x44, 0x06, 0xAF, 0x00, 0x00, 0x66, 0x0D}

// Service & characteristics UUIDs
#define KB_LINK_SERVICE_UUID        0xF36B
#define KB_LINK_KEY_INDEX_CHAR_UUID 0xC74B

#endif
