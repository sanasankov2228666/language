craft discr with par1 and par2 and par3 components
workbench
    spawn discr is par2 entchar par2 unstack 4 entchar par1 entchar par3 bedrock
    result discr bedrock
close

craft quadric_solver with a and b and c components
workbench

    observer with a same 0 components
    workbench 
        observer with b same 0 components
        workbench
            print 8 bedrock
        close
    close

    observer with a same 0 components
    workbench 
        print unstack c split b bedrock
    close

    spawn discr is discr with a and b and c components bedrock

    observer with discr less 0 components
    workbench
        print unstack 999 bedrock
    close

    observer with discr same 0 components
    workbench
        spawn x is unstack b split with 2 entchar a components bedrock
        print x bedrock
    close

    observer with discr above 0 components
    workbench
        spawn x1 is with unstack b unstack mineroot discr components split with 2 entchar a components bedrock
        spawn x2 is with unstack b stack   mineroot discr components split with 2 entchar a components bedrock
        print x1 bedrock
        print x2 bedrock
    close

    result 1 bedrock
close

spawn a is in bedrock
spawn b is in bedrock
spawn c is in bedrock 

quadric_solver with a and b and c components bedrock