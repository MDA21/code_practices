def max_depth(root):
    if root==None:
        return 0
    else:
        left_depth=max_depth(root.left)
        right_depth=max_depth(root.right)
    return 1+max(left_depth,right_depth)