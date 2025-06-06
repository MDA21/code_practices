def permute(nums):
    path=[]
    result=[]
    used=[False]*len(nums)
    def dfs():
        if len(path)==len(nums):
            result.append(path.copy())
            return
        for i in range(0,len(nums)):
            if not used[i]:
                used[i]=True
                path.append(nums[i])
                dfs()
                path.pop()
                used[i]=False
    dfs()
    return result