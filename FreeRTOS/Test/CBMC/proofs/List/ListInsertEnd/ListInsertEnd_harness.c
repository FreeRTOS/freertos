/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#include <stdint.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"
#include "cbmc.h"

void harness()
{
    List_t pxList;
    vListInitialise(&pxList);
    
    // Non-deterministically add 0 to (maxElements-1) elements to the
    // list. (The -1 ensures that there is space to insert 1 more element)
    for (UBaseType_t i = 0; i < configLIST_SIZE - 1; i++)
    //__CPROVER_assigns (i,__CPROVER_POINTER_OBJECT(pxList.xListData))
    //__CPROVER_loop_invariant (i >= 0 && i<=configLIST_SIZE)
    //__CPROVER_decreases (configLIST_SIZE - i)
    {
        if (nondet_bool()){
            pxList.xListData[pxList.uxNumberOfItems] = pvPortMalloc(sizeof(ListItem_t));
            if (pxList.xListData[pxList.uxNumberOfItems] == NULL){
                exit(1);
            }
            pxList.uxNumberOfItems++;
        }
    }

    // Set the pxIndex field to some value between 0 and number of items
    if (pxList.uxNumberOfItems == 0){
        pxList.pxIndex = 0;
    }
    else{
        pxList.pxIndex = notdet_uint32();
        __CPROVER_assume( pxList.pxIndex < pxList.uxNumberOfItems );
    }

    // Finally add 1 item.
    ListItem_t newItem;
    vListInsertEnd(&pxList, &newItem);
}