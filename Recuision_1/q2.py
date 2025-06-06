def reverse_list(lst):
    new_lst=[]
    for i in reversed(lst):
        if isinstance(i,list):
            new_lst.append(reverse_list(i))
        else:
            new_lst.append(i)
    return new_lst

lst = [1, [2, 3, [4, 5]], 6]
print(reverse_list(lst))