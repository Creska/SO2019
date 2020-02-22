#include "pcb.h"
#include "listx.h"
#include "const.h"
#include "types_bikaya.h"

pcb_t pcbFree_table[MAXPROC];

/*pcbFree's buddy*/
struct list_head pcbFree_h = LIST_HEAD_INIT(pcbFree_h);

struct PCBlistItem
{
    /*Struct composed by: *next     //&next_ListElement
                          *prev     //&prev_ListElement
                                                       */
    struct list_head header;
    
    /*PCB's body of each element's list*/
    pcb_t item;
};

/*Initialize the pcbFree list with all the elements of the pcbFree_table*/
void initPcbs()
{
    for (int i = 0; i < MAXPROC; i++)
    {
        freePcb(&pcbFree_table[i]);
    }
}

/*Insert the given pcb in the head of the list*/
void freePcb(pcb_t * p)
{   
    list_add(&p->p_next, &pcbFree_h);
}

/*Initialize the mkEmptyProcQ's queue putting the buddy's element*/
void mkEmptyProcQ(struct list_head *head)
{
    INIT_LIST_HEAD(head);
    list_add_tail(head, &head->next);
}

/*If the buddy's next pointer is equal to NULL the emptyProcQ is empty*/
int emptyProcQ(struct list_head *head)
{
    return &head->next == NULL;
}

/*Iterate each element of the queue. If the element to insert has high priority compared to the current pcb we add it between
  the previous element and the current pcb instead if che element to insert has less priority it will be put between the current 
  pcb and his next element*/
void insertProcQ(struct list_head* head, pcb_t* p)
{
    struct list_head *next_element;
    
    list_for_each(next_element, head)
    {
        pcb_t *check = container_of(next_element, PCBlistItem, p_next);
        int current_priority =  &check->priority;

        if(&p->priority > &check->priority)
        {
            __list_add(&p->p_next, &check->p_next.prev, &check->p_next);
        }
        else
        {
            __list_add(&p->p_next, &check->p_next, &check->p_next.next);
        }
    }
}

/*If the queue is not empty we return the pcb's pointer of the first element that is not the buddy otherwise NULL*/
pcb_t *headProcQ(struct list_head *head)
{
    if (!list_empty(head))
    {
        return container_of(&head->next, PCBlistItem, p_next);
    }

    return NULL;
}

/*Set the pointer request_pcb to the next item of the buddy. If the pcb is not the buddy we can remove it otherwise the queue is empty
  and we will return the NULL pointer's value*/
pcb_t *removeProcQ(struct list_head *head)
{
    pcb_t *request_pcb = list_next(head);

    if (request_pcb != head)
    {
        list_del(request_pcb);

        return request_pcb;
    }

    return NULL;
}

/*Iterate each element of the queue. If the element is inside the structure the pointer called pcb_requested is set to his memory address
  and deleted from the queue instead if the pcb is not present inside the queue we can return the pcb_requested pointer set by default to NULL.*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p)
{
    pcb_t *pcb_removed;
    struct list_head *next_element;

    list_for_each(next_element, head)
    {
        
        pcb_removed = container_of(next_element, PCBlistItem, p_next);
            
        if (pcb_removed == &p)
        {
            list_del(p);
        }
    }

    return pcb_removed;
}