craft discr with  par1 and  par2 and  par3 components
workbench
	spawn discr is  with  with  par2  entchar  par2  components  unstack  with  with  4  entchar  par1  components  entchar  par3  components  components  bedrock
result  discr  bedrock
close
craft quadric_solver with  a and  b and  c components
workbench
	observer with  a  same  0  components
workbench
observer with  b  same  0  components
workbench
observer with  c  same  0  components
workbench
print  8  bedrock
close
else
workbench
print  with unstack  111  components  bedrock
close
close
else
workbench
print  with  with unstack  c  components  split  b  components  bedrock
close
close
spawn D is  discr with  a  and  b  and  c  components  bedrock
observer with  D  less  0  components
workbench
print  with unstack  111  components  bedrock
close
observer with  D  same  0  components
workbench
spawn x is  with  with unstack  b  components  split  with  2  entchar  a  components  components  bedrock
print  x  bedrock
close
observer with  D  above  0  components
workbench
spawn x1 is  with  with  with unstack  b  components  unstack  with mineroot  D  components  components  split  with  2  entchar  a  components  components  bedrock
spawn x2 is  with  with  with unstack  b  components  stack  with mineroot  D  components  components  split  with  2  entchar  a  components  components  bedrock
print  x1  bedrock
print  x2  bedrock
close
result  1  bedrock
close
spawn a is  in  bedrock
spawn b is  in  bedrock
spawn c is  in  bedrock
 quadric_solver with  a  and  b  and  c  components  bedrock
