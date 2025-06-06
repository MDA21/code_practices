def fib (n,memo):
    if n in memo:
        return memo[n]
    else:
        result=fib(n-1,memo)+fib(n-2,memo)
        memo[n]=result
        return result

memo={0:0,1:1}
print(fib(5,memo))
print(memo)