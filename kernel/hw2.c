#include <linux/kernel.h>  
#include <linux/sched.h>
#include <asm/current.h>
#include <linux/list.h>

long heaviest_aux(struct task_struct* ptr, pid_t* heaviest_pid, long* max_weight);
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
    long sum=0;
    struct task_struct* t=current;
    while (t->pid != 0)
    {
        sum=sum + t->weight;
        t=t->parent;
    }
    //do{
    //    sum=sum+(t->weight);
    //    t=t->parent;
    //} while(t->pid != 0);
    return sum;
}

asmlinkage long sys_get_heaviest_descendant(void)
{
    if (list_empty(&current->children))
        return -ECHILD;
    return heaviest_aux(current, NULL, NULL);}

long heaviest_aux(struct task_struct* ptr, pid_t* heaviest_pid, long* max_weight)
{
    struct task_struct *task;
    struct list_head *list; 

    list_for_each(list, &ptr->children)
    {
        task = list_entry(list, struct task_struct, sibling);
        if(heaviest_pid==NULL)
            *heaviest_pid = task->pid;
        if(max_weight==NULL)
            *max_weight = task->weight;
        if ((task->weight > *max_weight) || ((task->weight == *max_weight) && (task->pid < *heaviest_pid))) {
            *heaviest_pid=task->pid;
            *max_weight=task->weight;
        }
    }
    return *heaviest_pid;
}