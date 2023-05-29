#include <linux/kernel.h>  
#include <linux/sched.h>
#include <asm/current.h>
#include <linux/list.h>

void heaviest_aux(struct task_struct* ptr, long* heaviest_pid, long* max_weight);
asmlinkage long sys_hello(void) {     
    printk("Hello, World!\n");     
    return 0; 
    } 
    
asmlinkage long sys_set_weight(int weight)
{
    if (weight<0)
        return -EINVAL;
    current->weight = weight;
    return 0;
}

asmlinkage long sys_get_weight(void)
{
    return current->weight;
}

asmlinkage long sys_get_ancestor_sum(void)
{
    long sum=current->weight;
    struct task_struct* t=current;
    if(t->pid ==0)
        return sum;
    do{
        t=t->parent;
        sum=sum + t->weight;
    }while (t->pid != 0);
    return sum;
}

asmlinkage long sys_get_heaviest_descendant(void)
{
    long heaviest_pid=-1;
    long max_weight=-1;
    if (list_empty(&current->children))
        return -ECHILD;
    heaviest_aux(current, &heaviest_pid, &max_weight);
    return heaviest_pid;
}


void heaviest_aux(struct task_struct* ptr, long* heaviest_pid, long* max_weight)
{
    struct task_struct *task;
    struct list_head *list; 

    list_for_each(list, &ptr->children)
    {
        task = list_entry(list, struct task_struct, sibling);
        if(*heaviest_pid==-1)
            *heaviest_pid = task->pid;
        if(*max_weight==-1)
            *max_weight = task->weight;
        if ((task->weight > *max_weight) || ((task->weight == *max_weight) && (task->pid < *heaviest_pid))) {
            *heaviest_pid=task->pid;
            *max_weight=task->weight;
        }
        heaviest_aux(task, heaviest_pid, max_weight);
    }
}