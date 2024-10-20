#include "list.h"
#include <stdio.h>

int main()
{
    struct list* list = list(int);
    printf("%d, %d\n\n", list->count, list->size);

    // for (float i = 0; i < list->count; i++)
    // {
    //     list_append(list, (float*)&i);
    // }
    // float ff = 1.5;
    // float *f;;
    // *f = ff;
    // list_append(list, f);
    //
    // list->size = 3;
    // list_append(list, (char*)'c');
    // list_append(list, (char*)'g');
    // list_append(list, (char*)'a');
    // list_append(list, (char*)'e');
    // list_append(list, (char*)'f');
    // list_append(list, (char*)'t');
    // //list_remove(list, 4);

    for (long i = 0; i < 36; i++)
    {
        list_append(list, (int*)i);
    }

    printf("%d, %d\n\n", list->count, list->size);

    for (int i = 0; i < list->count; i++)
    {
        printf("%d, ", *(int*)list_get(list, i));
    }
    return 0;
}
