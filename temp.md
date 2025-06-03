procedure downsample
require: nm, n
require: U_nm is uniformly distributed in $[0,n+m)$
ensures: U_x is uniformly distributed in $[0,x)$
ensures: x is n or n is m
ensures: B is distributed as a Bernoulli distribution
ensure: B is independent of U_x
Input: U_nm, nm, n
returns: U_x, x, B

procedure upsample
Require: B is Boolean
Require: 
Require: U_x is uniformly distributed in $[0,x)$
Require: B has a Ber
Ensure: U_nm is uniformly distributed in $[0,n+m)$
Input: U_x, x, nm, B
Output: U_nm