def unique_paths(m,n):
    memo={}
    def dfs(i,j):
        if (i,j) in memo:
            return memo[(i,j)]
        if (i,j)==(m-1,n-1):
            return 1
        if i>=m or j>=n:
            return 0
        path_right=dfs(i+1,j)
        path_down=dfs(i,j+1)
        memo[(i,j)]=path_down+path_right
        return memo[(i,j)]
    return dfs(0,0)

unique_paths(5,6)