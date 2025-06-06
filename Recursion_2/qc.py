def partition_palindrome(s):
    def helper(char):
        flag=True
        for k in range(0,len(char)):
            if char[k]!=char[len(char)-1-k]:
                flag=False
                break
        return flag
    path=[]
    result=[]
    def dfs(i):
        if i==len(s):
            result.append(path.copy())
            return
        for j in range(i,len(s)):
            substring=s[i:j+1]
            if helper(substring):
                path.append(substring)
                dfs(j+1)
                path.pop()
    dfs(0)
    return result


print(partition_palindrome("aab"))