def subsets(s):
    result=[]
    path=[]
    def dfs(i):
        if i==len(s):
            result.append(''.join(path))
            return
        dfs(i+1)
        path.append(s[i])
        dfs(i+1)
        path.pop()
    dfs(0)
    return result

print(subsets("abc"))