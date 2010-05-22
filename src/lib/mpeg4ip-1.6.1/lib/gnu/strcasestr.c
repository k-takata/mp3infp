/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is MPEG4IP.
 * 
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2000, 2001.  All Rights Reserved.
 * 
 * Contributor(s): 
 *              Bill May        wmay@cisco.com
 */
/*
 * player_util.c - utility routines for output
 */

#include "mpeg4ip.h"
#include "strcasestr.h"

#ifndef HAVE_STRCASESTR
char *strcasestr (const char *haystack, const char *needle)
{
  uint32_t nlen, hlen;
  uint32_t ix;

  if (needle == NULL || haystack == NULL) return (NULL);
  nlen = strlen(needle);
  hlen = strlen(haystack);
 
  for (ix = 0; ix + nlen <= hlen; ix++) {
    if (strncasecmp(needle, haystack + ix, nlen) == 0) {
      return ((char *)haystack + ix);
    }
  }
  return (NULL);
}
#endif
/* end file player_util.c */
