# ======== 题目 1：sum_nums ========
def sum_nums(n):
    # 这里写你的递归代码
    if n==0:
        return n
    else:
        return n+sum_nums(n-1)

# ======== 题目 2：reverse_list ========
def reverse_list(lst):
    # 这里写你的递归代码
    def helper(lst):
        for i in range(0,len(lst)):
            if isinstance(lst[i],list):
                helper(lst[i])
        lst[i].reverse()
    helper(lst)
    return lst

# ======== 题目 3：max_depth ========
class TreeNode:
    def __init__(self, val, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right

def max_depth(root):
    # 这里写你的递归代码
    if root==None:
        return 0
    else:
        left_depth=max_depth(root.left)
        right_depth=max_depth(root.right)
    return 1+max(left_depth,right_depth)

# ======== 题目 4：combination_sum ========
def combination_sum(candidates, target):
    # 这里写你的递归+回溯代码
    candidates.sort()
    results=[]
    path=[]
    def helper(start,remain):
        if remain==0:
            results.append(path.copy())
            return
        if remain<0:
            return
        for i in range(start,len(candidates)):
            c=candidates[i]
            if c>remain:
                break
            path.append(c)
            helper(i,remain-c)
            path.pop()
    helper(0,target)
    return results