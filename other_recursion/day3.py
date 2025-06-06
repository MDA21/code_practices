def climb_stairs(n):
    memo={0:1,1:1}
    def ways(i):
        if i in memo:
            return memo[i]
        if i==0:
            return 1
        if i<0:
            return
        #ways(i)=ways(i-1)+ways(i-2) 这句是错的，不是有效的赋值，ways(i)不能出现在左边
        way_prev1=ways(i-1)
        way_prev2=ways(i-2)
        memo[i]=way_prev1+way_prev2
        return memo[i]
    return ways(n)