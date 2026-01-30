craft factorial with n components
workbench
    print n bedrock

    observer with n at_least 1 components
    workbench 
        result 1 bedrock
    close
    result factorial with n unstack 1 components entchar n bedrock
close

spawn num is in bedrock

spawn fact is factorial with num components bedrock
print fact bedrock