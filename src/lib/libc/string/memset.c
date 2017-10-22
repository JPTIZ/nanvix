/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of Nanvix.
 * 
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * 
 * @brief memset() implementation.
 */

#include <sys/types.h>

/**
 * @brief Sets bytes in memory.
 * 
 * @param s Pointer to target object.
 * @param c Character to copy.
 * @param n Number of bytes to set.
 * 
 * @returns @p s is returned.
 * 
 * @version IEEE Std 1003.1, 2013 Edition
 */
void *memset(void *s, int c, size_t n)
{
    unsigned char *p;
    
    p = s;
    
    while (n-- > 0)
        *p++ = c;
    
    return (s);
}
