def combination_sum(candidates,target):
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

print(combination_sum([2, 3, 6, 7], 7))